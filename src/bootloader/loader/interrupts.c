#include <stdint.h>
#include <stdio.h>
#include <sys.h>
#include "vga.h"

typedef struct {
	uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax;
	uint32_t ds;
	uint32_t int_no, err_code;

	uint32_t eip, cs, eflags, u_esp, u_ss;

} __attribute__((packed)) interrupt_frame_t;

char *isr_msgs[] = {
     "Division by zero",
     "Debug",
     "External NMI",
     "Breakpoint",
     "Overflow",
     "Bound exceeded",
     "Opcode not valid",
     "Device not available",
     "Double fault",
     "CS overrun",
     "Invalid TSS",
     "Segment not present",
     "Stack fault",
     "GP fault",
     "Page fault",
     "Intel reserved",
     "Math fault",
     "Alignment check",
     "Machine check",
     "SIMD Floating point exception",
     "Virtualization exception",
     "Control Protection exception",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved",
     "Reserved"
};

void interrupt_handler(interrupt_frame_t *rg)
{
     printf("Fault #%d: %s, halting...", L_RED, rg->int_no, isr_msgs[rg->int_no]);

     STOP;
}

