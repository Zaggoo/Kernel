#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>
#include <stdint.h>

// ============================================================================
// ANSI KOLORE KODEAK (antolatzailearako)
// ============================================================================
#define COLOR_RESET      "\033[0m"
#define COLOR_BRIGHT_RED "\033[91m"

// ============================================================================
// SISTEMAREN KONFIGURAZIOA
// ============================================================================
#define TICKRATE 1
#define TEMP_KOPURU 2

// ============================================================================
// MEMORIAREN KONFIGURAZIOA
// ============================================================================
#define MEM_SIZE (1 << 24)           // 16 MB total
#define KERNEL_SIZE (1 << 22)        // 4 MB kernel
#define USER_SIZE (MEM_SIZE - KERNEL_SIZE)  // 12 MB erabiltzaile
#define PAGE_SIZE 4096
#define NUM_PAGES (USER_SIZE / PAGE_SIZE)
#define PAGE_TABLE_SIZE (NUM_PAGES * sizeof(uint32_t))

// ============================================================================
// CPUREN KONFIGURAZIOA
// ============================================================================
#define NUM_CORES 2
#define NUM_THREADS_PER_CORE 2
#define TLB_SIZE 16

// ============================================================================
// SINCRONIZAZIO ESTRUKTURAK
// ============================================================================
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

// ============================================================================
// MEMORIA BIRTUALAREN ESTRUKTURAK
// ============================================================================
typedef struct {
    uint32_t virtual_page;
    uint32_t physical_page;
    int valid;
} tlb_entry_t;

typedef struct {
    uintptr_t ptbr;  
    tlb_entry_t tlb[TLB_SIZE];
} mmu_t;

typedef struct {
    uint32_t code_address_v;
    uint32_t data_address_v;
    uintptr_t pgb_address_f;  
} mm_t;

// ============================================================================
// CPUREN ESTRUKTURAK
// ============================================================================
typedef struct {
    uint32_t pc;
    uint32_t ir;
    uint32_t registers[16];
    mmu_t mmu;
} thread_t;

typedef struct {
    thread_t threads[NUM_THREADS_PER_CORE];
} core_t;

typedef struct {
    core_t cores[NUM_CORES];
} cpu_t;

// ============================================================================
// PROZESUEN ESTRUKTURAK
// ============================================================================
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
    int death_count;  
    mm_t mm;
} pcb_t;

typedef struct pcb_node {
    pcb_t pcb;
    struct pcb_node *next;
} pcb_node_t;

// ============================================================================
// ALDAGAI GLOBALAK - DEBUG FLAGAK
// ============================================================================
extern int enable_clock_prints;
extern int enable_timer_prints;
extern int enable_process_prints;
extern int enable_scheduler_prints;
extern int enable_cpu_prints;

// ============================================================================
// ALDAGAI GLOBALAK - PROZESUAK
// ============================================================================
extern pcb_node_t *process_list;
extern pcb_node_t *running_processes[4];  // MAX_RUNNING = 4 (2 cores × 2 threads)
extern int next_pid;
extern char selected_program[50];

// ============================================================================
// ALDAGAI GLOBALAK - MEMORIA
// ============================================================================
extern uint8_t *memory;
extern uint8_t *kernel_memory;
extern uint8_t *user_memory;

// ============================================================================
// ALDAGAI GLOBALAK - CPU
// ============================================================================
extern cpu_t cpu;

// ============================================================================
// FUNZIOAK 
// ============================================================================
extern void load(int timerId);
extern void *kernelclock(void *arg);
extern void *timer(void *arg);
extern void *scheduler(void *arg);
extern void schedule();
extern void memory_init();
extern void cpu_init();
extern void cpu_load_process(int core, int thread, pcb_node_t *process);
extern void cpu_invalidate_tlb(int core, int thread);
extern uintptr_t cpu_translate_address(int core, int thread, uint32_t virtual_addr);
extern int cpu_execute_instruction(int core, int thread);
extern void cpu_tick();

#endif
