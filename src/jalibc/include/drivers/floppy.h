#ifndef __FLOPPY_H
#define __FLOPPY_H

#include <stdint.h>
#include <stddef.h>
#include <interrupts.h>

void floppy_init();
void floppy_read(uint32_t lba, void *bfr);
void floppy_write(uint32_t lba, const void *bfr_to_write, size_t bfr_s);

#endif
