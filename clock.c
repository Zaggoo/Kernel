#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *kernelclock(void *arg){
    shared_t *shared = (shared_t *)arg; 

    while(1){
        pthread_mutex_lock(&shared->mutex);
        while(shared->done < TEMP_KOPURU){
            pthread_cond_wait(&shared->cond, &shared->mutex);
        }
        usleep(1000000 / TICKRATE);
        if (enable_clock_prints) printf("[erlojua] tik\n");
        schedule();
        shared->done = 0;
        pthread_cond_broadcast(&shared->cond2);
        pthread_mutex_unlock(&shared->mutex);
    }
    return NULL;
}