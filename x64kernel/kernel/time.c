#include "../include/time.h"
#include "../include/io.h"
#include "../include/kernel.h"
#include "../include/idt.h"

#define CMOS_ADDR 0x70 // CMOS 地址寄存器
#define CMOS_DATA 0x71 // CMOS 数据寄存器

// CMOS 信息的寄存器索引
#define CMOS_SECOND 0x00    // (0 ~ 59)
#define CMOS_MINUTE 0x02    // (0 ~ 59)
#define CMOS_HOUR 0x04      // (0 ~ 23)
#define CMOS_WEEKDAY 0x06   // (1 ~ 7) 星期天 = 1, 星期六 = 7
#define CMOS_DAY 0x07       // (1 ~ 31)
#define CMOS_MONTH 0x08     // (1 ~ 12)
#define CMOS_YEAR 0x09      // (0 ~ 99)
#define CMOS_CENTURY 0x32   // 可能不存在
#define CMOS_NMI 0x80

#define MINUTE 60           // 每分钟的秒数
#define HOUR (60 * MINUTE)  // 每小时的秒数
#define DAY (24 * HOUR)     // 每天的秒数
#define YEAR (365 * DAY)    // 每年的秒数，以365天算

static int month[13] = {
        0, // 这里占位, 没有0月，从1月开始
        0,
        (31),
        (31 + 29),
        (31 + 29 + 31),
        (31 + 29 + 31 + 30),
        (31 + 29 + 31 + 30 + 31),
        (31 + 29 + 31 + 30 + 31 + 30),
        (31 + 29 + 31 + 30 + 31 + 30 + 31),
        (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
        (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
        (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
        (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)
};

typedef u32 time_t;

extern long startup_time;

int century;

u8 bcd_to_bin(u8 value) {
    return (value & 0xf) + (value >> 4) * 10;
}

u8 bin_to_bcd(u8 value) {
    return (value / 10) * 0x10 + (value % 10);
}

time_t mktime(tm *time) {
    time_t res;
    int year; // 1970年开始的年数
    // 下面从1900年开始的年数计算
    if (time->tm_year >= 70) {
        year = time->tm_year - 70;
    } else {
        year = time->tm_year - 70 + 100;
    }

    // 这些年经过的秒数时间
    res = YEAR * year;

    // 已经过去的润年, 每个加1天
    res += DAY * ((year + 1) / 4);

    // 已经过完的月份的时间
    res += month[time->tm_mon] * DAY;

    // 如果2月已经过了，并且当前不是润年，那么减去一天
    if (time->tm_mon > 2 && ((year + 2) % 4)) {
        res -= DAY;
    }

    // 这个月已经过去的天
    res += DAY * (time->tm_mday - 1);

    // 今天过去的小时
    res += HOUR * time -> tm_hour;

    // 这个小时过去的分钟
    res += MINUTE * time -> tm_min;

    // 这个分钟过去的秒
    res += time->tm_sec;
    return res;
}
int get_yday(tm *time) {
    int res = month[time->tm_mon]; // 已经过去的月的天数
    res += time->tm_mday;          // 这个月过去的天数

    int year;
    if (time->tm_year >= 70)
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;

    if ((year + 2) % 4 && time->tm_mon > 2)
    {
        res -= 1;
    }

    return res;
}

u8 cmos_read(u8 addr) {
    out_byte(CMOS_ADDR, CMOS_NMI | addr);
    return in_byte(CMOS_DATA);
};

void time_read_bcd(tm *time) {
    do {
        time->tm_sec = cmos_read(CMOS_SECOND);
        time->tm_min = cmos_read(CMOS_MINUTE);
        time->tm_hour = cmos_read(CMOS_HOUR);
        time->tm_wday = cmos_read(CMOS_WEEKDAY);
        time->tm_mday = cmos_read(CMOS_DAY);
        time->tm_mon = cmos_read(CMOS_MONTH);
        time->tm_year = cmos_read(CMOS_YEAR);
        century = cmos_read(CMOS_CENTURY);
    } while (time->tm_sec != cmos_read(CMOS_SECOND));
}

void time_read(tm *time) {
    time_read_bcd(time);
    time->tm_sec = bcd_to_bin(time->tm_sec);
    time->tm_min = bcd_to_bin(time->tm_min);
    time->tm_hour = bcd_to_bin(time->tm_hour);
    time->tm_wday = bcd_to_bin(time->tm_wday);
    time->tm_mday = bcd_to_bin(time->tm_mday);
    time->tm_mon = bcd_to_bin(time->tm_mon);
    time->tm_year = bcd_to_bin(time->tm_year);
    time->tm_yday = get_yday(time);
    time->tm_isdst = -1;

    century = bcd_to_bin(century);
}

void time_init(void) {
    tm time;
    time_read(&time);
    startup_time = mktime(&time);

//    printk("%d%02d-%02d-%02d ", century, time.tm_year, time.tm_mon, time.tm_mday);
//    printk("%02d:%02d:%02d(%d)\n", time.tm_hour, time.tm_min, time.tm_sec, startup_time);
}

void rtc_interrupt_handler() {

    // 先读寄存器C,判断发生的是什么中断
    // 如果发生周期性终端，位6为1,如果发生了更新结束中断，位4为1
    uchar creg = 0;

    asm volatile (
            "movb $0x8c, %%al;"     // Move 0x0c into AL
            "outb %%al, $0x70;"     // Output AL to port 0x70
            "inb $0x71, %%al;"
            "movb %%al, %0;"        // Input from port 0x71 into AL
            : "=r"(creg)
            :
            : "al", "memory");

    // 发生更新周期结束终端
    if (creg & 0x10) {
        tm time = {0};
        time_read(&time);

        printk("%d%02d-%02d-%02d ", century, time.tm_year, time.tm_mon, time.tm_mday);
        printk("%02d:%02d:%02d\n", time.tm_hour, time.tm_min, time.tm_sec);
    }

    // 发生周期性中断
    if (creg & 0x40) {
        printk("11\n");
    }


    send_eoi(0x28);
}
