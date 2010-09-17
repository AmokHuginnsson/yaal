#ifndef YAAL_MSVCXX_SYS_TIME_H_INCLUDED
#define YAAL_MSVCXX_SYS_TIME_H_INCLUDED 1

#include <algorithm>

#define select select_off
#define fd_set fd_set_off
#include <glibc/sys/time.h>
#include <sys/types.h>
#undef fd_set
#undef FD_ZERO
#undef FD_ISSET
#undef FD_SET
#undef select


struct fd_set
	{
	static int const MAXIMUM_FD_WAIT_OBJECTS = 64;
	int _data[MAXIMUM_FD_WAIT_OBJECTS];
	int _count;
	};

namespace asio
{

inline bool FD_ISSET( int const& fd_, fd_set const* fdset_ )
	{ return ( ( fdset_->_count > 0 ) && ( fdset_->_data[ 0 ] == fd_ ) ); }

inline void FD_ZERO( fd_set* fdset_ )
	{ ::memset( fdset_->_data, -1, fd_set::MAXIMUM_FD_WAIT_OBJECTS * sizeof ( int ) ); fdset_->_count = 0; }

inline void FD_SET( int const& fd_, fd_set* fdset_ )
	{ fdset_->_data[ fdset_->_count ++ ] = fd_; }

}

using namespace asio;

int unix_select( int, fd_set*, fd_set*, fd_set*, struct timeval* );
inline int select( int ndfs_,
		fd_set* readfs_, fd_set* writefs_,
		fd_set* extrafs_, struct timeval* timeout_ )
	{ return ( unix_select( ndfs_, readfs_, writefs_, extrafs_, timeout_ ) ); }


#endif /* not YAAL_MSVCXX_SYS_TIME_H_INCLUDED */

