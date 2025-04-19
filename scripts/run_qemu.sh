#!/bin/bash

#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#

#
# Automation script for building and running the Operating System through qemu.
#
# It SHOULD handle most of the edge-cases related to error-handling; however, if you encounter any unknown error, you can report it as an issue.
# It's a replacement to what in the future (I hope near!) might be a GUI version to build the operating system.
#
# Last modified: 15/04/2025
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
