#include "../include/kernel.h"
#include "../include/task.h"

extern void switch_task();

extern task_t* tasks[NR_TASKS];

task_t* current = NULL;

task_t* find_ready_task() {
    task_t* next = NULL;

    for (int i = 1; i < NR_TASKS; ++i) {
        task_t* task = tasks[i];

        if (NULL == task) continue;

        if (current == task && TASK_RUNNING == task->state) {
            task->state = TASK_READY;
        }

        if (TASK_READY != task->state) continue;

        next = task;
    }

    if (NULL == next) {
        next = tasks[0];
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

    next->state = TASK_RUNNING;
    current = next;

    switch_task();
}