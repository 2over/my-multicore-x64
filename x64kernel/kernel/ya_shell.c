#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/task.h"
#include "../include/ya_shell.h"
#include "../include/string.h"
#include "../include/assert.h"
#include "../include/cpu.h"
#include "../include/tty.h"
#include "../include/cpu.h"

extern uint32_t g_cpu_number;
extern void fs_test();
extern void test_page_fault(const char* param);

extern task_t* current;

bool g_active_shell = false;

// 用于存储键盘收入的shell命令
ushort g_shell_command_off = 0;
char g_shell_command[64] = {0};

static char *shell_functions[] = {

        "clear: clear the screen. example: clear",
        "tasks : view all tasks. example: tasks",
        "task count : create tasks. example: task 2",
        "cpus: view all CPU info. example: cpus",
        "run: all cpus grab the task. example: run",
        "cpu index: specify a CPU. example: cpu 1",
        "r [gdtr|idtr]: read gdtr | idtr register. example: r gdtr",
        "gdt: view all segments of the CPU. example: gdt",
        "idt: view all interrupt handling table. example: idt",
        "mm: view memory info. example: mm",
        "mmu addr: simulate the working principle of MMU. example: mmu 0",
};

static void print_shell_functions() {
    for (int i = 0; i < sizeof(shell_functions) / sizeof(char*); ++i) {
        printk("    %s\n", shell_functions[i]);
    }
}

bool ya_shell_is_active() {
    return true == g_active_shell;
}

void active_ya_shell() {
    g_active_shell = true;

    printk("ya shell activated! All functions are: \n");

    print_shell_functions();

    int processor_id = 0;

    asm volatile("swapgs;"
                 "mov %%gs:0, %0;"
                 "swapgs"
                 :"=r"(processor_id)::"rax");

    printk("[%d cover:/]:", processor_id);
}

void close_ya_shell() {
    g_active_shell = false;
}

void run_ya_shell(char ch) {
    if (!g_active_shell) return;

    if (g_shell_command_off >= 64) {
        panic("The command length exceeds 64");
    }

    g_shell_command[g_shell_command_off++] = ch;
}


/**
 * 解析用户输入的shell字符串有几个有限字符串(这些字符串就是命令 + 参数)
 */
static int shell_command_size() {
    int size = 0;

    int off = 0;
    char ch;
    while ('\0' != (ch = g_shell_command[off++])) {
        if (' ' == ch) {
            size++;

            // 后面接着的空格耗完
            while (' ' == g_shell_command[off]) {
                off++;
            }
        }
    }

    return size;
}

static pchar* parse_shell_command(OUT int* arr_len) {
    pchar* ret = kmalloc(4096);

    int size = 0;   // 碰到一个空格+ 1, 连续的空格不算
    int off = 0;    // 字符串游标
    int command_len = 0;    // 命令长度
    int command_start = 0;  // 命令或参数开始的位置

    char ch;

    while ('\0' != (ch = g_shell_command[off++])) {
        if (' ' != ch) {
            command_len++;
        }

        if ('\0' == g_shell_command[off]) {
            pchar str = kmalloc(4096);

            memset(str, 0, command_len + 1);

            memcpy(str, &g_shell_command[command_start], command_len);

            ret[size++] = str;

            break;
        }

        if (' ' == ch) {
            pchar str = kmalloc(4096);
            memset(str, 0, command_len + 1);

            memcpy(str, &g_shell_command[command_start], command_len);

            ret[size++] = str;

            command_len = 0;
            command_start = off;

            while (' ' == g_shell_command[off]) {
                command_start++;

                off++;
            }
        }
    }

    if (NULL != arr_len) {
        *arr_len = size;
    }

    return ret;
}

int string_to_int(char *str) {
    int result = 0;
    int sign = 1;

    // 处理可能的负数符号
    if (*str == '-') {
        sign = -1;
        str++;  // 移动到下一个字符
    }

    while (*str) {
        if (*str < '0' || *str > '9') {
            return 0; // 字符串中有非数字字符，返回0
        }

        result = result * 10 + (*str - '0');
        str++; // 移动到下一个字符
    }

    return sign * result;
}

int hex_digit_to_int(char ch) {
    if ('0' <= ch && ch <= '9') {
        return ch - '0';
    }
    if ('a' <= ch && ch <= 'f') {
        return ch - 'a' + 10;
    }

    if ('A' <= ch && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}

long hex_to_int(char* hex) {
    long result = 0;
    while (*hex) {
        int value = hex_digit_to_int(*hex++);
        if (value == -1) return -1; // 非法十六进制字符串
        result = result * 16 + value;
    }

    return result;
}

void exec_ya_shell() {
    if (!g_active_shell || 0 == g_shell_command_off) {
        goto end;
    }

    // 解析用户输入的shell字符串
    int commands_len = 0;
    pchar* commands = parse_shell_command(&commands_len);

    // 清空用户输入的shell字符串
    memset(g_shell_command, 0, 64);
    g_shell_command_off = 0;

    if (!strcmp("h", commands[0]) || !strcmp("help", commands[0])) {
        print_shell_functions();
    } else if (!strcmp("run", commands[0])) {
        console_init();
        cpu_broadcast();
    } else if (!strcmp("clear", commands[0])) {
        console_init();
    } else if (!strcmp("mm", commands[0])) {
        print_check_memory_info();
    } else if (!strcmp("cpus", commands[0])) {
        print_cpu_info();
    } else if(!strcmp("tasks", commands[0])) {
        print_tasks();
    } else if (!strcmp("r", commands[0])) {
        if (!strcmp("idtr", commands[1])) {
            print_idtr();
        } else {
            print_gdtr();
        }
    } else if (!strcmp("task", commands[0])) {
        int num = string_to_int(commands[1]);
        if (num > 4) {
            printk(" Not more than 4!\n");
        } else {
            task_test(num);
        }
    } else if (!strcmp("cpu", commands[0])) {
        int num = string_to_int(commands[1]);
        if (num > g_cpu_number) {
            printk("Exceed CPU count : %d!\n", g_cpu_number);
        } else {
            console_init();
            cpu_signal(num);
        }
    } else if (!strcmp("mmu", commands[0])) {
        long num = hex_to_int(commands[1]);
        print_viraddr_phyaddr(num);
    } else if (!strcmp("idt", commands[0])) {
        print_idt();
    } else if (!strcmp("gdt", commands[0])) {
        print_gdt();
    } else {
        for (int i = 0; i < commands_len; ++i) {
            printk("%s\n", commands[i]);
        }

        printk("\n");
    }

    g_shell_command_off = 0;
    memset(g_shell_command, 0, 64);

    end:
    {
        int proceesor_id = 0;
        asm volatile("swapgs;"
                     "mov %%gs:0, %0;"
                     "swapgs"
                     :"=r"(proceesor_id)::"rax");

        printk("[%d cover/]:", proceesor_id);
    };
}

/**
 * 如果按了删除键，前面输入的内容要从g_shell_command中删掉
 */
void del_ya_shell() {
    if (!g_active_shell) return;

    g_shell_command[--g_shell_command_off] = 0;
}

void print_shell_header() {
    int processor_id = 0;
    asm volatile("swapgs;"
                 "mov %%gs:0, %0;"
                 "swapgs;"
                 :"=r"(processor_id)::"rax");

    printk("[%d cover:/]:", processor_id);
}
