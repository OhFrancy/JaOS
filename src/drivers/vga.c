/*
 * Implementation for the driver to write to the screen, used for printf.
 *
 *
 *         Copyright (c) 2025 Francesco Lauro. All rights reserved.
 *         SPDX-License-Identifier: MIT
 */

#include <drivers/vga.h>
#include <asm/io.h>
#include <string.h>

static volatile uint8_t *video = VGA_PTR;

void video_init(uint8_t col)
{
	move_cursor(0, 0);
	for (int i = 0; i < VGA_W * VGA_L; ++i) {
		print_char(' ', col);
	}
     	move_cursor(0, 0);
}

void scroll_down()
{
	video = VGA_PTR;
	
    int bytes_per_line = VGA_W * 2;
    int lines_to_move = VGA_L - 1;

    void *source = (void *)(VGA_PTR + VGA_W);

    void *destination = (void *)VGA_PTR;

    // Total bytes to copy
    int total_bytes = lines_to_move * bytes_per_line;

    // Use memcpy to perform the block copy. This is much faster than a loop.
    memcpy(destination, source, total_bytes);


    int last_line_offset = (VGA_L - 1) * VGA_W;
    uint16_t blank_char = 0x20 | (WHITE << 8); // Space character with white-on-black color

    for (int i = 0; i < VGA_W; i++) {
        VGA_PTR[last_line_offset + i] = blank_char;
    }

    // Update the hardware cursor to reflect the new position
    move_cursor(0, get_cursor_y(get_cursor_pos()) -1 );
}

void print_char(unsigned char ch, uint8_t colour)
{
     	uint16_t curr_pos = get_cursor_pos();
     	video = VGA_PTR + (curr_pos * 2);
	
	if (ch == '\n') {
		if (get_cursor_y(curr_pos) + 1 >= VGA_L)
			scroll_down();
		move_cursor(0, get_cursor_y(curr_pos) + 1);
	}
	if (ch == '\t') {
		for (int i = 0; i < 4; ++i)
			print_char(' ', colour);
	}
	if (ch >= ' ') {
		*video++ = ch;
		*video++ = colour;
		move_cursor(get_cursor_x(curr_pos) + 1, get_cursor_y(curr_pos));
	}

}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
     outb(CRT_ADDR, 0x0A);
     outb(CRT_DATA, (inb(0x3D5) & 0xC0) | cursor_start);

     outb(CRT_ADDR, 0x0B);
     outb(CRT_DATA, (inb(0x3D5) & 0xE0) | cursor_end);
}

uint16_t get_cursor_x(uint16_t pos)
{
     return pos % VGA_W;
}

uint16_t get_cursor_y(uint16_t pos)
{
     return pos / VGA_W;
}

uint16_t get_cursor_pos()
{
    uint16_t pos = 0;
    outb(CRT_ADDR, 0x0F);
    pos |= inb(CRT_DATA);
    outb(CRT_ADDR, 0x0E);
    pos |= ((uint16_t) inb(CRT_DATA)) << 8;
    return pos;
}

void move_cursor(uint16_t col, uint16_t row)
{
	uint16_t pos = (row * VGA_W) + col;

	// Cursor Low port
	outb(CRT_ADDR, 0x0F);
	outb(CRT_DATA, (uint16_t)(pos & 0xFF));

	// Cursor High port
	outb(CRT_ADDR, 0x0E);
	outb(CRT_DATA, (uint16_t)((pos >> 8) & 0xFF));
}
