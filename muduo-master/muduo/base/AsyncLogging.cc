#include <muduo/base/AsyncLogging.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Timestamp.h>

#include <stdio.h>

using namespace muduo;

AsyncLogging::AsyncLogging(const string& basename,
                           size_t rollSize,
                           int flushInterval)
  : flushInterval_(flushInterval),
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(boost::bind(&AsyncLogging::threadFunc, this), "Logging"),
    latch_(1),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_()
{
  currentBuffer_->bzero();
  nextBuffer_->bzero();
  buffers_.reserve(16);
}

//前台线程，所有LOG_* 最终都会调用该append函数，将日志信息缓存到AsyncLogging的buffer中 
//前台线程将currentBuffer_ 、nextBuffer_ 两个buffer 都用掉了。
void AsyncLogging::append(const char* logline, int len)
{
  //可能有多个线程同时调用LOG_*写入日志，所以需要同步
  muduo::MutexLockGuard lock(mutex_);
  //currentBuffer还足够大，可以装下一条日志内容
  if (currentBuffer_->avail() > len)
  {
    currentBuffer_->append(logline, len);
  }
  else//currentBuffer已满，要将之存放到buffers_中
  {
    buffers_.push_back(currentBuffer_.release());

    if (nextBuffer_)
    {
      currentBuffer_ = boost::ptr_container::move(nextBuffer_);
    }
    else
    {
      currentBuffer_.reset(new Buffer); // Rarely happens
    }
    currentBuffer_->append(logline, len);
    //通知后台线程，已经有一个满的buffer了，可以将其输出到日志文件了
    cond_.notify();
  }
}

/**
 * 在threadFunc中调用了LogFile对象的append(...)函数，该函数将日志信息写入到日志文件
 * 并实现日志滚动功能。该函数将日志记录的buffer从前台线程转移到后台线程，后台线程进行异步
 * IO。后台线程总会在flushInterval时间内（3s）醒来，并继续执行后续代码
 */
void AsyncLogging::threadFunc()
{
  assert(running_ == true);
  latch_.countDown();
  //定义一个直接进行IO(输出日志到文件)的LogFile对象 output.append()函数会完成日志输出
  LogFile output(basename_, rollSize_, false);
  //两个后台线程的buffer
  BufferPtr newBuffer1(new Buffer);
  BufferPtr newBuffer2(new Buffer);
  newBuffer1->bzero();
  newBuffer2->bzero();
  //用来和前台的buffers_进行swap
  BufferVector buffersToWrite;
  buffersToWrite.reserve(16);
  //循环，从AsyncLogging的对象的buffers_中取出已满的buffer将其中数据输出到日志文件
  while (running_)
  {
    assert(newBuffer1 && newBuffer1->length() == 0);
    assert(newBuffer2 && newBuffer2->length() == 0);
    assert(buffersToWrite.empty());

    {
      muduo::MutexLockGuard lock(mutex_);
      if (buffers_.empty())  // unusual usage!
      {
        //睡眠的时间是日志库flush的时间,当cond_的条件满足时，即前台线程已经将一个满的buffer放到buffers_中了
        //或者超时了，waitForSeconds会从阻塞中返回，继续执行后续代码。
        cond_.waitForSeconds(flushInterval_);
      }
      /**无论cond是因何醒来，都要将currentBuffer_放到buffers_中。
       * 如果是因为时间到而醒，那么currentBuffer_还未满，此时也要将其写入到LogFile中；
       * 如果已经有一个前台buffer满了，那么在前台线程中就已经把这个已满的buffer放到buffers_中了，
       * 此时还是需要把currentBuffer_放到buffers_中（注意前后放置的不同的buffer，因为在前台线程中
       * currentBuffer_已经被换成nextBuffer_指向的buffer中了）
       */
      buffers_.push_back(currentBuffer_.release());
      currentBuffer_ = boost::ptr_container::move(newBuffer1);//归还一个buffer
      buffersToWrite.swap(buffers_);//交换
      if (!nextBuffer_)
      {
        nextBuffer_ = boost::ptr_container::move(newBuffer2);//如果需要归还第二个buffer
      }
    }

    assert(!buffersToWrite.empty());

    if (buffersToWrite.size() > 25)
    {
      char buf[256];
      snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
               Timestamp::now().toFormattedString().c_str(),
               buffersToWrite.size()-2);
      fputs(buf, stderr);
      output.append(buf, static_cast<int>(strlen(buf)));
      buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
    }

    //将已经满了的buffer内容写入到LogFile进行IO操作。
    for (size_t i = 0; i < buffersToWrite.size(); ++i)
    {
      // FIXME: use unbuffered stdio FILE ? or use ::writev ?
      //append 函数进行磁盘IO,阻塞型IO
      output.append(buffersToWrite[i].data(), buffersToWrite[i].length());
    }

    if (buffersToWrite.size() > 2)
    {
      // drop non-bzero-ed buffers, avoid trashing
      buffersToWrite.resize(2);
    }

    //前台buffer是由newBuffer1,newBuffer2归还的。现在把buffersToWrite中的buffer归还给后台buffer（newBuffer1,newBuffer2）
    if (!newBuffer1)
    {
      assert(!buffersToWrite.empty());
      newBuffer1 = buffersToWrite.pop_back();
      newBuffer1->reset();
    }

    if (!newBuffer2)
    {
      assert(!buffersToWrite.empty());
      newBuffer2 = buffersToWrite.pop_back();
      newBuffer2->reset();
    }

    buffersToWrite.clear();
    output.flush();//flush 日志内容到磁盘
  }
  output.flush();
}

