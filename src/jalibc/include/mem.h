#ifndef __MEM_H
#define __MEM_H

#include <stdint.h>

void *memcpy(void *dst, const void *src, size_t len);
void *memset(void *s, int c, size_t len);

#endif
