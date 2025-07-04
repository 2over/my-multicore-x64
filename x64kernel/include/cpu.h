//
// Created by xiehao on 25-7-3.
//

#ifndef MY_MULTICORE_X64_CPU_H
#define MY_MULTICORE_X64_CPU_H

#include "types.h"
#include "task.h"

// Kernel Processor Control Region 内核处理器控制区
typedef struct {
    uint64_t    id;
    task_t*     task;
    uint64_t    esp0;
    uint64_t    esp3;
    uint64_t    thread_esp0;
}__attribute__((packed)) kpcr_t;

kpcr_t* kpcr_create(uint32_t id);

void bsp_init();

void cpu_broadcast();

#endif //MY_MULTICORE_X64_CPU_H
