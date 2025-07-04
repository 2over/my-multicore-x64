[SECTION .text]
[BITS 64]

extern send_local_apic_eoi


global switch_task64
switch_task64:

    call send_local_apic_eoi
    ret