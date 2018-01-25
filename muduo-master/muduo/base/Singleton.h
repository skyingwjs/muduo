// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <stdlib.h> // atexit
#include <pthread.h>

namespace muduo
{

namespace detail
{
// This doesn't detect inherited member functions!
// http://stackoverflow.com/questions/1966362/sfinae-to-check-for-inherited-member-functions
template<typename T>
struct has_no_destroy
{
#ifdef __GXX_EXPERIMENTAL_CXX0X__
  template <typename C> static char test(decltype(&C::no_destroy));
#else
  template <typename C> static char test(typeof(&C::no_destroy));
#endif
  template <typename C> static int32_t test(...);
  const static bool value = sizeof(test<T>(0)) == 1;//判断如果是类的话，是否有destroy方法。
};
}

template<typename T>
class Singleton : boost::noncopyable
{
 public:
  static T& instance()
  {
    //第一次调用会在init函数内部创建T对象，pthread_once 保证该函数只被调用一次
    //pthread_once 保证线程安全，效率高于mutex
    pthread_once(&ponce_, &Singleton::init);
    assert(value_ != NULL);
    //利用pthread_once只构造一次对象
    return *value_;
  }

 private:
  Singleton();
  ~Singleton();

  static void init()
  {
    value_ = new T();
    //当模板参数是类，且没有no_destroy方法时才会注册atexit的destroy
    if (!detail::has_no_destroy<T>::value)
    {
      ::atexit(destroy);
    }
  }

  //程序正常结束后，自动调用该函数销毁，防止内存泄露
  static void destroy()
  {
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy; (void) dummy;

    delete value_;
    value_ = NULL;
  }

 private:
  static pthread_once_t ponce_;
  static T*             value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

}
#endif

