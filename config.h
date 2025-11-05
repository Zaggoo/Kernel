#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>

#define TICKRATE 2
// #define NTICKS 6
#define TEMP_KOPURU 2

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t cond2;
    int done;
} shared_t;

typedef struct {
    shared_t *shared;
    int id;
    int nticks;
} timer_struct;

#endif
