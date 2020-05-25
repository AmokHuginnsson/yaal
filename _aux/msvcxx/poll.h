#ifndef YAAL_MSVCXX_SYS_POLL_H_INCLUDED
#define YAAL_MSVCXX_SYS_POLL_H_INCLUDED 1

typedef int nfds_t;
#undef POLLIN
static int const POLLIN = 1;
#undef POLLOUT
static int const POLLOUT = 2;
#undef POLLHUP
static int const POLLHUP = 4;
#undef POLLERR
static int const POLLERR = 8;
#undef POLLPRI
static int const POLLPRI = 16;

namespace msvcxx {

struct pollfd {
	int fd;
	short int events;
	short int revents;
};

int poll( msvcxx::pollfd*, nfds_t, int );

}

inline int poll( msvcxx::pollfd* pfd_, nfds_t n_, int t_ ) {
	return ( msvcxx::poll( pfd_, n_, t_ ) );
}

#define pollfd msvcxx::pollfd

#endif /* not YAAL_MSVCXX_SYS_POLL_H_INCLUDED */

