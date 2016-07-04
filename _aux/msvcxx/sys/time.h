#ifndef YAAL_MSVCXX_SYS_TIME_H_INCLUDED
#define YAAL_MSVCXX_SYS_TIME_H_INCLUDED 1

#define log cmath_log
#include <algorithm>
#undef log
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
#undef FD_ISSET
#undef FD_ZERO
#undef FD_SET

#include "cleanup.hxx"

#include "hcore/macro.hxx"

struct fd_set {
	static int const MAXIMUM_FD_WAIT_OBJECTS = FD_SETSIZE;
	int _data[MAXIMUM_FD_WAIT_OBJECTS];
	int _count;
};

namespace asio {

inline bool FD_ISSET( int fd_, fd_set const* fdset_ ) {
	bool isSet( false );
	if ( fdset_->_count > 0 )
		isSet = ( std::find( fdset_->_data, fdset_->_data + fdset_->_count, fd_ ) != ( fdset_->_data + fdset_->_count ) );
	return ( isSet );
}

inline void FD_ZERO( fd_set* fdset_ ) {
	std::fill_n( fdset_->_data, fd_set::MAXIMUM_FD_WAIT_OBJECTS, -1 );
	fdset_->_count = 0;
}

inline void FD_SET( int fd_, fd_set* fdset_ )
	{ fdset_->_data[ fdset_->_count ++ ] = fd_; }
inline void FD_SET_ms( int fd_, fd_set* fdset_ )
	{ FD_SET( fd_, fdset_ ); }
#define FD_SET FD_SET_ms

}

using namespace asio;

inline struct tm *gmtime_r(const time_t *timep, struct tm *result) {
	gmtime_s( result, timep );
	return ( result );
}

inline struct tm *localtime_r(const time_t *timep, struct tm *result) {
	localtime_s( result, timep );
	return ( result );
}

typedef int clockid_t;
typedef void* timer_t;

struct itimerspec {
	timespec it_value;
	timespec it_interval;
};

M_YAAL_HCORE_PUBLIC_API
int timer_create( clockid_t, struct sigevent*, timer_t* );
M_YAAL_HCORE_PUBLIC_API
int timer_settime( timer_t, int, struct itimerspec const*, struct itimerspec* );
M_YAAL_HCORE_PUBLIC_API
int timer_delete( timer_t );

namespace msvcxx {

M_YAAL_HCORE_PUBLIC_API
int select( int, fd_set*, fd_set*, fd_set*, struct timeval* );

}

inline int select( int ndfs_,
									fd_set* readfs_, fd_set* writefs_,
									fd_set* extrafs_, struct timeval* timeout_ )
	{ return ( msvcxx::select( ndfs_, readfs_, writefs_, extrafs_, timeout_ ) ); }

extern "C"
int gettimeofday( struct timeval*, struct timezone* );

#endif /* not YAAL_MSVCXX_SYS_TIME_H_INCLUDED */

