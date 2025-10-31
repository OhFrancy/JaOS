#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stddef.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <drivers/vga.h>

#define false 0
#define true  1

int vprintf(const char *fmt, uint8_t colour, va_list args);
int printf(const char *fmt, uint8_t colour, ...);
const char *strerror(int errno);

#endif
