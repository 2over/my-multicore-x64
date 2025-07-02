//
// Created by xiehao on 25-7-1.
//

#ifndef MY_MULTICORE_X64_ASSERT_H
#define MY_MULTICORE_X64_ASSERT_H

void assertion_failure(char* exp, char *file, char *base, int line);

#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        assertion_failure(#exp, __FILE__, __BASE_FILE__,__LINE__)                    \
                    \

void panic(const char *fmt);
#endif //MY_MULTICORE_X64_ASSERT_H
