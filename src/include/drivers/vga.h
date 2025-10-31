#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_PTR  ((volatile uint8_t*)0xB8000)

#define VGA_W 80
#define VGA_L 25

#define CRT_ADDR 0x3D4
#define CRT_DATA 0x3D5

enum Colours{ 
	BLACK    = 0x0,
	BLUE     = 0x1,
	GREEN    = 0x2,
	CYAN     = 0x3,
	RED      = 0x4,
	PURPLE   = 0x5,
	BROWN    = 0x6,
	GRAY     = 0x7,
	D_GRAY   = 0x8,
	L_BLUE   = 0x9,
	L_GREEN  = 0xA,
	L_CYAN   = 0xB,
	L_RED    = 0xC,
	L_PURPLE = 0xD,
	YELLOW   = 0xE,
	WHITE    = 0xF,
};

/*
 * Clears the screen by setting the cursor at 0;0 and filling the screen with spaces of the given colour
 */
void video_init(uint8_t colour);

void print_char(unsigned char ch, uint8_t colour);

void enable_cursor(uint8_t start, uint8_t end);
void disable_cursor();
void scroll_down();

uint16_t get_cursor_x(uint16_t pos);
uint16_t get_cursor_y(uint16_t pos);
uint16_t get_cursor_pos();

void move_cursor(uint16_t col, uint16_t row);

#endif
