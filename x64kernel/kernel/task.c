#include "../include/task.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/assert.h"
#include "../include/string.h"

extern task_t* current;

task_t* tasks[NR_TASKS] = {0};

void* idle_task(void* arg) {
    for (int i = 0; i < 10000; ++i) {
        printk("%d\n", *(char*)0x1200);
    }
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
    task->esp0 = kmalloc(4096) + PAGE_SIZE;

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

task_context_t* get_task_context(task_t* task) {
    assert(NULL != task);
    return &task->context;
}

int inc_scheduling_times(task_t* task) {
    return task->scheduling_times++;
}

void task_exit(task_t* task, int exit_code) {
    assert(NULL != task);

    for (int i = 0; i < NR_TASKS; ++i) {
        task_t* tmp = tasks[i];

        if (task == tmp) {
            printk("task exit: %s\n", tmp->name);

            tmp->exit_code = exit_code;

            // 先移除，后面有父子进程再响应处理
            tasks[i] = NULL;

            current = NULL;

            kfree_s(task->esp0, 4096);
            kfree_s(task, sizeof(task_t));

            break;
        }
    }
}

int64 get_esp0(task_t* task) {
    assert(NULL != task);
    return task->esp0;
}
void task_init() {
    task_create(idle_task, "idle");
}