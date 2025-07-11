#include "../include/task.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/assert.h"
#include "../include/string.h"

extern task_t* current;
extern void* kernel_thread(void* arg);

task_t* tasks[NR_TASKS] = {0};


void* t1(void* arg) {
    int processor_id = 0;

    for (int i = 0; i < 500000; ++i) {
        asm volatile("swapgs;"
                     "mov %%gs:0, %0;"
                     "swapgs;":"=r"(processor_id)::"rax");

        printk_fixed_position(160*20, "processor id:%d, t1 %d", processor_id, i);
    }
}

void* t2(void* arg) {
    int processor_id = 0;
    for (int i = 0; i < 500000; ++i) {
        asm volatile("swapgs;"
                     "mov %%gs:0, %0;"
                     "swapgs;":"=r"(processor_id)::"rax");

        printk_fixed_position(160*21, "processor id:%d, t2 %d", processor_id, i);
    }
}

void* t3(void* arg) {
    int processor_id = 0;
    for (int i = 0; i < 500000; ++i) {
        asm volatile("swapgs;"
                     "mov %%gs:0, %0;"
                     "swapgs;":"=r"(processor_id)::"rax");

        printk_fixed_position(160*22, "processor id:%d, t3 %d", processor_id, i);
    }
}

void* t4(void* arg) {
    int processor_id = 0;
    for (int i = 0; i < 500000; ++i) {
        asm volatile("swapgs;"
                     "mov %%gs:0, %0;"
                     "swapgs;":"=r"(processor_id)::"rax");

        printk_fixed_position(160*23, "processor id:%d, t4 %d", processor_id, i);
    }
}
void* idle_task(void* arg) {
    for (int i = 0; i < 100000; ++i) {
        printk("%d\n", i);
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
//            printk("task exit: %s\n", tmp->name);

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

void set_task_ready(task_t* task) {
    assert(NULL != task);

    task->state = TASK_READY;
}

void print_tasks() {
    for (int i = 0; i < NR_TASKS; ++i) {
        task_t* task = tasks[i];
        if (NULL == task) continue;

        char* status = (1 == task->state) ? "running" : "ready";

        printk("[%s]status : %s, sched times : %d, stack: 0x%08x\n", task->name, status, task->scheduling_times, task->esp0);
    }
}

void task_init() {
    task_create(kernel_thread, "kernel_thread");
//    task_create(t1, "t1");
//    task_create(t2, "t2");
//    task_create(t3, "t3");
//    task_create(t4, "t4");
}

void into_to_char(int num, char *str) {
    int i = 0, j, rem, len = 0;
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num != 0) {
        rem = num % 10;
        if (rem > 9) str[i++] = (rem-10) + 'A';
        else str[i++] = rem + '0';
        num = num / 10;
    }

    str[i] = '\0';

    //Reverse the string to get the correct result
    for (j = 0, i = i - 1; j < i; j++, i--) {
        char temp = str[j];
        str[j] = str[i];
        str[i] = temp;
    }
}

void* t_function(void* arg) {
    int processor_id = 0;
    task_t* task = NULL;

    for (int i = 0; i < 200000; ++i) {
        asm volatile("swapgs;"
                     "mov %%gs:0, %0;"
                     "mov %%gs:8, %1;"
                     "swapgs;"
                     :"=r"(processor_id), "=r"(task)::"rax");

        printk_fixed_position(160 * (20 + task->pid), "[%s] processor id : %d, t1: %d, pid: %d", task->name, processor_id, i, task->pid);
    }
}

void task_test(int size) {
    for (int i = 0; i < size; ++i) {
        char name[32] = "t";
        char str[2] = {0};

        into_to_char(i, str);

        strcat(name, str);

        printk("%s\n", name);

        task_create(t_function, name);
    }
}