
#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/string.h"
#include "../include/linux/gdt.h"

#define FOUR_LEVEL_HEAD_TABLE_ADDR 0x8000
#define FIRST_PDPT_ADDR 0x9000
#define FIRST_PDT_ADDR 0xa000

extern void x64_cpu_check();

static void prepare_4level_page_table() {
    // 准备4级头表
    int* four_level_head_table_addr = (int*)FOUR_LEVEL_HEAD_TABLE_ADDR;

    memset(four_level_head_table_addr, 0, 4096);
    *four_level_head_table_addr = FIRST_PDPT_ADDR | 3;
    *(four_level_head_table_addr + 1) = 0;

    // 页目录指针表
    int* pdpt_addr = (int*)FIRST_PDPT_ADDR;

    memset(pdpt_addr, 0, 4096);

    *pdpt_addr = FIRST_PDT_ADDR | 3;
    *(pdpt_addr + 1) = 0;

    // 页目录表
    int* pdt_addr = (int*)FIRST_PDT_ADDR;
    memset(pdt_addr, 0, 4096);

    // 采用2M分页，这里直接填写低端2M内存映射
    *pdt_addr = 0 | 0x83;
    *(pdt_addr + 1) = 0;

    *(pdt_addr + 2) = 0x200000 | 0x83;
    *(pdt_addr + 3) = 0;

    asm volatile("xchg bx, bx; mov cr3, ebx"::"b"(four_level_head_table_addr));
}

static void enter_x64() {
    prepare_4level_page_table();

    // 开启物理地址扩展功能: PAE cr4.pae = 1
    asm volatile("xchg bx, bx; mov eax, cr4; bts eax, 5; mov cr4, eax;");

    // 设置型号专属寄存器IA32_EFER.LME. 允许IA_32e模式
    asm volatile("mov ecx, 0x0c0000080; rdmsr; bts eax, 8; wrmsr");

    // 开启分页cr0.pg = 1(必须在做完上面两部才能开启分页,否则会出错)
    asm volatile("mov eax, cr0; or eax, 0x80000000; mov cr0, eax;");

    BOCHS_DEBUG_MAGIC
    BOCHS_DEBUG_MAGIC

    install_x64_descriptor();
}

void kernel_main(void) {
    console_init();

    x64_cpu_check();
    enter_x64();

    while(true);
}