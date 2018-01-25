#include <muduo/base/BoundedBlockingQueue.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
using namespace muduo;
using namespace boost;
using namespace std;

void Produce(shared_ptr<BoundedBlockingQueue<int> > queue){

    while(true){
        int product=rand()%1000+1;
        std::cout<<"Produce: "<<product<<std::endl;
        queue->put(product);
        sleep(rand()%5);
    }
} 

void Consume(shared_ptr<BoundedBlockingQueue<int> > queue){

    while(true){
        int product =queue->take();
        std::cout<<"Consume :"<<product<<std::endl;
        sleep(rand()%5);
    }
}

int main(){
    shared_ptr<BoundedBlockingQueue<int> > boundedBlockingQueue(new BoundedBlockingQueue<int>(5));
    Thread t1(boost::bind(Produce,boundedBlockingQueue));
    Thread t2(boost::bind(Consume,boundedBlockingQueue));   

    t1.start();
    t2.start();

    t1.join();
    t2.join();

    return 0;         
}