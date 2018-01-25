#include <muduo/base/CountDownLatch.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Thread.h>
#include <boost/bind.hpp>

#include <stdio.h>

using namespace muduo;
CountDownLatch latch_(1);

void function(){
    latch_.wait();
    printf("Thread ID=%d ，Name= %s\n",CurrentThread::tid(),CurrentThread::name());
}

int main(){
    Thread t1(boost::bind(function),"thread 1");
    Thread t2(boost::bind(function),"thread 2");
    t1.start();
    t2.start();

    printf("main thread running before countDown\n");
    latch_.countDown();
    sleep(3);
    printf("main thread running after countDown\n");
    
    t1.join();
    t2.join();
    return 0;
}