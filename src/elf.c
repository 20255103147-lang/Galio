/* elf.c - ELF binary loader */
#include "elf.h"
#include "kprintf.h"
#include "paging.h"
#include "pmem.h"

u32 elf_load(void *elf_data) {
    elf_header_t *header = (elf_header_t *)elf_data;

    /* Validate ELF header */
    if (header->magic != ELF_MAGIC) {
        kprintf("elf_load: Invalid ELF magic\n");
        return 0;
    }

    if (header->ei_class != 1) {  /* 32-bit */
        kprintf("elf_load: Not a 32-bit ELF\n");
        return 0;
    }

    kprintf("ELF entry point: %08X\n", header->e_entry);

    /* Load program headers */
    for (u32 i = 0; i < header->e_phnum; i++) {
        elf_program_header_t *ph = (elf_program_header_t *)((u32)elf_data + header->e_phoff + i * header->e_phentsize);

        if (ph->p_type != 1) {  /* PT_LOAD */
            continue;
        }

        kprintf("Loading segment %u: vaddr=%08X, filesz=%u, memsz=%u\n",
                i, ph->p_vaddr, ph->p_filesz, ph->p_memsz);

        /* Copy segment data */
        u8 *src = (u8 *)elf_data + ph->p_offset;
        u8 *dst = (u8 *)ph->p_vaddr;

        for (u32 j = 0; j < ph->p_filesz; j++) {
            dst[j] = src[j];
        }

        /* Zero out remaining space */
        for (u32 j = ph->p_filesz; j < ph->p_memsz; j++) {
            dst[j] = 0;
        }
    }

    return header->e_entry;
}
