#include <sys/cdefs.h>
#define fd_set fd_set_win_off
#include <process.h>
#include <io.h>

#define pthread_sigmask pthread_sigmask_off
#define execl execl_off
#define execle execle_off
#define execv execv_off
#define execve execve_off
#define execlp execlp_off
#define execvp execvp_off
#define gethostname gethostname_off
#define timeval timeval_off
#define access access_off
#define lseek lseek_off
#define dup dup_off
#define dup2 dup2_off
#define getpid getpid_off
#define isatty isatty_off
#define getpwuid_r getpwuid_r_off

#undef fd_set
#undef FD_SET
#undef FD_CLR
#undef FD_ISSET
#undef FD_ZERO
#undef FD_SETSIZE
#undef timerclear
#undef timercmp
#undef fd_set
#undef FD_SET

#include <unistd.h>

#include "hcore/xalloc.hxx"
#include "cleanup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

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
