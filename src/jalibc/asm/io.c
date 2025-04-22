#include <asm/io.h>

void outb(uint8_t value, uint16_t port)
{
	asm volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

void outw(uint16_t value, uint16_t port)
{
	asm volatile("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

void outl(uint32_t value, uint16_t port)
{
	asm volatile("outl %0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port) {
	uint8_t ret;
	asm volatile("inb %w1, %0" : "=a"(ret) : "Nd"(port) : "memory");
	return ret;
}

uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile("inw %w1, %0" : "=a"(ret) : "Nd"(port) : "memory");
	return ret;
}

uint32_t inl(uint16_t port)
{
	uint32_t ret;
	asm volatile("inl %w1, %0" : "=a"(ret) : "Nd"(port) : "memory");
	return ret;
}
