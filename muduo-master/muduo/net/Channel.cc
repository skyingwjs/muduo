// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/base/Logging.h>
#include <muduo/net/Channel.h>
#include <muduo/net/EventLoop.h>

#include <sstream>

#include <poll.h>

using namespace muduo;
using namespace muduo::net;

//Channel 静态数据成员初始化 
const int Channel::kNoneEvent = 0;//不关注任何事件
const int Channel::kReadEvent = POLLIN | POLLPRI;//channel 关注的读事件 普通或优先级带数据可读，高优先级数据可读
const int Channel::kWriteEvent = POLLOUT;//普通数据可写



Channel::Channel(EventLoop* loop, int fd__)
  : loop_(loop),
    fd_(fd__),
    events_(0),//关注的事件 bit pattern
    revents_(0),//已经发生的事件 bit pattern
    index_(-1),//channel->fd 在poller->pollfds_中的索引
    logHup_(true),//?????
    tied_(false),//????
    eventHandling_(false),//true 表示正在执行handleEvent()函数
    addedToLoop_(false)//true 表示已加入IO thread,即已加入到poller_的channels成员中
{

}

/**
 * 只有在没有执行handleEvent(), 且loop_已不再拥有当前channel对象（this）时才能析构channel对象
 * ~Channel()必须满足一定条件后(确保在poller中执行了removeChannel())才能析构Channel对象
 */
Channel::~Channel()
{
  assert(!eventHandling_);
  assert(!addedToLoop_);
  if (loop_->isInLoopThread())
  {
    //确保已经poller_->removeChannel()已经调用
    assert(!loop_->hasChannel(this));
  }
}

void Channel::tie(const boost::shared_ptr<void>& obj)
{
  tie_ = obj;
  tied_ = true;
}

/**
 * 最终会调用poller_->updateChannel()，更新（增加或修改已存在的channel）poller的pollfds_
 * 成员中fd所关注的事件，以及poller中channels （fd与channel *对应关系）
 */
void Channel::update()
{
  addedToLoop_ = true;
  loop_->updateChannel(this);
}

/**
 * loop_->removeChannel(this)会转调用poller_->removeChannel(Channel *channel)
 * 将channel （this）从poller_->channels，并将channel对应的fd从poll->pollfds_中删除。
 * Channel::remove()函数一定在 ～Channel()之前被调用。poller->removeChannel(channel)是
 * Channel对象析构前最后一个被调用的最小单位函数
 */
void Channel::remove()
{
  //只有channel对应的fd上不在关注任何events时，才能removeChannel
  assert(isNoneEvent());
  addedToLoop_ = false;
  loop_->removeChannel(this);
}

/**
 * channel 核心函数，根据poller_->poll(...)中返回的activeChannels，调用相应已发生事件的回调函数
 * 在IO thread 中的loop_->loop(...)函数中被调用，currentActiveChannel->handleEvent()
 */
void Channel::handleEvent(Timestamp receiveTime)
{
  boost::shared_ptr<void> guard;
  if (tied_)//？？？
  {
    guard = tie_.lock();
    if (guard)
    {
      handleEventWithGuard(receiveTime);
    }
  }
  else
  {
    handleEventWithGuard(receiveTime);
  }
}

/**
 * 核心！！  各回调函数 **Callback的调用
 * POLLHUP:发生挂起
 * POLLIN:普通或优先级带数据可读
 * POLLNVAL:描述字不是一个打开的文件
 * POLLERR：发生错误
 * POLLPRI:高优先级数据可读
 * POLLOUT:普通数据可写
 */
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
  eventHandling_ = true;//表示正在执行handleEvent（）
  LOG_TRACE << reventsToString();
  
  if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
  {
    if (logHup_)
    {
      LOG_WARN << "fd = " << fd_ << " Channel::handle_event() POLLHUP";
    }
    if (closeCallback_) closeCallback_();
  }

  if (revents_ & POLLNVAL)
  {
    LOG_WARN << "fd = " << fd_ << " Channel::handle_event() POLLNVAL";
  }

  if (revents_ & (POLLERR | POLLNVAL))
  {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
  {
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revents_ & POLLOUT)
  {
    if (writeCallback_) writeCallback_();
  }

  eventHandling_ = false;
}


string Channel::reventsToString() const
{
  return eventsToString(fd_, revents_);
}

string Channel::eventsToString() const
{
  return eventsToString(fd_, events_);
}

string Channel::eventsToString(int fd, int ev)
{
  std::ostringstream oss;
  oss << fd << ": ";
  if (ev & POLLIN)
    oss << "IN ";
  if (ev & POLLPRI)
    oss << "PRI ";
  if (ev & POLLOUT)
    oss << "OUT ";
  if (ev & POLLHUP)
    oss << "HUP ";
  if (ev & POLLRDHUP)
    oss << "RDHUP ";
  if (ev & POLLERR)
    oss << "ERR ";
  if (ev & POLLNVAL)
    oss << "NVAL ";

  return oss.str().c_str();
}
