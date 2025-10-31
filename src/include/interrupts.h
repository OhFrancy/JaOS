#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

typedef struct {
	uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax;
	uint32_t ds;
	uint32_t int_no, err_code;

	uint32_t eip, cs, eflags, u_esp, u_ss;

} __attribute__((packed)) interrupt_frame_t;

typedef void (*irq_handler_t)(interrupt_frame_t *rg);

void register_irq(uint32_t irq_no, irq_handler_t handler);
#endif
