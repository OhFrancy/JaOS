#ifndef __IDT_H__
#define __IDT_H__

#define C_SELECTOR 0x08
#define IDT_FLAG   0x8E

#include <stdint.h>

typedef struct {
	uint16_t isr_low;
	uint16_t cs;
	uint8_t  reserved;
	uint8_t  attributes;
	uint16_t isr_high;

} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint16_t end;		// Size of IDT - 1
	uint32_t start;	// Starting address of IDT

} __attribute__((packed)) idtr_t;

void idt_init();

#endif
