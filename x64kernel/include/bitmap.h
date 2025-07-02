//
// Created by xiehao on 25-7-1.
//

#ifndef MY_MULTICORE_X64_BITMAP_H
#define MY_MULTICORE_X64_BITMAP_H

#include "types.h"

typedef struct bitmap_t {
    u8 *bits;   // 位图缓冲区
    u32 length; // 位图缓冲区长度
    u32 offset; // 位图开始的偏移
}bitmap_t;

// 初始化位图
void bitmap_init(bitmap_t *map, char *bits, u32 length, u32 offset);

// 构造位图
void bitmap_make(bitmap_t *map, char *bits, u32 length, u32 offset);

// 测试位图的某一位是否为1
bool bitmap_test(bitmap_t *map, u32 index);

// 设置位图某位的值
void bitmap_set(bitmap_t *map, u32 index, bool value);

// 从位图中得到连续的count位
int bitmap_scan(bitmap_t *map, u32 count);

#endif //MY_MULTICORE_X64_BITMAP_H
