
#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int next_pid = 1;
pcb_node_t *process_list = NULL;
uint32_t *next_kernel_alloc = (uint32_t *)kernel_memory;

void load(int parentId) {
    pcb_node_t *new_node = malloc(sizeof(pcb_node_t));
    if (!new_node) {
        printf("[loader] PCB-rako memoria esleitzeko errorea\n");
        return;
    }
    
    new_node->pcb.pid = next_pid++;
    new_node->pcb.parent_id = parentId;
    new_node->pcb.state = rand() % 4;
    new_node->pcb.time_waiting = (new_node->pcb.state == WAITING) ? 0:0;
    new_node->pcb.total_execution_time = 0;
    new_node->pcb.max_execution_time = rand() % 10 + 5; // 5-14 ticks
    new_node->pcb.atk = rand() % 10 + 1;
    new_node->pcb.def = rand() % 3 + 1;
    new_node->pcb.vit = rand() % 40 + 10;
    
    // Inicializar memory management
    new_node->pcb.mm.code_address = 0;
    new_node->pcb.mm.data_address = 0;
    new_node->pcb.mm.pgb_address = (uint32_t)next_kernel_alloc;
    next_kernel_alloc += PAGE_TABLE_SIZE / sizeof(uint32_t);
    if ((uint8_t*)next_kernel_alloc > kernel_memory + KERNEL_SIZE) {
        printf("[loader] Error: Kernel mailan ez dago lekurik orri-taularako\n");
        free(new_node);
        return;
    }
    
    new_node->next = process_list;
    process_list = new_node;
    
    if (enable_process_prints) printf("[loader] PCB sortua: PID=%d, Gurasoa=%d, Egoera=%d, Itxaroten daraman denbora=%d, Exekuzioan daraman denbora=%d, Exekuzio denbora maximoa=%d, Atk=%d, Def=%d, Vit=%d\n", 
           new_node->pcb.pid, new_node->pcb.parent_id, new_node->pcb.state, new_node->pcb.time_waiting, new_node->pcb.total_execution_time, new_node->pcb.max_execution_time, new_node->pcb.atk, new_node->pcb.def, new_node->pcb.vit);
}
