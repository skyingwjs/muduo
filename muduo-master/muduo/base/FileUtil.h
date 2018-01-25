// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef MUDUO_BASE_FILEUTIL_H
#define MUDUO_BASE_FILEUTIL_H

#include <muduo/base/StringPiece.h>
#include <boost/noncopyable.hpp>

namespace muduo
{

namespace FileUtil
{

// read small file < 64KB
class ReadSmallFile : boost::noncopyable
{
 public:
  ReadSmallFile(StringArg filename);
  ~ReadSmallFile();

  // return errno
  // 把整个文件读取出来放在字符串content中
  template<typename String>
  int readToString(int maxSize,
                   String* content,
                   int64_t* fileSize,
                   int64_t* modifyTime,
                   int64_t* createTime);

  /// Read at maxium kBufferSize into buf_
  // return errno
  int readToBuffer(int* size);
  //获得已读取的数据 64KB
  const char* buffer() const { return buf_; }

  static const int kBufferSize = 64*1024;

 private:
  int fd_;
  int err_;
  char buf_[kBufferSize];//64KB
};



// read the file content, returns errno if error happens.
//读取文件 是对ReadSmallFile的一个包装
template<typename String>
int readFile(StringArg filename,
             int maxSize,
             String* content,
             int64_t* fileSize = NULL,
             int64_t* modifyTime = NULL,
             int64_t* createTime = NULL)
{
  ReadSmallFile file(filename);
  return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}



// not thread safe   作为LogFile的数据成员
// 打开文件用于向文件中添加数据
// 日志信息的所有的IO操作最底层都是由AppendFile类完成的
class AppendFile : boost::noncopyable
{
 public:
  explicit AppendFile(StringArg filename);

  ~AppendFile();

  //LogFile直接把日志内容作为参数，调用该append函数，将日志信息写入文件
  void append(const char* logline, const size_t len);
  
  //刷新数据到文件（磁盘）
  void flush();

  size_t writtenBytes() const { return writtenBytes_; }

 private:

  size_t write(const char* logline, size_t len);

  FILE* fp_;//原始文件指针
  char buffer_[64*1024];//64kB
  size_t writtenBytes_;
};

}

}

#endif  // MUDUO_BASE_FILEUTIL_H

