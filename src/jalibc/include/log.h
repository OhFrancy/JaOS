#ifndef __LOG_H
#define __LOG_H

#include <stdint.h>
#include <stdarg.h>

enum Levels{
	INFO 	= 0,
	WARNING = (1 << 0),
	ERROR   = (1 << 1),
	FATAL	= (1 << 2),
};

int klog(uint8_t msg_level, const char *device, const char *fmt, ...);

#endif
