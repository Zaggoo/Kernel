#include "config.h"
#include <stdint.h>
#include <stdlib.h>

uint8_t *memory = NULL;
uint8_t *kernel_memory = NULL;
uint8_t *user_memory = NULL;

void memory_init() {
    memory = calloc(MEM_SIZE, sizeof(uint8_t));
    if (memory == NULL) {
        perror("Error allocating memory");
        exit(1);
    }
    kernel_memory = memory;
    user_memory = memory + KERNEL_SIZE;
}
