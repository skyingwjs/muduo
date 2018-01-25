#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h>

#include <unistd.h>

boost::scoped_ptr<muduo::LogFile> g_logFile;

void outputFunc(const char* msg, int len)
{
  g_logFile->append(msg, len);
}

void flushFunc()
{
  g_logFile->flush();
}

int main(int argc, char* argv[])
{
  char name[256];
  strncpy(name, argv[0], 256);
  //滚动日志肯定是输出到文件  basename即为程序名
  g_logFile.reset(new muduo::LogFile(::basename(name), 200*1000));

  muduo::Logger::setOutput(outputFunc);
  muduo::Logger::setFlush(flushFunc);

  muduo::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

  for (int i = 0; i < 10000; ++i)
  {
    //输出日志
    LOG_INFO << line << i;
    usleep(1000);
  }

}
