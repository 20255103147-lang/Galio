/* kernel.c
 * Utility functions for the kernel: memset, memcpy, panic
 */
#include "common.h"
#include "vga.h"

void *memset(void *s, int c, u32 n) {
    u8 *p = (u8*)s;
    while (n--) *p++ = (u8)c;
    return s;
}

void *memcpy(void *dest, const void *src, u32 n) {
    u8 *d = (u8*)dest;
    const u8 *s = (const u8*)src;
    while (n--) *d++ = *s++;
    return dest;
}

void panic(const char *msg) {
    vga_puts("KERNEL PANIC: ");
    vga_puts(msg);
    for (;;) __asm__ volatile("cli; hlt");
}
