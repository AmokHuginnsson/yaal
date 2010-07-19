#ifndef YAAL_MSVCXX_SYS_TIME_H_INCLUDED
#define YAAL_MSVCXX_SYS_TIME_H_INCLUDED 1

#define select select_off
#include <glibc/sys/time.h>
#undef select

int unix_select( int, fd_set*, fd_set*, fd_set*, struct timeval* );
inline int select( int ndfs_,
		fd_set* readfs_, fd_set* writefs_,
		fd_set* extrafs_, struct timeval* timeout_ )
	{ return ( unix_select( ndfs_, readfs_, writefs_, extrafs_, timeout_ ) ); }

#endif /* not YAAL_MSVCXX_SYS_TIME_H_INCLUDED */

