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
BL1_DIR	 := $(SRC_DIR)/bootloader/stage1
BL2_DIR	 := $(SRC_DIR)/bootloader/stage2
BL1_5_DIR := $(SRC_DIR)/bootloader/stage1_5
LOADER_DIR := $(SRC_DIR)/bootloader/kernel_loader
DRIVER_DIR := $(SRC_DIR)/drivers
JALIBC_DIR	:= $(SRC_DIR)/jalibc

HDD_IMG	 := $(BUILD_DIR)/jaos.img

JALIBC_INCLUDE := $(JALIBC_DIR)/include

BL1_BIN 	 := $(BUILD_DIR)/stage1/boot1.bin
BL1_5_BIN	 := $(BUILD_DIR)/stage1_5/boot1_5.bin
LOADER_BIN	 := $(BUILD_DIR)/kernel_loader/loader.bin

ASM_FLAGS := -f elf
LD_FLAGS  := -T
CC_FLAGS  := -ffreestanding -nostdlib -g -c -Wall -Wextra -Werror -Werror=format-security -I$(JALIBC_INCLUDE) -MMD -MP 

#
# First stage srcs & objects
#
LD1_SRC	  := $(BL1_DIR)/link.ld
ASM1_SRC  := $(shell find $(BL1_DIR) -name "*.asm")
ASM1_OBJ  := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM1_SRC))
OBJS1	  := $(ASM1_OBJ)

#
# 1.5 Stage srcs & objects
#
LD1_5_SRC  := $(BL1_5_DIR)/link.ld
ASM1_5_SRC := $(shell find $(BL1_5_DIR) -name "*.asm")
ASM1_5_OBJ := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM1_5_SRC))
OBJS1_5    := $(ASM1_5_OBJ)

#
# Second stage assembly and kernel loader
#
LD2_SRC	:= $(LOADER_DIR)/link.ld

ASM2_SRC	:= $(shell find $(BL2_DIR) -name "*.asm")
LOADER_SRC	:= $(shell find $(DRIVER_DIR) $(LOADER_DIR) $(JALIBC_DIR) -name "*.c")

ASM2_OBJ  	:= $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM2_SRC))
LOADER_OBJ      := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(LOADER_SRC))
OBJS2		:= $(ASM2_OBJ) $(LOADER_OBJ)

OBJS_ALL	:= $(OBJS1) $(OBJS1_5) $(OBJS2)
DEPS		:= $(OBJS_ALL:.o=.d)

build: clean install_deps build_toolchain

.PHONY: all clean build install_deps run run_d build_toolchain

include $(MISC_DIR)/toolchain.mk

$(HDD_IMG): bootloader
	@echo "Creating the hard disk image..."
	@dd if=/dev/zero of=$(HDD_IMG) bs=512 count=204800
	
	@# Converts it to FAT32
	@mkfs.fat -F 32 -n "JAOS" $(HDD_IMG)
	
	@#Load stage 1
	@dd if=$(BL1_BIN) of=$(HDD_IMG) bs=512 count=1 conv=notrunc

	@#Load stage 1.5
	@dd if=$(BL1_5_BIN) of=$(HDD_IMG) bs=512 seek=2 conv=notrunc

	@# Copy the second stage and loader as a filesystem in the root dir
	@mcopy -i $(HDD_IMG) $(LOADER_BIN) "::/loader.bin"

# Bootloader Creation
bootloader: $(BL1_BIN) $(BL1_5_BIN) $(LOADER_BIN)

$(BL1_BIN): $(LD1_SRC) $(OBJS1)
	@mkdir -p $(dir $@)
	@echo "	- Linking -> $@..."
	@$(T_LD) $(LD_FLAGS) $^ -o $@
	@echo "Stage 1 built successfully."
	
$(BL1_5_BIN): $(LD1_5_SRC) $(OBJS1_5)
	@mkdir -p $(dir $@)
	@echo "	- Linking -> $@..."
	@$(T_LD) $(LD_FLAGS) $^ -o $@
	@echo "Stage 1.5 built successfully."

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

install_deps:
	@$(MISC_DIR)/install_deps.sh

#
# Builds the cross-tools
#
build_toolchain:
	@if [ ! -d $$HOME/opt/cross_jaos/i686-elf ]; then	\
		echo "Installing toolchain..."	; \
		make clean_toolchain toolchain	; \
		fi
		@echo "Toolchain already installed, proceeding..."

run: clean $(HDD_IMG)
	@$(MISC_DIR)/run_qemu.sh virtio

#
# Debug with GDB
#
run_d: $(HDD_IMG)
	@$(MISC_DIR)/run_qemu.sh virtio debug

clean:
	@echo "Cleaning build folder"
	@rm -rf $(BUILD_DIR)/*

-include $(DEPS)
