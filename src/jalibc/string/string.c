#include <string.h>

size_t strlen(const char *str, size_t max_sz)
{
	char *end;
	if ((end = memchr(str, '\0', max_sz)) == NULL) {
		return max_sz;
	}
	return end - str;
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
