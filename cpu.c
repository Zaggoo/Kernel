#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

cpu_t cpu;
pcb_node_t *cpu_thread_process[NUM_CORES][NUM_THREADS_PER_CORE] = {{NULL}};

void cpu_init() {
    memset(&cpu, 0, sizeof(cpu_t));
    for (int c = 0; c < NUM_CORES; c++) {
        for (int t = 0; t < NUM_THREADS_PER_CORE; t++) {
            for (int i = 0; i < TLB_SIZE; i++) {
                cpu.cores[c].threads[t].mmu.tlb[i].valid = 0;
            }
            for (int r = 0; r < 16; r++) {
                cpu.cores[c].threads[t].registers[r] = 0;
            }
        }
    }
}

void cpu_load_process(int core, int thread, pcb_node_t *process) {
    if (core >= NUM_CORES || thread >= NUM_THREADS_PER_CORE || process == NULL) return;
    
    thread_t *hw_thread = &cpu.cores[core].threads[thread];
    
    cpu_thread_process[core][thread] = process;
    
    hw_thread->mmu.ptbr = process->pcb.mm.pgb_address_f;
    
    hw_thread->pc = process->pcb.mm.code_address_v;
    

    cpu_invalidate_tlb(core, thread);
}

void cpu_invalidate_tlb(int core, int thread) {
    if (core >= NUM_CORES || thread >= NUM_THREADS_PER_CORE) return;
    
    thread_t *hw_thread = &cpu.cores[core].threads[thread];
    for (int i = 0; i < TLB_SIZE; i++) {
        hw_thread->mmu.tlb[i].valid = 0;
    }
}

uintptr_t cpu_translate_address(int core, int thread, uint32_t virtual_addr) {
    if (core >= NUM_CORES || thread >= NUM_THREADS_PER_CORE) return 0;
    
    thread_t *hw_thread = &cpu.cores[core].threads[thread];
    uint32_t virtual_page = virtual_addr / PAGE_SIZE;
    uint32_t offset = virtual_addr % PAGE_SIZE;
    
    for (int i = 0; i < TLB_SIZE; i++) {
        if (hw_thread->mmu.tlb[i].valid && 
            hw_thread->mmu.tlb[i].virtual_page == virtual_page) {
            
            uint32_t physical_frame = hw_thread->mmu.tlb[i].physical_page;
            return (uintptr_t)(memory + physical_frame * PAGE_SIZE + offset);
        }
    }
    
    
    uint32_t *page_table = (uint32_t *)hw_thread->mmu.ptbr;
    uint32_t physical_frame = page_table[virtual_page];
    
    
    int tlb_index = rand() % TLB_SIZE;
    hw_thread->mmu.tlb[tlb_index].virtual_page = virtual_page;
    hw_thread->mmu.tlb[tlb_index].physical_page = physical_frame;
    hw_thread->mmu.tlb[tlb_index].valid = 1;
    
    
    return (uintptr_t)(memory + physical_frame * PAGE_SIZE + offset);
}

int cpu_execute_instruction(int core, int thread) {
    if (core >= NUM_CORES || thread >= NUM_THREADS_PER_CORE) return -1;
    
    thread_t *hw_thread = &cpu.cores[core].threads[thread];
    
    
    uintptr_t phys_addr = cpu_translate_address(core, thread, hw_thread->pc);
    uint32_t instruction = *(uint32_t *)(uintptr_t)(phys_addr);
    hw_thread->ir = instruction;
    
    
    uint8_t opcode = (instruction >> 28) & 0xF;
    
    if (enable_cpu_prints) {
        printf("[CPU Core %d Thread %d] PC=0x%04X: 0x%08X - ", core, thread, hw_thread->pc, instruction);
        fflush(stdout);  
    }
    
    switch(opcode) {
        case 0x0: { // LD 
            uint8_t reg = (instruction >> 24) & 0xF;
            uint32_t virt_addr = instruction & 0xFFFFFF;
            uintptr_t data_phys = cpu_translate_address(core, thread, virt_addr);
            uint32_t value = *(uint32_t *)(uintptr_t)(data_phys);
            hw_thread->registers[reg] = value;
            if (enable_cpu_prints) {
                printf("LD R%d, [0x%X] (value=0x%X)\n", reg, virt_addr, value);
            }
            hw_thread->pc += 4;
            break;
        }
        case 0x1: { // ST 
            uint8_t reg = (instruction >> 24) & 0xF;
            uint32_t virt_addr = instruction & 0xFFFFFF;
            uintptr_t data_phys = cpu_translate_address(core, thread, virt_addr);
            *(uint32_t *)(uintptr_t)(data_phys) = hw_thread->registers[reg];
            if (enable_cpu_prints) {
                printf("ST [0x%X], R%d (value=0x%X)\n", virt_addr, reg, hw_thread->registers[reg]);
            }
            hw_thread->pc += 4;
            break;
        }
        case 0x2: { // ADD
            uint8_t rd = (instruction >> 24) & 0xF;
            uint8_t rs1 = (instruction >> 20) & 0xF;
            uint8_t rs2 = (instruction >> 16) & 0xF;
            hw_thread->registers[rd] = hw_thread->registers[rs1] + hw_thread->registers[rs2];
            if (enable_cpu_prints) {
                printf("ADD R%d, R%d, R%d (0x%X + 0x%X = 0x%X)\n", rd, rs1, rs2, 
                       hw_thread->registers[rs1], hw_thread->registers[rs2], hw_thread->registers[rd]);
            }
            hw_thread->pc += 4;
            break;
        }
        case 0xF: { // EXIT
            if (enable_cpu_prints) {
                printf("EXIT\n");
                fflush(stdout);  
            }
            return 1; 
        }
        default:
            if (enable_cpu_prints) {
                printf("INVALID (opcode=0x%X)\n", opcode);
            }
            return -1;
    }
    
    return 0;
}

void cpu_tick() {
    
    for (int core = 0; core < NUM_CORES; core++) {
        for (int thread = 0; thread < NUM_THREADS_PER_CORE; thread++) {
            pcb_node_t *process = cpu_thread_process[core][thread];
            
            if (process != NULL && process->pcb.state == RUNNING) {
                
                int result = cpu_execute_instruction(core, thread);
                
                if (result == 1) {
                    
                    process->pcb.state = TERMINATED;
                    cpu_thread_process[core][thread] = NULL;
                } else if (result == -1) {
                    
                    process->pcb.state = TERMINATED;
                    cpu_thread_process[core][thread] = NULL;
                }
            }
        }
    }
}
