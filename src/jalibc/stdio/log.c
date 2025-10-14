#include <stdio.h>
#include <log.h>

int klog(uint8_t msg_level, const char *device, const char *fmt, ...)
{
	int count = 0, color = GRAY;
	va_list args;

	va_start(args, fmt);
	printf("[ %s ]: ", GRAY, device);
	switch (msg_level) {
		case INFO:
			color = GRAY;
			break;
		case WARNING:
			color = YELLOW;
			printf("Warning: ", color);
			break;
		case ERROR:
			color = L_RED;
			printf("Error: ", color);
			break;
		case FATAL:
			color = RED;
			printf("Fatal: ", color);
			break;
	}
	vprintf(fmt, color, args);
	va_end(args);
	return count;
}

