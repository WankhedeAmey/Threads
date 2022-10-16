/*two consumer threads, two producer threads
 *shared resource -> queue (implemented in Queue.c file)
 *threads are created in joinable mode
 *Following are the constraints applied :

1. When producer threads produce an element and add it to the Queue, it does not release the Queue
untill the Queue is full i.e producer thread release the Queue only when it is full

2. When consumer threads consume an element from the Queue, it consumes the entire Queue and
do not release it until the Queue is empty.

3. Consumer Signals the Producers when Queue is Exhausted, Producers Signals the Consumers when Queue	
becomes full

gcc -g -c Queue.c -o Queue.o
gcc -g -c producer_consumer.c -o producer_consumer.o
gcc -g producer_consumer.o Queue.o -o exe -lpthread
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "Queue.h"

static int new_int() {

	static int a = 0;
	a++;
	return a;
}


struct Queue_t *q;

static const char *prod1 = "TP1";//producer thread #
static const char *prod2 = "TP2";
static const char *cons1 = "TC1";//consumer thread #
static const char *cons2 = "TC2";

//producer thread function
//this function is accessed by 'x' producer threads 
void *prod_fn(void *arg){
    char *th_name=(char *)arg;

    printf("thread %s is waiting to lock the queue\n",th_name);
    pthread_mutex_lock(&q->q_mutex);
    printf("thread %s has locked the queue\n",th_name);

    //check for condition/predicate
    while(is_queue_full(q)){
        printf("thread %s is blocked by itself, q is already full\n",th_name);
        pthread_cond_wait(&q->q_cv,&q->q_mutex);
        printf("thread %s has woken up, checking q status again\n",th_name);
    }

    //start the producer C.S. (start pushing only if q is empty)
    assert(is_queue_empty(q));
    int i;
    while(!is_queue_full(q)) { 
		i = new_int();
		printf("Thread %s produces new integer %d\n", th_name, i);
		enqueue(q, (void *)i); 
		printf("Thread %s pushed integer %d in Queue, Queue size = %d\n", th_name, i, q->count);
	}
    printf("Thread %s Filled up the Queue, signalling and releasing lock\n", th_name);
    pthread_cond_broadcast(&q->q_cv); 
	//end of producer C.S.
    pthread_mutex_unlock(&q->q_mutex); /* S9 */
	printf("Thread %s finished, and exit\n", th_name);
	return NULL;
}

static void *
cons_fn(void *arg) {

	char *th_name = (char *)arg;

	printf("Thread %s waiting to lock the Queue\n", th_name);
	pthread_mutex_lock(&q->q_mutex); /* S1 */
	printf("Thread %s locks the Queue\n", th_name);

	/* Predicate check S2 */
	while (is_queue_empty(q)) {
		printf("Thread %s blocks itself, Q is already empty\n", th_name);
		pthread_cond_wait(&q->q_cv, &q->q_mutex); /* S3 */
		printf("Thread %s wakes up, checking the Queue status again\n", th_name);
	}

	/*  Start of C.S of Consumer. Consumer must start consuming elements from Full Queue only */
	assert(is_queue_full(q));

	/* S4 begin*/
	int i;
	while(!is_queue_empty(q)) {
		i = (int)deque(q);
		printf("Thread %s consumes an integer %d, Queue size = %d\n",
				th_name, i, q->count);
	}
	/* send signal to Producer thread waiting on Queue */
	printf("Thread %s Drains the entire Queue, sending signal to Blocking Threads\n",
		th_name);
	pthread_cond_broadcast(&q->q_cv);
	/*  S4 end*/
	
	printf("Thread %s releasing lock\n", th_name);

	/* End of C.S of Consumer */
	pthread_mutex_unlock(&q->q_mutex);	/* Step S5 */
	printf("Thread %s finished, and exit\n", th_name);
	return NULL;
}

int main(int argc,char **argv){
    /* initialize the queue and its mutex+cv */
    q=initQ();

    //create joinable threads & for that set attr
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    pthread_t prod_th1, prod_th2; //producer thread handles
    pthread_t cons_th1, cons_th2; //consumer thread handles

    //create threads
    pthread_create(&prod_th1, &attr, prod_fn, (void *)prod1);
    pthread_create(&prod_th2, &attr, prod_fn, (void *)prod2);
    pthread_create(&cons_th1, &attr, cons_fn, (void *)cons1);
    pthread_create(&cons_th2, &attr, cons_fn, (void *)cons2);

    pthread_join(prod_th1, 0);
	pthread_join(prod_th2, 0);
	pthread_join(cons_th1, 0);
	pthread_join(cons_th2, 0);

    printf("Program Finished\n");
	pthread_exit(0);
	return 0;
}