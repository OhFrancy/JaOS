#ifndef ERRNO_H
#define ERRNO_H

enum Errno {
	SUCCESS		= 0,
	EPERM 		= 1, 	// Not super-user
	ENOENT 		= 2, 	// No such file or directory
	ESRCH 		= 3, 	// No such process
	EIO 		= 5, 	// I/O error
	ENXIO 		= 6, 	// No such device or address
	ENODEV 		= 19, 	// No such device
	ENOTDIR 	= 20, 	// Not a directory
	EISDIR 		= 21, 	// Is a directory
	EINVAL 		= 22, 	// Invalid argument
};

#endif
