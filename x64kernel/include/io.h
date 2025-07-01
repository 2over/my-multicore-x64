//
// Created by xiehao on 25-7-1.
//

#ifndef MY_MULTICORE_X64_IO_H
#define MY_MULTICORE_X64_IO_H

char in_byte(int port);
short in_word(int port);

void out_byte(int port, int v);
void out_word(int port, int v);

#endif //MY_MULTICORE_X64_IO_H
