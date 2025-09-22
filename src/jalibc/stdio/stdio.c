#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <drivers/vga.h>

#define STR_BUFFER   2048

#define HEX		 16
#define DECIMAL	 10

#define STATE_NORMAL 0
#define STATE_SPEC   1

#define SIZE_32	 2
#define SIZE_64	 3

extern uint32_t _udiv64(uint32_t high, uint32_t low, uint32_t divisor,  uint64_t *quotient);

static int write_ch(unsigned char ch, uint8_t colour) {
	print_char(ch, colour);
	return 1;
}

static int write_str(char *s, uint8_t colour) {
	for(;*s; ++s){
		write_ch(*s, colour);
	}

	return strlen(s, STR_BUFFER);
}

static void divide_64bit(uint64_t n, uint32_t base, uint64_t *quotient, uint32_t *remainder)
{
	uint32_t high_n = n >> 32;
	*remainder = _udiv64(high_n, n, base, quotient);
}

static int write_digit(long long int n, uint32_t base, uint8_t div_size, uint8_t colour)
{
	uint64_t quotient;
	uint32_t remainder;

	char *itoa_list = "0123456789abcdef";
	char buffer[STR_BUFFER];
	int pos = 0;

	bool negative  = false;

	if (n < 0) {
		negative = true;
	}

	do {
		if (div_size == SIZE_32) {
			buffer[pos++] = itoa_list[(uint32_t)n % base];
			n = (uint32_t)n / base;
		}
		if (div_size == SIZE_64) {
			divide_64bit(n, base, &quotient, &remainder);
			buffer[pos++] = remainder;
			n = quotient;
		}
	} while(n > 0);

	if (negative) {
		buffer[pos++] = '-';
	}

	// Reverse buffer
	for (int i = 0; i < pos / 2; ++i) {
		unsigned char tmp = buffer[i];
		buffer[i] = buffer[pos - 1 - i];
		buffer[pos - 1 - i] = tmp;
	}
	buffer[pos] = '\0';

	return write_str(buffer, colour);
}

int printf(const char *fmt, uint8_t colour, ...)
{
	va_list args;
	int count;

	uint8_t state = STATE_NORMAL;

	va_start(args, colour);

	for(;*fmt; ++fmt) {
		switch (state) {
			case STATE_NORMAL:
				if (*fmt == '%') {
					state = STATE_SPEC;
				}
				else
					count += write_ch(*fmt, colour);
				break;

			case STATE_SPEC:
				switch(*fmt) {
					case '%':
						count += write_ch('%', colour);
						break;
					case 'c':
						count += write_ch(va_arg(args, int), colour);
						break;
					case 'd':
						count += write_digit(va_arg(args, int64_t), DECIMAL, SIZE_32, colour);
						break;
					case 'x':
						count += write_str("0x", colour);
						count += write_digit(va_arg(args, int), HEX, SIZE_32, colour);
						break;
					case 's':
						count += write_str(va_arg(args, char*), colour);
						break;
				}
				state = STATE_NORMAL;
		}
	}
	va_end(args);

	return count;
}
