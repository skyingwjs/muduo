#include <muduo/net/EventLoop.h>

#include <iostream>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

class Printer : boost::noncopyable
{
 public:
  Printer(muduo::net::EventLoop* loop)
    : loop_(loop),
      count_(0)
  {
    // Note: loop.runEvery() is better for this use case.
    //在loop_->runAfter()及其调用的其它函数中已经完成了timer的构造，callback的设置，POLLIN | POLLPRI事件的注册等所有的工作
    loop_->runAfter(2, boost::bind(&Printer::print, this));
  }

  ~Printer()
  {
    std::cout << "Final count is " << count_ << "\n";
  }

  void print()
  {
    if (count_ < 10)
    {
      std::cout << count_ << "\n";
      ++count_;

      loop_->runAfter(2, boost::bind(&Printer::print, this));
    }
    else
    {
      loop_->quit();
    }
  }

private:
  muduo::net::EventLoop* loop_;
  int count_;
};

int main()
{
  muduo::net::EventLoop loop;
  Printer printer(&loop);
  loop.loop();
}

