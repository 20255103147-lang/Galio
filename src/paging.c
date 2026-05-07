/* paging.c - Virtual memory paging */
#include "paging.h"
#include "pmem.h"
#include "kprintf.h"

#define PAGE_SIZE 4096
#define TABLE_SIZE 1024

static page_directory_t *kernel_pd = NULL;

/* Assembly function to enable paging */
extern void paging_enable_asm(u32 pd_addr);

void paging_init(void) {
    kprintf("Initializing paging system...\n");
    kernel_pd = paging_create_directory();
    
    /* Identity map first 4MB for bootloader */
    for (u32 i = 0; i < 1024; i++) {
        u32 phys = i * PAGE_SIZE;
        paging_map(kernel_pd, phys, phys, PAGE_PRESENT | PAGE_RW);
    }

    /* Identity map kernel space (1MB - 4MB) */
    for (u32 i = 256; i < 1024; i++) {
        u32 phys = i * PAGE_SIZE;
        paging_map(kernel_pd, phys, phys, PAGE_PRESENT | PAGE_RW);
    }

    /* Enable paging */
    paging_enable(kernel_pd);
    kprintf("Paging enabled\n");
}

page_directory_t *paging_create_directory(void) {
    page_directory_t *pd = (page_directory_t *)pmem_alloc(1);
    if (!pd) return NULL;

    /* Allocate and initialize tables */
    for (int i = 0; i < 1024; i++) {
        pd->page_tables[i] = (u32 *)pmem_alloc(1);
        if (!pd->page_tables[i]) {
            return NULL;
        }
        
        /* Clear page table */
        for (int j = 0; j < 1024; j++) {
            pd->page_tables[i][j] = 0;
        }
    }

    return pd;
}

void paging_map(page_directory_t *pd, u32 vaddr, u32 paddr, u32 flags) {
    u32 pd_index = vaddr / (PAGE_SIZE * TABLE_SIZE);
    u32 pt_index = (vaddr / PAGE_SIZE) % TABLE_SIZE;

    if (!pd->page_tables[pd_index]) {
        pd->page_tables[pd_index] = (u32 *)pmem_alloc(1);
        if (!pd->page_tables[pd_index]) {
            kprintf("paging_map: Failed to allocate page table\n");
            return;
        }
        for (int i = 0; i < 1024; i++) {
            pd->page_tables[pd_index][i] = 0;
        }
    }

    /* Set page table entry */
    u32 pte = (paddr & 0xFFFFF000) | flags;
    pd->page_tables[pd_index][pt_index] = pte;

    /* Invalidate TLB entry */
    __asm__ volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
}

void paging_unmap(page_directory_t *pd, u32 vaddr) {
    u32 pd_index = vaddr / (PAGE_SIZE * TABLE_SIZE);
    u32 pt_index = (vaddr / PAGE_SIZE) % TABLE_SIZE;

    if (pd->page_tables[pd_index]) {
        pd->page_tables[pd_index][pt_index] = 0;
        __asm__ volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
    }
}

u32 paging_get_physical(page_directory_t *pd, u32 vaddr) {
    u32 pd_index = vaddr / (PAGE_SIZE * TABLE_SIZE);
    u32 pt_index = (vaddr / PAGE_SIZE) % TABLE_SIZE;

    if (!pd->page_tables[pd_index]) {
        return 0;
    }

    u32 pte = pd->page_tables[pd_index][pt_index];
    if (!(pte & PAGE_PRESENT)) {
        return 0;
    }

    return (pte & 0xFFFFF000) | (vaddr & 0xFFF);
}

void paging_enable(page_directory_t *pd) {
    kernel_pd = pd;
    paging_enable_asm((u32)pd);
}

page_directory_t *paging_get_current(void) {
    return kernel_pd;
}
