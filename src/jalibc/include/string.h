#ifndef __STRING_H
#define __STRING_H

#include <stddef.h>

/*
 * Implemented using memchr, will read till max_sz, if string is not valid, returns max_sz;
 */
size_t strlen(const char *str, size_t max_sz);

/*
 * Poorly optimized implementation of memchr, wrote in C, loops one by one, no inline assembly,
 * Returns NULL if char is not found.
 */
void *memchr(const void *str, char ch, size_t n);

#endif
