#include <muduo/base/Mutex.h>
#include <muduo/base/Condition.h>
#include <muduo/base/Thread.h>
#include <boost/bind.hpp>

#include <stdio.h>

using namespace muduo;

struct msg{
    struct msg*next;
    int num;
};

struct msg *head=NULL;

MutexLock mutex;
Condition cond(mutex);

void consumer(){
    struct msg *mp;
    for(;;){
        {
            MutexLockGuard lock(mutex);
            while(head==NULL)//无货的话 等待生产者生产
                cond.wait();
            mp=head;
            head=mp->next;  
        }
        printf("Consume %d\n",mp->num);
        free(mp);
        sleep(rand()%5);
    }
}


void producer(){
    struct msg *mp;
    for(;;){
        mp=static_cast<msg *>(malloc(sizeof(struct msg)));
        mp->num=rand()%1000+1;
        printf("Produce %d\n",mp->num);
        {
            MutexLockGuard lock(mutex);
            mp->next=head;
            head=mp;
        }
        cond.notify();
        sleep(rand()%5);
    }
}

int main(){
    Thread t1(boost::bind(producer),"Producer");
    Thread t2(boost::bind(consumer),"Consumer");
    t1.start();
    t2.start();
    t1.join();
    t2.join();
    return 0;
}