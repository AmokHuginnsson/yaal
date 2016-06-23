#ifndef YAAL_MSVCXX_SYS_IOCTL_H_INCLUDED
#define YAAL_MSVCXX_SYS_IOCTL_H_INCLUDED 1

#define TIOCGWINSZ 0x5413

#ifdef __MSVCXX__

struct winsize {
	int ws_col;
	int ws_row;
};

namespace msvcxx {
int ioctl( int, int, void* );
}

int ioctl( int fd_, int request_, void* param_ ) {
	return ( msvcxx::ioctl( fd_, request_, param_ ) );
}

#endif /* #ifdef __MSVCXX__ */

#endif /* not YAAL_MSVCXX_SYS_IOCTL_H_INCLUDED */

