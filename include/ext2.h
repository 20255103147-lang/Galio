#ifndef EXT2_H
#define EXT2_H

#include "common.h"

/* Simplified EXT2 filesystem driver */

#define EXT2_SIGNATURE 0xEF53
#define EXT2_BLOCK_SIZE 1024
#define EXT2_INODE_SIZE 128

/* Superblock structure */
typedef struct {
    u32 inodes_count;
    u32 blocks_count;
    u32 r_blocks_count;
    u32 free_blocks_count;
    u32 free_inodes_count;
    u32 first_data_block;
    u32 log_block_size;
    u32 log_frag_size;
    u32 blocks_per_group;
    u32 frags_per_group;
    u32 inodes_per_group;
    u32 mtime;
    u32 wtime;
    u16 mnt_count;
    u16 max_mnt_count;
    u16 magic;
    u16 state;
    u16 errors;
    u16 minor_rev_level;
    u32 lastcheck;
    u32 checkinterval;
    u32 creator_os;
    u32 rev_level;
    u16 def_resuid;
    u16 def_resgid;
} ext2_superblock_t;

/* Inode structure */
typedef struct {
    u16 mode;
    u16 uid;
    u32 size;
    u32 atime;
    u32 ctime;
    u32 mtime;
    u32 dtime;
    u16 gid;
    u16 links_count;
    u32 blocks;
    u32 flags;
    u32 osd1;
    u32 block[15];
    u32 generation;
    u32 file_acl;
    u32 dir_acl;
    u32 faddr;
    u32 osd2[3];
} ext2_inode_t;

/* Directory entry */
typedef struct {
    u32 inode;
    u16 rec_len;
    u8 name_len;
    u8 file_type;
    char name[255];
} ext2_dirent_t;

/* Initialize EXT2 filesystem */
i32 ext2_init(void);

/* Read inode */
i32 ext2_read_inode(u32 inode_num, ext2_inode_t *inode);

/* Read data block */
i32 ext2_read_block(u32 block_num, void *buffer);

/* Write data block */
i32 ext2_write_block(u32 block_num, const void *buffer);

/* Find inode by path */
u32 ext2_find_inode(const char *path);

#endif /* EXT2_H */
