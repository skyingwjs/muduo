// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_THREADLOCALSINGLETON_H
#define MUDUO_BASE_THREADLOCALSINGLETON_H

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

namespace muduo
{

template<typename T>
class ThreadLocalSingleton : boost::noncopyable
{
 public:
  //获取一个线程内 T 类型的单例实例。 
  static T& instance()
  {
    if (!t_value_)
    {
      t_value_ = new T();//若不存在线程本地数据，则创建一个空的T对象。
      deleter_.set(t_value_);//设置线程本地存储数据对象
    }
    return *t_value_;
  }

  static T* pointer()
  {
    return t_value_;
  }

 private:
  //ThreadLocalSingleton只提供了static方法instance(),获取单例用的是static方法，没有实例化对象，所以就没有析构，就不需要在
  //析构函数中释放 T* ,即使使用了非static instance()方法(先实例化ThreadLocalSingleton)，如果在析构函数中释放了T*,那下次
  //获取的时候是会new的，那就不是单例了.
  ThreadLocalSingleton();
  ~ThreadLocalSingleton();

  //释放本地存储的数据
  static void destructor(void* obj)
  {
    assert(obj == t_value_);
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy; (void) dummy;
    delete t_value_;
    t_value_ = 0;
  }

  /**
   *Deleter类的作用相当于ThreadLocal 类的作用。 
   */
  class Deleter
  {
   public:
    Deleter()
    {
      pthread_key_create(&pkey_, &ThreadLocalSingleton::destructor);
    }

    ~Deleter()
    {
      pthread_key_delete(pkey_);
    }

    //设置thread specific data
    void set(T* newObj)
    {
      assert(pthread_getspecific(pkey_) == NULL);
      pthread_setspecific(pkey_, newObj);
    }

    pthread_key_t pkey_;
  };

  static __thread T* t_value_;//单例 线程的生命周期内都只有一个T*,需要static， thread 私有的
  static Deleter deleter_;//整个进程只需要一个pthread_key_t (一级索引)，所以静态。某个线程退出时，就调用ThreadLocalSingleton::destructior
  //析构该线程特定数据T*, TSD并不是要进程退出了在析构deleter_时才一次性析构TSD， 析构deleter_调用的pthread_key_delete(pkey_)只是删除key
  
};

/*初始化ThreadLocalSingleton<T>的数据成员 t_value_  和 deleter_ */
template<typename T>
__thread T* ThreadLocalSingleton<T>::t_value_ = 0;

template<typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

}
#endif
