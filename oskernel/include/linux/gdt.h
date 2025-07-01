//
// Created by xiehao on 25-7-1.
//

#ifndef MY_MULTICORE_X64_GDT_H
#define MY_MULTICORE_X64_GDT_H

#include "types.h"

#pragma pack(2)

typedef struct _gdtr32_data_t {
    short   limit;
    int     base;
} gdtr32_data_t;
#pragma pack()

typedef struct _gdt_item_t {
    unsigned short limit_low;       // 段界限 0 ～ 15位
    unsigned int base_low : 24;     // 基地址0 ~ 23位 16M
    unsigned char type: 4;          // 段类型
    unsigned char segment : 1;      // 1 表示代码段或数据段, 0 表示系统段
    unsigned char DPL : 2;          // Descriptor Privilege Level 描述符特权等级 0 ～ 3
    unsigned char present : 1;      // 存在位 1 在内存中， 0 在磁盘上
    unsigned char limit_high : 4;   // 段界限 16 ~ 19
    unsigned char available : 1;    // 该安排的都安排了，送给操作系统了
    unsigned char long_mode : 1;    // 64位扩展标志
    unsigned char big : 1;          // 32位 还是 16位
    unsigned char granularity : 1;  // 粒度4KB 还是 1B
    unsigned char base_high;        // 基地址 24 ～ 31位
} __attribute__((packed)) gdt_item_t;


void install_x64_descriptor();


#endif //MY_MULTICORE_X64_GDT_H
