#include <stdio.h>
#include <sys.h>
#include <interrupts.h>
#include <drivers/vga.h>
#include <drivers/floppy.h>
#include <asm/io.h>

// Values for the 82077AA Configure command
enum {
	SEEK_ENABLE   = 1,
	FIFO_DISABLE  = 0,
	DPOLL_DISABLE = 1,
	THRESHOLD_B   = 15,
};

static uint8_t irq_done = 0;
static uint8_t active_drive = 0;

void floppy_init() 
{ 
	// Send the version command to make sure the FDC is 82077AA
	outb(VERSION, DATA_FIFO);
	if (inb(DATA_FIFO) != 0x90) {
		printf("FD controller different than 82077AA is not supported, halting...", L_RED);
		STOP;
	}
	
	outb(CONFIGURE, DATA_FIFO);
	outb(0, DATA_FIFO);
	outb((SEEK_ENABLE << 6) | (FIFO_DISABLE << 5) | (DPOLL_DISABLE << 4) | (THRESHOLD_B - 1), DATA_FIFO);
	outb(0, DATA_FIFO);

	// Lock the configuration so that it doesn't default at every reset
	outb(LOCK, 0x94);
	
	floppy_reset();
	floppy_recalibrate();

}

void floppy_specify(uint8_t srt, uint8_t hlt, uint8_t hut, uint8_t ndma)
{
	outb(SPECIFY, DATA_FIFO);
	outb(srt << 4 | hut, DATA_FIFO);
	outb(hlt << 1 | ndma, DATA_FIFO);
}

void floppy_recalibrate()
{
	uint8_t st0, cyl;
	floppy_motor(1);
	
	// Trying 5 times till the calibration works and we get to cylinder 0
	for (int i = 0; i < 5; ++i) {
		outb(RECALIBRATE, DATA_FIFO);
		outb(active_drive, DATA_FIFO);
		wait_floppy_irq();
		
		outb(SENSE_INTERRUPT, DATA_FIFO);
		st0 = inb(DATA_FIFO);
		cyl = inb(DATA_FIFO);
	
		if (cyl == 0 && st0 == 0x20) {
			floppy_motor(0);
			return;
		}
				
	}
	floppy_motor(0);
}

void floppy_reset()
{
	register_irq(6, floppy_irq_handle);
	fdc_disable();
	fdc_enable();
	
	wait_floppy_irq();

	// Set 500KB datarate (1.44MB floppy)
	outb(0, CCR);

 	// These are specific safe values for a 1.44MB, 3.5 inch floppy disk. (NDMA to 0 = DMA ON)
	floppy_specify(8, 5, 0, 0);
}

void floppy_irq_handle(interrupt_frame_t *rg) 
{
	rg = rg;
	irq_done = true;
}

void wait_floppy_irq()
{
	while(!irq_done)
		irq_done = 0;
}

void floppy_motor(bool state)
{
	uint32_t motor_no;

	if (active_drive > 3)
		return;

	switch (active_drive) {
		case 0:
			motor_no = DOR_MOTA;
			break;
		case 1:
			motor_no = DOR_MOTB;
			break;
		case 2:
			motor_no = DOR_MOTC;
			break;
		case 3:
			motor_no = DOR_MOTD;
			break;
	}
	if (state)
		outb((motor_no | DOR_RESET | DOR_IRQ_DMA | active_drive), DOR);
	else
		outb(DOR_RESET, DOR);
}

void set_floppy_drive(uint8_t drive)
{
	active_drive = drive;
}

void fdc_disable() 
{
	outb(0, DOR);
}

void fdc_enable()
{
	outb(DOR_IRQ_DMA | DOR_RESET, DOR);
}
