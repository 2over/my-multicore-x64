#include "../include/task.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/assert.h"
#include "../include/string.h"

task_t* tasks[NR_TASKS] = {0};

void* idle_task(void* arg) {
    printk("cover\n");

//    while (true) {
//        asm volatile("sti;");
//        asm volatile("hlt;");
//    }
}

static int find_empty_process() {
    int ret = 0;

    bool is_finded = false;

    for (int i = 0; i < NR_TASKS; ++i) {
        if (0 == tasks[i]) {
            is_finded = true;

            ret = i;
            break;
        }
    }

    if (!is_finded) {
        printk("no valid pid\n");
        return -1;
    }

    return ret;
}

task_t* task_create(task_fun_t fun, char* name) {
    task_t* task = kmalloc(sizeof(task_t));

    task->state = TASK_INIT;

    task->pid = find_empty_process();
    assert(task->pid < NR_TASKS);

    // 加入任务队列供调度
    tasks[task->pid] = task;

    task->function = fun;

    strcpy(task->name, name);

    // 任务创建完成，设为ready， 等待调度
    task->state = TASK_READY;

    return task;
}

task_fun_t get_task_function(task_t* task) {
    assert(NULL != task);
    assert(NULL != task->function);

    return task->function;
}

void task_init() {
    task_create(idle_task, "idle");
}