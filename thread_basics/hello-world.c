#include<stdio.h>
#include<stdlib.h>
#include<pthread.h> //work with posix threads
#include <unistd.h> //for pause() & sleep()


//thread callback must have following prototypes
// void *(*thread_fn)(void *)
static void* thread_fn_callback(void *arg){
    char *input=(char *) arg;
    int a=0;
    while(a<7){
        printf("input string = %s\n",input);
        sleep(1);
        if(a==4){
            pthread_exit(0);
        }
        a++;
    }
}
void thread1_create(){

    pthread_t pthread1;//create a thread handle

    /* Take some arg to be passed to thread fn
     * Always pass the persistent memory(static declaration or heap)
     * Don't pass caller's local variable OR stack memory
     * Why? since local varible is destroyed after its function in which its defined is terminated.
     * Here, thread_input is passed to other thread & if it were a local variable,it means thread1 would've
     * got invalid location
    */
    char *thread_input = "I am thread no 1";

    /*
    Return 0 on succes, negative on failure
    */

    //fork point -> chronology ->1)main started, 2)calls thread1_create(), 3)creates a child thread pthread1
    int rc=pthread_create(&pthread1,NULL, thread_fn_callback, (void *) thread_input);
    //4)pthread runs on it's own, 5)thread1_create function terminates


}

int main(int argc,char **argv){
    //all child threads by parent thread terminates when parent thread is terminated
    //if t1,t2,t3 are threads created by parent thread(here main thread,in main) t0,if main function terminates
    //it means that main process is terminated=>main thread t0 terminates => child thread t1,t2,t3 terminates
    thread1_create(); //a fucntion that creates a thread
    //pause();//pause the main thread here( & let child thread run)
    //if no pause taken by main, it instantly terminates & along with it any child thread it has created(int his case pthread1)
    return 0;
}