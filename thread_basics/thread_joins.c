#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<errno.h>



//callback fn prototype -> void* callback_fn(void *)

void* thread_fn_callback(void *arg){
    int th_id=*(int *) arg;

    free(arg);//deallocate heap memory,why keep it filled when variable is used
    
    int rc=0;
    while(rc!=th_id){
        printf("Thread %d is running.\n",th_id);
        sleep(1);
        rc++;
    }
    //for threads, always work on heap memory(while returning from function or passing parameters to functions)
    int *result=calloc(1,sizeof(int));
    *result=th_id*th_id;
    return (void *) result;
}


void thread_create(pthread_t *thread_handle,int thread_id){
    pthread_attr_t attr;//stores attributes of thread created
    
    //always pass static or heap memoery variables to threads(never pass a local variable)
    int *_th_id=calloc(1,sizeof(int));
    *_th_id=thread_id;

    //set state of thread
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE /*PTHREAD_CREATE_DETACHED*/);

    //create a new thread(line 22 -> fork point)
    pthread_create(thread_handle,&attr,thread_fn_callback,(void *)_th_id );
}

//create thread handles
pthread_t t2;
pthread_t t3;

int main(int argc,char **argv){
    
    thread_create(&t2,2);
    thread_create(&t3,10);

    printf("main function blocked on pthread join for thread with thread_id=2\n");
    void *thread_result2;
    pthread_join(t2,&thread_result2);//result of t2 thread are stored in second variable
    if(thread_result2){
        printf("returned result from thread 2 = %d\n",*(int *) thread_result2);
        free(thread_result2);
        thread_result2=NULL;
    }

    printf("main function blocked on pthread join for thread with thread_id=10\n");
    void *thread_result3;
    pthread_join(t3,&thread_result3);//result of t3 thread are stored in second variable
    if(thread_result3!=NULL){
        printf("returned result from thread 2 = %d\n",*(int *) thread_result3);
        free(thread_result3);
        thread_result3=NULL;
    }
    return 0;
}