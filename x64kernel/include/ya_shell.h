//
// Created by xiehao on 25-7-4.
//

#ifndef MY_MULTICORE_X64_YA_SHELL_H
#define MY_MULTICORE_X64_YA_SHELL_H

#include "types.h"

typedef struct _filepath_parse_data_t{
    u32     data_size;
    char**  data;
}__attribute__((packed)) _filepath_parse_data_t;

bool ya_shell_is_active();
void active_ya_shell();
void close_ya_shell();

void run_ya_shell(char ch);
void exec_ya_shell();
void del_ya_shell();

void print_shell_header();



#endif //MY_MULTICORE_X64_YA_SHELL_H
