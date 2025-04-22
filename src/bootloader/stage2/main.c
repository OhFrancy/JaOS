#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <asm/io.h>
#include <sys.h>
#include "vga.h"
#include "idt.h"

#define TAB_SPACES 4

void main() {
	//uint16_t curr_x, curr_y;

	const char *ok = " Ok\n";
	const char *init_lidt_msg = "Initializing LIDT...";
     const char *ascii_logo = "\n   ____.  _____   ________    _________	\n"
						"    |    | /  _  \\  \\_____  \\  /   _____/\n"
						"    |    |/  /_\\  \\  /   |   \\ \\_____  \\\n"
						"/\\__|    /    |    \\/    |    \\/        \\\n"
						"\\________\\____|__  /\\_______  /_______  /\n"
						"                 \\/         \\/        \\/\n";

	// Entered Pmode successfully
	printf(ok, GRAY);

	printf(init_lidt_msg, GRAY);
	idt_init();
	printf(ok, GRAY);

	printf(ascii_logo, BLUE);

	unsigned long long int x = *(volatile int*)0xFFFFFFFFFFFF;
	printf("%d", x);

	STOP;
}

