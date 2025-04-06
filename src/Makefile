BUILD_DIR := build
SRC_DIR	:= src

BL_SRC 	:= $(SRC_DIR)/bootloader/boot1.asm
BL2_SRC	:= $(SRC_DIR)/bootloader/boot2.asm

# Kernel not present yet
# KL_SRC 	:= $(SRC_DIR)/kernel/

FLOPPY 	:= $(BUILD_DIR)/jaos.img
BL_BIN 	:= $(BUILD_DIR)/boot1.bin
BL2_BIN	:= $(BUILD_DIR)/boot2.bin

# KL_BIN 	:= $(BUILD_DIR)/kernel.bin


CC		:= gcc
ASM		:= nasm
ASMFLAGS  := -f bin

.PHONY: all ex bootloader kernel clean build_fat make_dir

#
# Floppy disk img Creation
#

all: $(FLOPPY) build_fat

ex: all
	./run


$(FLOPPY): bootloader # kernel
	@echo "Creating the floppy disk image..."
	@dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880

	@# Converts it to FAT-12 format
	@mkfs.fat -F 12 -n "JAOS" $(FLOPPY)

	@# Writes the bootloader in the boot section of the FAT file, not truncating the file
	@dd if=$(BL_BIN) of=$(FLOPPY) conv=notrunc

	@# Copy the second stage of the bootloader as a filesystem in the root dir
	@mcopy -i $(FLOPPY)	$(BL2_BIN) "::boot2.bin"

#
# Bootloader Creation
#
bootloader: $(BL_BIN) $(BL2_BIN)

$(BL_BIN): $(BL_SRC) make_dir
	@echo "Assembling the first stage of the bootloader..."
	@$(ASM) $(ASMFLAGS) $< -o $@

$(BL2_BIN): $(BL2_SRC) make_dir
	@echo "Assembling the second stage of the bootloader..."
	@$(ASM) $(ASMFLAGS) $< -o $@

#
# Kernel Creation (on-hold)
#

# kernel: $(KL_BIN)

#$(BUILD_DIR)/kernel.bin: kernel/$(KL_SRC) make_dir
#	@echo "Assembling $< to $@..."
#	@$(ASM) $(ASMFLAGS) $< -o $@

make_dir:
	@echo "Trying to create $(BUILD_DIR) directory..."
	@mkdir -p $(BUILD_DIR)

clean:
	@echo "Cleaning build folder"
	@rm -rf $(BUILD_DIR)/*

