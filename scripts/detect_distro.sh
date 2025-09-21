#
# Distribution and packet manager detection, os-release and PM checking.
#
#	Copyright (c) 2025 Francesco Lauro. All rights reserved.
#	SPDX-License-Identifier: MIT
#

# Variable setting functions
ubuntu_debian() {
	DISTRO='Ubuntu/Debian'
	CHECK='dpkg -s'
	UPDATE='sudo apt-get update'
	INSTALL='sudo apt-get install'
	DEPENDENCIES=( "${UBUNTU_DEBIAN[@]}" )

     	throw_info 2 "detected $DISTRO as running distribution."
}

# Additional check for dnf and yum
fedora() {
	PM=
	if command -v dnf &> /dev/null; then
		PM='dnf'
	elif command -v yum &> /dev/null; then
		PM='yum'
	else
		throw_info 1 "unknown packet manager for Fedora, can't continue."
	fi
	DISTRO='Fedora'
	CHECK='rpm -q'
	INSTALL='sudo $PM install'
	DEPENDENCIES=( "${FEDORA[@]}" )

     throw_info 2 "detected $DISTRO as running distribution."

}

rhel_centos() {
	DISTRO='RHEL/CentOS'S
	CHECK='rpm -q'
	INSTALL='sudo yum install'
	DEPENDENCIES=( "${FEDORA[@]}" )

     throw_info 2 "detected $DISTRO as running distribution."
}
arch() {
	DISTRO='Arch'
	CHECK='pacman -Qi'
	UPDATE='sudo pacman -Syy'
	INSTALL='sudo pacman -S'
	DEPENDENCIES=( "${ARCH[@]}" )

     throw_info 2 "detected $DISTRO as running distribution."
}


# Fallbacks if /etc/os-release is not found or distribution is not detected in either ID or ID_LIKE
check_distro() {
     if [ -f /etc/os-release ]; then
		. /etc/os-release
		echo "Starting distribution detection through /etc/os-release."

		case "$ID" in

		"ubuntu" | "debian")
		ubuntu_debian
		;;

		"fedora")
		fedora
		;;

		"centos")
		rhel_centos
		;;

		"arch")
		arch
		;;

		*)
		throw_info 0 "${DEF}detected ${W}${BF}$ID${DEF} but it didn't match any known distribution, trying with ID_LIKE."

		case "$ID_LIKE" in

			*ubuntu* | *debian*)
			ubuntu_debian
			;;

			*fedora*)
			fedora
			;;

			*rhel* | *centos*)
			rhel_centos
			;;

			*arch*)
			arch
			;;

			*)
			throw_info 1 "distribution detection through /etc/os-release failed."
			;;
		esac
		;;
		esac
     else
          echo "Could not find ${W}${BF}/etc/os-release${DEF}, trying another method."
		fallback
	fi
}

fallback() {
	if command -v apt-get &> /dev/null; then
		ubuntu

	elif command -v dnf &> /dev/null; then
		fedora

	elif command -v yum &> /dev/null; then
		rhel_centos

	elif command -v pacman &> /dev/null; then
		arch
	else
		throw_info 1 "fallback failed, unknown linux distribution, please check the documentation."
	fi
}
