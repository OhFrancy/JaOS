#
# Building Makefile for the Cross-toolchain.
#

TARGET := i686-elf

GCC_VERSION      := 15.2.0
BINUTILS_VERSION := 2.42

GCC_URL		  := https://ftpmirror.gnu.org/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.gz
BINUTILS_URL     := https://ftpmirror.gnu.org/binutils/binutils-$(BINUTILS_VERSION).tar.gz

OUT_DIR := $$HOME/opt/cross_jaos
BIN_DIR := $(OUT_DIR)/build-binutils-$(BINUTILS_VERSION)
GCC_DIR := $(OUT_DIR)/build-gcc-$(GCC_VERSION)

PREFIX = $(OUT_DIR)/$(TARGET)
export PATH := "$(PREFIX)/bin:$(PATH)"

.PHONY: toolchain gcc binutils clean_all
toolchain: clean_toolchain makedir_toolchain gcc cleanup

binutils:
	@cd $(OUT_DIR) && wget $(BINUTILS_URL)
	@cd $(OUT_DIR) && tar -xvf binutils-$(BINUTILS_VERSION).tar.gz
	@rm $(OUT_DIR)/binutils-$(BINUTILS_VERSION).tar.gz
	@mkdir -p $(BIN_DIR)
	@cd $(BIN_DIR) && ../binutils-$(BINUTILS_VERSION)/configure \
		--prefix="$(PREFIX)" \
		--target=$(TARGET)	 \
		--with-sysroot		 \
		--disable-nls		 \
		--disable-werror \
		CFLAGS="-Wno-error=maybe-uninitialized"
	@make -j$(nproc --ignore 1) -C $(BIN_DIR)
	@make -j$(nproc --ignore 1) -C $(BIN_DIR) install

gcc: binutils
	@cd $(OUT_DIR) && wget $(GCC_URL)
	@cd $(OUT_DIR) && tar -xvf gcc-$(GCC_VERSION).tar.gz
	@rm $(OUT_DIR)/gcc-$(GCC_VERSION).tar.gz
	@mkdir -p $(GCC_DIR)
	@cd $(GCC_DIR) && ../gcc-$(GCC_VERSION)/configure \
		--prefix="$(PREFIX)"	  \
		--target=$(TARGET)	       \
		--disable-nls			  \
		--enable-languages=c,c++   \
		--without-headers	\
		--disable-werror
	@make -C $(GCC_DIR) all-gcc all-target-libgcc CFLAGS="-Wno-error" CXXFLAGS="-Wno-error"
	@make -C $(GCC_DIR) install-gcc install-target-libgcc CFLAGS="-Wno-error" CXXFLAGS="-Wno-error"

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
