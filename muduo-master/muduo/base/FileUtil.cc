// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//

#include <muduo/base/FileUtil.h>
#include <muduo/base/Logging.h> // strerror_tl

#include <boost/static_assert.hpp>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace muduo;

//传入文件名称(一般是创建的日志文件名称) 打开文件，文件指针保存到fp_
//打开文件 并设置文件读写的缓冲区为自己定义的缓冲区。
FileUtil::AppendFile::AppendFile(StringArg filename)
  : fp_(::fopen(filename.c_str(), "ae")),  // 'e' for O_CLOEXEC
    writtenBytes_(0)
{
  assert(fp_);//断言文件已打开
  ::setbuffer(fp_, buffer_, sizeof buffer_);
  // posix_fadvise POSIX_FADV_DONTNEED ?
}

//关闭文件
FileUtil::AppendFile::~AppendFile()
{
  ::fclose(fp_);//关闭fp_
}

//追加数据到缓冲区，知道调用flush才会把数据真正写入文件中(磁盘)，同时记录写入的字节数
void FileUtil::AppendFile::append(const char* logline, const size_t len)
{
  //写入数据
  size_t n = write(logline, len);//n： 实际写入的字节数
  size_t remain = len - n;//未写完的字节数
  //remain>0 则不断写入,直到remain==0，若是发错不能写入了，则输出相关错误信息
  while (remain > 0)//这个循环的作用是保证数据能被全部写入
  {
    size_t x = write(logline + n, remain);
    if (x == 0)
    {
      int err = ferror(fp_);
      if (err)
      {
        fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
      }
      break;
    }
    n += x;
    remain = len - n; // remain -= x
  }

  writtenBytes_ += len;//已写入文件的字节数
}

//数据刷新到文件中
void FileUtil::AppendFile::flush()
{
  ::fflush(fp_);
}

//不加锁写，非线程安全，效率高
size_t FileUtil::AppendFile::write(const char* logline, size_t len)
{
  // #undef fwrite_unlocked  unlocked方式写入，效率会高些
  return ::fwrite_unlocked(logline, 1, len, fp_);
}



//打开文件并获取文件描述符 
FileUtil::ReadSmallFile::ReadSmallFile(StringArg filename)
  : fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)),
    err_(0)
{
  buf_[0] = '\0';
  if (fd_ < 0)
  {
    err_ = errno;
  }
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
  if (fd_ >= 0)
  {
    ::close(fd_); // FIXME: check EINTR
  }
}

// return errno
// 读出来的数据放在content中
template<typename String>
int FileUtil::ReadSmallFile::readToString(int maxSize,
                                          String* content,
                                          int64_t* fileSize,
                                          int64_t* modifyTime,
                                          int64_t* createTime)
{
  BOOST_STATIC_ASSERT(sizeof(off_t) == 8);
  assert(content != NULL);
  int err = err_;

  if (fd_ >= 0)
  {
    content->clear();

    if (fileSize)
    {
      struct stat statbuf;
      //获取文件信息
      if (::fstat(fd_, &statbuf) == 0)
      {
        if (S_ISREG(statbuf.st_mode))
        {
          //获取文件大小
          *fileSize = statbuf.st_size;
          //为字符串分配内存
          content->reserve(static_cast<int>(std::min(implicit_cast<int64_t>(maxSize), *fileSize)));
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
          err = EISDIR;//是文件夹
        }
        if (modifyTime)
        {
          *modifyTime = statbuf.st_mtime;//修改时间
        }
        if (createTime)
        {
          *createTime = statbuf.st_ctime;//创建时间
        }
      }
      else
      {
        err = errno;
      }
    }
    //一直读直到达到最大内存限制
    while (content->size() < implicit_cast<size_t>(maxSize))
    {
      size_t toRead = std::min(implicit_cast<size_t>(maxSize) - content->size(), sizeof(buf_));
      ssize_t n = ::read(fd_, buf_, toRead);
      if (n > 0)
      {
        content->append(buf_, n);
      }
      else
      {
        if (n < 0)
        {
          err = errno;
        }
        break;
      }
    }
  }
  return err;
}

//把数据读出来，放在对象的buf_中
int FileUtil::ReadSmallFile::readToBuffer(int* size)
{
  int err = err_;
  if (fd_ >= 0)
  {
    ssize_t n = ::pread(fd_, buf_, sizeof(buf_)-1, 0);
    if (n >= 0)
    {
      if (size)
      {
        *size = static_cast<int>(n);
      }
      buf_[n] = '\0';//最后添加‘\0’
    }
    else
    {
      err = errno;
    }
  }
  return err;
}

template int FileUtil::readFile(StringArg filename,
                                int maxSize,
                                string* content,
                                int64_t*, int64_t*, int64_t*);

template int FileUtil::ReadSmallFile::readToString(
    int maxSize,
    string* content,
    int64_t*, int64_t*, int64_t*);

#ifndef MUDUO_STD_STRING
template int FileUtil::readFile(StringArg filename,
                                int maxSize,
                                std::string* content,
                                int64_t*, int64_t*, int64_t*);

template int FileUtil::ReadSmallFile::readToString(
    int maxSize,
    std::string* content,
    int64_t*, int64_t*, int64_t*);
#endif

