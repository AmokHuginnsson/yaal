#ifndef YAAL_MSVCXX_SYS_TIME_H_INCLUDED
#define YAAL_MSVCXX_SYS_TIME_H_INCLUDED 1

#include <algorithm>
#include <cerrno>
#include <pthread.h>

#define select select_off
#define fd_set fd_set_off
#define inet_ntop inet_ntop_off
#include <ws2tcpip.h>
#undef inet_ntop
#undef inet_ntop
#undef select
#undef fd_set

#include "cleanup.hxx"

#include "hcore/macro.hxx"

struct timezone {
	int tz_minutewest;
	int tz_dsttime;
};

int gettimeofday( struct timeval*, struct timezone* );

struct fd_set;
typedef int clockid_t;

namespace msvcxx {

M_YAAL_HCORE_PUBLIC_API
int select( int, fd_set*, fd_set*, fd_set*, struct timeval* );

}

inline int select(
	int ndfs_,
	fd_set* readfs_, fd_set* writefs_,
	fd_set* extrafs_, struct timeval* timeout_
) {
	return ( msvcxx::select( ndfs_, readfs_, writefs_, extrafs_, timeout_ ) );
}

#endif /* not YAAL_MSVCXX_SYS_TIME_H_INCLUDED */

