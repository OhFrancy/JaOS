#include <string.h>
#include <mem.h>

void *memcpy(void *dst, const void *src, size_t len)
{
	char *d = (char *)dst;
	char *s = (char *)src;

	for (size_t i = 0; i < len; ++i)
		d[i] = s[i];

	return dst;
}

void *memset(void *s, int c, size_t n)
{
	unsigned char *dst = s;

	for (size_t i = 0; i < n; ++i) {
		dst[i] = c;	
	}
	return s;
}
