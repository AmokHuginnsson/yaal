#ifndef YAAL_MSVCXX_SYS_IOCTL_H_INCLUDED
#define YAAL_MSVCXX_SYS_IOCTL_H_INCLUDED 1

#define _SYS_IOCTL_H 1

#define TIOCGWINSZ 0x5413

#include <features.h>
#include <bits/ioctls.h>
#include <bits/ioctl-types.h>
#include <sys/ttydefaults.h>

#ifdef __MSVCXX__

namespace msvcxx {
int ioctl( int, int, void* );
}

int ioctl( int fd_, int request_, void* param_ ) {
	return ( msvcxx::ioctl( fd_, request_, param_ ) );
}

#endif /* #ifdef __MSVCXX__ */

#endif /* not YAAL_MSVCXX_SYS_IOCTL_H_INCLUDED */

