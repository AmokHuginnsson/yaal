#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <process.h>
#include <io.h>

#define fill fill_off
#include "csignal"
#include "unistd.h"
#undef fill
#undef waitpid
#include "sys/wait.h"

#include "hcore/memory.hxx"
#include "hcore/hfile.hxx"
#include "msio.hxx"
#include "emu_unistd.hxx"
#include "crit.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace msvcxx;

namespace msvcxx {

class ProcessTable {
public:
	static int const PROCESS_GROUP_LEADER = 0;
	typedef std::vector<int> process_group_t;
	typedef std::unordered_map<int, int> processes_t;
	typedef std::unordered_map<int, process_group_t> process_groups_t;
private:
	processes_t _processes;
	process_groups_t _processGroups;
	mutable CMutex _mutex;
public:
	void insert( int pid_, int pgid_ ) {
		CLock l( _mutex );
		_processes.insert( std::make_pair( pid_, pgid_ ) );
		_processGroups[pgid_].push_back( pid_ );
	}
	void erase( int pid_ ) {
		CLock l( _mutex );
		processes_t::iterator pit( _processes.find( pid_ ) );
		if ( pit == _processes.end() ) {
			return;
		}
		int pgid( pit->second );
		_processes.erase( pit );
		process_groups_t::iterator git( _processGroups.find( pgid ) );
		if ( git == _processGroups.end() ) {
			return;
		}
		process_group_t& pg( git->second );
		pg.erase( std::remove( pg.begin(), pg.end(), pid_ ), pg.end() );
		if ( pg.empty() ) {
			_processGroups.erase( git );
		}
		return;
	}
	process_group_t process_group( int pgid_ ) const {
		CLock l( _mutex );
		process_groups_t::const_iterator git( _processGroups.find( pgid_ ) );
		if ( git == _processGroups.end() ) {
			return ( process_group_t() );
		}
		return ( git->second );
	}
	bool is_known_pid( int pid_ ) const {
		CLock l( _mutex );
		bool isKnownPid( _processes.find( pid_ ) != _processes.end() );
		return ( isKnownPid );
	}
} _processTable_;

namespace {

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

HString quoted( HString const& s_ ) {
	if ( s_.find( ' '_ycp ) == HString::npos ) {
		return ( s_ );
	}
	HString s;
	s.assign( s_ ).replace( "\"", "\\\"" ).shift_right( 1, '"'_ycp ).push_back( '"'_ycp );
	return ( s );
}

inline bool is_in_range( int val_, int start_, int end_ ) {
	return ( ( val_ >= start_ ) && ( val_ <= end_ ) );
}

}

int spawn_and_exit( char const* image_, char const* const* argv_ ) {
	int argc( 0 );
	for ( char const* const* a( argv_ ); *a; ++ a ) {
		++ argc;
	}
	char** argv = memory::calloc<char*>( argc + 1 );
	HUTF8String utf8;
	for ( int i( 0 ); i < argc; ++ i ) {
		utf8.assign( quoted( argv_[i] ) );
		argv[ i ] = xstrdup( utf8.c_str() );
	}
	intptr_t processHandle( ::spawnvp( P_WAIT, image_, argv ) );
	if ( processHandle != -1 ) {
		TerminateProcess( OpenProcess( SYNCHRONIZE | PROCESS_TERMINATE, TRUE, GetCurrentProcessId() ), 0 );
	} else {
		for ( int i( 0 ); i < argc; ++ i ) {
			memory::free( argv[ i ] );
		}
		memory::free( argv );
	}
	return ( -1 );
}

M_EXPORT_SYMBOL
HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::create_spawner(
	yaal::hcore::HString const& path_, yaal::tools::HPipedChild::argv_t const& argv_, int pgid_, int* in_, int* out_, int* err_, int* message_, bool joinedErr_
) {
	return ( HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn( path_, argv_, pgid_, in_, out_, err_, message_, joinedErr_ ) );
}

M_EXPORT_SYMBOL
HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn::HYaalWorkAroundForNoForkOnWindowsForHPipedChildSpawn(
	yaal::hcore::HString const& path_, yaal::tools::HPipedChild::argv_t const& argv_, int pgid_, int* in_, int* out_, int* err_, int* message_, bool joinedErr_
) : _path( path_ )
	, _argv( argv_ )
	, _pgid( pgid_ )
	, _in( in_ )
	, _out( out_ )
	, _err( err_ )
	, _message( message_ )
	, _joinedErr( joinedErr_ ) {
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
	HUTF8String utf8( quoted( _path ) );
	argv[ 0 ] = xstrdup( utf8.c_str() );
	int i = 1;
	for ( HPipedChild::argv_t::iterator it( _argv.begin() ), end( _argv.end() ); it != end; ++ it, ++ i ) {
		utf8.assign( quoted( *it ) );
		argv[ i ] = xstrdup( utf8.c_str() );
	}

	utf8.assign( _path );
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
	if (
		TEMP_FAILURE_RETRY( ::close( hStdIn ) )
		|| TEMP_FAILURE_RETRY( ::close( hStdOut ) )
		|| TEMP_FAILURE_RETRY( ::close( hStdErr ) )
	) {
		M_THROW( "close", errno );
	}

	for ( int  k = 0, size = _argv.size(); k < size; ++ k ) {
		memory::free( argv[ k ] );
	}
	memory::free( argv );

	if ( pid >= 0 ) {
		int pgid( _pgid > ProcessTable::PROCESS_GROUP_LEADER ? _pgid : pid );
		_processTable_.insert( pid, pgid );
	}
	M_ENSURE( msvcxx::write( _message[1], "\0", 1 ) == 1 );
	return ( pid );
}

#undef waitpid
int waitpid( int pid_, int* status_, int options_ ) {
	if ( ( options_ & ~( WNOHANG | WUNTRACED | WCONTINUED ) ) != 0 ) {
		errno = EINVAL;
		return ( -1 );
	}
	if ( ( pid_ == -1 ) || ( pid_ == -2 ) ) {
		errno = ECHILD;
		return ( -1 );
	}
	if ( ( pid_ > 0 ) && ! _processTable_.is_known_pid( pid_ ) ) {
		errno = ECHILD;
		return ( -1 );
	}
	ProcessTable::process_group_t pg(
		pid_ < 0
			? _processTable_.process_group( -pid_ )
			: ProcessTable::process_group_t( { pid_ } )
	);
	typedef std::vector<Handle> processes_t;
	typedef std::vector<HANDLE> handles_t;
	processes_t processes;
	for ( int pid : pg ) {
		Handle proc( OpenProcess( SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, pid ) );
		if ( proc.get() == INVALID_HANDLE_VALUE ) {
			continue;
		}
		if ( options_ & WNOHANG ) {
			if ( status_ ) {
				*status_ = 0;
			}
			return ( 0 );
		}
		processes.push_back( std::move( proc ) );
	}
	if ( processes.empty() ) {
		errno = ECHILD;
		return ( -1 );
	}
	handles_t handles;
	for ( Handle const& h : processes ) {
		handles.push_back( h.get() );
	}
	DWORD waitRes( ::WaitForMultipleObjects( handles.size(), handles.data(), FALSE, INFINITE ) );
	if (
		( waitRes == WAIT_FAILED )
		|| ( waitRes == WAIT_TIMEOUT )
		|| ! (
			is_in_range( waitRes, WAIT_OBJECT_0, WAIT_OBJECT_0 + handles.size() )
			|| is_in_range( waitRes, WAIT_ABANDONED_0, WAIT_ABANDONED_0 + handles.size() )
		)
	) {
		errno = ECHILD;
		return ( -1 );
	}
	int idx( waitRes - ( is_in_range( waitRes, WAIT_OBJECT_0, WAIT_OBJECT_0 + handles.size() ) ? WAIT_OBJECT_0 : WAIT_ABANDONED_0 ) );
	DWORD status( 0 );
	if ( ! GetExitCodeProcess( handles[idx], &status ) ) {
		errno = ECHILD;
		return ( -1 );
	}
	if ( status_ ) {
		*status_ = status << 8;
	}
	_processTable_.erase( pg[idx] );
	return ( pg[idx] );
}

}
