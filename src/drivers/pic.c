/*
 * Implementation for the PCI.
 *
 * 	Copyright (c) 2025 Francesco Lauro. All rights reserved.
 * 	SPDX-License-Identifier: MIT
 *
 * 	based on the implementation from the OSDev Wiki, 
 * 	see: 'https://wiki.osdev.org/8259_PIC'
 *
 */

#include <stdio.h>
#include <asm/io.h>
#include <drivers/pic.h>

void remap_pic(int offset)
{
	outb(ICW1_INIT | ICW1_ICW4, PIC1_COMMAND);  // starts the initialization sequence (in cascade mode)
	outb(ICW1_INIT | ICW1_ICW4, PIC2_COMMAND);
	outb(offset, PIC1_DATA);                 // ICW2: Master PIC vector offset
	outb(offset + 8, PIC2_DATA);                 // ICW2: Slave PIC vector offset
	outb(1 << CASCADE_IRQ, PIC1_DATA);        // ICW3: tell Master PIC that there is a slave PIC at IRQ2
	outb(2, PIC2_DATA);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	
	outb(ICW4_8086, PIC1_DATA);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	outb(ICW4_8086, PIC2_DATA);

	outb(0, PIC1_DATA);
	outb(0, PIC2_DATA);
}

void send_eoi_pic(uint32_t irq_no)
{
	if (irq_no >= 8)
		outb(PIC_EOI, PIC2_COMMAND);
	outb(PIC_EOI, PIC1_COMMAND);
}


