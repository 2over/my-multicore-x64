//
// Created by xiehao on 25-7-1.
//

#ifndef MY_MULTICORE_X64_KERNEL_H

#include "types.h"
#include "stdarg.h"

#define CLI     asm volatile("cli;");
#define STI     asm volatile("sti;");
#define PAUSE   asm volatile("pause;");

int vsprintf(char *buf, const char *fmt, va_list args);

int printk(const char * fmt, ...);

int printk_intr(const char* fmt, ...);
#define MY_MULTICORE_X64_KERNEL_H

#endif //MY_MULTICORE_X64_KERNEL_H
