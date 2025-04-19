#
# Building Makefile for JaOS
#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#

BUILD_DIR  := build
SRC_DIR	 := src

FLOPPY 	 := $(BUILD_DIR)/jaos.img
BL_BIN 	 := $(BUILD_DIR)/stage1/boot1.bin
BL2_BIN	 := $(BUILD_DIR)/stage2/boot2.bin

build: install_deps build_toolchain $(FLOPPY)

include scripts/toolchain.mk

install_deps:
	@scripts/install_deps.sh

$(FLOPPY): bootloader # kernel
	@echo "Creating the floppy disk image..."
	@dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880

	@# Converts it to FAT-12 format
	@mkfs.fat -F 12 -n "JAOS" $(FLOPPY)

	@# Writes the bootloader in the boot section of the FAT file, not truncating the file
	@dd if=$(BL_BIN) of=$(FLOPPY) conv=notrunc

	@# Copy the second stage of the bootloader as a filesystem in the root dir
	@mcopy -i $(FLOPPY)	$(BL2_BIN) "::boot2.bin"

	@# Copy the kernel
	@#mcopy -i $(FLOPPY) $(KL_BIN) "::kernel.bin"

# Bootloader Creation
bootloader: clean $(BL_BIN) $(BL2_BIN)

$(BL_BIN):
	@mkdir -p $(BUILD_DIR)/stage1
	@echo "Assembling the first stage of the bootloader..."
	@make -C $(SRC_DIR)/bootloader/stage1

$(BL2_BIN):
	@mkdir -p $(BUILD_DIR)/stage2
	@echo "Building the second stage of the bootloader..."
	@make -C $(SRC_DIR)/bootloader/stage2
#
# Builds the cross-tools
#
build_toolchain:
	@if [ ! -d $$HOME/opt/cross_jaos/i686-elf ]; then	\
		echo "Installing toolchain..."	; \
		make clean_toolchain toolchain	; \
	fi
	@echo "Toolchain already installed, continuing..."

#
# Run script, currently with QEMU
#
run:
	@scripts/run_qemu.sh

clean:
	@echo "Cleaning build folder"
	@rm -rf $(BUILD_DIR)/*

