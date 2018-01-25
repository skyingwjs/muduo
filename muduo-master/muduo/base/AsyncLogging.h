#ifndef MUDUO_BASE_ASYNCLOGGING_H
#define MUDUO_BASE_ASYNCLOGGING_H

#include <muduo/base/BlockingQueue.h>
#include <muduo/base/BoundedBlockingQueue.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Thread.h>
#include <muduo/base/LogStream.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace muduo
{

class AsyncLogging : boost::noncopyable
{
 public:

  AsyncLogging(const string& basename,
               size_t rollSize,
               int flushInterval = 3);

  ~AsyncLogging()
  {
    if (running_)
    {
      stop();
    }
  }

  void append(const char* logline, int len);

  //启动线程 并执行线程函数
  void start()
  {
    running_ = true;
    thread_.start();
    latch_.wait();
  }

  //join线程 指直到threadFunc运行结束
  void stop()
  {
    running_ = false;
    //
    cond_.notify();
    thread_.join();
  }

 private:

  // declare but not define, prevent compiler-synthesized functions
  AsyncLogging(const AsyncLogging&);  // ptr_container
  void operator=(const AsyncLogging&);  // ptr_container

  void threadFunc();//thread_ 变量的 thread func

  typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;//4000000个固定字节的buffer
  typedef boost::ptr_vector<Buffer> BufferVector;//Buffer 指针类型的vector
  typedef BufferVector::auto_type BufferPtr;//Buffer 类型的指针

  const int flushInterval_;
  bool running_;

  string basename_;//basename_即为程序名
  size_t rollSize_;//滚动日志文件大小
  muduo::Thread thread_;//线程成员变量

  muduo::CountDownLatch latch_;
  muduo::MutexLock mutex_;
  muduo::Condition cond_;

  BufferPtr currentBuffer_;//Buffer ptr
  BufferPtr nextBuffer_;//Buffer ptr
  BufferVector buffers_;//Buffer ptr vector
};

}
#endif  // MUDUO_BASE_ASYNCLOGGING_H
