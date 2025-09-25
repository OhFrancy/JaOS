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
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  	// starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, offset);                 	// ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset + 8);                 	// ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 1 << CASCADE_IRQ);        	// ICW3: tell Master PIC that there is a slave PIC at IRQ2
	outb(PIC2_DATA, 2);                       	// ICW3: tell Slave PIC its cascade identity (0000 0010)
	
	outb(PIC1_DATA, ICW4_8086);               	// ICW4: have the PICs use 8086 mode (and not 8080 mode)
	outb(PIC2_DATA, ICW4_8086);

	outb(PIC1_DATA, 0x0);
	outb(PIC2_DATA, 0x0);
}

void send_eoi_pic(uint32_t irq_no)
{
	if (irq_no >= 8)
		outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}


