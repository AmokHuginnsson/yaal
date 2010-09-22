#ifndef YAAL_MSVCXX_UNISTD_H_INCLUDED
#define YAAL_MSVCXX_UNISTD_H_INCLUDED 1

#define close close_off
#define read read_off
#define write write_off
#include <cstdio>
#include <cerrno>
#define unlink unlink_gnu
#undef tmpfile
extern "C" FILE* tmpfile( void );

#undef EDEADLOCK
#define select select_off
#define __socklen_t_defined 1
#undef environ
#define fd_set fd_set_off
#define pipe pipe_off
#include <glibc/unistd.h>
#include <sys/types.h>
#include <libintl.h>
#undef fd_set
#undef FD_SET
#include <sys/time.h>
#undef environ
#undef select
#undef fprintf
#undef close
#undef read
#undef write
#undef pipe

#undef gethostname
#define gethostname ms_gethostname
int ms_gethostname( char*, int );

#define fork ms_fork

#undef unlink

int unix_close( int const& );
__declspec( dllimport ) int long unix_read( int const&, void*, int long );
__declspec( dllimport ) int long unix_write( int const&, void const*, int long );
__declspec( dllimport ) int unix_pipe( int* );

inline int pipe( int* fds_ )
	{ return ( unix_pipe( fds_ ) ); }

inline int long read( int const& fd_, void* buf_, int long size_ )
	{ return ( unix_read( fd_, buf_, size_ ) ); }

inline int long write( int const& fd_, void const* buf_, int long size_ )
	{ return ( unix_write( fd_, buf_, size_ ) ); }

template<typename T>
inline int close( T const& fd_ )
	{ return ( unix_close( fd_ ) ); }

#include "tools/hpipedchild.hxx"

#define ms_fork HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( image_, argv_, fileDesIn, fileDesOut, fileDesErr )
class HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn
	{
	int* _in;
	int* _out;
	int* _err;
	yaal::hcore::HString _path;
	yaal::tools::HPipedChild::argv_t _argv;
public:
	static HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn create_spawner( yaal::hcore::HString const&, yaal::tools::HPipedChild::argv_t const&, int*, int*, int* );
	HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( yaal::hcore::HString const&, yaal::tools::HPipedChild::argv_t const&, int*, int*, int* );
	int operator()( void );
	};

#endif /* not YAAL_MSVCXX_UNISTD_H_INCLUDED */

