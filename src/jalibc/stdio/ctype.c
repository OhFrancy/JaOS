#include <ctype.h>

int toupper(int c)
{
	if ((unsigned char)c >= 'a' && (unsigned char)c <= 'z')
		return c - 0x20;
	return c;
}
