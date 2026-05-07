#ifndef KPRINTF_H
#define KPRINTF_H

#include "common.h"

/* Kernel printf for debugging */
void kprintf(const char *fmt, ...);

/* Character output function */
void putc(char c);

#endif /* KPRINTF_H */
