#!/bin/bash
#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#

. src/scripts/config.sh

DISK_IMG=jaos.img
MEDIA=build/$DISK_IMG			# Bootable image
ARCH=x86_64				# Architecture
RAM=4196M				# MBs
CPU=core2duo

echo "Running $MEDIA through QEMU"

if [ "$1" = "debug" ]; then
	qemu-system-$ARCH \
	-cpu host \
	-accel kvm \
	-pflash OVMF/OVMF.fd \
	-m $RAM \
	-s -S
else
	qemu-system-$ARCH \
	-cpu host \
	-accel kvm \
	-m $RAM \
	-pflash OVMF/OVMF_CODE.fd \
	-pflash OVMF/OVMF_VARS.fd \
	-drive file=$MEDIA,format=raw \
	-net none
fi

