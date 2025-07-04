#include "../include/acpi.h"
#include "../include/assert.h"
#include "../include/kernel.h"
#include "../include/string.h"
#include "../include/mm.h"

#define EBDA_ADDRESS_POINTER 0x40E
#define RSDP_SIGNATURE "RSD PTR "
#define ACPI_SIG 0x20445352
#define APIC_SIG 0x43495041

extern srat_t* g_srat;

rsdp_t* g_rsdp;
rsdt_t* g_rsdt;
madt_t* g_apic;

// CPU书来那个
uint32_t g_cpu_number;

local_apic_t  g_local_apic[CPU_MAX];
io_apic_t g_io_apic[CPU_MAX];

uint8_t* g_local_apic_addr;

int* find_rsdp() {
    // check EBDA
    ushort ebda_base = *((ushort*) EBDA_ADDRESS_POINTER);
    ebda_base <<= 4; // Convert from segment to physical address

    for (uint addr = ebda_base; addr < ebda_base + 0x400; addr += 16) {
        if (ACPI_SIG == *(int*)addr) {
            return (int*)addr;
        }
    }

    // check main BIOS area
    for (uint addr = 0xE0000; addr < 0x100000; addr += 16) {
        if (ACPI_SIG == *(int*)addr) {
            return (int*)addr;
        }
    }

    return NULL; // RSDP not found
}

static uint8_t compute_checksum(void* addr, size_t len) {
    uint8_t *p = addr;
    uint8_t sum = 0;

    for (size_t i = 0; i < len; i++) {
        sum += p[i];
    }

    return sum;
}

void print_rsdp_info() {
    rsdp_t* rsdp = find_rsdp();

    if (NULL == rsdp) {
        printk("not found rsdp, exit...\n");
        while(true);
    }

    printk("rsdp address : %08x\n", rsdp);

    // 判断校验和
    if (0 == rsdp->revision) {
        // 如果修订号为0,表示ACPI 1.0,只有结构中的前20字节
        if (0 != compute_checksum(rsdp, 20)) {
            printk("rsdp checksum check fail...\n");
            while(true);
        }

        printk("rsdp revision: %d->ACPI 1.0\n", rsdp->revision);
    } else {
        printk("ACPI2.0, pass....\n");
        while(true);
    }

    char buf[10] = {0};

    memcpy(buf, rsdp->signature, 8);
    printk("rsdp signature: %s\n", buf);

    memset(buf, 10, 0);
    memcpy(buf, rsdp->oem_id, 8);
    printk("rsdp oem_id: %s\n", buf);

    printk("rsdp address: 0x%08x\n", rsdp->rsdt_address);

    if (2 == rsdp->revision) {
        // 输出属于ACPI2.0 的数据，用不上
    }
}

void find_apic() {
    assert(NULL != g_rsdt);

    // 检查checksum
    if (0 != compute_checksum(g_rsdt, g_rsdt->header.length)) {
        panic("rsdt data error!\n");
    }

    for (int i = 0; i < 8; ++i) {
        if (0 == g_rsdt->entry[i]) continue;

        // 0x43495041是APIC的ASCII码
        if (APIC_SIG == *(int*)g_rsdt->entry[i]) {
            g_apic = g_rsdt->entry[i];
            g_local_apic_addr = g_apic->local_controller_address;
            printk("APIC addr: 0x%08x\n", g_apic);
            printk("local_controller_address:: 0x%08x\n", g_local_apic_addr);
            break;
        }
    }

    if (NULL == g_apic) {
        panic("not found APIC!\n");
    }
}

// 遍历中断控制器结构列表
void get_apic_info() {
    assert(NULL != g_apic);

    // 检查checksum
    if (0 != compute_checksum(g_apic, g_apic->header.length)) {
        panic("APIC data error!\n");
    }

    uint8_t* addr = &g_apic->table;
    uint8_t* addr_end = (uint8_t*)g_apic + g_apic->header.length;

    while(addr < addr_end) {
        if (0 == *addr) {
            local_apic_t *apic = addr;

            // CPU数量+  1
            g_cpu_number++;
            assert(g_cpu_number <= CPU_MAX);

            // 将local apic信息copy到全局变量中
            local_apic_t* des = (local_apic_t*)((uint8_t*)&g_local_apic + apic->apic_id * sizeof(local_apic_t));
            memcpy(des, apic, apic->header.length);

            addr += apic->header.length;
        } else if (1 == *addr) {
            io_apic_t* apic = addr;

            io_apic_t *des = (io_apic_t*)((uint8_t*)&g_io_apic + apic->io_apic_id * sizeof(io_apic_t));
            memcpy(des, apic, apic->header.length);

            physics_addr_map_virtual_addr_2m_2(apic->io_apic_address, apic->io_apic_address, 0x9f, 0x1f);

            printk("io apic address: 0x%08x\n", apic->io_apic_address);

            addr += apic->header.length;
        } else {
            // 不管是什么结构，第二个参数都是这个结构的长度
            addr += *(addr + 1);
        }
    }
}

void acpi_init() {
    g_rsdp = find_rsdp();

    if (0 == g_rsdp->revision) {
        // 如果修订号为0,表示ACPI1.0,只有结构中的前20字节
        if (0 != compute_checksum(g_rsdp, 20)) {
            printk("rsdp checksum check fail...\n");
            while(true);
        }

        printk("rsdp revision: %d->ACPI 1.0\n", g_rsdp->revision);

        // 为RSDT address做虚拟内存一个内设
        {
            char* addr = g_rsdp->rsdt_address;
            printk("rsdt address: 0x%08x\n",addr);

            g_rsdt = addr;
            physics_map_virtual_addr_2m(addr, addr);

        }

        // 找到APIC
        {
            find_apic();
            physics_addr_map_virtual_addr_2m_2(g_apic->local_controller_address,
                                               g_apic->local_controller_address, 0x9f, 0x1f);
        }

        // 获取local apic与io apic信息
        {
            get_apic_info();
        }

    } else {
        printk("ACPI2.0, pass ...\n");
        while(true);
    }
}

void print_rsdt_info() {
    assert(NULL != g_rsdt);

    char buf[10] = {0};

    // 检查checksum
    if (0 != compute_checksum(g_rsdt, g_rsdt->header.length)) {
        panic("rsdt data error!\n");
    }

    printk("===== start: RSDT INFO =====\n");
    printk("rsdt addr: 0x%08x\n", g_rsdt);
    printk("rsdt length: %d\n", g_rsdt->header.length);
    printk("rsdt entry addr: 0x%08x\n", (char*)g_rsdt + sizeof(acpi_sdt_header_t));

    memcpy(buf, g_rsdt->header.signature, 4);
    printk("rsdt signature: %s\n", buf);

    printk("rsdt revision: %d\n", g_rsdt->header.revision);

    memset(buf, 0, 10);
    memcpy(buf, g_rsdt->header.oem_id, 6);
    printk("rsdt oem_id: %s\n", buf);

    memset(buf,0 ,10);
    memcpy(buf, g_rsdt->header.oem_table_id, 8);
    printk("rsdt oem_table_id: %s\n", buf);

    printk("rsdt oem_revision: 0x%08x\n", g_rsdt->header.oem_revision);
    printk("rsdt creator_id :0x%08x\n", g_rsdt->header.creator_id);
    printk("rsdt creator_revision: 0x%08x\n", g_rsdt->header.creator_revision);

    printk("rsdt MADT list: \n");
    for (int i = 0; i < 8; ++i) {
        if (0 == g_rsdt->entry[i]) continue;

        printk("    0x%08x\n", g_rsdt->entry[i]);
    }

    printk("===== end : RSDT INFO =====\n");

}

void print_apic_info() {
    assert(NULL != g_apic);

    // 检查checksum
    if (0 != compute_checksum(g_apic, g_apic->header.length)) {
        panic("APIC data error!\n");
    }

    printk("===== start: APIC INFO =====\n");

    printk("APIC local_controller_address : 0x%08x\n", g_apic->local_controller_address);
    printk("APIC flags: 0x%08x\n", g_apic->flags);

    // 遍历中断控制寄存器列表
    uint8_t* addr = &g_apic->table;
    uint8_t* addr_end = (uint8_t*)g_apic + g_apic->header.length;

    while (addr < addr_end) {
        if (0 == *addr) {
            local_apic_t *apic = addr;

            printk("-----------\n");
            printk("Local APIC len: %d\n", apic->header.length);
            printk("acpi_processor_id: 0x%02x\n", apic->acpi_processor_id);
            printk("apic_id: 0x%02x\n", apic->apic_id);
            printk("flags: 0x%08x\n", apic->flags);
            printk("-----------\n");

            addr += apic->header.length;
        } else if (1 == *addr) {
            io_apic_t* apic = addr;

            printk("-----------\n");
            printk("IO APIC len: %d\n", apic->header.length);
            printk("io_apic_id: 0x%02x\n", apic->io_apic_id);
            printk("io_apic_address: 0x%08x\n", apic->io_apic_address);
            printk("global_system_interrupt_base: 0x%08x\n", apic->global_system_interrupt_base);
            printk("-----------\n");
            addr += apic->header.length;
        } else {
            addr += *(addr + 1);
        }
    }
    printk("===== end: APIC INFO =====\n");


}

void print_sart_info() {
    assert(NULL != g_srat);

    char buf[0x10] = {0};

    // 检查checksum
    if (0 != compute_checksum(g_srat, g_srat->header.length)) {
        panic("SRAT data error!\n");
    }

    printk("===== start: SRAT INFO =====\n");
    printk("SRAT addr: 0x%08x\n", g_srat);
    printk("SRAT length: %d\n", g_srat->header.length);
    printk("SRAT entry addr: 0x%08x\n", &(g_srat->table));

    memcpy(buf, g_srat->header.signature, 4);
    printk("SRAT signature: %s\n", buf);

    printk("SRAT revision: %d\n", g_srat->header.revision);

    memset(buf, 0, 10);
    memcpy(buf, g_srat->header.oem_id, 6);
    printk("SRAT oem_id: %s\n", buf);

    memset(buf, 0, 10);
    memcpy(buf, g_srat->header.oem_table_id, 8);
    printk("SRAT oem_table_id: %s\n", buf);

    printk("SRAT oem_revision: 0x%08x\n", g_srat->header.oem_revision);
    printk("SRAT creator_id: 0x%08x\n", g_srat->header.creator_id);
    printk("SRAT creator_revision: 0x%08x\n", g_srat->header.creator_revision);

    {
        uint8_t* addr = &g_srat->table;

        uint8_t* addr_end = (uint8_t*)g_srat + g_srat->header.length;
        while (addr < addr_end) {
            if (0 == *addr) {
                srat_lapic_sapic_affinity* sapic =addr;

                addr += sapic->length;
            } else if (1 == *addr) {
                srat_lapic_sapic_affinity* mem = addr;
                print_srat_mem_affinity_info(mem);

                addr += mem->length;
            } else {
                // 这里为什么这样写？因为此表中的每个value对应的结构的length在index=1的地方
                addr += *(addr + 1);
            }
        }
    }

    printk("===== end: SRAT INFO =====\n");
}

void print_srat_lapic_sapic_affinity_info(srat_lapic_sapic_affinity* p) {
    assert(NULL != p);

    printk("lapic_sapic_affinity domain: %d %d %d %d\n",
           p->proximity_domain_low, p->proximity_domain_high[0],
           p->proximity_domain_high[1], p->proximity_domain_high[2]);

    printk("apic_id: %d, flags: %d\n", p->apic_id, p->flags);

    printk("local_sapic_eid: %d, clock_domain: %d\n", p->local_sapic_eid, p->clock_domain);
}

void print_srat_mem_affinity_info(srat_mem_affinity* p) {
    assert(NULL != p);

    printk("------------------------------------------\n");
    printk("proximity_domain: %d, flags: %d\n", p->proximity_domain, p->flags);
    printk("from : %x%08x, len: %x%08x\n", p->base_addr_high, p->base_addr_low, p->length_high, p->length_low);

    printk("------------------------------------------\n");
}

