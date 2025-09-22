#include <stdint.h>
#include <stdio.h>
#include <sys.h>
#include <drivers/vga.h>
#include <interrupts.h>
#include <drivers/pic.h>

typedef void (*irq_handler_t)(interrupt_frame_t *rg);

static irq_handler_t handler_reg[16];

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

void isr_handler(interrupt_frame_t *rg)
{
     	printf("Fault #%d: ", L_RED, rg->int_no);
	printf("%s, halting...", L_RED, isr_msgs[rg->int_no]);
     	STOP;
}

/*
 * We are registering the function that will handle that specific IRQ so that we can later fetch it from the array and call it
 */
void register_irq(uint32_t irq_no, irq_handler_t handler) 
{
	if (irq_no <= 16) 
		handler_reg[irq_no] = handler;
}

void irq_handler(interrupt_frame_t *rg)
{ 
	uint32_t irq_no = rg->int_no - 32;
	irq_handler_t handler = handler_reg[irq_no];
	if (handler)	
		handler(rg);
	send_eoi_pic(irq_no);
}


