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

typedef struct task_t {
    int             pid;
    char            name[32];
    task_state_t    state;
    task_fun_t      function;
}__attribute__((packed)) task_t;

task_t* taask_create(task_fun_t fun, char *name);

void task_init();

#endif //MY_MULTICORE_X64_TASK_H
