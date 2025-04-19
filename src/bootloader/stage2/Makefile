#
# Building Makefile for the JaOS bootloader's second stage
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT

include ../../../scripts/config.mk
BUILD_DIR = ../../../build/stage2

TARGET	= $(BUILD_DIR)/boot2.bin

ASM_FLAGS = -f elf32
LD_FLAGS  = -T link.ld
CC_FLAGS  = -m32 -ffreestanding -nostdlib -g -c -Wall -Werror -Werror=format-security

ASM_SRC   = $(wildcard *.asm)
C_SRC	= $(wildcard *.c)

ASM_OBJ	= $(patsubst %.asm,tmp/%.o,$(ASM_SRC))
C_OBJ	= $(patsubst %.c,tmp/%.o,$(C_SRC))

.PHONY: all makedir clean

all: $(TARGET) clean

$(TARGET): $(ASM_OBJ) $(C_OBJ)
	@echo "	- Linking into a single flat binary..."
	@$(T_LD) $(LD_FLAGS) $^ -o $@
	@echo "	- Stage 2 built successfully."

tmp/%.o: %.asm makedir
	@echo "	- Assembling the entry point..."
	@$(ASM) $(ASM_FLAGS) $< -o $@

tmp/%.o: %.c makedir
	@echo "	- Compiling second stage..."
	@$(T_CC) $(CC_FLAGS) $< -o $@

clean:
	@echo "	- Cleaning temporary directory..."
	@rm -rf tmp

makedir:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p tmp

