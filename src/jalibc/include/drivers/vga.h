#ifndef __VGA_H
#define __VGA_H

#include <stdint.h>

#define VGA_PTR  ((volatile uint8_t*)0xB8000)

#define VGA_W 80
#define VGA_L 25

#define CRT_ADDR 0x3D4
#define CRT_DATA 0x3D5

#define BLACK    0x0
#define BLUE     0x1
#define GREEN    0x2
#define CYAN     0x3
#define RED      0x4
#define PURPLE   0x5
#define BROWN    0x6
#define GRAY     0x7
#define D_GRAY   0x8
#define L_BLUE   0x9
#define L_GREEN  0xA
#define L_CYAN   0xB
#define L_RED    0xC
#define L_PURPLE 0xD
#define YELLOW   0xE
#define WHITE    0xF

/*
 * Clears the screen by setting the cursor at 0;0 and filling the screen with spaces of the given colour
 */
void init_video(uint8_t colour);

void print_char(unsigned char ch, uint8_t colour);

void enable_cursor(uint8_t start, uint8_t end);
void disable_cursor();

uint16_t get_cursor_x(uint16_t pos);
uint16_t get_cursor_y(uint16_t pos);
uint16_t get_cursor_pos();

void move_cursor(uint16_t col, uint16_t row);

#endif
