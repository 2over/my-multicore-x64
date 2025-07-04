#include "../include/kernel.h"
#include "../include/task.h"

extern void switch_task();

extern task_t* tasks[NR_TASKS];

task_t* current = NULL;

task_t* find_ready_task() {
    task_t* next = NULL;

    for (int i = 0; i < NR_TASKS; ++i) {
        task_t* task = tasks[i];

        if (NULL == task) continue;

        if (TASK_READY == task->state) {
            next = task;
            break;
        }
    }

    return next;
}

void sched() {
    if (NULL != current) {
        if (TASK_SLEEPING != current-> state) {
            current->state = TASK_READY;
        }

        current = NULL;
    }

    task_t* next = find_ready_task();
    if (NULL == next) {
        return ;
    }

    next->state = TASK_RUNNING;
    current = next;

    switch_task();
}

task_t* sched64() {
    if (NULL != current) {
        if (TASK_SLEEPING != current->state) {
            current->state = TASK_READY;
        }

        current = NULL;
    }

    task_t* next = find_ready_task();
    if (NULL == next) {
        return NULL;
    }

    next->state = TASK_RUNNING;

    return next;
}