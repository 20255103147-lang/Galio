/* heap.c - Simple kernel heap allocator */
#include "heap.h"
#include "pmem.h"
#include "kprintf.h"

#define HEAP_START      0x500000
#define HEAP_MAX_SIZE   0x1000000    /* 16 MB heap */
#define MIN_BLOCK_SIZE  64

/* Metadata for each allocated block */
typedef struct {
    u32 magic;      /* 0xDEADBEEF for validation */
    size_t size;
    u8 is_free;
    struct {
        struct block *next;
        struct block *prev;
    } *list;
} block_metadata_t;

#define METADATA_SIZE sizeof(block_metadata_t)
#define MAGIC 0xDEADBEEF

static u32 heap_end = HEAP_START;
static block_metadata_t *free_list = NULL;

static block_metadata_t *get_metadata(void *ptr) {
    return (block_metadata_t *)ptr - 1;
}

void heap_init(void) {
    kprintf("Initializing kernel heap at %08X\n", HEAP_START);
    heap_end = HEAP_START;
}

void *kmalloc(size_t size) {
    if (size == 0) return NULL;

    /* Align size */
    size = (size + 7) & ~7;

    /* Try to find a free block in free list */
    block_metadata_t *block = free_list;
    while (block) {
        if (block->is_free && block->size >= size) {
            /* Use this block */
            block->is_free = 0;
            
            /* Split if necessary */
            if (block->size > size + METADATA_SIZE + MIN_BLOCK_SIZE) {
                u32 new_block_addr = (u32)block + METADATA_SIZE + size;
                block_metadata_t *new_block = (block_metadata_t *)new_block_addr;
                
                new_block->magic = MAGIC;
                new_block->size = block->size - size - METADATA_SIZE;
                new_block->is_free = 1;
                
                block->size = size;
            }
            
            return (void *)(block + 1);
        }
        block = (block_metadata_t *)block->list->next;
    }

    /* Allocate new block from heap end */
    if (heap_end + METADATA_SIZE + size > HEAP_START + HEAP_MAX_SIZE) {
        kprintf("kmalloc: Heap exhausted\n");
        return NULL;
    }

    block = (block_metadata_t *)heap_end;
    block->magic = MAGIC;
    block->size = size;
    block->is_free = 0;

    heap_end += METADATA_SIZE + size;

    return (void *)(block + 1);
}

void *kcalloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *ptr = kmalloc(total);
    if (ptr) {
        __builtin_memset(ptr, 0, total);
    }
    return ptr;
}

void *krealloc(void *ptr, size_t size) {
    if (!ptr) return kmalloc(size);
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }

    block_metadata_t *block = get_metadata(ptr);
    if (block->magic != MAGIC) {
        kprintf("krealloc: Invalid block\n");
        return NULL;
    }

    if (block->size >= size) {
        return ptr;
    }

    void *new_ptr = kmalloc(size);
    if (!new_ptr) return NULL;

    __builtin_memcpy(new_ptr, ptr, block->size);
    kfree(ptr);
    
    return new_ptr;
}

void kfree(void *ptr) {
    if (!ptr) return;

    block_metadata_t *block = get_metadata(ptr);
    if (block->magic != MAGIC) {
        kprintf("kfree: Invalid block\n");
        return;
    }

    block->is_free = 1;
    
    /* TODO: Implement coalescing of adjacent free blocks */
}
