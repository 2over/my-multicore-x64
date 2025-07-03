#include "../include/mm.h"
#include "../include/kernel.h"
#include "../include/assert.h"

#define FOUR_LEVEL_HEAD_TABLE_ADDR  0x8000
#define FIRST_PDPT_ADDR             0x9000
#define FIRST_PDT_ADDR              0xa000

#define PML4_SHIFT  39
#define PDPT_SHIFT  30
#define PDT_SHIFT   21

#define INDEX_MASK  0x1FF // 用于获取9位索引的掩码

static void get_indexes(uint64_t addr, uint64_t *pml4_index,uint64_t *pdpt_index, uint64_t* pdt_index) {
    *pml4_index = (addr >> PML4_SHIFT) & INDEX_MASK;
    *pdpt_index = (addr >> PDPT_SHIFT) & INDEX_MASK;
    *pdt_index = (addr >> PDT_SHIFT) & INDEX_MASK;
}

static uint64_t get2mb_page(uint64_t addr) {
    return (addr >> 21) << 21;
}

void physics_map_virtual_addr_2m(uint32_t addr, uint32_t phy_addr_base) {
    assert(NULL != addr);
    assert(addr < 0xffffffff); // 目前只支持4G以内内存，超过4G, 生成描述符代码需要调整

    uint64_t pml4_index, pdpt_index, pdt_index;

    get_indexes(addr, &pml4_index, &pdpt_index, &pdt_index);

    printk("PML4 index : %d\n", pml4_index);
    printk("PDPT index : %d\n", pdpt_index);
    printk("PDT index: %d\n", pdt_index);

    // 看4级头表对应的位置有没有挂pdpt描述符号
    uint64_t* PML4_addr = (uint64_t*)FOUR_LEVEL_HEAD_TABLE_ADDR;
    uint64_t pdpt_descriptor = *(PML4_addr + pml4_index);
    if (0 == pdpt_descriptor) {
        // 需要申请物理页, 并生成描述符挂到4级头表中

        int64 p = (int64)get_free_page();
        *(PML4_addr + pml4_index) = p | 3;
        pdpt_descriptor = p | 3;
        pdpt_descriptor = p | 3;
    }

    printk("PML4: %d->0x%x\n", pml4_index, pdpt_descriptor);

    uint64_t* pdpt_addr = (uint64_t*)(pdpt_descriptor >> 12 << 12);
    uint64_t pdt_descriptor = (uint64_t)*(pdpt_addr + pdpt_index);
    if (0 == pdt_descriptor) {
        // 需要申请物理页，并生成描述符挂到4级头表中

        int64 p = (int64)get_free_page();
        *(pdpt_addr + pdpt_index) = p | 3;
        pdt_descriptor = p | 3;
    }

    printk("pdpt: %d->0x%x\n", pdpt_index, pdt_descriptor);

    uint64_t* pdt_addr = (uint64_t*)(pdt_descriptor >> 12 << 12);
    *(pdt_addr + pdt_index) = phy_addr_base >> 21 << 21 | 0x83;

}