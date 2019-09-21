#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <process.h>
#include <io.h>

#define execl execl_off
#define execle execle_off
#define execv execv_off
#define execve execve_off
#define execlp execlp_off
#define execvp execvp_off
#define access access_off
#define lseek lseek_off

#define fill fill_off
#include "csignal"
#include "unistd.h"
#undef fill
#undef waitpid
#include "sys/wait.h"

#include "synchronizedunorderedset.hxx"
#include "hcore/memory.hxx"
#include "hcore/hfile.hxx"
#include "msio.hxx"
#include "emu_unistd.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace msvcxx;

typedef SynchronizedUnorderedSet<int> pid_set_t;

pid_set_t _children_;

M_EXPORT_SYMBOL
HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( yaal::hcore::HString const& path_, yaal::tools::HPipedChild::argv_t const& argv_, int* in_, int* out_, int* err_, int* message_, bool joinedErr_ ) {
	return ( HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( path_, argv_, in_, out_, err_, message_, joinedErr_ ) );
}

M_EXPORT_SYMBOL
HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( yaal::hcore::HString const& path_, yaal::tools::HPipedChild::argv_t const& argv_, int* in_, int* out_, int* err_, int* message_, bool joinedErr_ )
	: _path( path_ )
	, _argv( argv_ )
	, _in( in_ )
	, _out( out_ )
	, _err( err_ )
	, _message( message_ )
	, _joinedErr( joinedErr_ ) {
}

char* xstrdup( char const* str_ ) {
	char* str = 0;
	if ( ! str_ ) {
		::perror( "xstrdup: request to duplicate NULL pointer string" );
		::abort();
	}
	str = memory::calloc<char>( static_cast<int long>( ::strlen( str_ ) ) + 1 );
	::strcpy( str, str_ );
	return ( str );
}

M_EXPORT_SYMBOL
int HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::operator()( void ) {
	/* Make a backup of original descriptors. */
	int stdinFd( _fileno( stdin ) );
	int stdoutFd( _fileno( stdout ) );
	int stderrFd( _fileno( stderr ) );
	int hStdIn = _dup( stdinFd );
	std::cout << std::flush;
	int hStdOut = _dup( stdoutFd );
	std::cerr << std::flush;
	int hStdErr = _dup( stderrFd );

	/* ensure backup went ok */
	M_ENSURE( ( hStdIn >= 0 ) && ( hStdOut >= 0 ) && ( hStdErr >= 0 ) );

	/* Overwrite *standard* descriptors with our communication pipe descriptors. */
	M_ENSURE( ::dup2( _in[0], stdinFd ) == 0 );
	M_ENSURE( ::dup2( _out[1], stdoutFd ) == 0 );
	M_ENSURE( ::dup2( ( _joinedErr ? _out : _err )[1], stderrFd ) == 0 );

	char** argv = memory::calloc<char*>( _argv.size() + 2 );
	HUTF8String utf8( _path );
	argv[ 0 ] = xstrdup( utf8.c_str() );
	int i = 1;
	for ( HPipedChild::argv_t::iterator it( _argv.begin() ), end( _argv.end() ); it != end; ++ it, ++ i ) {
		utf8 = *it;
		argv[ i ] = xstrdup( utf8.c_str() );
	}

	utf8 = _path;
	intptr_t processHandle( ::spawnvp( P_NOWAIT, utf8.c_str(), argv ) );
	if ( processHandle == -1 ) {
		log_windows_error( "spawnvp" );
	}
	int pid( ::GetProcessId( reinterpret_cast<HANDLE>( processHandle ) ) );
	if ( pid <= 0 ) {
		log_windows_error( "GetProcessId" );
	}

	/* Restore backed up standard descriptors. */
	M_ENSURE( ::dup2( hStdIn, stdinFd ) == 0 );
	M_ENSURE( ::dup2( hStdOut, stdoutFd ) == 0 );
	M_ENSURE( ::dup2( hStdErr, stderrFd ) == 0 );

	/* Close backups. */
	if ( TEMP_FAILURE_RETRY( ::close( hStdIn ) )
		|| TEMP_FAILURE_RETRY( ::close( hStdOut ) )
		|| TEMP_FAILURE_RETRY( ::close( hStdErr ) ) ) {
		M_THROW( "close", errno );
	}

	for ( int  k = 0, size = _argv.size(); k < size; ++ k ) {
		memory::free( argv[ k ] );
	}
	memory::free( argv );

	M_ENSURE( msvcxx::write( _message[1], "\0", 1 ) == 1 );
	_children_.insert( pid );
	return ( pid );
}

namespace msvcxx {

#undef waitpid
int waitpid( int pid_, int* status_, int options_ ) {
	int ret( -1 );
	do {
		if ( ( options_ & ~( WNOHANG | WUNTRACED ) ) != 0 ) {
			errno = EINVAL;
			break;
		}
		if ( ( pid_ == -1 ) || ( pid_ == -2 ) ) {
			errno = ECHILD;
			break;
		}
		HANDLE proc( OpenProcess( SYNCHRONIZE, FALSE, pid_ ) );
		if ( proc == nullptr ) {
			errno = ECHILD;
			break;
		}
		if ( ::WaitForSingleObject( proc, static_cast<DWORD>( -1L ) ) == WAIT_FAILED ) {
			errno = ECHILD;
			break;
		}
		DWORD status( 0 );
		if ( !GetExitCodeProcess( proc, &status ) ) {
			errno = ECHILD;
			break;
		}
		if ( status_ ) {
			*status_ = status;
		}
		CloseHandle( proc );
		ret = pid_;
	} while ( false );
	if ( ( ret == -1 ) && ( _children_.count( pid_ ) > 0 ) ) {
		if ( ! kill( pid_, 0 ) ) {
			ret = pid_;
		}
	}
	if ( ret == pid_ ) {
		_children_.erase( pid_ );
	}
	return ( ret );
}

}
