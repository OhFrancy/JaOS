#include <idt.h>

idt_entry_t idt_table[256];
idtr_t	  idt_ptr;

extern void *isr_stub_table[];
extern void *irq_stub_table[];

static inline void lidt(idtr_t *idtr)
{
	asm volatile("lidt %0" : : "m"(*idtr));
}

static void idt_set_gate(uint8_t n, uint32_t isr_addr, uint16_t selector, uint8_t flag) {
	idt_table[n].isr_low    = isr_addr & 0xFFFF;
	idt_table[n].isr_high   = (isr_addr >> 16) & 0xFFFF;
	idt_table[n].cs	    = selector;
	idt_table[n].reserved   = 0;
	idt_table[n].attributes = flag;
}

void idt_init()
{
	idt_ptr.end   = (sizeof(idt_table)) - 1;
	idt_ptr.start = (uint32_t)&idt_table;

	for (int i = 0; i < 32; ++i) {
		idt_set_gate(i, (uint32_t)isr_stub_table[i], 0x8, 0x8E);
	}
	
	// Fill the table [32-47] with IRQs
	for (int i = 0; i < 16; ++i) {
		idt_set_gate(32 + i, (uint32_t)irq_stub_table[i], 0x8, 0x8E);
	}


	lidt(&idt_ptr);
}

