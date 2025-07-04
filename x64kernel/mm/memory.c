#include "../include/mm.h"
#include "../include/kernel.h"
#include "../include/string.h"
#include "../include/assert.h"

#define ARDS_ADDR 0x7e00

// 把1M以下内存称为无效内存
#define VALID_MEMORY_FROM   0x100000

// 物理内存从10M的位置开始用
#define PHY_MEMORY_USE_FROM 0xa00000

#define ZONE_VALID 1    // ARDS 可用内存区域
#define ZONE_RESERVED 2 // ARDS 不可用区域

physics_memory_info_t g_physics_memory;
physics_memory_map_t g_physics_memory_map;

void phy_memory_init() {
    check_memory_info_t *p = (check_memory_info_t*)ARDS_ADDR;
    check_memory_item_t* p_data = (check_memory_item_t*)(ARDS_ADDR + 2);

    for (int i = 0; i < p->times; ++i) {
        check_memory_item_t* tmp = p_data + i;

        if (tmp->base_addr_low > 0 && tmp->type == ZONE_VALID) {
            g_physics_memory.addr_start = tmp->base_addr_low;
            g_physics_memory.valid_mem_size = tmp->length_low;
            g_physics_memory.addr_end = tmp->base_addr_low + tmp->length_low;
        }
    }

    // 如果没有可用内存
    if (VALID_MEMORY_FROM != g_physics_memory.addr_start) {
        printk("[%s:%d] no valid physics memory\n", __FILE__, __LINE__);
        return;
    }

    g_physics_memory.pages_total = g_physics_memory.addr_end >> 12;
    g_physics_memory.pages_used = 0;
    g_physics_memory.pages_free = g_physics_memory.pages_total - ((PHY_MEMORY_USE_FROM  - VALID_MEMORY_FROM)>> 12);

    {
        g_physics_memory_map.addr_base = PHY_MEMORY_USE_FROM;
        g_physics_memory_map.bitmap_buf = (uchar*)0x500;
        g_physics_memory_map.pages_total = g_physics_memory.pages_free;


        // 初始化为0,防止之前的数据干扰
        memset(g_physics_memory_map.bitmap_buf, 0, 512);
        // 每个byte关联一个page,计算剩余的page需要多少字节
        u32 bitmap_len = g_physics_memory_map.pages_total / 8;

        bitmap_make(&g_physics_memory_map.bitmap, g_physics_memory_map.bitmap_buf, bitmap_len, 0);
    }
}

// 分配、释放物理内存
void* get_free_page() {
    int64 ret;
    int index = bitmap_scan(&g_physics_memory_map.bitmap, 1);
    assert(-1 != index);

    ret = g_physics_memory_map.addr_base + (index << 12);

    return (void*)ret;
}

void free_page(void* p) {
    int index = ((int)p - g_physics_memory_map.addr_base) >> 12;
    assert(index < g_physics_memory_map.bitmap.length);

    bitmap_set(&g_physics_memory_map.bitmap, index, 0);
}

void print_check_memory_info() {
    check_memory_info_t *p = (check_memory_info_t*)ARDS_ADDR;
    check_memory_item_t* p_data = (check_memory_item_t*)(ARDS_ADDR + 2);


    unsigned short times = p->times;

    printk("======== memory check info ========\n");

    for (int i = 0; i < times; ++i) {
        check_memory_item_t* tmp = p_data + i;

        printk("\t from :0x%x%08x,  len: 0x%x%08x, flag: %d\n",
               tmp->base_addr_high, tmp->base_addr_low, tmp->length_high, tmp->length_low, tmp->type);
    }
    printk("======== memory check info ========\n");


}