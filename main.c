#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int enable_clock_prints = 0;
int enable_timer_prints = 0;
int enable_process_prints = 0;
int enable_scheduler_prints = 0;
int enable_cpu_prints = 0;

char selected_program[50];  


int total_processes_executed = 0;
int processes_normal_exit = 0;
int processes_critical_failure = 0;


void load_program(const char *program_path) {
    strcpy(selected_program, program_path);
    load(0);
    total_processes_executed++;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-debug") == 0) {
            enable_process_prints = 1;
            enable_clock_prints = 1;
            enable_timer_prints = 1;
            printf("[DEBUG] Debug mezuak pantailaratzen\n");
        } else if (strcmp(argv[i], "-scheduler") == 0) {
            enable_scheduler_prints = 1;
            printf("[DEBUG] antolatzaiearen mezuak pantailaratzen\n");
        } else if (strcmp(argv[i], "-execution") == 0) {
            enable_cpu_prints = 1;
            printf("[DEBUG] CPU exekuzioaren trazak pantailaratzen\n");
        } else {
            printf("Erabilera: %s [-debug] [-scheduler] [-execution]\n", argv[0]);
            printf("  -debug      : Debug mezuak pantailarazteko\n");
            printf("  -scheduler  : Antolatzailearen mezuak pantailarazteko\n");
            printf("  -execution  : CPU exekuzioaren trazak pantailarazteko\n");
            return 1;
        }
    }

    memory_init();
    cpu_init();
    printf("=== Sistema hasita ===\n");
    printf("Memoria: %d byte (Kernel: %d, Erabiltzaile: %d)\n", MEM_SIZE, KERNEL_SIZE, USER_SIZE);
    printf("CPU: %d cores x %d threads = %d hari guztira\n\n", NUM_CORES, NUM_THREADS_PER_CORE, NUM_CORES * NUM_THREADS_PER_CORE);

    int opcion;
    printf("Aukeratu zenbat programa kargatu:\n");
    printf("  1. Programa bakar bat\n");
    printf("  2. Lau programa kargatu (CPUa osorik betetzeko)\n");
    printf("  3. Zortzi programa kargatu (CPUa betetzeko eta ilaran prozesu gehiago izateko)\n");
    printf("Aukera: ");
    scanf("%d", &opcion);
    printf("\n");

    const char *programs[] = {
        "prog/prog042.elf",
        "prog/prog043.elf",
        "prog/prog044.elf",
        "prog/prog045.elf"
    };
    int num_programs = 4;

    if (opcion == 1) {
        printf("Programa kargatzen: %s\n\n", programs[0]);
        load_program(programs[0]);
    } else if (opcion == 2) {
        
        printf("Lau programa kargatzen...\n\n");
        for (int i = 0; i < num_programs; i++) {
            printf("Programa %d kargatzen: %s\n", i + 1, programs[i]);
            load_program(programs[i]);
        }
        printf("\n");
    } else if (opcion == 3) {
        
        printf("Zortzi programa kargatzen...\n\n");
        for (int repeticion = 0; repeticion < 2; repeticion++) {
            for (int i = 0; i < num_programs; i++) {
                int num_programa = repeticion * num_programs + i + 1;
                printf("Programa %d kargatzen: %s\n", num_programa, programs[i]);
                load_program(programs[i]);
            }
        }
        printf("\n");
    } else {
        printf("Aukera ezegokia. Amaitzen...\n");
        return 1;
    }
    
    printf("=== Exekuzioa hasten ===\n\n");
    int max_iterations = 1000;  
    for (int i = 0; i < max_iterations; i++) {
        schedule();
        
        int has_running = 0;
        extern pcb_node_t *running_processes[];
        extern pcb_node_t *process_list;
        
        for (int j = 0; j < (NUM_CORES * NUM_THREADS_PER_CORE); j++) {
            if (running_processes[j] != NULL) {
                has_running = 1;
                break;
            }
        }
        
        if (!has_running && process_list != NULL) {
            has_running = 1;
        }
        
        if (!has_running) {
            printf("\n=== Prozesu guztiak amaitu dira ===\n");
            break;
        }
        
        usleep(100000); 
    }

    // Azken laburpena erakutsi
    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║          EXEKUZIOAREN LABURPENA            ║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║ Prozesu totalak:               %3d         ║\n", total_processes_executed);
    printf("║ Normal amaitutakoak:           %3d         ║\n", processes_normal_exit);
    printf("║ Hutsegite kritikoak:           %3d         ║\n", processes_critical_failure);
    printf("╚════════════════════════════════════════════╝\n");

    return 0;
}