#ifndef STRING_H
#define STRING_H

#include "common.h"
#include <stddef.h>   // <-- add this so size_t is defined

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(const char *s);

#endif /* STRING_H */
