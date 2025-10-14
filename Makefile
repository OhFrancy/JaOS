#
# Building Makefile for JaOS
#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#
include src/miscellaneous/config.mk

BUILD_DIR  := build
SRC_DIR	 := src
MISC_DIR := src/miscellaneous

FLOPPY 	 := $(BUILD_DIR)/jaos.img

BL1_DIR	 := $(SRC_DIR)/bootloader/stage1
BL2_DIR	 := $(SRC_DIR)/bootloader/stage2
LOADER_DIR := $(SRC_DIR)/bootloader/loader
DRIVER_DIR := $(SRC_DIR)/drivers

JALIBC_DIR	:= $(SRC_DIR)/jalibc
JALIBC_INCLUDE := $(JALIBC_DIR)/include

BL1_BIN 	 := $(BUILD_DIR)/stage1/boot1.bin
LOADER_BIN	 := $(BUILD_DIR)/loader/loader.bin

ASM_FLAGS := -f elf32
LD_FLAGS  := -T
CC_FLAGS  := -ffreestanding -nostdlib -g -c -Wall -Wextra -Werror -Werror=format-security -I$(JALIBC_INCLUDE) -MMD -MP 

#
# First stage srcs & objects
#

LD1_SRC	:= $(SRC_DIR)/bootloader/stage1/link.ld

ASM1_SRC  := $(shell find $(BL1_DIR) -name "*.asm")

ASM1_OBJ	:= $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM1_SRC))
OBJS1	:= $(ASM1_OBJ)

#
# Second stage assembly and kernel loader
#

LD2_SRC	:= $(SRC_DIR)/bootloader/loader/link.ld

ASM2_SRC	:= $(shell find $(BL2_DIR) -name "*.asm")
LOADER_SRC	:= $(shell find $(DRIVER_DIR) $(LOADER_DIR) $(JALIBC_DIR) -name "*.c")

ASM2_OBJ  := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM2_SRC))
LOADER_OBJ    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(LOADER_SRC))
OBJS2	:= $(ASM2_OBJ) $(LOADER_OBJ)

OBJS_ALL	:= $(OBJS1) $(OBJS2)

DEPS		:= $(OBJS_ALL:.o=.d)

build: clean install_deps build_toolchain

include $(MISC_DIR)/toolchain.mk
install_deps:
	@$(MISC_DIR)/install_deps.sh

$(FLOPPY): bootloader # kernel
	@echo "Creating the floppy disk image..."
	@dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880

	@# Converts it to FAT-12 format
	@mkfs.fat -F 12 -n "JAOS" $(FLOPPY)

	@# Writes the bootloader in the boot section of the FAT file, not truncating the file
	@dd if=$(BL1_BIN) of=$(FLOPPY) conv=notrunc

	@# Copy the second stage and loader as a filesystem in the root dir
	@mcopy -i $(FLOPPY) $(LOADER_BIN) "::loader.bin"

	@mcopy -i $(FLOPPY) $(SRC_DIR)/t.bin "::t.bin"

# Bootloader Creation
bootloader: $(BL1_BIN) $(LOADER_BIN)

$(BL1_BIN): $(LD1_SRC) $(OBJS1)
	@mkdir -p $(dir $@)
	@echo "	- Linking -> $@..."
	@$(T_LD) $(LD_FLAGS) $^ -o $@
	@echo "Stage 1 built successfully."

$(LOADER_BIN): $(LD2_SRC) $(OBJS2)
	@mkdir -p $(dir $@)
	@echo "	- Linking -> $@..."
	@$(T_LD) $(LD_FLAGS) $^ -o $@
	@echo "Stage 2 built successfully."

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	@echo "	- Assembling $< -> $@..."
	@$(ASM) $(ASM_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "	- Compiling $< -> $@..."
	@$(T_CC) $(CC_FLAGS) $< -o $@

#
# Builds the cross-tools
#
build_toolchain:
	@if [ ! -d $$HOME/opt/cross_jaos/i686-elf ]; then	\
		echo "Installing toolchain..."	; \
		make clean_toolchain toolchain	; \
		fi
		@echo "Toolchain already installed, proceeding..."

#
# Run script, currently emulated with QEMU
#
run: $(FLOPPY)
	@$(MISC_DIR)/run_qemu.sh

makedir:
	@mkdir -p $(BUILD_DIR)

clean:
	@echo "Cleaning build folder"
	@rm -rf $(BUILD_DIR)/*

-include $(DEPS)

