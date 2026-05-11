/* ext2.c - Simplified EXT2 filesystem driver */

#include "ext2.h"
#include "ata.h"
#include "kprintf.h"
#include "string.h"

#define EXT2_ROOT_INODE 2

static ext2_superblock_t superblock;
static u32 block_size = EXT2_BLOCK_SIZE;
static u32 inodes_per_block;
static u32 blocks_per_group;

/* Read a block from disk */
static i32 read_block(u32 block_num, void *buffer) {
    u32 sector = block_num * (block_size / 512);
    u32 sectors = block_size / 512;
    return ata_read_sectors(sector, sectors, buffer);
}

/* Write a block to disk */
static i32 write_block(u32 block_num, const void *buffer) {
    u32 sector = block_num * (block_size / 512);
    u32 sectors = block_size / 512;
    return ata_write_sectors(sector, sectors, buffer);
}

i32 ext2_init(void) {
    kprintf("Initializing EXT2 filesystem...\n");

    /* Read superblock (block 1) */
    if (read_block(1, &superblock) != 0) {
        kprintf("EXT2: Failed to read superblock\n");
        return -1;
    }

    if (superblock.magic != EXT2_SIGNATURE) {
        kprintf("EXT2: Invalid signature 0x%x\n", superblock.magic);
        return -1;
    }

    block_size = 1024 << superblock.log_block_size;
    inodes_per_block = block_size / EXT2_INODE_SIZE;
    blocks_per_group = superblock.blocks_per_group;

    kprintf("EXT2: %u inodes, %u blocks, block size %u\n",
            superblock.inodes_count, superblock.blocks_count, block_size);

    return 0;
}

i32 ext2_read_inode(u32 inode_num, ext2_inode_t *inode) {
    u32 group = (inode_num - 1) / superblock.inodes_per_group;
    u32 index = (inode_num - 1) % superblock.inodes_per_group;
    u32 block = superblock.first_data_block + 1 + group * blocks_per_group +
                (index / inodes_per_block);

    u8 buffer[block_size];
    if (read_block(block, buffer) != 0) {
        return -1;
    }

    memcpy(inode, buffer + (index % inodes_per_block) * EXT2_INODE_SIZE, sizeof(ext2_inode_t));
    return 0;
}

i32 ext2_read_block(u32 block_num, void *buffer) {
    return read_block(block_num, buffer);
}

i32 ext2_write_block(u32 block_num, const void *buffer) {
    return write_block(block_num, buffer);
}

u32 ext2_find_inode(const char *path) {
    if (!path || path[0] != '/') return 0;

    u32 current_inode = EXT2_ROOT_INODE;
    const char *p = path + 1;

    while (*p) {
        /* Skip slashes */
        while (*p == '/') p++;

        if (!*p) break;

        /* Get next component */
        const char *end = p;
        while (*end && *end != '/') end++;

        char name[256];
        u32 len = end - p;
        memcpy(name, p, len);
        name[len] = 0;

        /* Read current inode */
        ext2_inode_t inode;
        if (ext2_read_inode(current_inode, &inode) != 0) {
            return 0;
        }

        /* Search directory */
        u8 buffer[block_size];
        u32 found = 0;

        for (u32 i = 0; i < 12 && inode.block[i]; i++) {  /* Direct blocks only */
            if (read_block(inode.block[i], buffer) != 0) continue;

            ext2_dirent_t *dent = (ext2_dirent_t *)buffer;
            while ((u8 *)dent < buffer + block_size) {
                if (dent->inode && strncmp(dent->name, name, dent->name_len) == 0) {
                    current_inode = dent->inode;
                    found = 1;
                    break;
                }
                dent = (ext2_dirent_t *)((u8 *)dent + dent->rec_len);
            }
            if (found) break;
        }

        if (!found) return 0;
        p = end;
    }

    return current_inode;
}
