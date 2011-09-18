#ifndef YAAL_MSVCXX_FCNTL_H_INCLUDED
#define YAAL_MSVCXX_FCNTL_H_INCLUDED 1

#define fcntl fcntl_off
#include <glibc/fcntl.h>
#undef fcntl

namespace msvcxx {

int fcntl( int, int, int );

} /* namespace msvcxx */

inline int fcntl( int fd_, int cmd_, int arg_ )
	{ return ( msvcxx::fcntl( fd_, cmd_, arg_ ) ); }

#endif /* not YAAL_MSVCXX_FCNTL_H_INCLUDED */

