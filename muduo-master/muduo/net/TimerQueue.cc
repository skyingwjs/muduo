// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <muduo/net/TimerQueue.h>

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Timer.h>
#include <muduo/net/TimerId.h>

#include <boost/bind.hpp>

#include <sys/timerfd.h>
#include <unistd.h>

namespace muduo
{
namespace net
{
namespace detail
{

/**
 * 创建Timer 用到了timerfd_create（）
 */
int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0)
  {
    LOG_SYSFATAL << "Failed in timerfd_create";
  }
  return timerfd;
}

/**
 * 计算超时时刻when与当前时间now的时间差，返回timespec （××秒+××纳秒）
 * 
 */
struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
                         - Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 100)//精度不够，所以<100 都设置为100
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}
/**
 * 处理超时事件，超时后，timerfd变得可读
 */
void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;//howmany 为超时次数
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
  if (n != sizeof howmany)
  {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}

/**
 * 重置定时器的超时时间，用到了timerfd_settime（）
 */
void resetTimerfd(int timerfd, Timestamp expiration)
{
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret)
  {
    LOG_SYSERR << "timerfd_settime()";
  }
}

}
}
}

using namespace muduo;
using namespace muduo::net;
using namespace muduo::net::detail;

/**
 * 在EventLoop中含有数据成员timerQueue，在loop对象构造的时候，timerQueue也被构造；
 * TimerQueue(EventLoop* loop)主要设置了timerfdChannel_的readCallback_并在timerfdChannel_上注册kReadEvent
 * 事件，timerfdChannel_中的fd在定时器超时的那一刻变得readable，即发生kReadEvent事件；
 */
TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),//createTimerfd 将时间变成了一个文件fd，该fd在timer超时的那一刻变得readable
    timerfdChannel_(loop, timerfd_),//利用timerfd_创建timerfdChannel_
    timers_(),//timers list
    callingExpiredTimers_(false)
{
  timerfdChannel_.setReadCallback(
      boost::bind(&TimerQueue::handleRead, this));
  // we are always reading the timerfd, we disarm it with timerfd_settime.
  timerfdChannel_.enableReading();
}



TimerQueue::~TimerQueue()
{
  timerfdChannel_.disableAll();
  timerfdChannel_.remove();
  ::close(timerfd_);
  // do not remove channel, since we're in EventLoop::dtor();
  for (TimerList::iterator it = timers_.begin();
      it != timers_.end(); ++it)
  {
    delete it->second;//手动释放Timer*
  }
}




/**
 * TimerQueue::addTimer(...) 在EventLoop的runAt()  runAfter() runEvery()函数中被调用
 * 用于向timeQueue中添加timer；
 * 
 */
TimerId TimerQueue::addTimer(const TimerCallback& cb,
                             Timestamp when,
                             double interval)
{
  //创建新的timer，timer中指定了在何时调用Callback 
  Timer* timer = new Timer(cb, when, interval);
  //addTimerInLoop 实际addTimer的函数....
  loop_->runInLoop(
      boost::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer, timer->sequence());
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
TimerId TimerQueue::addTimer(TimerCallback&& cb,
                             Timestamp when,
                             double interval)
{
  Timer* timer = new Timer(std::move(cb), when, interval);
  loop_->runInLoop(
      boost::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer, timer->sequence());
}
#endif

void TimerQueue::cancel(TimerId timerId)
{
  loop_->runInLoop(
      boost::bind(&TimerQueue::cancelInLoop, this, timerId));
}

/**
 * 向timers_中添加timer
 */
void TimerQueue::addTimerInLoop(Timer* timer)
{
  loop_->assertInLoopThread();
  //INSERT一个timer 可能使得最早期的timer发生改变
  bool earliestChanged = insert(timer);

  if (earliestChanged)
  {
    //重置定时器的超时时刻
    resetTimerfd(timerfd_, timer->expiration());
  }
}

/**
 * 插入一个timer
 */
bool TimerQueue::insert(Timer* timer)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  bool earliestChanged = false;
  Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  if (it == timers_.end() || when < it->first)
  {
    earliestChanged = true;
  }
  {
    //插入到timers_中
    std::pair<TimerList::iterator, bool> result
      = timers_.insert(Entry(when, timer));
    assert(result.second); (void)result;
  }
  {
    //插入到activeTimers_中
    std::pair<ActiveTimerSet::iterator, bool> result
      = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second); (void)result;
  }

  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}


/**
 * 取消timer实际调用了cancelInLoop(TimerId timerId)
 */
void TimerQueue::cancelInLoop(TimerId timerId)
{
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  //要取消的定时器timer
  ActiveTimer timer(timerId.timer_, timerId.sequence_);
  //查找该定时器
  ActiveTimerSet::iterator it = activeTimers_.find(timer);

  //要取消的timer在当前激活的activeTimers中
  if (it != activeTimers_.end())
  {
    size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
    assert(n == 1); (void)n;
    delete it->first; // FIXME: no delete please  如果使用了unique_ptr这里就不需要手动删除了
    activeTimers_.erase(it);//从activeTimers中取消
  }
  //如果正在执行超时定时器的回调函数，则加入到cannelingTimers中
  else if (callingExpiredTimers_)
  {
    cancelingTimers_.insert(timer);
  }
  assert(timers_.size() == activeTimers_.size());
}


/**
 * 有timer 到期时，调用该函数
 */
void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();
  Timestamp now(Timestamp::now());
  //读取timerfd_
  readTimerfd(timerfd_, now);
  //获取该时刻前所有的定时器列表，即超时的定时器列表；timerfd_关联着多个定时器的触发
  std::vector<Entry> expired = getExpired(now);

  callingExpiredTimers_ = true;
  cancelingTimers_.clear();
  // safe to callback outside critical section
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    //回调定时器处理函数
    it->second->run();
  }
  callingExpiredTimers_ = false;
  //把重复的定时器重新加入到定时器中？？？
  reset(expired, now);
}

/**
 * 获取已到期的timer并以vector形式返回...
 */
std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
  assert(timers_.size() == activeTimers_.size());
  std::vector<Entry> expired;
  //UINTPTR_MAX 表示最大的地址
  Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  //返回第一个未到期的timer的迭代器
  TimerList::iterator end = timers_.lower_bound(sentry);
  assert(end == timers_.end() || now < end->first);
  //[begin,end)之间的元素(到期的timer)追加到expired的尾部
  std::copy(timers_.begin(), end, back_inserter(expired));
  //从timers_中移除到期的定时器
  timers_.erase(timers_.begin(), end);
  //从activeTimers中移除到期的timer
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    size_t n = activeTimers_.erase(timer);
    assert(n == 1); (void)n;
  }

  assert(timers_.size() == activeTimers_.size());
  return expired;
}



void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
  Timestamp nextExpire;

  for (std::vector<Entry>::const_iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    
    ActiveTimer timer(it->second, it->second->sequence());
    //如果是重复的定时器并且不在cannelingTimers中，则重启该定时器
    if (it->second->repeat()
        && cancelingTimers_.find(timer) == cancelingTimers_.end())
    {
      it->second->restart(now);
      insert(it->second);
    }
    else
    {
      // FIXME move to a free list
      //一次性定时器或者已被取消的定时器是不能重置的，则应该删除该定时器
      delete it->second; // FIXME: no delete please
    }
  }

  if (!timers_.empty())
  {
    //获取最早到期的定时器超时时间
    nextExpire = timers_.begin()->second->expiration();
  }

  if (nextExpire.valid())
  {
    //重置定时器的超时时刻
    resetTimerfd(timerfd_, nextExpire);
  }
}

