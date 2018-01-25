#include <muduo/base/LogFile.h>

#include <muduo/base/FileUtil.h>
#include <muduo/base/ProcessInfo.h>

#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace muduo;

LogFile::LogFile(const string& basename,
                 size_t rollSize,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryN)
  : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),//日志写入的间隔时间 默认是3s
    checkEveryN_(checkEveryN),
    count_(0),
    mutex_(threadSafe ? new MutexLock : NULL),//若果是线程安全的则构造一个互斥锁
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0)
{
  assert(basename.find('/') == string::npos);//断言basename不能找到‘/’
  rollFile();//滚动日志 第一次产生一个文件
}

LogFile::~LogFile()
{
}


/**
 * LogFile::append(...)一般作为Logger::setOutputFunc(...)的参数来设置g_output,
 * 将LogFile的buffer中的缓存的数据输出到日志文件中，并执行了日志回滚机制
 */
void LogFile::append(const char* logline, int len)
{
  if (mutex_)//线程安全的方式 写日志文件 效率低
  {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
  }
  else//效率高
  {
    append_unlocked(logline, len);
  }
}

void LogFile::flush()
{
  if (mutex_)
  {
    MutexLockGuard lock(*mutex_);
    file_->flush();
  }
  else
  {
    file_->flush();
  }
}

//日志滚动的条件有两种: 1.单个日志文件的大小达到了最大限制 2.到达指定的写日志时间  一般是每天零点
void LogFile::append_unlocked(const char* logline, int len)
{
  file_->append(logline, len);
  //写入的字节数 超过指定的最大字节数
  if (file_->writtenBytes() > rollSize_)
  {
    rollFile();//滚动日志
  }
  else
  {
    ++count_;
    if (count_ >= checkEveryN_)//否则 查看计数count是否超过checkEveryN_
    {
      count_ = 0;
      time_t now = ::time(NULL);
      time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;//将时间调整到当天的零点
      if (thisPeriod_ != startOfPeriod_)//如果不等则应该就是第二天的零点
      {
        rollFile();//滚动日志
      }
      else if (now - lastFlush_ > flushInterval_)//时间差大于日志写入的间隔时间
      {
        lastFlush_ = now;
        file_->flush();
      }
    }
  }
}

//滚动日志
bool LogFile::rollFile()
{
  time_t now = 0;
  //获取文件名 并返回时间
  string filename = getLogFileName(basename_, &now);
  //调整到零点
  time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

  if (now > lastRoll_)
  {
    lastRoll_ = now;
    lastFlush_ = now;
    startOfPeriod_ = start;
    file_.reset(new FileUtil::AppendFile(filename));//产生一个新的日志文件
    return true;
  }
  return false;
}

string LogFile::getLogFileName(const string& basename, time_t* now)
{
  string filename;
  filename.reserve(basename.size() + 64);
  filename = basename;

  char timebuf[32];
  struct tm tm;
  *now = time(NULL);
  gmtime_r(now, &tm); // FIXME: localtime_r ?
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
  filename += timebuf;

  filename += ProcessInfo::hostname();

  char pidbuf[32];
  snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
  filename += pidbuf;

  filename += ".log";

  return filename;
}

