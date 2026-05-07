/* paging.c - Virtual memory paging */
#include "paging.h"
#include "pmem.h"
#include "kprintf.h"

#define PAGE_SIZE 4096
#define TABLE_SIZE 1024

static page_directory_t kernel_pd_storage;
static page_directory_t *kernel_pd = NULL;

/* Assembly function to enable paging */
extern void paging_enable_asm(u32 pd_addr);

void paging_init(void) {
    kprintf("Initializing paging system...\n");
    kernel_pd = paging_create_directory();
    if (!kernel_pd) {
        kprintf("paging_init: Failed to create page directory\n");
        panic("Paging initialization failed");
    }
    
    kprintf("paging_init: Page directory created at %p (phys=%x)\n", kernel_pd->directory, (u32)kernel_pd->directory);

    /* Identity map first 4MB of memory so the kernel is still valid after paging is enabled */
    kprintf("paging_init: Identity-mapping first 4 MB...\n");
    u32 count = 0;
    for (u32 i = 0; i < 4 * 1024 * 1024; i += PAGE_SIZE) {
        paging_map(kernel_pd, i, i, PAGE_PRESENT | PAGE_RW);
        count++;
    }
    kprintf("paging_init: Mapped %u pages\n", count);

    /* Enable paging */
    kprintf("paging_init: Enabling paging with CR3=%x...\n", (u32)kernel_pd->directory);
    paging_enable(kernel_pd);
    kprintf("Paging enabled successfully\n");
}

page_directory_t *paging_create_directory(void) {
    /* Try to allocate an aligned page directory */
    u32 pd_phys = pmem_alloc(1);
    if (!pd_phys) {
        kprintf("paging_create_directory: Failed to allocate page directory\n");
        return NULL;
    }
    
    /* Check alignment (must be 4 KiB aligned) */
    if (pd_phys & 0xFFF) {
        kprintf("paging_create_directory: Page directory not aligned: %x\n", pd_phys);
        return NULL;
    }
    
    kprintf("paging_create_directory: Allocated PD at phys=%x\n", pd_phys);
    
    /* Store physical address in directory pointer */
    kernel_pd_storage.directory = (u32 *)pd_phys;
    
    /* Before writing, verify the address is accessible (should be identity-mapped) */
    /* Initialize directory entries to all zeros by writing to physical address */
    kprintf("paging_create_directory: Clearing directory at virt=%x\n", (u32)kernel_pd_storage.directory);
    for (int i = 0; i < 1024; ++i) {
        kernel_pd_storage.directory[i] = 0;
        kernel_pd_storage.tables[i] = NULL;
    }
    
    kprintf("paging_create_directory: Directory initialized successfully\n");
    return &kernel_pd_storage;
}

void paging_map(page_directory_t *pd, u32 vaddr, u32 paddr, u32 flags) {
    u32 pd_index = (vaddr >> 22) & 0x3FF;
    u32 pt_index = (vaddr >> 12) & 0x3FF;

    /* Create page table if it doesn't exist */
    if (!pd->tables[pd_index]) {
        u32 pt_phys = pmem_alloc(1);
        if (!pt_phys) {
            kprintf("paging_map: Failed to allocate page table\n");
            return;
        }
        
        /* Check alignment */
        if (pt_phys & 0xFFF) {
            kprintf("paging_map: Page table not aligned: %x\n", pt_phys);
            return;
        }
        
        pd->tables[pd_index] = (u32 *)pt_phys;
        
        /* Clear the page table (write to physical address/virtual mapping) */
        for (int i = 0; i < 1024; ++i) {
            pd->tables[pd_index][i] = 0;
        }
        
        /* Set PDE to point to the page table with present and RW flags */
        u32 pde = (pt_phys & 0xFFFFF000) | PAGE_PRESENT | PAGE_RW;
        pd->directory[pd_index] = pde;
        kprintf("paging_map: Created page table [%d] at phys=%x, PDE=%x\n", pd_index, pt_phys, pde);
    }

    /* Set PTE to map vaddr to paddr with given flags */
    u32 pte = (paddr & 0xFFFFF000) | flags;
    pd->tables[pd_index][pt_index] = pte;
    
    __asm__ volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
}

void paging_unmap(page_directory_t *pd, u32 vaddr) {
    u32 pd_index = (vaddr >> 22) & 0x3FF;
    u32 pt_index = (vaddr >> 12) & 0x3FF;

    if (pd->tables[pd_index]) {
        pd->tables[pd_index][pt_index] = 0;
        __asm__ volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
    }
}

u32 paging_get_physical(page_directory_t *pd, u32 vaddr) {
    u32 pd_index = (vaddr >> 22) & 0x3FF;
    u32 pt_index = (vaddr >> 12) & 0x3FF;

    if (!pd->tables[pd_index]) {
        return 0;
    }

    u32 pte = pd->tables[pd_index][pt_index];
    if (!(pte & PAGE_PRESENT)) {
        return 0;
    }

    return (pte & 0xFFFFF000) | (vaddr & 0xFFF);
}

void paging_enable(page_directory_t *pd) {
    if (!pd || !pd->directory) {
        kprintf("paging_enable: Invalid page directory\n");
        return;
    }
    
    kernel_pd = pd;
    u32 pd_addr = (u32)pd->directory;
    kprintf("paging_enable: Loading CR3 with %x, enabling paging...\n", pd_addr);
    
    paging_enable_asm(pd_addr);
    
    kprintf("paging_enable: Paging is now active\n");
}

page_directory_t *paging_get_current(void) {
    return kernel_pd;
}
