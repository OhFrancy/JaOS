#ifndef __STRING_H
#define __STRING_H

#include <stddef.h>
#include <mem.h>

size_t strlen(const char *str);

/*
 * Poorly optimized implementation of memchr, wrote in C, loops one by one, no inline assembly,
 * Returns NULL if char is not found.
 */
void *memchr(const void *str, char ch, size_t n);

#endif
