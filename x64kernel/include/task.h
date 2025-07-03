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
    int64 rflags;
    int64 rip;
    int64 rax;
    int64 rbx;
    int64 rcx;
    int64 rdx;
    int64 rdi;
    int64 rsi;
    int64 rbp;
    int64 rsp;
    int64 r8;
    int64 r9;
    int64 r10;
    int64 r11;
    int64 r12;
    int64 r13;
    int64 r14;
    int64 r15;
    int64 cs;
    int64 ss;
    int64 ds;
    int64 fs;
    int64 gs;
}__attribute__((packed)) task_context_t;

typedef struct task_t {
    int             pid;
    char            name[32];
    task_state_t    state;
    task_fun_t      function;
    task_context_t  context;
    int             scheduling_times;   // 调度次数
    int             exit_code;
}__attribute__((packed)) task_t;

task_t* task_create(task_fun_t fun, char *name);
task_fun_t get_task_function(task_t* task);
task_context_t* get_task_context(task_t* task);

int inc_scheduling_times(task_t* task);

void task_init();

void sched();

void task_exit(task_t* task, int exit_code);

#endif //MY_MULTICORE_X64_TASK_H
