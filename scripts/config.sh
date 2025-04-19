BF=$(tput bold)
DEF=$(tput sgr0)

R=$(tput setaf 1)
G=$(tput setaf 2)
M=$(tput setaf 5)
W=$(tput setaf 7)

# Warning if $1 = 0, panic if $1 = 1, success if $1 = 2, defaults to warning.
# $2 must be the string to print
throw_info() {
	if [ "$1" -eq 1 ]; then
		echo "${R}${BF}Panic: ${W}${BF}$2${DEF}"
		exit 1
	elif [ "$1" -eq 2 ]; then
		echo "${G}${BF}Success: ${W}${BF}$2${DEF}"
		return 0
	fi
     echo "${M}${BF}Warning: ${W}${BF}$2${DEF}"
}
