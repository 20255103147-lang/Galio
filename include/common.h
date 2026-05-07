#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

/* Typedefs for clarity */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t  i32;

/* Kernel utility functions */
void *memset(void *s, int c, u32 n);
void *memcpy(void *dest, const void *src, u32 n);
void panic(const char *msg);

/* NEW: kernel status reporting */
void kernel_status(void);

#endif /* COMMON_H */
