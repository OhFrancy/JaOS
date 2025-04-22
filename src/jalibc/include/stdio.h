#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdint.h>

#define false 0
#define true  1
typedef uint8_t bool;

int printf(const char *fmt, uint8_t colour, ...);

#endif
