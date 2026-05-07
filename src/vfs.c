/* vfs.c - Virtual Filesystem Layer */
#include "vfs.h"
#include "kprintf.h"

static initrd_header_t *initrd = NULL;

void vfs_init(void *initrd_addr) {
    initrd = (initrd_header_t *)initrd_addr;
    
    if (!initrd) {
        kprintf("VFS: No initrd mounted\n");
        return;
    }

    kprintf("VFS initialized, %u files available\n", initrd->file_count);
}

file_entry_t *vfs_find_file(const char *name) {
    if (!initrd) return NULL;

    for (u32 i = 0; i < initrd->file_count; i++) {
        if (__builtin_strcmp(initrd->files[i].name, name) == 0) {
            return &initrd->files[i];
        }
    }

    return NULL;
}

u32 vfs_read_file(const char *name, void *buffer, u32 size) {
    file_entry_t *file = vfs_find_file(name);
    if (!file) {
        kprintf("VFS: File not found: %s\n", name);
        return 0;
    }

    u32 to_read = (size < file->size) ? size : file->size;
    u8 *src = (u8 *)initrd + file->offset;
    u8 *dst = (u8 *)buffer;

    for (u32 i = 0; i < to_read; i++) {
        dst[i] = src[i];
    }

    return to_read;
}

void vfs_listdir(void) {
    if (!initrd) {
        kprintf("No filesystem mounted\n");
        return;
    }

    kprintf("Files in initrd:\n");
    for (u32 i = 0; i < initrd->file_count; i++) {
        kprintf("  %s (%u bytes)\n", initrd->files[i].name, initrd->files[i].size);
    }
}
