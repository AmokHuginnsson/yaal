#undef _INC_SWPRINTF_INL_
#define NtCurrentTeb NtCurrentTeb_off

#include <list>

#define access gnu_access
#define close gnu_close
#define lseek gnu_lseek
#define read gnu_read
#define write gnu_write
#define dup gnu_dup
#define dup2 gnu_dup2
#undef swab
#define swab gnu_swab
#define _environ gnu__environ
#define fd_set gnu_fdset
#define select gnu_select
#define isatty gnu_isatty
#define timespec gnu_timespec
#define timeval gnu_timeval
#define execl gnu_execl
#define execle gnu_execle
#define execv gnu_execv
#define execve gnu_execve
#define execlp gnu_execlp
#define execvp gnu_execvp
#define getpid gnu_getpid
#define gethostname gethostname_off

#include <unistd.h>
#include <glibc/sys/time.h>

#define getpwuid_r getpwuid_r_off
#include <pwd.h>
#undef getpwuid_r

#undef gethostname
#undef socket
#undef bind
#undef listen
#undef accept
#undef connect
#undef shutdown
#undef setsockopt
#undef htons
#undef ntohs
#undef inet_ntop
#undef FD_SET
#undef FD_CLR
#undef FD_SETSIZE
#undef FD_ISSET
#undef FD_ZERO
#undef access
#undef close
#undef lseek
#undef read
#undef write
#undef dup
#undef dup2
#undef swab
#undef _environ
#undef fd_set
#undef select
#undef isatty
#undef __volatile
#undef timespec
#undef timeval
#undef execl
#undef execle
#undef execv
#undef execve
#undef execlp
#undef execvp
#undef getpid
#undef timerclear
#undef timercmp
#undef MOUSE_MOVED
#undef _WINSOCKAPI_

#include <WinSock2.h>
#include <Windows.h>
#include <dbghelp.h>
#include <process.h>
#include <io.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>

#define sigaddset sigaddset_off
#define sigdelset sigdelset_off
#define sigwait sigwait_off
#define sigemptyset sigemptyset_off
#define kill kill_off
#define pthread_sigmask pthread_sigmask_off
#include <glibc/signal.h>
#undef pthread_sigmask
#undef sigaddset
#undef sigdelset
#undef sigwait
#undef sigemptyset
#undef kill

#undef gethostname

#include "hcore/base.hxx"
#include "hcore/xalloc.hxx"
#include "cleanup.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hlog.hxx"
#include "tools/hpipedchild.hxx"

#include "cxxabi.h"

#include "cleanup.hxx"

#undef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY( x ) ( x )

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace abi
{

char* __cxa_demangle( char const* const a, int, int, int* )
	{
	char* buf = xcalloc<char>( 1024 );
	UnDecorateSymbolName( a, buf, 1023, 0 );
	return ( buf );
	}

}

template<typename T>
class SynchronizedQueue
	{
	list<T> _data;
	yaal::hcore::HMutex _mutex;
	yaal::hcore::HSemaphore _semaphore;
public:
	SynchronizedQueue( void )
		: _data(), _mutex(), _semaphore()
		{ /* HSemaphore leak is intentional. */ }
	bool pop( T& );
	void push( T const& );
	};

template<typename T>
bool SynchronizedQueue<T>::pop( T& elem )
	{
	_semaphore.wait();
	HLock l( _mutex );
	bool found = false;
	if ( ! _data.empty() )
		{
		found = true;
		elem = _data.front();
		_data.pop_front();
		}
	return ( ! found );
	}

template<typename T>
void SynchronizedQueue<T>::push( T const& elem )
	{
	HLock l( _mutex );
	_data.push_back( elem );
	_semaphore.signal();
	return;
	}

SynchronizedQueue<int> _signalQueue_;

int kill( int pid, int signo )
	{
	if ( pid == getpid() )
		_signalQueue_.push( signo );
	else
		TerminateProcess( reinterpret_cast<HANDLE>( pid ), signo );
	return ( 0 );
	}

int sigwait( sigset_t*, int* signo )
	{
	if ( _signalQueue_.pop( *signo ) )
		*signo = SIGURG;
	return ( 0 );
	}

int sigaddset( sigset_t*, int )
	{
	return ( 0 );
	}

int sigemptyset( sigset_t* )
	{
	return ( 0 );
	}

void win_signal_handler( int signo )
	{
	_signalQueue_.push( signo );
	}

int sigaction( int signo, struct sigaction*, void* )
	{
	if ( ( signo != SIGURG ) && ( signo != SIGBUS ) && ( signo != SIGTRAP ) && ( signo != SIGSYS ) ) //&& ( signo != 5 ) && ( signo != 12 ) )
		signal( signo, win_signal_handler );
	return ( 0 );
	}

extern "C"
int pthread_sigmask( int, sigset_t*, void* )
	{
	return ( 0 );
	}

extern "C"
int getpwuid_r( uid_t, struct passwd* p, char* buf, int size, struct passwd** )
	{
	p->pw_name = buf;
	DWORD s = size;
	int err( ! GetUserName( buf, &s ) );
	return ( err );
	}

int ms_gethostname( char* buf_, int len_ )
	{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );

	return ( gethostname( buf_, len_ ) );
	}

extern "C"
int gethostbyname_r( char const* a0, struct hostent* a1, char* a2, size_t a3, struct hostent** a4, int* a5 )
	{
	return ( 0 );
	}

extern "C"
int gethostbyaddr_r( void const* a0, int a1, int a2, struct hostent* a3, char* a4, int long unsigned a5, struct hostent** a6, int* a7 )
	{
	return ( 0 );
	}

int ESCDELAY = 0;

extern "C"
int poll( struct pollfd*, int, int )
	{
	return ( 0 );
	}

void log_windows_error( char const* pszAPI )
	{
	LPVOID lpvMessageBuffer;
	CHAR szPrintBuffer[512];

	DWORD err = GetLastError(); 
	
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpvMessageBuffer, 0, NULL );

	wsprintf(szPrintBuffer,
		"ERROR: API    = %s.\n   error code = %d.\n   message    = %s.\n",
		pszAPI, err, static_cast<char*>( lpvMessageBuffer ) );

	log << szPrintBuffer << endl;
	LocalFree(lpvMessageBuffer);
	return;
	}

int close( short const& fd )
	{
	return ( fd < 1000 ? ::_close( static_cast<int>( fd ) ) : closesocket( fd ) );
	}

int WSAAPI unix_select( int ndfs, fd_set* readFds, fd_set* writeFds, fd_set* exceptFds, struct timeval* timeout )
	{
	int count = ( readFds ? readFds->fd_count : 0 ) + ( writeFds ? writeFds->fd_count : 0 ) + ( exceptFds ? exceptFds->fd_count : 0 );
	fd_set set;
	int s = -1;
	if ( ! count )
		{
		s = ::socket( PF_INET, SOCK_STREAM, 0 );
		FD_ZERO( &set );
		FD_SET( s, &set );
		++ count;
		readFds = &set;
		}
	int ret = select( ndfs ? ndfs : FD_SETSIZE, readFds, writeFds, exceptFds, timeout );
	int err = WSAGetLastError();
	char const* p = strerror( err );
	if ( s >= 0 )
		closesocket( s );
	return ( ret );
	}

namespace msvcxx
{

int unix_bind( int s, const struct sockaddr* name, socklen_t namelen )
	{
	int ret = 0;
	if ( name->sa_family == PF_UNIX )
		{
		closesocket( s );
		s = -1;
		HANDLE h = CreateFile( name->sa_data,                // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_NEW,          // overwrite existing
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template
		if ( h != INVALID_HANDLE_VALUE )
			{
			CloseHandle( h );
			HString n( "\\\\.\\pipe" );
			n += name->sa_data;
			n.replace( "/", "\\" );
			h = CreateNamedPipe( n.raw(),
				PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT ,
				1, 1024, 1024, 1000, NULL );
			}
		LPVOID lpMsgBuf = NULL;
		if ( h == INVALID_HANDLE_VALUE )
			{
			log_windows_error( "CreateNamedPipe" );
			ret = -1;
			}
		else
			s = _open_osfhandle( reinterpret_cast<long>( h ), 0 );
		if ( lpMsgBuf )
			LocalFree(lpMsgBuf);
		}
	else
		ret = bind( s, name, namelen );
	return ( ret );
	}

int unix_socket( int af, int type, int protocol )
	{
	SOCKET s = -1;
	if ( af == PF_UNIX )
		s = ::socket( PF_INET, SOCK_STREAM, 0 );
	else
		s = socket( af, type, protocol );
	return ( s );
	}

int unix_listen( int const& s, int const& backlog )
	{
	int ret = 0;
	if ( s < 1000 )
		{
		//if ( !ConnectNamedPipe( reinterpret_cast<HANDLE>( _get_osfhandle( s ) ), NULL ) )
		//	ret = -1;
		}
	else
		ret = listen( static_cast<SOCKET>( s ), backlog );
	return ( ret );
	}


int unix_accept( int fd_, struct sockaddr* addr_, socklen_t* len_ )
	{
	return ( 0 );
	}

int unix_connect( int fd_, struct sockaddr* addr_, socklen_t len_ )
	{
	return ( 0 );
	}

int unix_shutdown( int fd_, int how_ )
	{
	return ( 0 );
	}

}

extern "C"
int unix_setsockopt( int fd_, int level_, int optname_, const void* optval_, socklen_t optlen_)
	{
	return ( 0 );
	}

typedef short unsigned uint16_t;

extern "C"
uint16_t unix_htons( uint16_t hostshort_ )
	{
	return ( htons( hostshort_ ) );
	}

extern "C"
uint16_t unix_ntohs( uint16_t hostshort_ )
	{
	return ( ntohs( hostshort_ ) );
	}

extern "C"
const char* unix_inet_ntop( int af_, const void* cp_, char* buf_, socklen_t len_ )
	{
	return ( 0 );
	}

HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( yaal::hcore::HString const& path_, yaal::tools::HPipedChild::argv_t const& argv_, int* in_, int* out_, int* err_ )
	{
	return ( HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( path_, argv_, in_, out_, err_ ) );
	}

HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( yaal::hcore::HString const& path_, yaal::tools::HPipedChild::argv_t const& argv_, int* in_, int* out_, int* err_ )
	: _path( path_ ), _argv( argv_ ), _in( in_ ), _out( out_ ), _err( err_ )
	{
	}

int HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::operator()( void )
	{
	/* Make a backup of original descriptors. */
	int hStdIn = _dup( _fileno( stdin ) );
	int hStdOut = _dup( _fileno( stdout ) );
	int hStdErr = _dup( _fileno( stderr ) );

	/* ensure backup went ok */
	M_ENSURE( ( hStdIn >= 0 ) && ( hStdOut >= 0 ) && ( hStdErr >= 0 ) );

	/* Overwrite *standard* descrptors with our communication pipe descriptors. */
	M_ENSURE( _dup2( _in[0], _fileno( stdin ) ) == 0 );
	M_ENSURE( _dup2( _out[1], _fileno( stdout ) ) == 0 );
	M_ENSURE( _dup2( _err[1], _fileno( stderr ) ) == 0 );

	char** argv = xcalloc<char*>( _argv.size() + 2 );
	argv[ 0 ] = xstrdup( _path.raw() );
	int i = 1;
	for ( HPipedChild::argv_t::iterator it = _argv.begin(); it != _argv.end(); ++ it, ++ i )
		argv[ i ] = xstrdup( it->raw() );
	
	int pid = spawnvp( P_NOWAIT, _path.raw(), argv );

	/* Restore backed up standard descriptors. */
	M_ENSURE( _dup2( hStdIn, _fileno( stdin ) ) == 0 );
	M_ENSURE( _dup2( hStdOut, _fileno( stdout ) ) == 0 );
	M_ENSURE( _dup2( hStdErr, _fileno( stderr ) ) == 0 );

	/* Close backups. */
	if ( TEMP_FAILURE_RETRY( ::close( hStdIn ) )
		|| TEMP_FAILURE_RETRY( ::close( hStdOut ) )
		|| TEMP_FAILURE_RETRY( ::close( hStdErr ) ) )
		M_THROW( "close", errno );

	for ( int  k = 0, size = _argv.size(); k < size; ++ k )
		xfree( argv[ k ] );
	xfree( argv );
	return ( pid );
	}
