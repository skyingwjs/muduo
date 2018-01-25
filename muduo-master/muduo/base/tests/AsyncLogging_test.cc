#include <muduo/base/AsyncLogging.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

int kRollSize = 500*1000*1000;

muduo::AsyncLogging* g_asyncLog = NULL;

//通过LOG_*宏获取的日志信息，最终会传入到此函数，缓存到g_asyncLog对象的buffer中，
//然后通过g_asyncLog的后台线程输出到日志文件中
void asyncOutput(const char* msg, int len)
{
  //Logger输出重定向到AsyncLogging对象的append函数
  g_asyncLog->append(msg, len);
}

//异步日志性能测试
void bench(bool longLog)
{
  muduo::Logger::setOutput(asyncOutput);

  int cnt = 0;
  const int kBatch = 1000;
  muduo::string empty = " ";
  muduo::string longStr(3000, 'X');
  longStr += " ";

  for (int t = 0; t < 30; ++t)
  {
    muduo::Timestamp start = muduo::Timestamp::now();
    for (int i = 0; i < kBatch; ++i)
    {
      //前台线程（调用LOG_*的线程）会将日志信息缓存到AsyncLogging对象的buffer中
      LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
               << (longLog ? longStr : empty)
               << cnt;
      ++cnt;
    }
    muduo::Timestamp end = muduo::Timestamp::now();
    printf("%f\n", timeDifference(end, start)*1000000/kBatch);
    struct timespec ts = { 0, 500*1000*1000 };
    nanosleep(&ts, NULL);
  }
}

int main(int argc, char* argv[])
{
  {
    // set max virtual memory to 2GB.
    size_t kOneGB = 1000*1024*1024;
    rlimit rl = { 2*kOneGB, 2*kOneGB };
    setrlimit(RLIMIT_AS, &rl);
  }

  printf("pid = %d\n", getpid());

  char name[256];
  strncpy(name, argv[0], 256);
  muduo::AsyncLogging log(::basename(name), kRollSize);
  log.start();//启动log的线程thread_

  g_asyncLog = &log;

  bool longLog = argc > 1;

  //性能测试
  bench(longLog);
}
