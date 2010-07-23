#ifndef YAAL_MSVCXX_SYS_TIME_H_INCLUDED
#define YAAL_MSVCXX_SYS_TIME_H_INCLUDED 1

#include <hash_set>

#define select select_off
#define fd_set fd_set_off
#include <glibc/sys/time.h>
#include <sys/types.h>
#undef fd_set
#undef FD_ZERO
#undef FD_ISSET
#undef FD_SET
#undef select

typedef stdext::hash_set<int> fd_set;

namespace asio
{

inline bool FD_ISSET( int const& fd_, fd_set const* fdset_ )
	{ return ( fdset_->count( fd_ ) == 1 ); }

inline void FD_ZERO( fd_set* fdset_ )
	{ fdset_->clear(); }

inline void FD_SET( int const& fd_, fd_set* fdset_ )
	{ fdset_->insert( fd_ ); }

}

using namespace asio;

int unix_select( int, fd_set*, fd_set*, fd_set*, struct timeval* );
inline int select( int ndfs_,
		fd_set* readfs_, fd_set* writefs_,
		fd_set* extrafs_, struct timeval* timeout_ )
	{ return ( unix_select( ndfs_, readfs_, writefs_, extrafs_, timeout_ ) ); }


#endif /* not YAAL_MSVCXX_SYS_TIME_H_INCLUDED */

