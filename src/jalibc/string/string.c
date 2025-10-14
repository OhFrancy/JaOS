#include <string.h>

size_t strlen(const char *str)
{
	const char *s = str;

	for (; *s; ++s);
	return s - str;
}

void *memchr(const void *str, char ch, size_t n)
{
	if (n == 0) {
		return NULL;
	}
	const char *s = str;
	while (n != 0) {
		if (*s++ == ch)
			return ((void*)(s - 1));
		--n;
	}
	return NULL;
}
