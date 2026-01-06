#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

cpu_t cpu;

void cpu_init() {
    memset(&cpu, 0, sizeof(cpu_t));
    // Inicializar TLB entries a invalid
    for (int c = 0; c < NUM_CORES; c++) {
        for (int t = 0; t < NUM_THREADS_PER_CORE; t++) {
            for (int i = 0; i < TLB_SIZE; i++) {
                cpu.cores[c].threads[t].mmu.tlb[i].valid = 0;
            }
        }
    }
}
