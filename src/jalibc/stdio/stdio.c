#include <stdio.h>
#include <sys.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <log.h>
#include <limits.h>
#include <drivers/vga.h>

#define STR_BUFFER   2048

#define HEX	 16
#define DECIMAL	 10

#define DEF_PREC 6
#define MAX_PREC 9

enum States 
{
	STATE_NORMAL 	= (0 << 0),
	STATE_SPEC   	= (1 << 0),
	STATE_PRECISION = (1 << 1),
};

static int write_float(double n, uint8_t spec, uint8_t colour);
static int write_signed(int32_t n, uint8_t base, uint8_t colour, uint8_t hex_uppercase);
static int write_unsigned(uint32_t n, uint8_t base, uint8_t colour, uint8_t hex_uppercase);
static int write_str(char *s, uint8_t colour);
static int write_ch(unsigned char ch, uint8_t colour);
	
static char *itoa_list = "0123456789abcdef";

/*
 * Simple state machine implementation of vprintf
 */
int vprintf(const char *fmt, uint8_t colour, va_list args) 
{
	int count;
	uint8_t prec = DEF_PREC;

	uint8_t state = STATE_NORMAL;
	for(;*fmt; ++fmt) {
		switch (state) {
			case STATE_NORMAL:
				if (*fmt == '%')
					state = STATE_SPEC;
				else
					count += write_ch(*fmt, colour);
				break;

			case STATE_SPEC:
				switch(*fmt) {
					case '.':
						state = STATE_PRECISION;
						continue;
					case '%':
						count += write_ch('%', colour);
						break;
					case 'c':
						count += write_ch(va_arg(args, int), colour);
						break;
					case 'd':
						count += write_signed(va_arg(args, int32_t), DECIMAL, colour, false);
						break;
					case 'u':
						count += write_unsigned(va_arg(args, uint32_t), DECIMAL, colour, false);
						break;
					case 'f':
						// there is no 'float' as a va_arg, it's promoted to a double
						count += write_float(va_arg(args, double), prec, colour);
						break;
					case 'x':
						count += write_str("0x", colour);
						count += write_unsigned(va_arg(args, uint32_t), HEX, colour, false);
						break;
					case 'X':
						count += write_str("0x", colour);
						count += write_unsigned(va_arg(args, uint32_t), HEX, colour, true);
						break;
					case 's':
						count += write_str(va_arg(args, char*), colour);
						break;
				}
				state = STATE_NORMAL;
				break;

			case STATE_PRECISION:
				// if it's not a valid char, we assume precision 6, we go back by one so that we don't skip the specifier
				if (!(*fmt >= '0' && *fmt <= '9')) {
					prec = DEF_PREC;
					state = STATE_SPEC;
					fmt--;
					break;
				}
				state = STATE_SPEC;
				prec = *fmt - '0';
				break;
		}
	}
	return count;

}

int printf(const char *fmt, uint8_t colour, ...)
{
	va_list args;
	int count = 0;
	va_start(args, colour);
	count = vprintf(fmt, colour, args);
	va_end(args);

 	return count;
}

static int write_ch(unsigned char ch, uint8_t colour)
{
	print_char(ch, colour);
	return 1;
}

static int write_str(char *s, uint8_t colour) 
{
	while(*s)
		write_ch(*s++, colour);

	return strlen(s);
}


static int write_float(double n, uint8_t prec, uint8_t colour)
{
	char buffer[STR_BUFFER];
	uint8_t base = DECIMAL;
	uint32_t whole_n = (uint32_t)n, pos = 0;

	// we use powers of 10 based on the precision (numbers after the decimal point) we want
	const double pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
	
	// if given index is out of bound, we default to 6
	if (prec >= sizeof(pow10) / sizeof(pow10[0]))
		prec = 6;

	// check for overflow 
	if ((pow10[prec] * (n - whole_n)) > UINT_MAX) {
		klog(WARNING, "printf", "buffer overflow prevented.");
		return 0;
	}

	// write fractional
	uint32_t frac_n = pow10[prec] * (n - whole_n);
	do {
		if (pos >= (sizeof(buffer) / sizeof(buffer[0]))) {
			klog(WARNING, "printf", "buffer overflow prevented.");
			return 0;
		}
		buffer[pos++] = (frac_n % 10) + '0';
		frac_n /= base;
	} while(frac_n > 0);
	buffer[pos++] = '.';
	
	// write whole
	do {
		if (pos >= (sizeof(buffer) / sizeof(buffer[0]))) {
			klog(WARNING, "printf", "buffer overflow prevented.");
			return 0;
		}
		buffer[pos++] = (whole_n % 10) + '0';
		whole_n /= base;
	} while(whole_n > 0);

	// Reverse buffer
	for (uint32_t i = 0; i < pos / 2; ++i) {
		unsigned char tmp = buffer[i];
		buffer[i] = buffer[pos - 1 - i];
		buffer[pos - 1 - i] = tmp;
	}
	buffer[pos] = '\0';

	printf("%s", colour, buffer);
	return 1;
}


static int write_unsigned(uint32_t n, uint8_t base, uint8_t colour, uint8_t hex_uppercase)
{
	char buffer[STR_BUFFER];
	uint32_t pos = 0;

	do {
		if (pos >= (sizeof(buffer) / sizeof(buffer[0]))) {
			klog(WARNING, "printf", "buffer overflow prevented.");
			break;
		}
		char c = itoa_list[n % base];
		if (hex_uppercase)
			c = toupper(c);
		buffer[pos++] = c;
		n /= base;
	} while(n > 0);

	// Reverse buffer
	for (uint32_t i = 0; i < pos / 2; ++i) {
		unsigned char tmp = buffer[i];
		buffer[i] = buffer[pos - 1 - i];
		buffer[pos - 1 - i] = tmp;
	}
	buffer[pos] = '\0';

	return write_str(buffer, colour);
}

static int write_signed(int32_t n, uint8_t base, uint8_t colour, uint8_t hex_uppercase)
{
	char *itoa_list = "0123456789abcdef";
	char buffer[STR_BUFFER];
	int32_t pos = 0;

	bool negative  = false;

	if (n < 0) {
		negative = true;
	}

	do {
		if (pos >= (int32_t)(sizeof(buffer) / sizeof(buffer[0]))) {
			klog(WARNING, "printf", "buffer overflow prevented.");
			break;
		}
		char c = itoa_list[n % base];
		if (hex_uppercase)
			c = toupper(c);
		buffer[pos++] = c;
		n /= base;
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

