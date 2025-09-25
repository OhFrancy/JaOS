/*
 * Implementation of the floppy disk driver to interact with the FDC.
 *
 * 	Copyright (c) 2025 Francesco Lauro. All rights reserved.
 * 	SPDX-License-Identifier: MIT
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys.h>
#include <log.h>
#include <interrupts.h>
#include <drivers/vga.h>
#include <drivers/floppy.h>
#include <asm/io.h>

#define BYTES_P_SECTOR  512
#define SECTORS_P_TRACK 18
#define READ 0x6
#define WRITE 0x5
#define GAP1 0x1B
#define DMA (char *)(0x1000)

enum FloppyRegisters
{
	SRA		= 0x3F0, // R-only
	SRB		= 0x3F1, // R-only
	DOR		= 0x3F2,
	TDR		= 0x3F3,
	MSR		= 0x3F4, // R-only
	DSR		= 0x3F4, // W-only
	DATA_FIFO	= 0x3F5, // Used for commands 
	DIR		= 0x3F7, // R-only
	CCR		= 0x3F7
};

enum FloppyCommands
{
   	READ_TRACK =                 0x2,	  // generates IRQ6
   	SPECIFY =                    0x3,      // set drive parameters
   	SENSE_DRIVE_STATUS =         0x4,
   	WRITE_DATA =                 0x5,      // write to the disk
   	READ_DATA =                  0x6,      // read from the disk
   	RECALIBRATE =                0x7,      // seek to cylinder 0
   	SENSE_INTERRUPT =            0x8,      // get status of last command
   	WRITE_DELETED_DATA =         0x9,
   	READ_ID =                    0xA,	  // generates IRQ6
   	READ_DELETED_DATA =          0xC,
   	FORMAT_TRACK =               0xD,     
   	DUMPREG =                    0xE,
   	SEEK =                       0xF,      // seek both heads to cylinder X
   	VERSION =                    0x10,	  // used during initialization
   	SCAN_EQUAL =                 0x11,
   	PERPENDICULAR_MODE =         0x12,	  // used during initialization
   	CONFIGURE =                  0x13,     // set controller parameters
   	LOCK =                       0x14,     // protect controller params from a reset
   	VERIFY =                     0x16,
   	SCAN_LOW_EQUAL  =            0x19,
   	SCAN_HIGH_EQUAL =            0x1D,
};

enum FloppyOptionalBits
{
	MT_BIT   = 0x80,
	MFM_BIT  = 0x40,
	SK_BIT	 = 0x20,
};

enum DORMask
{
	DOR_MOTD     = 0x80,	// Drive 3 motor ON
	DOR_MOTC     = 0x40,	// Drive 2 motor ON
	DOR_MOTB     = 0x20,	// Drive 1 motor ON
	DOR_MOTA     = 0x10, 	// Drive 0 motor ON
	DOR_IRQ_DMA  = 0x8, 	// Enable IRQs and DMA
	DOR_RESET    = 0x4, 	
};

// Values for the 82077AA Configure command
enum Configure {
	SEEK_ENABLE   = 1,
	FIFO_DISABLE  = 0,
	DPOLL_DISABLE = 1,
	THRESHOLD_B   = 15,
};

static void floppy_reset();
static void floppy_recalibrate();
static void floppy_specify(uint8_t srt, uint8_t hlt, uint8_t hut, uint8_t ndma);
static void floppy_read_write_params(uint32_t cyl, uint32_t head, uint32_t sector);
static void floppy_motor(bool state);
static void fdc_enable();
static void fdc_disable();
static void floppy_irq_handle(interrupt_frame_t *rg);
static void floppy_set_drive(uint8_t drive);
static void wait_floppy_irq();
static void dma_init();
static void dma_prepare_write();
static void dma_prepare_read();
static void lba_to_chs(uint32_t lba, uint32_t *cyl, uint32_t *head, uint32_t *sector);
static void sense_interrupt(uint8_t *st0, uint8_t *cyl);
static void write_fifo(uint8_t byte);
static uint8_t read_fifo();

static volatile uint8_t irq_done = 0;
static uint8_t active_drive = 0;

void floppy_init() 
{ 
	register_irq(6, floppy_irq_handle);
	floppy_set_drive(0);

	// Send the version command to make sure the FDC is 82077AA
	write_fifo(VERSION);
	if (read_fifo() != 0x90) {
		klog(FATAL, "Floppy", "FDC chip is not supported, can't initialize the floppy disk, halting...");
		STOP;
	}
	
	write_fifo(CONFIGURE);
	write_fifo(0);
	write_fifo((SEEK_ENABLE << 6) | (FIFO_DISABLE << 5) | (DPOLL_DISABLE << 4) | (THRESHOLD_B - 1));
	write_fifo(0);

	// Lock the configuration so that it doesn't default at every reset
	write_fifo(LOCK);
	read_fifo();

	floppy_reset();
	floppy_recalibrate();
	dma_init();
}
	
void floppy_read(uint32_t lba, void *bfr) 
{
	if (!bfr) {
		klog(ERROR, "Floppy", "given reading buffer pointer is NULL.");
		return;
	}

	floppy_motor(1);

	irq_done = 0;
	uint32_t cyl = 0, head = 0, sector = 0;

	lba_to_chs(lba, &cyl, &head, &sector); 
	dma_prepare_read();

	write_fifo(MT_BIT | MFM_BIT | READ);

	floppy_read_write_params(cyl, head, sector);
	wait_floppy_irq();

	for (int i = 0; i < 7; ++i) {
		read_fifo();
	}
	floppy_motor(0);
	memcpy(bfr, DMA, BYTES_P_SECTOR);
}  

void floppy_write(uint32_t lba, const void *bfr_to_write, size_t bfr_s) 
{
	if (!bfr_to_write) {
		klog(ERROR, "Floppy", "given writing buffer pointer is NULL.");
		return;
	}
	uint32_t cyl = 0, head = 0, sector = 0;
	lba_to_chs(lba, &cyl, &head, &sector);
	memcpy(DMA, bfr_to_write, bfr_s);

	// We zero the rest of the sector if we are not writing it fully
	if (bfr_s < BYTES_P_SECTOR) {
		memset(DMA + bfr_s, 0, BYTES_P_SECTOR - bfr_s);
	}

	floppy_motor(1);

	irq_done = 0;
	dma_prepare_write();
	write_fifo(MT_BIT | MFM_BIT | WRITE);

	floppy_read_write_params(cyl, head, sector);
	wait_floppy_irq();

	for (int i = 0; i < 7; ++i) {
		read_fifo();
	}
	floppy_motor(0);
}

/*
 * We use this function for either write or read operations as the parameters are the same
 */
static void floppy_read_write_params(uint32_t cyl, uint32_t head, uint32_t sector)
{
	uint8_t EOT = (sector == SECTORS_P_TRACK) ? SECTORS_P_TRACK : sector + 1;

	write_fifo((head << 2) | active_drive);
	write_fifo(cyl);
	write_fifo(head);
	write_fifo(sector);
	write_fifo(2);
	write_fifo(EOT);
	write_fifo(GAP1);
	write_fifo(0xFF);
}

static void floppy_specify(uint8_t srt, uint8_t hlt, uint8_t hut, uint8_t ndma)
{
	write_fifo(SPECIFY);
	write_fifo((srt << 4) | hut);
	write_fifo((hlt << 1) | ndma);
}

static void floppy_recalibrate()
{
	uint8_t st0 = 0, cyl = 0;
	floppy_motor(1);

	irq_done = 0;

	// Trying 5 times till the calibration works and we get to cylinder 0
	for (int i = 0; i < 5; ++i) {
		write_fifo(RECALIBRATE);
		write_fifo(active_drive);

		wait_floppy_irq();

		sense_interrupt(&st0, &cyl);

		if (cyl == 0 && st0 == 0x20) {
			floppy_motor(0);
			return;
		}
				
	}
	klog(ERROR, "Floppy", "calibration failed, next operations may not work as expected.");
	floppy_motor(0);
}

static void floppy_reset()
{
	uint8_t st0 = 0, cyl = 0;
	irq_done = 0;

	fdc_disable();
	fdc_enable();
	
	wait_floppy_irq();

	sense_interrupt(&st0, &cyl);

	// Set 500KB datarate (1.44MB floppy)
	outb(CCR, 0x0);

 	// These are specific safe values for a 1.44MB, 3.5 inch floppy disk. (NDMA to 0 = DMA ON)
	floppy_specify(8, 5, 0, 0);
}

static void floppy_motor(bool state)
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
		outb(DOR, (motor_no | DOR_RESET | DOR_IRQ_DMA | active_drive));
	else
		outb(DOR, DOR_RESET);
}

static void floppy_irq_handle(interrupt_frame_t *rg) 
{
	rg = rg;
	irq_done = true;
}

static void wait_floppy_irq()
{
	while(irq_done == false)
		irq_done = false;
}

static void lba_to_chs(uint32_t lba, uint32_t *cyl, uint32_t *head, uint32_t *sector)
{	
	if (!head || !cyl || !sector)
		return;
	*head = (lba % (SECTORS_P_TRACK * 2)) / SECTORS_P_TRACK;
	*cyl = (lba / (SECTORS_P_TRACK * 2));
	*sector = (lba % SECTORS_P_TRACK + 1);
}

/*
 * A sense interrupt is required after a lot of commands, so we make a function for convenience
 */
static void sense_interrupt(uint8_t *st0, uint8_t *cyl)
{ 
	if (!st0 || !cyl)
		klog(ERROR, "Floppy", "sense interrupt aborted, got NULL pointer as parameter");
	write_fifo(SENSE_INTERRUPT);
	*st0 = read_fifo();
	*cyl = read_fifo();
}

static void write_fifo(uint8_t byte)
{
	outb(DATA_FIFO, byte);
}

static uint8_t read_fifo()
{
	return inb(DATA_FIFO);
}

static void floppy_set_drive(uint8_t drive)
{
	active_drive = drive;
}

static void fdc_disable() 
{
	outb(DOR, 0);
}

static void fdc_enable()
{
	outb(DOR, DOR_IRQ_DMA | DOR_RESET);
}

/*
 * We are setting the starting address of the DMA to 0x1000
 */
static void dma_init()
{
	outb(0x0C, 0xFF);	

	outb(0x04, 0x0);
	outb(0x80, 0x0);	// Dummy writing to make some delay between the double consecutive I/O Port writing	
	outb(0x04, 0x10);
	
	outb(0x0C, 0xFF);

	outb(0x05, 0xFF);
	outb(0x80, 0x0);	// Dummy
	outb(0x05, 0x23);
	
	outb(0x81, 0x0);
	outb(0x0A, 0x02);
}

static void dma_prepare_write()
{
	outb(0x0A, 0x06);
	outb(0x0B, 0x5A);
	outb(0x0A, 0x02);
}

static void dma_prepare_read()
{
	outb(0x0A, 0x06);
	outb(0x0B, 0x56);
	outb(0x0A, 0x02);
}
