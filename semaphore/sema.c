#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include "sema.h"
//1.1init a new semaaphore
sema_t * sema_get_new_semaphore() {

    sema_t *sema = calloc(1, sizeof(sema_t));
    return sema;
}

void sema_init(sema_t *sema, int permit_counter) {
    //initialize semaphore
    sema->permit_counter = permit_counter;
    pthread_cond_init(&sema->cv, NULL);
    pthread_mutex_init(&sema->mutex, NULL);
}


//implement sema wait
void sema_wait(sema_t *sema) {

    //semaphore obj is used as common resource, guard it to avoid change in state of it's attributes
   pthread_mutex_lock(&sema->mutex);
   sema->permit_counter--;
   if (sema->permit_counter < 0) {
    //cs is full, block the thread untill it has space to accomodate current thread
       pthread_cond_wait(&sema->cv, &sema->mutex);
   }
   pthread_mutex_unlock(&sema->mutex);
}

//implement sema post
void sema_post(sema_t *sema) {

    bool any_thread_waiting;
    pthread_mutex_lock(&sema->mutex);

    any_thread_waiting = sema->permit_counter < 0 ? true : false;
    sema->permit_counter++;

    //if a thread is waiting, send out cond_signal to unblock the blocked thread
    if (any_thread_waiting) {
        pthread_cond_signal(&sema->cv);
    }
    pthread_mutex_unlock(&sema->mutex);
}

void sema_destroy(sema_t *sema) {

    sema->permit_counter = 0;
    pthread_mutex_unlock(&sema->mutex);
    pthread_cond_destroy(&sema->cv);	
    pthread_mutex_destroy(&sema->mutex);
}

int sema_getvalue(sema_t *sema) {

	return sema->permit_counter;
}
