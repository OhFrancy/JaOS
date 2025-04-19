#!/bin/bash

#
# Install missing dependency packages, integrates 'scripts/detect_distro.sh'.
#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#

. scripts/config.sh
. scripts/detect_distro.sh

DISTRO=
CHECK=
UPDATE=L
INSTALL=
DEPENDENCIES=

UBUNTU_DEBIAN=(
     build-essential
	gcc
	make
     nasm
	qemu-system-x86
	qemu-system-common
	libgmp3-dev
	libmpc-dev
	libmpfr-dev
	gcc-multilib
	g++-multilib
	libc6-dev-i386
	lib32gcc-13-dev
	aria2
	mtools
)
FEDORA=(
	gcc
	gcc-c++
	make
	nasm
	qemu-system-x86
	qemu-system-common
	gmp-devel
	libmpc-devel
	mpfr-devel
	glibc-devel
	libstdc++-devel.i686
	libgcc.i686
	aria2
	mtools
)
ARCH=(
	 gcc
	 make
	 nasm
	 qemu-system-x86
	 qemu-system-common
	 libgmp-static
	 libmpc
	 mpfr
	 aria2
	 mtools
)

# Installs all packages in the first argument passed, using the packet manager specified in $INSTALL
install() {
	PKGS=("$@")
	PKGS=${PKGS[@]}

	prompt_confirm "Installing through '$INSTALL $PKGS', do you want to continue?"
	status=$?
	if [ "$status" -eq 1 ]; then
		throw_info 1 "dependencies installation process stopped by the user"
	fi

	$UPDATE
	status=$?
	if [ ! "$status" -eq 0 ]; then
		throw_info 0 "update process failed, proceeding to the installation."
	fi

	$INSTALL $PKGS
	status=$?
	if [ ! "$status" -eq 0 ]; then
		throw_info 1 "dependencies installation process failed."
	fi
}

prompt_confirm() {
	while true; do
		read -p "$1 [Y/n] " REPLY
			case $REPLY in
			[yY]) echo ; return 0 ;;
	[nN]) echo ; return 1 ;;
	*) echo "${R}{BF}Invalid input..."
		esac
		done
}

# To install will be filled by check_distro if successful
to_install=()
check_distro

echo "Checking for dependencies packages."

echo ""
if [ "${#to_install[@]}" -gt 0 ]; then
	install "${to_install[@]}"
else
	echo "Found all packages already installed."
fi

throw_info 2 "dependencies installation process completed."

