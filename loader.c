
#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int next_pid = 1;
pcb_node_t *process_list = NULL;
uint32_t *next_kernel_alloc = NULL;
uint8_t *next_user_alloc = NULL;

void load(int parentId) {
    
    if (next_kernel_alloc == NULL) {
        next_kernel_alloc = (uint32_t *)kernel_memory;
        next_user_alloc = user_memory;
    }
    
    if (selected_program[0] == '\0') return;

    FILE *fp = fopen(selected_program, "r");
    if (!fp) {
        perror("Errorea programaren fitxategia irekitzerakoan");
        return;
    }

    uint32_t code_start = 0, data_start = 0;
    uint32_t *inst_buf = malloc(1024 * sizeof(uint32_t));
    uint32_t *data_buf = malloc(1024 * sizeof(uint32_t));
    size_t inst_count = 0, data_count = 0;
    char line[256];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        if (line_num == 1) {
            sscanf(line, ".text %x", &code_start);
        } else if (line_num == 2) {
            sscanf(line, ".data %x", &data_start);
        } else {
            int num_inst_lines = (data_start - code_start) / 4;
            if (line_num >= 3 && line_num < 3 + num_inst_lines) {
                
                uint32_t instruction;
                if (sscanf(line, "%x", &instruction) == 1) {
                    inst_buf[inst_count++] = instruction;
                }
            } else if (line_num >= 3 + num_inst_lines) {
                
                int32_t value;
                if (sscanf(line, "%x", &value) == 1) {
                    data_buf[data_count++] = (uint32_t)value;
                }
            }
        }
    }
    fclose(fp);
    
    size_t inst_len = inst_count * sizeof(uint32_t);
    size_t data_len = data_count * sizeof(uint32_t);
    
    pcb_node_t *new_node = malloc(sizeof(pcb_node_t));
    if (!new_node) {
        printf("[loader] PCB-rako memoria esleitzeko errorea\n");
        free(inst_buf);
        free(data_buf);
        return;
    }
    
    new_node->pcb.pid = next_pid++;
    new_node->pcb.parent_id = parentId;
    new_node->pcb.state = READY;
    new_node->pcb.time_waiting = 0;
    new_node->pcb.total_execution_time = 0;
    new_node->pcb.max_execution_time = 50; 
    new_node->pcb.atk = rand() % 10 + 1;
    new_node->pcb.def = rand() % 3 + 1;
    new_node->pcb.vit = rand() % 40 + 10;
    new_node->pcb.death_count = 0;  
    
    
    new_node->pcb.mm.code_address_v = code_start;
    new_node->pcb.mm.data_address_v = data_start;
    new_node->pcb.mm.pgb_address_f = (uintptr_t)next_kernel_alloc;
    next_kernel_alloc += PAGE_TABLE_SIZE / sizeof(uint32_t);
    if ((uint8_t*)next_kernel_alloc > kernel_memory + KERNEL_SIZE) {
        printf("[loader] Error: Kernel mailan ez dago lekurik orri-taularako\n");
        free(new_node);
        free(inst_buf);
        free(data_buf);
        return;
    }
    
    
    if (enable_process_prints) {
        printf("[loader] Instrucciones cargadas: %zu bytes (%zu instrucciones)\n", inst_len, inst_count);
        for (size_t i = 0; i < inst_count && i < 10; i++) {
            printf("  [%zu] 0x%08X\n", i, inst_buf[i]);
        }
        printf("[loader] Datos cargados: %zu bytes (%zu valores)\n", data_len, data_count);
        for (size_t i = 0; i < data_count && i < 10; i++) {
            printf("  [%zu] 0x%08X\n", i, data_buf[i]);
        }
    }
    
    
    uint32_t total_size = (data_start - code_start) + data_len;
    size_t total_pages = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;
    
   
    uint8_t *prog_phys = next_user_alloc;
    if (prog_phys + total_pages * PAGE_SIZE > user_memory + USER_SIZE) {
        printf("[loader] Error: No hay suficiente espacio en user memory\n");
        free(inst_buf);
        free(data_buf);
        return;
    }
    
    
    memcpy(prog_phys + (code_start % PAGE_SIZE), inst_buf, inst_len);
    memcpy(prog_phys + (data_start - code_start), data_buf, data_len);
    next_user_alloc += total_pages * PAGE_SIZE;
    
    
    uint32_t *page_table = (uint32_t *)new_node->pcb.mm.pgb_address_f;
    
    uint32_t start_page = code_start / PAGE_SIZE;
    for (size_t i = 0; i < total_pages; i++) {
        uint32_t virtual_page = start_page + i;
        uint32_t physical_frame = ((prog_phys - memory) / PAGE_SIZE) + i;
        page_table[virtual_page] = physical_frame;
        if (enable_process_prints) {
            printf("[loader] Mapeatzen: orrialde birtuala=%u -> orrialde fisikoa=%u\n", virtual_page, physical_frame);
        }
    }

    
    new_node->next = process_list;
    process_list = new_node;
    
    free(inst_buf);
    free(data_buf);
    
    
    strcpy(selected_program, "");
    
    if (enable_process_prints) printf("[loader] PCB sortua: PID=%d, Gurasoa=%d, Egoera=%d, Itxaroten daraman denbora=%d, Exekuzioan daraman denbora=%d, Exekuzio denbora maximoa=%d, Atk=%d, Def=%d, Vit=%d, Code addr=%x, Data addr=%x\n", 
           new_node->pcb.pid, new_node->pcb.parent_id, new_node->pcb.state, new_node->pcb.time_waiting, new_node->pcb.total_execution_time, new_node->pcb.max_execution_time, new_node->pcb.atk, new_node->pcb.def, new_node->pcb.vit, new_node->pcb.mm.code_address_v, new_node->pcb.mm.data_address_v);
}
