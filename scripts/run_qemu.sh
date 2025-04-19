#!/bin/bash

#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#

. scripts/config.sh

argv=$@
argc=$#

DISK_IMG=jaos.img
MEDIA=build/$DISK_IMG	# Bootable image
ARCH=i386				# Architecture
RAM=4196M				# MBs
CPU=core2duo

# Run arguments
qemu-system-$ARCH \
-cpu host \
-accel kvm \
-m $RAM \
-drive file=$MEDIA,format=raw,if=floppy \
-vga std
