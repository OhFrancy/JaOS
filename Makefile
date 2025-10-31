#
# Building Makefile for JaOS
#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#
include src/scripts/config.mk

BUILD_DIR	:= build
SRC_DIR		:= src

SCRIPTS_DIR	:= $(SRC_DIR)/scripts
BOOT_DIR	:= $(SRC_DIR)/boot
KERNEL_DIR	:= $(SRC_DIR)/kernel
DRIVER_DIR	:= $(SRC_DIR)/drivers
JALIBC_DIR	:= $(SRC_DIR)/jalibc
INCLUDE_DIR	:= $(SRC_DIR)/include
EFI_DIR		:= gnu-efi

DISK_IMG	:= $(BUILD_DIR)/jaos.img

BOOT		:= $(BUILD_DIR)/boot/BOOTX64.EFI
BOOT_SO		:= $(BUILD_DIR)/boot/main.so

KERNEL		:= $(BUILD_DIR)/kernel/kernel.elf

EFI_INC		:= /usr/include/efi

ASM_FLAGS	:= -f elf64
CC_FLAGS	:= -fpic -ffreestanding -nostdlib -g -c -Wall -Wextra -I$(INCLUDE_DIR) -I$(EFI_INC) -MMD -MP -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone
OBJC_FLAGS	:= -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10

LD_FLAGS_BOOT	:= -shared -Bsymbolic -L$(EFI_DIR) -z noexecstack -T 
LD_FLAGS_KERNEL	:= -T 

#
# UEFI Bootloader srcs & objs
#
BOOT_SRC	:= $(shell find $(BOOT_DIR) -name "*.c")
BOOT_OBJ	:= $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(BOOT_SRC))
BOOT_LD		:= $(EFI_DIR)/elf_x86_64_efi.lds $(EFI_DIR)/crt0-efi-x86_64.o 
#
# Kernel, drivers and libc srcs & objs
#
KERNEL_SRC_ALL	:= $(shell find $(KERNEL_DIR) -name "*.c")
KERNEL_OBJ	:= $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(KERNEL_SRC_ALL))
KERNEL_LD	:= $(KERNEL_DIR)/link.ld

OBJS_ALL	:= $(BOOT_OBJ) $(KERNEL_OBJ)
DEPS		:= $(OBJS_ALL:.o=.d)

.PHONY: all clean build install_deps run run_d build_toolchain bootloader kernel

all: $(DISK_IMG)

include $(SCRIPTS_DIR)/toolchain.mk

$(DISK_IMG): bootloader kernel
	@echo "Creating the image..."
	@mkdir -p $(BUILD_DIR)
	@dd if=/dev/zero of=$@ bs=1M count=100 status=none
	
	@echo "Formatting as FAT32..."
	@mkfs.fat -F 32 -n "JAOS" $@ > /dev/null 2>&1
	
	@echo "Creating EFI directory structure..."
	@mmd -i $@ ::/EFI
	@mmd -i $@ ::/EFI/BOOT
	
	@echo "Copying bootloader..."
	@mcopy -i $@ $(BOOT) "::/EFI/BOOT/BOOTX64.EFI"
	
	@echo "Copying kernel..."
	@mcopy -i $@ $(KERNEL) "::/kernel.elf"
	
	@echo "Disk image created: $@"

bootloader: $(BOOT)

$(BOOT): $(BOOT_LD) $(BOOT_OBJ)
	@mkdir -p $(dir $@)
	@echo "Building UEFI Bootloader:"
	@echo "		- Linking $^ -> $(BOOT_SO)"
	@$(T_LD) $(LD_FLAGS_BOOT) $^ -o $(BOOT_SO) -lgnuefi -lefi
	@echo "		- Object copying $(BOOT_SO) -> $@"
	@$(OBJC) $(OBJC_FLAGS) $(BOOT_SO) $@
	@echo "UEFI Bootloader built successfully."

kernel: $(KERNEL)

$(KERNEL): $(KERNEL_LD) $(KERNEL_OBJ)
	@mkdir -p $(dir $@)
	@echo "Building kernel:"
	@echo "		- Linking $^ -o $@"
	@$(T_LD) $(LD_FLAGS_KERNEL) $^ -o $@
	@echo "Kernel built successfully."

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $< -> $@"
	@$(T_CC) $(CC_FLAGS) $< -o $@

install_deps:
	@$(SCRIPTS_DIR)/install_deps.sh

#
# Builds the cross-tools
#
build_toolchain:
	@if [ ! -d $$HOME/opt/cross_jaos/x86_64-elf ]; then \
		echo "Installing toolchain..."; \
		make clean_toolchain toolchain; \
	else \
		echo "Toolchain already installed, proceeding..."; \
	fi

build: clean install_deps build_toolchain all

run: clean $(DISK_IMG)
	@$(SCRIPTS_DIR)/run_qemu.sh 

#
# Debug with GDB
#
run_d: $(DISK_IMG)
	@$(SCRIPTS_DIR)/run_qemu.sh debug

clean:
	@echo "Cleaning build folder"
	@rm -rf $(BUILD_DIR)/*

-include $(DEPS)
