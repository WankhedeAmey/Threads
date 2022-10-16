#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> /* For working with POSIX threads*/
#include <unistd.h>  /* For pause() and sleep() */
#include <errno.h>	 /* For using Global variable errno */
#include <assert.h>

//global array -> shared resource
int arr[]={1,2,3,4,5};
//define mutex
pthread_mutex_t arr_mutex;

//implement callback functions
static void *thread_fn_callback_sum(void *arg){
    int i;
    int sum;
    int arr_size=sizeof(arr)/sizeof(int);

    do{
        //protect CS with mutex.
        /*If sum thread has mutex,other thread can't enter it's swap function (cs) & vice-versa
        */
        pthread_mutex_lock(&arr_mutex);
        sum=0;
        i=0;
        while(i<arr_size){
            sum+=arr[i];
            i++;
        }
        printf("sum = %d\n",sum);
        sleep(1);
        pthread_mutex_unlock(&arr_mutex);
    } while(1);
}

static void *thread_fn_callback_swap(void *arg){
    int temp;
    int arr_size=sizeof(arr)/sizeof(int);

    do{
        //write operation, thread which is handling this can preempt in b/w any instruction & depending on that, diff array would be produced
        //guard this CS with mutex
        pthread_mutex_lock(&arr_mutex);
        temp=arr[0];
        arr[0]=arr[arr_size-1];
        arr[arr_size-1]=temp;
        pthread_mutex_unlock(&arr_mutex);
    } while(1);
}

void sum_thread_create(){
    pthread_t pthread1;
    //create thread
    int rc=pthread_create(&pthread1,NULL,thread_fn_callback_sum,NULL);
    if(rc!=0){
        printf("error, thread couldn't be created errno = %d\n",rc);
        exit(0);
    }
}

void swap_thread_create(){
    pthread_t pthread2;

    int rc=pthread_create(&pthread2,NULL,thread_fn_callback_swap,NULL);
    if(rc!=0){
        printf("error, thread couldn't be created errno = %d\n",rc);
        exit(0);
    }
}
int main(){
    //initalize mutex
    pthread_mutex_init(&arr_mutex,0);
    sum_thread_create();//write
    swap_thread_create();//read

    pthread_exit(0);
    return 0;
}