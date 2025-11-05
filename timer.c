
#include "config.h"
#include <pthread.h>
#include <stdio.h>

void *timer(void *arg){
    timer_struct *ts = (timer_struct *)arg;
    shared_t *shared = ts->shared;
    int tick = 0;

    pthread_mutex_lock(&shared->mutex);    
    while(1){
        shared->done++;
        tick++;
        if(tick == ts->nticks){
            tick = 0;
            // SEINALEA BIDALI PROCESS GENERATORRI
            printf("Timer %d\n", ts->id);
        }
        pthread_cond_signal(&shared->cond);
        pthread_cond_wait(&shared->cond2, &shared->mutex);
    }
    return NULL;
}
