TARG			:= i686-elf
TOOLCHAIN_PATH := $$HOME/opt/cross_jaos/$(TARG)/bin
PREFIX		:= $(TOOLCHAIN_PATH)/$(TARG)-

ASM	 := nasm
T_CC	 := $(PREFIX)gcc
T_LD  := $(PREFIX)ld
T_ASM := $(PREFIX)as


