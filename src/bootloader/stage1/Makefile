#
# Building Makefile for the JaOS bootloader's first stage.
#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#

include ../../../scripts/config.mk

BUILD_DIR := ../../../build/stage1
TARGET	:= $(BUILD_DIR)/boot1.bin

LD_FLAGS  := -T link.ld
ASM_FLAGS := -f elf32

SRC  	= $(wildcard *.asm)
OBJS		= $(patsubst %.asm,tmp/%.o,$(SRC))

.PHONY: all makedir clean

all: $(TARGET) clean

$(TARGET): $(OBJS)
	@echo "	- Linking into a single flat binary..."
	@$(T_LD) $(LD_FLAGS) $^ -o $@
	@echo "	- Stage 1 built successfully."

tmp/%.o: %.asm makedir
	@echo "	- Assembling first stage..."
	@$(ASM) $(ASM_FLAGS) $< -o $@

clean:
	@echo "	- Cleaning temporary directory..."
	@rm -rf tmp

makedir:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p tmp
