#ifndef __SYS_H__
#define __SYS_H__

#define HALT { asm volatile("hlt"); }
#define STOP while (1) { HALT; }

#endif
