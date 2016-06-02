#ifndef YAAL_MSVCXX_FCNTL_H_INCLUDED
#define YAAL_MSVCXX_FCNTL_H_INCLUDED 1

namespace msvcxx {

int fcntl( int, int, int );

} /* namespace msvcxx */

inline int ms_fcntl( int fd_, int cmd_, int arg_ )
	{ return ( msvcxx::fcntl( fd_, cmd_, arg_ ) ); }
#define fcntl ms_fcntl

#endif /* not YAAL_MSVCXX_FCNTL_H_INCLUDED */

