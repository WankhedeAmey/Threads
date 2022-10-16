#include<stdio.h>
#include<assert.h>
#include "rec_mutex.h"

void
rec_mutex_init(rec_mutex_t *rec_mutex){
    rec_mutex->n=0;
    rec_mutex->locking_thread=NULL;
    pthread_cond_init(&rec_mutex->cv,NULL);
    pthread_mutex_init(&rec_mutex->state_mutex,NULL);
    rec_mutex->n_waited=0;
}

void
rec_mutex_lock(rec_mutex_t *rec_mutex){
    pthread_mutex_lock(&rec_mutex->state_mutex);

    //case I -> when rec_mutex obj is not initially locked
    if(rec_mutex->n==0){
        //check if locking_thread is also NULL for any bug
        assert(rec_mutex->locking_thread==NULL);
        rec_mutex->n=1;
        rec_mutex->locking_thread=pthread_self();
        pthread_mutex_unlock(&rec_mutex->state_mutex);
        return;
    }

    //case II -> when mutex is locked by self already
    if(rec_mutex->locking_thread == pthread_self()){
        assert(rec_mutex->n==0);//check for constraint
        rec_mutex->n++;
        pthread_mutex_unlock(&rec_mutex->state_mutex);
        return;
    }

    //case III -> when this rec_mutex is being used by another thread
    while(rec_mutex->locking_thread != NULL && rec_mutex->locking_thread != pthread_self()){
        rec_mutex->n_waited++;
        pthread_cond_wait(&rec_mutex->cv,&rec_mutex->state_mutex);
        rec_mutex->n_waited--;
    }
    //now once the rec_lock is released by another thread, check if it's being used by another
    assert(rec_mutex->n==0);
    assert(rec_mutex->locking_thread == NULL);
    rec_mutex->n=1;
    rec_mutex->locking_thread=pthread_self();
    pthread_mutex_unlock(&rec_mutex->state_mutex);
    return;
}

void
rec_mutex_unlock(rec_mutex_t *rec_mutex){
    pthread_mutex_lock(&rec_mutex->state_mutex);

    //case I -> when rec_mutex is not already locked
    if(rec_mutex->n==0){
        assert(rec_mutex->locking_thread==NULL);
        assert(0);
    }

    //case II -> when rec_mutex is locked by self
    if(rec_mutex->locking_thread==pthread_self()){
        assert(rec_mutex->n>0);
        rec_mutex->n--;
        if(rec_mutex->n>0){
            pthread_mutex_unlock(&rec_mutex->state_mutex);
            return;
        }

        //else rec_mutex->n has become 0 => signal waiting threads
        if(rec_mutex->n_waited>0){
            pthread_cond_signal(&rec_mutex->cv);
        }
        rec_mutex->locking_thread=NULL;
        pthread_mutex_unlock(&rec_mutex->state_mutex);
        return;
    }

    //case III -> when rec_mutex is locked by some another thread
    while(rec_mutex->locking_thread!=NULL && rec_mutex->locking_thread==pthread_self()){
        assert(0);
    }
}

void
rec_mutex_destroy(rec_mutex_t *rec_mutex) {
    //before destroying any object, check if it's being used by any thread/process
    assert(!rec_mutex->n);
    assert(!rec_mutex->locking_thread);
    assert(!rec_mutex->n_waited);
    pthread_mutex_destroy(&rec_mutex->state_mutex);
    pthread_cond_destroy(&rec_mutex->cv);
}