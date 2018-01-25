// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/net/EventLoop.h>

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/Channel.h>
#include <muduo/net/Poller.h>
#include <muduo/net/SocketsOps.h>
#include <muduo/net/TimerQueue.h>

#include <boost/bind.hpp>

#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

namespace
{
/*t_loopInThisThread 记录当前线程的 EventLoop 对象*/
__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 50;//MODIFY BY WJS 20171218

/**
 * createEventfd()的返回值用于初始化 eventLoop 对象的wakeupFd_; 
 */
int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    LOG_SYSERR << "Failed in eventfd";
    abort();
  }
  return evtfd;
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
class IgnoreSigPipe
{
 public:
  IgnoreSigPipe()
  {
    ::signal(SIGPIPE, SIG_IGN);
    // LOG_TRACE << "Ignore SIGPIPE";
  }
};
#pragma GCC diagnostic error "-Wold-style-cast"

IgnoreSigPipe initObj;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
  return t_loopInThisThread;
}

/**
 * EventLoop()会检查当前线程是否已创建了其他EventLoop对象，若已创建，则LOG_FATAL 终止程序;
 * 创建wakeupChannel_并在wakeupChannel_上注册kReadEvent事件，当其他线程调用loop_->runInLoop(functor)
 * 函数或者loop_->queueInLoop(functor)函数时会转调用wakeup()函数，wakeup()会在wakeupChannel_的fd中写入数据
 * poller_->poll()便可检测到wakeupChannel_的kReadEvent事件发生，从poll()函数中返回，接下来便会执行
 * loop（）函数中的doPendingFunctors()执行用户回调函数。
 * 
 * 何时需要通过wakeup()唤醒loopThread线程，需要分情况考虑。
 */
EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    iteration_(0),
    threadId_(CurrentThread::tid()),//记住本loop对象所属线程  即为当前线程
    poller_(Poller::newDefaultPoller(this)),//IO multiplexing
    timerQueue_(new TimerQueue(this)),
    wakeupFd_(createEventfd()),//linux 可通过eventfd (详见createEventFd())来实现线程间通信
    wakeupChannel_(new Channel(this, wakeupFd_)),//根据wakeupFd_创建wakeupChannel_
    currentActiveChannel_(NULL)
{
  LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
  //确保当前线程只有一个EventLoop 对象，即 one loop per thread
  if (t_loopInThisThread)
  {
    LOG_FATAL << "Another EventLoop " << t_loopInThisThread
              << " exists in this thread " << threadId_;
  }
  else
  {
    t_loopInThisThread = this;
  }

  //wakeupChannel_用于处理wakeupFd_上的redable事件，将事件分发至回到函数EventLoop::handleRead()
  wakeupChannel_->setReadCallback( boost::bind(&EventLoop::handleRead, this) );//设置wakeupChannel_的readCallback_

  // we are always reading the wakeupfd
  /**
   * void Channel::enableReading() { events_ |= kReadEvent; update(); }
   * void Channel::update(){ addedToLoop_ = true; loop_->updateChannel(this); }
   * void EventLoop::updateChannel(Channel* channel){  poller_->updateChannel(channel);}
   * 在 poller_->updateChannel(channel)中会根据channel->fd()将channel对应的fd，及该fd上关注的
   * events添加到pollfds_数组中，poller->poll(...)便可实现IO MULTIPLEXING  
   */
  wakeupChannel_->enableReading();//设置wakeupChannel_对应的wakeupFd_关注kReadEvent事件，并将wakeupFd_在poller_->pollfds_中update
}

EventLoop::~EventLoop()
{
  LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_
            << " destructs in thread " << CurrentThread::tid();
  wakeupChannel_->disableAll();
  wakeupChannel_->remove();
  ::close(wakeupFd_);//wakeup_的生命周期由EventLoop管理
  t_loopInThisThread = NULL;
}

/**
 * 事件循环主体， poller_->poll（...）获取activeChannels
 * currentActiveChannel_->handleEvent(pollReturnTime_) 事件回调函数
 * doPendingFunctors() IO线程被唤醒后 执行用户回调函数
 */
void EventLoop::loop()
{
  assert(!looping_);
  assertInLoopThread();//loop()函数只能在LoopThread中执行
  looping_ = true;
  quit_ = false;  // FIXME: what if someone calls quit() before loop() ?
  LOG_TRACE << "EventLoop " << this << " start looping";

  while (!quit_)
  {
    activeChannels_.clear();
    pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
    ++iteration_;//记录poll()被调用的次数
    if (Logger::logLevel() <= Logger::TRACE)
    {
      printActiveChannels();
    }

    // TODO sort channel by priority
    eventHandling_ = true;
    for (ChannelList::iterator it = activeChannels_.begin();
        it != activeChannels_.end(); ++it)
    {
      currentActiveChannel_ = *it;
      //处理activeChannels_上已发生的事件，回调注册的事件处理函数
      currentActiveChannel_->handleEvent(pollReturnTime_);
    }
    currentActiveChannel_ = NULL;
    eventHandling_ = false;
    //执行timerQueue_中 等待的functors
    doPendingFunctors();
  }

  LOG_TRACE << "EventLoop " << this << " stop looping";
  looping_ = false;
}

void EventLoop::quit()
{
  quit_ = true;
  // There is a chance that loop() just executes while(!quit_) and exits,
  // then EventLoop destructs, then we are accessing an invalid object.
  // Can be fixed using mutex_ in both places.
  //如果不是在LoopThread中调用EventLoop::quit()函数，则需要wakeup loopThread来终止loop循环
  if (!isInLoopThread())
  {
    wakeup();
  }
}

/**
 * loop->runInLoop（cb）的主要作用是将cb转移到LOOP IO THREAD中调用
 * 也即 runInLoop（cb）函数可以跨线程调用(即在非LOOP IO THREAD 中调用)
 */
void EventLoop::runInLoop(const Functor& cb)
{
  //case 1: runInLoop(cb) 在LoopThread中被调用，则直接执行cb()
  if (isInLoopThread())
  {
    cb();
  }
  else//case 2:runInLoop(cb)不在LoopThread线程内调用,则执行queueInLoop(cb)
  {
    queueInLoop(cb);
  }
}

/**
 * 用户回调函数cb 被加入loop的回调函数队列pendingFunctors； IO线程会被唤醒（从poller->poll(...)中返回）
 * 来调用pendingFunctor_中的用户回调函数
 */
void EventLoop::queueInLoop(const Functor& cb)
{
  
  {
      MutexLockGuard lock(mutex_);
      pendingFunctors_.push_back(cb);
  }

  //queueInLoop不在loop的IO线程被调用，或者loop()正在执行pendingFunctors_中的函数，则需要wakeup loopThread
  if (!isInLoopThread() || callingPendingFunctors_)
  {
    wakeup();//唤醒IO thread， 会执行loop 函数中的doPendingFunctors()
  }
}


size_t EventLoop::queueSize() const
{
  MutexLockGuard lock(mutex_);
  return pendingFunctors_.size();
}

/**
 * runAt() runAfter() runEvery() 主要是向timerQueue中添加timer；
 * cancel()主要是？？？？
 * 这几个函数何时，在哪里被调用？？？ 一般在最外层的客户程序被调用比如/timer4/timer.cc，实际上
 * 在loop_->run×××()及其调用的其它函数中已经完成了timer的构造，callback的设置，POLLIN | POLLPRI事件的注册等所有的工作
 * 
 */

//在指定的时间time 执行cb
TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
  //timerQueue->addTimer中完成Timer的构造(指明了在何时执行何定时任务)，并将其添加到了EventLoop对象的数据成员timerQueue中
  return timerQueue_->addTimer(cb, time, 0.0);
}

//延迟delay秒后 执行cb
TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
  Timestamp time(addTime(Timestamp::now(), delay));
  return runAt(time, cb);
}

//已interval为间隔时间 执行cb
TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
  Timestamp time(addTime(Timestamp::now(), interval));
  return timerQueue_->addTimer(cb, time, interval);
}

//取消定时任务 
void EventLoop::cancel(TimerId timerId)
{
  return timerQueue_->cancel(timerId);
}




#ifdef __GXX_EXPERIMENTAL_CXX0X__
// FIXME: remove duplication
void EventLoop::runInLoop(Functor&& cb)
{
  if (isInLoopThread())
  {
    cb();
  }
  else
  {
    queueInLoop(std::move(cb));
  }
}

void EventLoop::queueInLoop(Functor&& cb)
{
  {
  MutexLockGuard lock(mutex_);
  pendingFunctors_.push_back(std::move(cb));  // emplace_back
  }

  if (!isInLoopThread() || callingPendingFunctors_)
  {
    wakeup();
  }
}

TimerId EventLoop::runAt(const Timestamp& time, TimerCallback&& cb)
{
  return timerQueue_->addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback&& cb)
{
  Timestamp time(addTime(Timestamp::now(), delay));
  return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback&& cb)
{
  Timestamp time(addTime(Timestamp::now(), interval));
  return timerQueue_->addTimer(std::move(cb), time, interval);
}
#endif


//update channel对应的fd上的 events，EventLoop::updateChannel(Channel* channel)只能在LoopThread中执行
void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  //updateChannel真正的实际操作
  poller_->updateChannel(channel);
}

//从pollfds_中remove channel 对应的fd，EventLoop::removeChannel(Channel* channel)只能在LoopThread中执行
void EventLoop::removeChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  /**当前loop对象正在 loop()函数中执行currentActiveChannel->handleEvent()函数 */
  if (eventHandling_)
  {
    //只能remove 不在activeChannels中的channel
    assert(currentActiveChannel_ == channel ||
        std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
  }
  //removeChannel 真正的实际操作
  poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  return poller_->hasChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " <<  CurrentThread::tid();
}

/**
 * 作用:从非IO线程唤醒loop IO Thread，并让loop->loop(...)调用doPedningFunctors()
 * 通过sockets::write(wakeupFd_, &one, sizeof one)对wakupFd_写入数据便可唤醒loop IO Thread;  why ???
 */
void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);
  if (n != sizeof one)
  {
    LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

/**
 *  EventLoop::handleRead()是wakeupFd_上readable事件的回调函数(该函数注册在wakeupChannel的readCallback_上)，
 *  它从wakeupFd_读出数据,
 */
void EventLoop::handleRead()
{
  uint64_t one = 1;
  ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
  if (n != sizeof one)
  {
    LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
  }
}

/**
 * 执行用户pendingFunctors_队列中的用户的回调函数
 */
void EventLoop::doPendingFunctors()
{
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;

  {
    MutexLockGuard lock(mutex_);
    //why swap（）？？？
    functors.swap(pendingFunctors_);
  }

  for (size_t i = 0; i < functors.size(); ++i)
  {
    functors[i]();
  }
  callingPendingFunctors_ = false;
}


void EventLoop::printActiveChannels() const
{
  for (ChannelList::const_iterator it = activeChannels_.begin();
      it != activeChannels_.end(); ++it)
  {
    const Channel* ch = *it;
    LOG_TRACE << "{" << ch->reventsToString() << "} ";
  }
}

