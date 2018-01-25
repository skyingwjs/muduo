#ifndef MUDUO_BASE_LOGFILE_H
#define MUDUO_BASE_LOGFILE_H

#include <muduo/base/Mutex.h>
#include <muduo/base/Types.h>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{

namespace FileUtil
{
class AppendFile;
}

class LogFile : boost::noncopyable
{
 public:
  LogFile(const string& basename,
          size_t rollSize,
          bool threadSafe = true,
          int flushInterval = 3,
          int checkEveryN = 1024 );
  ~LogFile();

  void append(const char* logline, int len);//不加锁的append方式
  void flush();
  bool rollFile();

 private:
  void append_unlocked(const char* logline, int len);

  static string getLogFileName(const string& basename, time_t* now);//获取日志文件的名称

  const string basename_;//日志文件basename
  const size_t rollSize_;//日志文件的大小达到rollSize则生成一个新的日志文件
  const int flushInterval_;//日志写入间隔时间
  const int checkEveryN_;

  int count_;//计数器 检测是否需要更新新文件

  boost::scoped_ptr<MutexLock> mutex_;
  time_t startOfPeriod_;//开始记录日志时间(调整至零点时间，UTC时间，用来比较同一天 跟日志文件名无关)
  time_t lastRoll_;//上一滚动日志文件时间
  time_t lastFlush_;//上一次写入日志文件时间
  boost::scoped_ptr<FileUtil::AppendFile> file_;//文件智能指针

  const static int kRollPerSeconds_ = 60*60*24;//时间间隔为一天
};

}
#endif  // MUDUO_BASE_LOGFILE_H
