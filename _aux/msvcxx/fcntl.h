#ifndef YAAL_MSVCXX_FCNTL_H_INCLUDED
#define YAAL_MSVCXX_FCNTL_H_INCLUDED 1

#define fcntl fcntl_off
#include <glibc/fcntl.h>
#undef fcntl

int unix_fcntl( int, int, ... );
template<typename arg_t>
inline int fcntl( int fd_, int cmd_, arg_t arg_ )
	{ return ( unix_fcntl( fd_, cmd_, arg_ ) ); }

#endif /* not YAAL_MSVCXX_FCNTL_H_INCLUDED */

