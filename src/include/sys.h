#ifndef SYS_H
#define SYS_H

#define HALT { asm volatile("hlt"); }
#define STOP while (1) { HALT; }

#endif
