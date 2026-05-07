#ifndef VFS_H
#define VFS_H

#include "common.h"
#include <stddef.h>

/* Virtual Filesystem Layer */

typedef struct {
    char name[256];
    u32 size;
    u32 offset;
} file_entry_t;

typedef struct {
    u32 magic;
    u32 file_count;
    file_entry_t files[128];
} initrd_header_t;

/* Mount Root InitRD */
void vfs_init(void *initrd_addr);

/* Find file in filesystem */
file_entry_t *vfs_find_file(const char *name);

/* Read file data */
u32 vfs_read_file(const char *name, void *buffer, u32 size);

/* List files */
void vfs_listdir(void);

#endif /* VFS_H */
