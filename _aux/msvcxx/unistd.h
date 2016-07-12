#ifndef YAAL_MSVCXX_UNISTD_H_INCLUDED
#define YAAL_MSVCXX_UNISTD_H_INCLUDED 1

#include <cstdio>
#include <cerrno>
#include <process.h>
#include <libintl.h>

#define _SYS_SOCKET_H 1
#define _STRUCT_TIMEVAL 1
#undef EDEADLOCK
#if ( ETIMEDOUT != WSAETIMEDOUT )
#undef ETIMEDOUT
#define ETIMEDOUT WSAETIMEDOUT
#endif /* #if ( ETIMEDOUT != WSAETIMEDOUT ) */
#if ( EINPROGRESS != WSAEINPROGRESS )
#undef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#endif /* #if ( EINPROGRESS != WSAEINPROGRESS ) */

#include "sys/time.h"
/* cleanup inside "sys/time.h" */

#include "hcore/macro.hxx"
#include "emu_unistd.hxx"

typedef int uid_t;

static int const STDIN_FILENO = 0;
static int const STDOUT_FILENO = 1;
static int const STDERR_FILENO = 2;

static int const WCONTINUED = 0;

static int const _SC_PAGESIZE = 1;
static int const _SC_NPROCESSORS_ONLN = 2;
static int const _SC_GETPW_R_SIZE_MAX = 3;
static int const _SC_GETGR_R_SIZE_MAX = 4;
static int const _PC_NAME_MAX = 3;
static int const _XOPEN_NAME_MAX = 2048;

#define fork ms_fork
#define getuid ms_getuid
#define gethostname ms_gethostname
#define unsetenv unsetenv_fix
#define geteuid getuid
#undef gai_strerror
#define gai_strerror msvcxx::windows_strerror

int ms_gethostname( char*, int );
M_YAAL_HCORE_PUBLIC_API uid_t ms_getuid( void );
int unsetenv_fix( char const* );
int long sysconf( int );
int long pathconf( char const*, int );

#if ! defined( HAVE_PTHREAD_SETNAME_NP )
#define HAVE_PTHREAD_SETNAME_NP 1
void pthread_setname_np( void*, char const* );
#endif /* #if ! defined( HAVE_PTHREAD_SETNAME_NP ) */

#define dup2 msvcxx::dup2
#define isatty msvcxx::isatty

inline int pipe( int* fds_ )
	{ return ( msvcxx::pipe( fds_ ) ); }

inline int long read( int fd_, void* buf_, int long size_ )
	{ return ( msvcxx::read( fd_, buf_, size_ ) ); }

inline int long write( int fd_, void const* buf_, int long size_ )
	{ return ( msvcxx::write( fd_, buf_, size_ ) ); }

inline int long sendto( int fd_, void const* buf_, int long size_, int flags_, sockaddr const* to_, int toLen_ )
	{ return ( msvcxx::sendto( fd_, buf_, size_, flags_, to_, toLen_ ) ); }

inline int long recvfrom( int fd_, void* buf_, int long size_, int flags_, sockaddr* from_, int* fromLen_ )
	{ return ( msvcxx::recvfrom( fd_, buf_, size_, flags_, from_, fromLen_ ) ); }

template<typename T>
inline int close( T const& fd_ )
	{ return ( msvcxx::close( fd_ ) ); }

inline int lockf( int fd_, int cmd_, size_t off_ )
	{ return ( msvcxx::lockf( fd_, cmd_, off_ ) ); }

#include "tools/hpipedchild.hxx"

#define ms_fork HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( image_, argv_, fileDesIn, fileDesOut, fileDesErr )
class M_YAAL_HCORE_PUBLIC_API HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn {
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

