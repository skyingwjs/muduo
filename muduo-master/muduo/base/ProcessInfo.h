// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_BASE_PROCESSINFO_H
#define MUDUO_BASE_PROCESSINFO_H

#include <muduo/base/StringPiece.h>
#include <muduo/base/Types.h>
#include <muduo/base/Timestamp.h>
#include <vector>
#include <sys/types.h>

namespace muduo
{

namespace ProcessInfo
{
  pid_t pid();//取得进程id
  string pidString();//进程id的字符串
  uid_t uid();//用户id
  string username();//用户名
  uid_t euid();//有效的用户id
  Timestamp startTime();//进程开始时间
  int clockTicksPerSecond();//每秒钟的时钟数
  int pageSize();//返回一页大小
  bool isDebugBuild();  // constexpr 是否使用调试模式构建

  string hostname();//主机名
  string procname();//读取自己的进程名称
  StringPiece procname(const string& stat);

  /// read /proc/self/status
  string procStatus();//读取进程状态

  /// read /proc/self/stat
  string procStat();//进程的文件状态

  /// read /proc/self/task/tid/stat
  string threadStat();

  /// readlink /proc/self/exe
  string exePath();//执行路径

  int openedFiles();//已打开的文件数
  int maxOpenFiles();//最大可打开的文件数

  struct CpuTime//cpu时间
  {
    double userSeconds;//用户时间
    double systemSeconds;//系统时间

    CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }
  };
  CpuTime cpuTime();

  int numThreads();//线程数量
  std::vector<pid_t> threads();//线程队列
}

}

#endif  // MUDUO_BASE_PROCESSINFO_H
