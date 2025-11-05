#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

extern void *kernelclock(void *arg);
extern void *timer(void *arg);

int main(int argc, char *argv[]) {

    shared_t shared;

    pthread_mutex_init(&shared.mutex, NULL);
    pthread_cond_init(&shared.cond, NULL);
    pthread_cond_init(&shared.cond2, NULL);
    shared.done = 0;

    pthread_t t_clock, t_timer1, t_timer2;

    timer_struct ts1, ts2;
    ts1.shared = &shared;
    ts1.id = 1;
    ts1.nticks = 2;
    ts2.shared = &shared;
    ts2.id = 2;
    ts2.nticks = 3;

    pthread_create(&t_clock, NULL, kernelclock, &shared);
    pthread_create(&t_timer1, NULL, timer, &ts1);
    pthread_create(&t_timer2, NULL, timer, &ts2);

    pthread_detach(t_clock);
    pthread_detach(t_timer1);
    pthread_detach(t_timer2);


    while(1){
        sleep(1);
    }

    return 0;
}