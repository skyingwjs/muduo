// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

#include <stdint.h>

namespace muduo
{
  /**
   * namespace CurrentThread 中定义了获取部分线程属性的全局函数
   */  
  namespace CurrentThread
  {
    // internal
    extern __thread int t_cachedTid;//线程真实id
    extern __thread char t_tidString[32];//线程真实id的字符串形式
    extern __thread int t_tidStringLength;//线程真实id字符串的长度
    extern __thread const char* t_threadName;//线程的name
    
    void cacheTid();//在thread.cc中实现 缓存thread真实id

    inline int tid() //get current thread id
    {
      if (__builtin_expect(t_cachedTid == 0, 0))
      {
        cacheTid();
      }
      return t_cachedTid;
    }

    inline const char* tidString() // for logging
    {
      return t_tidString;
    }

    inline int tidStringLength() // for logging
    {
      return t_tidStringLength;
    }

    inline const char* name()
    {
      return t_threadName;
    }

    /**
     * 在thread.cc 中实现
     */
    bool isMainThread();

    void sleepUsec(int64_t usec);
  }
}

#endif
