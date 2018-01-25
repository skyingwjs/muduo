#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

void * printPid(void *param){
    printf("child getpid()=%d ,syscall(__NR_gettid)=%ld ,pthread_self()=%ld \n",
                  getpid(),(long int)syscall(__NR_gettid),pthread_self());
    while(1){
        sleep(5);
    }
}

int main(){
    printf("main getpid()=%d ,syscall(__NR_gettid)=%ld ,pthread_self()=%ld \n",
                  getpid(),(long int)syscall(__NR_gettid),pthread_self());
    pthread_t tid=0;
    int ret=pthread_create(&tid,NULL,printPid,NULL);
    if(ret==0){
        printf("child tid= %ld\n",tid);
    }else{
        printf("create printPid thread failed!");
    }              
    while(1){
        sleep(5);
    }
    return 0; 
}