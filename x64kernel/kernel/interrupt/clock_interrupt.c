#include "../../include/task.h"

void clock_interrupt_handler() {
    sched();
}