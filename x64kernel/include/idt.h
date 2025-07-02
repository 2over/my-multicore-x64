//
// Created by xiehao on 25-7-2.
//

#ifndef MY_MULTICORE_X64_IDT_H
#define MY_MULTICORE_X64_IDT_H

#include "types.h"

typedef struct _idtr_data_t {
    ushort  limit;
    int64   base;
}__attribute__((packed)) idtr_data_t;

typedef struct _idt_item_t {
    short offset0;
    short selector;
    char ist;
    char type : 4;      // 任务门/中断门/陷阱门
    char segment : 1;   // segment = 0 表示系统段
    char dpl : 2;       // 使用int指令访问的最低权限
    char present : 1;   // 是否有效
    short offset1;
    int offset2;
    int reserved;
}__attribute__((pakced)) idt_item_t;

void idt_init();

#endif //MY_MULTICORE_X64_IDT_H
