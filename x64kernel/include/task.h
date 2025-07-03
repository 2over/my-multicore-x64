//
// Created by xiehao on 25-7-2.
//

#ifndef MY_MULTICORE_X64_TASK_H
#define MY_MULTICORE_X64_TASK_H

#include "types.h"

// 进程上限
#define NR_TASKS 64

typedef void* (*task_fun_t)(void*);

typedef enum task_state_t {
    TASK_INIT,      // 初始化
    TASK_RUNNING,   // 执行
    TASK_READY,     // 就绪
    TASK_BLOCKED,   // 阻塞
    TASK_SLEEPING,  // 睡眠
    TASK_WAITING,   // 等待
    TASK_DIED,      // 死亡
}task_state_t;

typedef struct {
    int64 rflags;   // 8 * 0
    int64 rip;      // 8 * 1
    int64 rax;      // 8 * 2
    int64 rbx;      // 8 * 3
    int64 rcx;      // 8 * 4
    int64 rdx;      // 8 * 5
    int64 rdi;      // 8 * 6
    int64 rsi;      // 8 * 7
    int64 rbp;      // 8 * 8
    int64 rsp;      // 8 * 9
    int64 r8;       // 8 * 10
    int64 r9;       // 8 * 11
    int64 r10;      // 8 * 12
    int64 r11;      // 8 * 13
    int64 r12;      // 8 * 14
    int64 r13;      // 8 * 15
    int64 r14;      // 8 * 16
    int64 r15;      // 8 * 17
    int64 cs;       // 8 * 18
    int64 ss;       // 8 * 19
    int64 ds;       // 8 * 20
    int64 fs;       // 8 * 21
    int64 gs;       // 8 * 22
}__attribute__((packed)) task_context_t;

typedef struct task_t {
    int             pid;
    char            name[32];
    task_state_t    state;
    task_fun_t      function;
    task_context_t  context;
    int             scheduling_times;   // 调度次数
    int             exit_code;
    int64           esp0;
}__attribute__((packed)) task_t;

task_t* task_create(task_fun_t fun, char *name);
task_fun_t get_task_function(task_t* task);
task_context_t* get_task_context(task_t* task);

int inc_scheduling_times(task_t* task);

void task_init();

void sched();

void task_exit(task_t* task, int exit_code);

int64 get_esp0(task_t* task);

#endif //MY_MULTICORE_X64_TASK_H
