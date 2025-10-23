#!/bin/bash

#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#

. src/miscellaneous/config.sh

DISK_IMG=jaos.img
MEDIA=build/$DISK_IMG			# Bootable image
ARCH=i386				# Architecture
RAM=4196M				# MBs
CPU=core2duo

echo "Running $MEDIA through QEMU"

if [ "$2" = "debug" ]; then
	qemu-system-$ARCH \
	-cpu host \
	-accel kvm \
	-m $RAM \
	-drive file=$MEDIA,format=raw,if=$1 \
	-s -S \
	-device VGA
else
	qemu-system-$ARCH \
	-cpu host \
	-accel kvm \
	-m $RAM \
	-drive file=$MEDIA,format=raw,if=$1 \
	-device VGA
fi
