#include "../../include/task.h"
#include "../../include/acpi.h"

void clock_interrupt_handler() {
    send_local_apic_eoi();

    sched();
}