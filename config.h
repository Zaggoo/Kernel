#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>

#define TICKRATE 1
// #define NTICKS 6
#define TEMP_KOPURU 2
#define MEM_SIZE (1 << 24)
#define KERNEL_SIZE (1 << 22)
#define USER_SIZE (MEM_SIZE - KERNEL_SIZE)
#define NUM_CORES 4
#define NUM_THREADS_PER_CORE 2
#define TLB_SIZE 16
#define PAGE_SIZE 4096
#define NUM_PAGES (USER_SIZE / PAGE_SIZE)
#define PAGE_TABLE_SIZE (NUM_PAGES * sizeof(uint32_t))

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t cond2;
    int done;
} shared_t;

typedef enum { TIMER, LOADER } timer_function_t;

typedef struct {
    shared_t *shared;
    int id;
    int nticks;
    timer_function_t function;
} timer_struct;

typedef struct {
    uint32_t virtual_page;
    uint32_t physical_page;
    int valid;
} tlb_entry_t;

typedef struct {
    uint32_t ptbr;
    tlb_entry_t tlb[TLB_SIZE];
} mmu_t;

typedef struct {
    uint32_t pc;
    uint32_t ir;
    mmu_t mmu;
} thread_t;

typedef struct {
    thread_t threads[NUM_THREADS_PER_CORE];
} core_t;

typedef struct {
    core_t cores[NUM_CORES];
} cpu_t;

typedef struct {
    uint32_t code_address_v;
    uint32_t data_address_v;
    uint32_t pgb_address_f;
} mm_t;

typedef enum { READY, RUNNING, WAITING, TERMINATED } state_t;

typedef struct {
    int pid;
    int parent_id;
    state_t state;
    int time_waiting;
    int total_execution_time;
    int max_execution_time;
    int atk;
    int def;
    int vit;
    mm_t mm;
} pcb_t;

typedef struct pcb_node {
    pcb_t pcb;
    struct pcb_node *next;
} pcb_node_t;

extern void load(int timerId);
extern void *kernelclock(void *arg);
extern void *timer(void *arg);
extern void *scheduler(void *arg);
extern void schedule();
extern void memory_init();

extern int enable_clock_prints;
extern int enable_timer_prints;
extern int enable_process_prints;
extern int enable_scheduler_prints;

extern pcb_node_t *process_list;
extern int next_pid;
extern pcb_node_t *current_process;

extern uint8_t *memory;
extern uint8_t *kernel_memory;
extern uint8_t *user_memory;

extern cpu_t cpu;
extern void cpu_init();

#endif
