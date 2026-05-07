#ifndef HEAP_H
#define HEAP_H

#include "common.h"
#include <stddef.h>

/* Kernel heap allocator */

void heap_init(void);
void *kmalloc(size_t size);
void *kcalloc(size_t nmemb, size_t size);
void *krealloc(void *ptr, size_t size);
void kfree(void *ptr);

#endif /* HEAP_H */
