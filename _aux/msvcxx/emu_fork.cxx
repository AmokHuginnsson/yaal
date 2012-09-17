#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <sys/cdefs.h>
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
#define dup dup_off
#define getpid getpid_off
#define isatty isatty_off
#define getpwuid_r getpwuid_r_off

#define fill fill_off
#include <csignal>
#include <unistd.h>
#undef fill
#undef waitpid
#include <sys/wait.h>

#include "synchronizedunorderedset.hxx"
#include "hcore/memory.hxx"
#include "hcore/hfile.hxx"
#include "cleanup.hxx"
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
HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner( yaal::hcore::HString const& path_, yaal::tools::HPipedChild::argv_t const& argv_, int* in_, int* out_, int* err_ ) {
	return ( HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( path_, argv_, in_, out_, err_ ) );
}

M_EXPORT_SYMBOL
HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( yaal::hcore::HString const& path_, yaal::tools::HPipedChild::argv_t const& argv_, int* in_, int* out_, int* err_ )
	: _path( path_ ), _argv( argv_ ), _in( in_ ), _out( out_ ), _err( err_ ) {
}

char* xstrdup( char const* const str_ ) {
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
	external_lock_t stdinLock( hcore::cin.acquire() );
	external_lock_t sdtoutLock( hcore::cout.acquire() );
	external_lock_t stderrLock( hcore::cerr.acquire() );
	int stdinFd( _fileno( stdin ) );
	int stdoutFd( _fileno( stdout ) );
	int stderrFd( _fileno( stderr ) );
	int hStdIn = _dup( stdinFd );
	::fflush( stdout );
	hcore::cout << hcore::flush;
	std::cout << std::flush;
	int hStdOut = _dup( stdoutFd );
	::fflush( stderr );
	hcore::cerr << hcore::flush;
	std::cerr << std::flush;
	int hStdErr = _dup( stderrFd );

	/* ensure backup went ok */
	M_ENSURE( ( hStdIn >= 0 ) && ( hStdOut >= 0 ) && ( hStdErr >= 0 ) );

	/* Overwrite *standard* descrptors with our communication pipe descriptors. */
	M_ENSURE( ::dup2( _in[0], stdinFd ) == 0 );
	M_ENSURE( ::dup2( _out[1], stdoutFd ) == 0 );
	M_ENSURE( ::dup2( _err[1], stderrFd ) == 0 );

	char** argv = memory::calloc<char*>( _argv.size() + 2 );
	argv[ 0 ] = xstrdup( _path.raw() );
	int i = 1;
	for ( HPipedChild::argv_t::iterator it( _argv.begin() ), end( _argv.end() ); it != end; ++ it, ++ i )
		argv[ i ] = xstrdup( it->raw() );
	
	intptr_t processHandle( ::spawnvp( P_NOWAIT, _path.raw(), argv ) );
	if ( processHandle == -1 )
		log_windows_error( "spawnvp" );
	int pid( ::GetProcessId( reinterpret_cast<HANDLE>( processHandle ) ) );
	if ( pid <= 0 )
		log_windows_error( "GetProcessId" );

	/* Restore backed up standard descriptors. */
	M_ENSURE( ::dup2( hStdIn, stdinFd ) == 0 );
	M_ENSURE( ::dup2( hStdOut, stdoutFd ) == 0 );
	M_ENSURE( ::dup2( hStdErr, stderrFd ) == 0 );

	/* Close backups. */
	if ( TEMP_FAILURE_RETRY( ::close( hStdIn ) )
		|| TEMP_FAILURE_RETRY( ::close( hStdOut ) )
		|| TEMP_FAILURE_RETRY( ::close( hStdErr ) ) )
		M_THROW( "close", errno );

	for ( int  k = 0, size = _argv.size(); k < size; ++ k )
		memory::free( argv[ k ] );
	memory::free( argv );

	_children_.insert( pid );
	return ( pid );
}

namespace msvcxx {

#undef waitpid
int waitpid( int pid_, int* status_, int options_ ) {
	int ret( ::waitpid( pid_, status_, options_ ) );
	if ( ret == -1 ) {
		if ( _children_.count( pid_ ) > 0 ) {
			if ( ! kill( pid_, 0 ) )
				ret = pid_;
		}
	}
	if ( ret == pid_ )
		_children_.erase( pid_ );
	return ( ret );
}

}
