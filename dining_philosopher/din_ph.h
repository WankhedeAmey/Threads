#ifndef _DIN_PHIL_
#define _DIN_PHIL_

#include<stdbool.h>
#include<pthread.h>

//structure for philosopher
typedef struct phil_{
    int phil_id;
    pthread_t thread_handle;
    int eat_count;
} phil_t;

typedef struct spoon{
    int spoon_id;
    bool is_used;//if spoon is currently being used or not
    phil_t *phil;//who is using the spoon
    pthread_mutex_t mutex;//spoon specific mutex
    pthread_cond_t cv;//spoon sepecific cv
} spoon_t;


#endif  /* _DIN_PHIL_ */