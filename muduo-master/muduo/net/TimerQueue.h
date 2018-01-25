// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is an internal header file, you should not include this.

#ifndef MUDUO_NET_TIMERQUEUE_H
#define MUDUO_NET_TIMERQUEUE_H

#include <set>
#include <vector>

#include <boost/noncopyable.hpp>

#include <muduo/base/Mutex.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/Channel.h>

namespace muduo
{
namespace net
{

class EventLoop;
class Timer;
class TimerId;

///
/// A best efforts timer queue.
/// No guarantee that the callback will be on time.
///
class TimerQueue : boost::noncopyable
{
 public:
  explicit TimerQueue(EventLoop* loop);
  ~TimerQueue();

  ///
  /// Schedules the callback to be run at given time,
  /// repeats if @c interval > 0.0.
  ///
  /// Must be thread safe. Usually be called from other threads.
  //一定是线程安全的，可以跨线程调用。通常情况下被其他线程调用

  TimerId addTimer(const TimerCallback& cb,
                   Timestamp when,
                   double interval);

#ifdef __GXX_EXPERIMENTAL_CXX0X__
  TimerId addTimer(TimerCallback&& cb,
                   Timestamp when,
                   double interval);
#endif

  void cancel(TimerId timerId);

 private:

  // FIXME: use unique_ptr<Timer> instead of raw pointers.
  // This requires heterogeneous comparison lookup (N3465) from C++14
  // so that we can find an T* in a set<unique_ptr<T>>.
  /**
   * unique_ptr是c++11标准的一个独享所有权的智能指针，无法得到指向同一对象的两个unique_ptr,但可以
   * 进行移动构造和移动赋值操作，即所有权转移到另一个对象
   * 两种类型的set，一种按时间戳排序，一种那timer 的地址排序，实际上这两个set保存的是相同的定时器列表
   */
  typedef std::pair<Timestamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;
  typedef std::pair<Timer*, int64_t> ActiveTimer;
  typedef std::set<ActiveTimer> ActiveTimerSet;
  
  /**
   * 以下成员函数只可能在其所属的IO线程中调用，因而不必加锁，服务器性能杀手之一是锁竞争，尽可能少用锁
   */
  void addTimerInLoop(Timer* timer);
  void cancelInLoop(TimerId timerId);
  // called when timerfd alarms
  void handleRead();

  // move out all expired timers
  //返回超时的定时器列表
  std::vector<Entry> getExpired(Timestamp now);
  void reset(const std::vector<Entry>& expired, Timestamp now);

  bool insert(Timer* timer);

  EventLoop* loop_;
  //timerfd_和timerfdChannel_使得EventLoop可以像处理TCP IO事件一样处理定时事件
  const int timerfd_; 
  Channel timerfdChannel_;
  // Timer list sorted by expiration 按到期时间排序
  TimerList timers_;

  // for cancel()
  //timers与activeTimers_保存的是相同的数据，activeTimers_是按timer对象的地址排序
  ActiveTimerSet activeTimers_;
  bool callingExpiredTimers_; /* atomic   */ //是否正在处理超时事件
  ActiveTimerSet cancelingTimers_;//保存的是被取消的定时器
};

}
}
#endif  // MUDUO_NET_TIMERQUEUE_H
