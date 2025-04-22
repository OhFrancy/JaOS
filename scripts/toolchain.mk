#
# Building Makefile for the Cross-toolchain.
#

TARGET := i686-elf

GCC_VERSION      := 13.1.0
BINUTILS_VERSION := 2.38

GCC_URL		  := https://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.gz
BINUTILS_URL     := https://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.gz

OUT_DIR := $$HOME/opt/cross_jaos
BIN_DIR := $(OUT_DIR)/build-binutils-$(BINUTILS_VERSION)
GCC_DIR := $(OUT_DIR)/build-gcc-$(GCC_VERSION)

PREFIX = $(OUT_DIR)/$(TARGET)
export PATH := "$(PREFIX)/bin:$(PATH)"

ARIA	   := aria2c -x 16 -s 16

.PHONY: toolchain gcc binutils clean_all
toolchain: clean_toolchain makedir_toolchain gcc cleanup

binutils:
	@cd $(OUT_DIR) && $(ARIA) $(BINUTILS_URL)
	@cd $(OUT_DIR) && tar -xvf binutils-$(BINUTILS_VERSION).tar.gz
	@rm $(OUT_DIR)/binutils-$(BINUTILS_VERSION).tar.gz
	@mkdir -p $(BIN_DIR)
	@cd $(BIN_DIR) && ../binutils-$(BINUTILS_VERSION)/configure \
		--prefix="$(PREFIX)" \
		--target=$(TARGET)	 \
		--with-sysroot		 \
		--disable-nls		 \
		--disable-werror
	@make -j$(nproc --ignore 1) -C $(BIN_DIR)
	@make -j$(nproc --ignore 1) -C $(BIN_DIR) install

gcc: binutils
	@cd $(OUT_DIR) && $(ARIA) $(GCC_URL)
	@cd $(OUT_DIR) && tar -xvf gcc-$(GCC_VERSION).tar.gz
	@rm $(OUT_DIR)/gcc-$(GCC_VERSION).tar.gz
	@mkdir -p $(GCC_DIR)
	@cd $(GCC_DIR) && ../gcc-$(GCC_VERSION)/configure \
		--prefix="$(PREFIX)"	  \
		--target=$(TARGET)	       \
		--disable-nls			  \
		--enable-languages=c,c++   \
		--without-headers
	@make -j$(nproc --ignore 1) -C $(GCC_DIR) all-gcc all-target-libgcc
	@make -j$(nproc --ignore 1) -C $(GCC_DIR) install-gcc install-target-libgcc

makedir_toolchain:
	@mkdir -p $$HOME/opt/cross_jaos

cleanup:
	@echo "Process completed successfully, proceeding to cleanup."
	@for dir in $(OUT_DIR)/*/; do \
		if [ "$$(basename "$$dir")" = "i686-elf" ]; then continue ; fi; \
		rm -rf "$$dir" ; \
	done

clean_toolchain:
	@echo "Removing the toolchain..."
	@rm -rf $$HOME/opt/cross_jaos/*
