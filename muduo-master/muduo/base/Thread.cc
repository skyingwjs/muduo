// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/base/Thread.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Exception.h>
#include <muduo/base/Logging.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/weak_ptr.hpp>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace muduo
{
  
  /**
   * 定义并初始化命名空间CurrentThread内的全局变量
   */
  namespace CurrentThread
  {
    __thread int t_cachedTid = 0;//__thread修饰的变量是线程局部存储的，线程不共享，线程安全
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "unknown";
    //？？？？
    const bool sameType = boost::is_same<int, pid_t>::value;//boost::is_name用来检测参数是不是相同的类型
    BOOST_STATIC_ASSERT(sameType);
  }//namespace CurrentThread


  /**
   * namespace detail中定义了一个class 和 一个struct
   * 
   * 1.class ThreadNameInitializer 为创建线程做准备，该类有一个全局实例，创建该实例时会
   * 先调用它的构造函数，在构造函数中为线程的创建做好环境的准备。其中用到了pthread_atfork(
   * NULL,NULL,&afterFrok);
   * 
   * 2.struct ThreadData,用来辅助调用线程执行的函数
   */
  namespace detail
  {

    /**
     * 获取thread的真实id 是一个int 整数，并非pthread_t类型
     * 在CurrentThread::cacheId()中被调用
     */
    pid_t gettid()
    {
      return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    //在调用fork()后的子进程中执行
    void afterFork()
    {
      muduo::CurrentThread::t_cachedTid = 0;//当前为0，先清零tid
      //为main，为什么要赋值为0和main，因为fork可能在主线程中调用，也可能在子线程中调用。fork得到一个新进程
      muduo::CurrentThread::t_threadName = "main";
      CurrentThread::tid();//再缓存tid，新进程只有一个执行序列，只有一个线程
      // no need to call pthread_atfork(NULL, NULL, &afterFork);
    }

    class ThreadNameInitializer
    {
      public:
        ThreadNameInitializer()
        {
          muduo::CurrentThread::t_threadName = "main";
          CurrentThread::tid();
          //调用fork()并在子进程中执行afterFork函数
          pthread_atfork(NULL, NULL, &afterFork);
        }
    };//end class ThreadNameInitializer

    ThreadNameInitializer init;//全局实例，这个对象的构造先于main()函数

    struct ThreadData
    {
      typedef muduo::Thread::ThreadFunc ThreadFunc;
      ThreadFunc func_;
      string name_;
      pid_t* tid_;
      CountDownLatch* latch_;

      ThreadData(const ThreadFunc& func,
                const string& name,
                pid_t* tid,
                CountDownLatch* latch)
        : func_(func),
          name_(name),
          tid_(tid),
          latch_(latch)
      { 

      }

      void runInThread()
      {
        *tid_ = muduo::CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;

        muduo::CurrentThread::t_threadName = name_.empty() ? "muduoThread" : name_.c_str();
        ::prctl(PR_SET_NAME, muduo::CurrentThread::t_threadName);
        try
        {
          func_();//运行线程函数
          muduo::CurrentThread::t_threadName = "finished";//执行完的线程name
        }
        catch (const Exception& ex)
        {
          muduo::CurrentThread::t_threadName = "crashed";
          fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
          fprintf(stderr, "reason: %s\n", ex.what());
          fprintf(stderr, "stack trace: %s\n", ex.stackTrace());//打印函数调用堆栈
          abort();
        }
        catch (const std::exception& ex)
        {
          muduo::CurrentThread::t_threadName = "crashed";
          fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
          fprintf(stderr, "reason: %s\n", ex.what());
          abort();
        }
        catch (...)
        {
          muduo::CurrentThread::t_threadName = "crashed";
          fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
          throw; // rethrow
        }
      }
    };//end struct ThreadData

    void* startThread(void* obj)//线程启动
    {
      ThreadData* data = static_cast<ThreadData*>(obj);
      data->runInThread();
      delete data;
      return NULL;
    }

  }//end namespace detail
}//end namespace muduo



/**
 * namespace CurrentThread内全局函数的实现
 */

using namespace muduo;

//通过系统调用获取线程真实id 并缓存current thread id
void CurrentThread::cacheTid()
{
  if (t_cachedTid == 0)
  {
    t_cachedTid = detail::gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

bool CurrentThread::isMainThread()
{
  //::getpid()获取当前进程id 并与当前线程id（通过tid()获得）比较
  return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
  struct timespec ts = { 0, 0 };
  ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
  ::nanosleep(&ts, NULL);
}



/**
 * Thread 类中 各函数的实现
 */

/**记录第几次创建线程实例 */
AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const string& n)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(func),
    name_(n),
    latch_(1)
{
  setDefaultName();
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
Thread::Thread(ThreadFunc&& func, const string& n)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(std::move(func)),
    name_(n),
    latch_(1)
{
  setDefaultName();
}

#endif

Thread::~Thread()
{
  if (started_ && !joined_)
  {
    pthread_detach(pthreadId_);
  }
}

void Thread::setDefaultName()
{
  int num = numCreated_.incrementAndGet();
  if (name_.empty())
  {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d", num);
    name_ = buf;
  }
}

void Thread::start()
{
  assert(!started_);
  started_ = true;
  // FIXME: move(func_)
  detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
  {
    started_ = false;
    delete data; // or no delete?
    LOG_SYSFATAL << "Failed in pthread_create";
  }
  else
  {
    latch_.wait();
    assert(tid_ > 0);
  }
}

int Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}

