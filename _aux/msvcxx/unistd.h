#ifndef YAAL_MSVCXX_UNISTD_H_INCLUDED
#define YAAL_MSVCXX_UNISTD_H_INCLUDED 1

#include <cstdio>
#include <cerrno>
#include <process.h>
#define write write_off
#define read read_off
#include <io.h>
#undef read
#undef write
#include <direct.h>
#include <libintl.h>

#if _MSC_VER >= 1910
#include <emmintrin.h>
#endif

#define _SYS_SOCKET_H 1
#define _STRUCT_TIMEVAL 1
#undef EDEADLOCK
#if ( EINPROGRESS != WSAEINPROGRESS )
#undef EINPROGRESS
#define EINPROGRESS WSAEINPROGRESS
#endif /* #if ( EINPROGRESS != WSAEINPROGRESS ) */

#include "sys/time.h"
/* cleanup inside "sys/time.h" */

#include "hcore/macro.hxx"
#include "emu_unistd.hxx"

char* basename( char* );

#include "tools/hscheduledasynccaller.hxx"

typedef int uid_t;
typedef int gid_t;
typedef int pid_t;
#ifndef MODE_T_DEFINED
typedef int unsigned mode_t;
#define MODE_T_DEFINED 1
#endif

static int const STDIN_FILENO = 0;
static int const STDOUT_FILENO = 1;
static int const STDERR_FILENO = 2;

static int const F_ULOCK = 0x0;
static int const F_LOCK  = 0x1;

static int const WCONTINUED = 0;

static int const _SC_PAGESIZE = 1;
static int const _SC_NPROCESSORS_ONLN = 2;
static int const _SC_GETPW_R_SIZE_MAX = 3;
static int const _SC_GETGR_R_SIZE_MAX = 4;
static int const _PC_NAME_MAX = 3;
static int const _XOPEN_NAME_MAX = 2048;

#define fork ms_fork
#define geteuid getuid
#undef gai_strerror
#define gai_strerror msvcxx::windows_strerror

uid_t getuid( void );
gid_t getgid( void );
int setpgid( pid_t, pid_t );
int getpgrp( void );
int tcsetpgrp( int, pid_t );
int readlink( char const*, char*, size_t );
int setenv( char const*, char const*, int );
int unsetenv( char const* );
int long sysconf( int );
int long pathconf( char const*, int );
namespace msvcxx {
void sendalarm( void );
}
inline int unsigned alarm( int unsigned seconds_ ) {
	yaal::tools::HScheduledAsyncCaller::get_instance().call_in( yaal::hcore::time::duration( seconds_, yaal::hcore::time::UNIT::SECOND ), yaal::hcore::call( &msvcxx::sendalarm ) );
	return ( 0 );
}

#if ! defined( HAVE_PTHREAD_SETNAME_NP )
#define HAVE_PTHREAD_SETNAME_NP 1
void pthread_setname_np( pthread_t, char const* );
#endif /* #if ! defined( HAVE_PTHREAD_SETNAME_NP ) */

#define dup msvcxx::dup
#define dup2 msvcxx::dup2
#define isatty msvcxx::isatty

inline char* getcwd( char* buf_, size_t size_ ) {
	return( _getcwd( buf_, size_ ) );
}

inline int mkdir( char const* path_, mode_t ) {
	return ( _mkdir( path_ ) );
}

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

inline int close( int fd_ )
	{ return ( msvcxx::close( fd_ ) ); }

inline int lockf( int fd_, int cmd_, size_t off_ )
	{ return ( msvcxx::lockf( fd_, cmd_, off_ ) ); }

DWORD win_read_console_key( void );

#include "tools/hpipedchild.hxx"

#define ms_fork HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( image_, argv_, pipeIn._res, pipeOut._res, pipeErr._res, message._res, joinedErr )
class M_YAAL_HCORE_PUBLIC_API HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn {
	int* _in;
	int* _out;
	int* _err;
	int* _message;
	yaal::hcore::HString _path;
	yaal::tools::HPipedChild::argv_t _argv;
	bool _joinedErr;
public:
	static HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn create_spawner( yaal::hcore::HString const&, yaal::tools::HPipedChild::argv_t const&, int*, int*, int*, int*, bool );
	HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( yaal::hcore::HString const&, yaal::tools::HPipedChild::argv_t const&, int*, int*, int*, int*, bool );
	int operator()( void );
};

#endif /* not YAAL_MSVCXX_UNISTD_H_INCLUDED */

