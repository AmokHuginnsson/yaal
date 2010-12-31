#ifndef YAAL_MSVCXX_UNISTD_H_INCLUDED
#define YAAL_MSVCXX_UNISTD_H_INCLUDED 1

#include <cstdio>
#include <cerrno>
#undef tmpfile
extern "C" FILE* tmpfile( void );

#define _SYS_SOCKET_H 1
#define _SYS_RESOURCE_H 1
#undef EDEADLOCK
#define __socklen_t_defined 1
#undef environ
#define unlink unlink_off
#define pipe pipe_off
#define close close_off
#define read read_off
#define write write_off
#define timeval timeval_off
#define gethostname gethostname_off
#define dup2 dup2_off
#define getuid getuid_off
#include <glibc/unistd.h>
#undef gethostname
#include <sys/types.h>
#include <libintl.h>
#undef getuid
#undef timeval
#undef environ
#undef select
#undef fprintf
#undef close
#undef read
#undef write
#undef pipe
#undef unlink
#undef dup2

#include <sys/time.h>

#define fork ms_fork
#define getuid ms_getuid
#define gethostname ms_gethostname

#include "hcore/macro.hxx"
#include "emu_unistd.hxx"

int ms_gethostname( char*, int );
M_YAAL_HCORE_PUBLIC_API uid_t ms_getuid( void );

#if ! defined( HAVE_PTHREAD_SETNAME_NP )
#define HAVE_PTHREAD_SETNAME_NP 1
void pthread_setname_np( void*, char const* );
#endif /* #if ! defined( HAVE_PTHREAD_SETNAME_NP ) */

#define dup2 msvcxx::dup2

inline int pipe( int* fds_ )
	{ return ( msvcxx::pipe( fds_ ) ); }

inline int long read( int const& fd_, void* buf_, int long size_ )
	{ return ( msvcxx::read( fd_, buf_, size_ ) ); }

inline int long write( int const& fd_, void const* buf_, int long size_ )
	{ return ( msvcxx::write( fd_, buf_, size_ ) ); }

template<typename T>
inline int close( T const& fd_ )
	{ return ( msvcxx::close( fd_ ) ); }

#include "tools/hpipedchild.hxx"

#define ms_fork HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( image_, argv_, fileDesIn, fileDesOut, fileDesErr )
class M_YAAL_HCORE_PUBLIC_API HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn
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

