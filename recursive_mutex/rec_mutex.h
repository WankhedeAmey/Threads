#ifndef _rec_mutex_
#define _rec_mutex_

#include<pthread.h>
#include<stdint.h>
#include<stdbool.h>

typedef struct rec_mutex{
    /*no. of self-locks taken*/
    uint16_t n;
    /*pthread_id for thread which has this locks*/
    pthread_t locking_thread;
    /* CV to make thread block*/
    pthread_cond_t cv;
    /*A mutex required. Why? To make changes to state of rec_mutex in
    exclusive manner*/
    pthread_mutex_t state_mutex;
    /*no. of threads waiting for this mutex grant*/
    uint16_t n_waited;
} rec_mutex_t;

void rec_mutex_init(rec_mutex_t *rec_mutex);

void rec_mutex_lock(rec_mutex_t *rec_mutex);

void rec_mutex_unlock(rec_mutex_t *rec_mutex);

void rec_mutex_destroy(rec_mutex_t *rec_mutex);

#endif