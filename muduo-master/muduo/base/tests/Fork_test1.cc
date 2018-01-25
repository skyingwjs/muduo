#include <unistd.h>
#include <stdio.h>

int main(void){
    int i=0;
    
    for(i=0;i<3;i++){
        pid_t fpid=fork();
        if(fpid==0){
            printf("son\n");
        }else{
            printf("father\n");
        }
    }
    return 0;
}


/*
int main(void){
    int i=0;
    printf("i son/pa  ppid  pid  fpid\n");
    for(i=0;i<2;i++){
        pid_t fpid=fork();
        if(fpid==0){
            printf("%d  child  %4d  %4d  %4d\n",i,getppid(),getpid(),fpid);
        }else{
            printf("%d  parent  %4d  %4d  %4d\n",i,getppid(),getpid(),fpid);
        }
    }
    return 0;
}
*/



/*
int main(){
    pid_t fpid;
    int count=0;
    fpid=fork();
    if(fpid<0){
        printf("error i fork()");
    }else if(fpid==0){
        printf("I am the child process, my process id is %d\n",getpid());
        count++;
    }else{
        printf("I am the parent proces, my process id is %d\n",getpid());
        count++;
    }
    printf("count=: %d\n",count);
    return 0;
}
*/