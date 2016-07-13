#ifndef YAAL_MSVCXX_FCNTL_H_INCLUDED
#define YAAL_MSVCXX_FCNTL_H_INCLUDED 1

static int const O_RDONLY   = 0x0;
static int const O_WRONLY   = 0x1;
static int const O_RDWR     = 0x2;
static int const O_NOCTTY   = 0x0;
static int const O_NONBLOCK = 0x4;
static int const FD_CLOEXEC = 0x1;
static int const F_GETFD    = 0x1;
static int const F_SETFD    = 0x2;
static int const F_GETFL    = 0x3;
static int const F_SETFL    = 0x4;

namespace msvcxx {

int fcntl( int, int, int );

} /* namespace msvcxx */

inline int ms_fcntl( int fd_, int cmd_, int arg_ )
	{ return ( msvcxx::fcntl( fd_, cmd_, arg_ ) ); }
#define fcntl ms_fcntl

#endif /* not YAAL_MSVCXX_FCNTL_H_INCLUDED */

