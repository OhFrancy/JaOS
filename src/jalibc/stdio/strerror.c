#include <stdio.h>
#include <errno.h>

/*
 * Returns the string of the given error code, so that we can easily log it out
 */
const char *strerror(int errno)
{
	// negate back to positive value so that we don't need to negate in every case
	switch(-errno) {
		case SUCCESS: return "Success.";
		case EPERM: return "Operation not permitted.";
		case ENOENT: return "No such file or directory found.";
		case ESRCH: return "No such process.";
		case EIO: return "I/O error.";
		case ENXIO: return "No such device or address";
		case ENODEV: return "No such device.";
		case ENOTDIR: return "Not a directory.";
		case EISDIR: return "Is a directory";
		case EINVAL: return "Invalid argument passed to a function.";
	}
	
	return "Unknown error.";
}

