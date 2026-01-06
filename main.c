#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int enable_clock_prints = 0;
int enable_timer_prints = 0;
int enable_process_prints = 0;
int enable_scheduler_prints = 0;

int main(int argc, char *argv[]) {

    shared_t shared;

    pthread_mutex_init(&shared.mutex, NULL);
    pthread_cond_init(&shared.cond, NULL);
    pthread_cond_init(&shared.cond2, NULL);
    shared.done = 0;

    char input;
    printf("¿Erlojuaren inprimaketak aktibatu? (y/n): ");
    scanf(" %c", &input);
    enable_clock_prints = (input == 'y' || input == 'Y');

    printf("¿Tenporizadorearen inprimaketak aktibatu? (y/n): ");
    scanf(" %c", &input);
    enable_timer_prints = (input == 'y' || input == 'Y');

    printf("¿Loaderraren inprimaketak aktibatu? (y/n): ");
    scanf(" %c", &input);
    enable_process_prints = (input == 'y' || input == 'Y');

    printf("¿Antolatzailearen inprimaketak aktibatu? (y/n): ");
    scanf(" %c", &input);
    enable_scheduler_prints = (input == 'y' || input == 'Y');

    pthread_t t_clock, t_timer1, t_timer2;

    timer_struct ts1, ts2;
    ts1.shared = &shared;
    ts1.id = 1;
    ts1.nticks = 2;
    ts1.function = LOADER;
    ts2.shared = &shared;
    ts2.id = 2;
    ts2.nticks = 6;
    ts2.function = TIMER;

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