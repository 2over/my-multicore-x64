//
// Created by xiehao on 25-7-1.
//

#ifndef MY_MULTICORE_X64_STRING_H
#define MY_MULTICORE_X64_STRING_H
#include "types.h"

char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);
size_t strlen(const char* str);
int strcmp(const char* lhs, const char* rhs);
char* strchr(const char* str, int ch);
char* strrchr(const char* str, int ch);

int memcmp(const void* lhs, const void* rhs, size_t count);
void* memset(void* dest, int ch, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
void* memchr(const void* ptr, int ch, size_t count);

#endif //MY_MULTICORE_X64_STRING_H
