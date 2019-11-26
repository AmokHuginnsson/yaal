/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal> /* for FreeBSD */
#include <sys/wait.h>
#include <unistd.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hpipedchild.hxx"
#include "hfsitem.hxx"
#include "hcore/hrawfile.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hclock.hxx"
#include "hcore/duration.hxx"
#include "util.hxx"
#include "halarm.hxx"
#include "hterminal.hxx"
#ifdef __HOST_OS_TYPE_CYGWIN__
/* Work around for buggy child process handling in Cygwin. */
#	include "sleep.hxx"
#endif /* #ifdef __HOST_OS_TYPE_CYGWIN__ */

using namespace yaal::hcore;

namespace yaal {

namespace tools {

#undef OUT
#undef IN
enum class PIPE_END {
	OUT = 0,
	IN = 1
};

struct OPipeResGuard {
	int _res[2];
	OPipeResGuard( void )
		: _res() {
		_res[0] = _res[1] = -1;
	}
	~OPipeResGuard( void ) {
		if ( _res[ 0 ] >= 0 ) {
			M_ENSURE( M_TEMP_FAILURE_RETRY( hcore::system::close( _res[ 0 ] ) ) == 0 );
		}
		if ( _res[ 1 ] >= 0 ) {
			M_ENSURE( M_TEMP_FAILURE_RETRY( hcore::system::close( _res[ 1 ] ) ) == 0 );
		}
	}
	int& operator[]( PIPE_END pipeEnd_ ) {
		return ( _res[static_cast<int>( pipeEnd_ )] );
	}
	bool uninitialized( void ) {
		return ( ( _res[0] == -1 ) && ( _res[1] == -1 ) );
	}
};

inline int stream_to_fd( yaal::hcore::HStreamInterface const* stream_ ) {
	return ( static_cast<int>( reinterpret_cast<int_native_t>( stream_->data() ) ) );
}

static void close_and_invalidate( HStreamInterface::ptr_t& stream_ ) {
	M_PROLOG
	try {
		if ( dynamic_cast<HRawFile*>( stream_.raw() ) ) {
			static_cast<HRawFile*>( stream_.raw() )->close();
		} else if ( dynamic_cast<HFile*>( stream_.raw() ) ) {
			static_cast<HFile*>( stream_.raw() )->close();
		}
	} catch ( ... ) {
	}
	stream_.reset();
	return;
	M_EPILOG
}

static void close_and_invalidate( int& fd_ ) {
	M_PROLOG
	if ( fd_ >= 0 ) {
		M_ENSURE( M_TEMP_FAILURE_RETRY( hcore::system::close( fd_ ) ) == 0 );
	}
	fd_ = -1;
	return;
	M_EPILOG
}

static void fixupFds( int& fd_, HStreamInterface::ptr_t const& owned_, HStreamInterface const*& ref_, char const* name_ ) {
	M_PROLOG
	if ( fd_ < 0 ) {
		return;
	}
	if ( !! owned_ ) {
		if ( ref_ ) {
			throw HPipedChildException( to_string( name_ ).append( " stream is already defined." ) );
		} else {
			ref_ = owned_.raw();
		}
	}
	if ( ref_ ) {
		/*
		 * We have a non-pipe stream so we can free internal pipe.
		 */
		close_and_invalidate( fd_ );
		fd_ = stream_to_fd( ref_ );
		if ( ! owned_ ) {
			/*
			 * We do not own the external stream, so we should make a copy of the descriptor
			 * so we do not break external stream when we clean up after `spawn()`.
			 */
			fd_ = ::dup( fd_ );
		}
	}
	return;
	M_EPILOG
}

static void fixupFds( HStreamInterface const* ref_, OPipeResGuard& pipe_, PIPE_END direction_, HStreamInterface::ptr_t& owned_ ) {
	M_PROLOG
	if ( pipe_.uninitialized() ) {
		return;
	}
	PIPE_END otherEnd( direction_ == PIPE_END::OUT ? PIPE_END::IN : PIPE_END::OUT );
	if ( ! ref_ ) {
		M_ASSERT( ! owned_ );
		close_and_invalidate( pipe_[ otherEnd ] );
		owned_ = make_pointer<HRawFile>( pipe_[ direction_ ], HRawFile::OWNERSHIP::ACQUIRED );
		pipe_[direction_] = -1;
	} else if ( !! owned_ ) {
		pipe_[otherEnd] = -1;
	}
	return;
	M_EPILOG
}

int HPipedChild::_killGracePeriod = static_cast<int>( time::in_units<time::UNIT::MILLISECOND>( time::duration( 1, time::UNIT::SECOND ) ) );

HPipedChild::HPipedChild(
	yaal::hcore::HStreamInterface::ptr_t in_,
	yaal::hcore::HStreamInterface::ptr_t out_,
	yaal::hcore::HStreamInterface::ptr_t err_
) : _pid( -1 )
	, _in( in_ )
	, _out( out_ )
	, _err( err_ )
	, _processGroupId( -1 )
	, _foreground( false )
	, _status() {
	return;
}

HPipedChild::~HPipedChild( void ) {
	M_PROLOG
	if ( _pid > 0 ) {
		finish();
	}
	return;
	M_DESTRUCTOR_EPILOG
}

#ifndef HAVE_SIGHANDLER_T
#ifdef HAVE___SIGHANDLER_T
typedef __sighandler_t* sighandler_t;
#elif defined ( HAVE_SIG_PF )
typedef SIG_PF sighandler_t;
#elif defined ( __HOST_OS_TYPE_DARWIN__ )
typedef void (*sighandler_t)( int );
#else /* #elif defined ( __HOST_OS_TYPE_DARWIN__ ) */
#error No signal handler type definition available.
#endif /* #else #elif defined ( __HOST_OS_TYPE_DARWIN__ ) */
#endif /* #ifndef HAVE_SIGHANDLER_T */

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename T>
inline bool FWD_WIFEXITED( T val_ ) {
	return ( WIFEXITED( val_ ) ? true : false );
}
template<typename T>
inline int FWD_WEXITSTATUS( T val_ ) {
	return ( WEXITSTATUS( val_ ) );
}
template<typename T>
inline bool FWD_WIFSIGNALED( T val_ ) {
	return ( WIFSIGNALED( val_ ) ? true : false );
}
template<typename T>
inline int FWD_WTERMSIG( T val_ ) {
	return ( WTERMSIG( val_ ) );
}
template<typename T>
inline bool FWD_WIFSTOPPED( T val_ ) {
	return ( WIFSTOPPED( val_ ) ? true : false );
}
template<typename T>
inline int FWD_WSTOPSIG( T val_ ) {
	return ( WSTOPSIG( val_ ) );
}
template<typename T>
inline bool FWD_WIFCONTINUED( T val_ ) {
	return ( WIFCONTINUED( val_ ) ? true : false );
}
static sighandler_t const FWD_SIG_ERR = SIG_ERR;
static sighandler_t const FWD_SIG_IGN = SIG_IGN;
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
static sighandler_t const FWD_SIG_DFL = SIG_DFL;
#pragma GCC diagnostic pop
}

HPipedChild::STATUS const& HPipedChild::get_status_change( bool noHang_ ) {
	M_PROLOG
	if ( _pid <= 0 ) {
		M_ASSERT( ( _status.type == STATUS::TYPE::UNSPAWNED ) || ( _status.type == STATUS::TYPE::FINISHED ) || ( _status.type == STATUS::TYPE::ABORTED ) );
		return ( _status );
	}
	M_ASSERT( ( _status.type == STATUS::TYPE::RUNNING ) || ( _status.type == STATUS::TYPE::PAUSED ) );
	int status( 0 );
	int pid( 0 );
	M_ENSURE( ( pid = ::waitpid( _pid, &status, ( noHang_ ? WNOHANG : 0 ) | WUNTRACED | WCONTINUED ) ) != -1 );
	if ( pid == 0 ) {
		return ( _status );
	}
	update_status( status );
	return ( _status );
	M_EPILOG
}

void HPipedChild::update_status( int status_ ) {
	if ( FWD_WIFEXITED( status_ ) ) {
		_status.type = STATUS::TYPE::FINISHED;
		_status.value = FWD_WEXITSTATUS( status_ );
		_pid = -1;
	} else if ( FWD_WIFSIGNALED( status_ ) ) {
		_status.type = STATUS::TYPE::ABORTED;
		_status.value = FWD_WTERMSIG( status_ );
		_pid = -1;
	} else if ( FWD_WIFSTOPPED( status_ ) ) {
		_status.type = STATUS::TYPE::PAUSED;
		_status.value = FWD_WSTOPSIG( status_ );
	} else if ( FWD_WIFCONTINUED( status_ ) ) {
		_status.type = STATUS::TYPE::RUNNING;
	}
	return;
}

HPipedChild::STATUS const& HPipedChild::get_status( void ) {
	M_PROLOG
	return ( get_status_change( true ) );
	M_EPILOG
}

HPipedChild::STATUS const& HPipedChild::wait( void ) {
	M_PROLOG
	close_and_invalidate( _in );
	if ( _pid > 0 ) {
		get_status_change( false );
	}
	if ( _pid <= 0 ) {
		close_and_invalidate( _out );
		close_and_invalidate( _err );
		restore_parent_term();
	}
	return ( _status );
	M_EPILOG
}

void HPipedChild::restore_parent_term( void ) {
	M_PROLOG
	if ( ! _foreground ) {
		return;
	}
	int iofds[] = { fileno( stdin ), fileno( stdout ), fileno( stderr ) };
	for ( int fd : iofds ) {
		if ( ! is_a_tty( fd ) ) {
			continue;
		}
		M_ENSURE( ::tcsetpgrp( fd, getpgrp() ) == 0 );
	}
	_foreground = false;
	return;
	M_EPILOG
}

void HPipedChild::bring_to_foreground( void ) {
	M_PROLOG
	int iofds[] = { fileno( stdin ), fileno( stdout ), fileno( stderr ) };
	for ( int fd : iofds ) {
		if ( ! is_a_tty( fd ) ) {
			continue;
		}
		M_ENSURE( ( ::tcsetpgrp( fd, _processGroupId ) == 0 ) || ( errno == EACCES ) );
	}
	_foreground = true;
	return;
	M_EPILOG
}

void HPipedChild::do_continue( void ) {
	M_PROLOG
	M_ENSURE( hcore::system::kill( _pid, SIGCONT ) == 0 );
	int pid( 0 );
	int status( 0 );
	do {
		M_ENSURE( ( ( pid = ::waitpid( _pid, &status, WUNTRACED | WCONTINUED ) ) != -1 ) || ( errno == EINTR ) );
		if ( ( pid != _pid ) && ( errno == EINTR ) ) {
			continue;
		}
		M_ASSERT( pid == _pid );
	} while ( ! FWD_WIFCONTINUED( status ) );
	update_status( status );
	return;
	M_EPILOG
}

HPipedChild::STATUS const& HPipedChild::finish( i64_t finishIn_ ) {
	M_PROLOG
	M_ASSERT( _pid != 0 );
	get_status();
	if ( _pid <= 0 ) {
		M_ASSERT( ( _status.type == STATUS::TYPE::UNSPAWNED ) || ( _status.type == STATUS::TYPE::FINISHED ) || ( _status.type == STATUS::TYPE::ABORTED ) );
		restore_parent_term();
		return ( _status );
	}
	M_ASSERT( ( _status.type == STATUS::TYPE::RUNNING ) || ( _status.type == STATUS::TYPE::PAUSED ) );
	if ( _status.type == STATUS::TYPE::PAUSED ) {
		do_continue();
	}
	close_and_invalidate( _in );
	close_and_invalidate( _out );
	close_and_invalidate( _err );
	int status( 0 );
	int pid( 0 );
	if ( finishIn_ > 0 ) {
		HClock clock;
		i64_t elapsed( 0 );
		while ( ( pid != _pid ) && ( ( elapsed = clock.get_time_elapsed( time::UNIT::MILLISECOND ) ) < finishIn_ ) ) {
			HAlarm alarm( time::in_units<time::UNIT::MILLISECOND>( time::duration( finishIn_ - elapsed, time::UNIT::MILLISECOND ) ) );
			M_ENSURE( ( ( pid = ::waitpid( _pid, &status, WUNTRACED ) ) != -1 ) || ( errno == EINTR ) );
		}
	} else {
		M_ENSURE( ( pid = ::waitpid( _pid, &status, WNOHANG | WUNTRACED ) ) != -1 );
	}
	if ( pid != _pid ) {
		M_ENSURE( hcore::system::kill( _pid, SIGTERM ) == 0 );
		HAlarm alarm( _killGracePeriod );
		M_ENSURE( ( ( pid = ::waitpid( _pid, &status, 0 ) ) != -1 ) || ( errno == EINTR ) );
	}
	if ( pid != _pid ) {
		M_ENSURE( hcore::system::kill( _pid, SIGKILL ) == 0 );
		M_ENSURE( ::waitpid( _pid, &status, 0 ) == _pid );
	}
	update_status( status );
	_pid = -1;
	restore_parent_term();
	return ( _status );
	M_EPILOG
}

void HPipedChild::spawn(
	HString const& image_,
	argv_t const& argv_,
	yaal::hcore::HStreamInterface const* in_,
	yaal::hcore::HStreamInterface const* out_,
	yaal::hcore::HStreamInterface const* err_,
	int pgid_,
	bool foreground_
) {
	M_PROLOG
	M_ENSURE( !foreground_ || ( pgid_ >= PROCESS_GROUP_LEADER ) );
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	OPipeResGuard pipeIn, pipeOut, pipeErr;
	HFSItem image( image_ );
	M_ENSURE( !! image, image_ );
	if ( ! ( image.is_executable() && image.is_file() ) ) {
		throw HPipedChildException( "Not an executable: "_ys.append( image_ ) );
	}
	bool joinedErr( ( !! _out && ( _out == _err ) ) || ( ! _out && out_ && ( out_ == err_ ) ) );
	M_ENSURE(
		( ! ::pipe( pipeIn._res ) )
		&& ( ! ::pipe( pipeOut._res ) )
		&& ( joinedErr || ( ! ::pipe( pipeErr._res ) ) )
	);
	HChunk argv( chunk_size<char const*>( argv_.size() + 2 ) );
	HLock stdinLock( cin.acquire() );
	HLock stdoutLock( cout.acquire() );
	HLock stderrLock( cerr.acquire() );
	int const stdinFd( fileno( stdin ) );
	int const stdoutFd( fileno( stdout ) );
	int const stderrFd( fileno( stderr ) );
	int iofds[] = { stdinFd, stdoutFd, stderrFd };
	::fflush( stderr );
	cout << hcore::flush;
	::fflush( stdout );
	cerr << hcore::flush;
	/*
	 * `fixupFds()` must be called before `fork()` due to how `fork()` call is emulated on `msvcxx` target.
	 */
	fixupFds( pipeIn[ PIPE_END::OUT ], _in, in_, "Input" );
	fixupFds( pipeOut[ PIPE_END::IN ], _out, out_, "Output" );
	fixupFds( pipeErr[ PIPE_END::IN ], _err, err_, "Error" );
	OPipeResGuard message;
	M_ENSURE( ::pipe( message._res ) == 0 );
	_pid = ::fork();
	M_ENSURE( _pid >= 0, "fork()" );
	if ( ! _pid ) {
		bool notified( false );
		try {
			HUTF8String utf8Image( image_ );
			argv.get<char const*>()[ 0 ] = utf8Image.c_str();
			int i( 1 );
			typedef HArray<HUTF8String> utf8_strings_t;
			utf8_strings_t utf8Argv;
			utf8Argv.reserve( argv.get_size() );
			for ( argv_t::const_iterator it( argv_.begin() ), end( argv_.end() ); it != end; ++ it, ++ i ) {
				utf8Argv.emplace_back( *it );
				argv.get<char const*>()[ i ] = utf8Argv.back().c_str();
			}
			sigset_t all;
			M_ENSURE( ::sigfillset( &all ) == 0 );
			M_ENSURE( ::sigprocmask( SIG_UNBLOCK, &all, nullptr ) == 0 );
			if ( pgid_ >= PROCESS_GROUP_LEADER ) {
				int pid( getpid() );
				int pgid( pgid_ > PROCESS_GROUP_LEADER ? pgid_ : pid );
				M_ENSURE( ( ::setpgid( pid, pgid ) == 0 ) || ( errno == EACCES ) );
				if ( foreground_ && is_a_tty( stdinFd ) ) {
					M_ENSURE( signal( SIGTTOU, FWD_SIG_IGN ) != FWD_SIG_ERR );
					M_ENSURE( signal( SIGTTIN, FWD_SIG_IGN ) != FWD_SIG_ERR );
				}
				for ( int fd : iofds ) {
					if ( foreground_ && is_a_tty( fd ) ) {
						M_ENSURE( ( ::tcsetpgrp( fd, pgid ) == 0 ) || ( errno == EACCES ) );
					}
				}
			}
			int signals[] = {
#if defined( HAVE_DECL_SIGIOT ) && ( HAVE_DECL_SIGIOT == 1 )
				SIGIOT,
#endif /* HAVE_DECL_SIGIOT */
				SIGURG, SIGINT, SIGHUP, SIGILL, SIGQUIT, SIGFPE, SIGSYS, SIGUSR1, SIGBUS, SIGSEGV,
				SIGABRT, SIGTERM, SIGTSTP, SIGTRAP, SIGCONT, SIGPIPE, SIGALRM, SIGCHLD, SIGTTIN, SIGTTOU
			};
			for ( int s : signals ) {
				M_ENSURE( signal( s, FWD_SIG_DFL ) != FWD_SIG_ERR );
			}
			M_ENSURE( ::dup2( pipeIn[ PIPE_END::OUT ], stdinFd ) >= 0 );
			M_ENSURE( ::dup2( pipeOut[ PIPE_END::IN ], stdoutFd ) >= 0 );
			M_ENSURE( ::dup2( ( joinedErr ? pipeOut : pipeErr )[ PIPE_END::IN ], stderrFd ) >= 0 );
			M_ENSURE( ( notified = ( ::write( message[PIPE_END::IN], "\0", 1 ) == 1 ) ) );
			OPipeResGuard* pgs[] = { &pipeIn, &pipeOut, joinedErr ? &pipeOut : &pipeErr, &message };
			for ( OPipeResGuard* pg : pgs ) {
				close_and_invalidate( (*pg)[PIPE_END::IN] );
				close_and_invalidate( (*pg)[PIPE_END::OUT] );
			}
			::execv( argv.get<char const*>()[ 0 ], const_cast<char* const*>( argv.get<char const*>() ) );
			::fprintf( stderr, "%s\n", error_message( errno ) );
		} catch ( HException const& e ) {
			e.print_error();
		} catch ( ... ) {
		}
		if ( ! notified ) {
			M_ENSURE( ::write( message[PIPE_END::IN], "\0", 1 ) == 1 );
		}
		::close( stdinFd );
		::close( stdoutFd );
		::close( stderrFd );
		_exit( 1 );
	} else {
		_status.type = STATUS::TYPE::RUNNING;
		fixupFds( in_, pipeIn, PIPE_END::IN, _in );
		fixupFds( out_, pipeOut, PIPE_END::OUT, _out );
		fixupFds( err_, pipeErr, PIPE_END::OUT, _err );
		char dummy( 0 );
		M_ENSURE( ::read( message[PIPE_END::OUT], &dummy, 1 ) == 1 );
		if ( pgid_ >= PROCESS_GROUP_LEADER ) {
			_processGroupId = pgid_ > PROCESS_GROUP_LEADER ? pgid_ : _pid;
			M_ENSURE( ( ::setpgid( _pid, _processGroupId ) == 0 ) || ( errno == EACCES ) );
			if ( foreground_ ) {
				bring_to_foreground();
			}
		}
	}
	return;
	M_EPILOG
}

bool HPipedChild::is_running( void ) const {
	int res( 0 );
	if ( _pid > 0 ) {
		res = hcore::system::kill( _pid, 0 );
	}
	return ( ( _pid > 0 ) && !res  );
}

int HPipedChild::get_pid( void ) const {
	M_PROLOG
	return ( _pid );
	M_EPILOG
}

yaal::hcore::HStreamInterface& HPipedChild::in( void ) {
	M_PROLOG
	return ( *_in );
	M_EPILOG
}

yaal::hcore::HStreamInterface& HPipedChild::out( void ) {
	M_PROLOG
	return ( *_out );
	M_EPILOG
}

yaal::hcore::HStreamInterface& HPipedChild::err( void ) {
	M_PROLOG
	return ( *_err );
	M_EPILOG
}

yaal::hcore::HStreamInterface::ptr_t HPipedChild::stream_in( void ) {
	M_PROLOG
	return ( _in );
	M_EPILOG
}

yaal::hcore::HStreamInterface::ptr_t HPipedChild::stream_out( void ) {
	M_PROLOG
	return ( _out );
	M_EPILOG
}

yaal::hcore::HStreamInterface::ptr_t HPipedChild::stream_err( void ) {
	M_PROLOG
	return ( _err );
	M_EPILOG
}

void HPipedChild::close_in( void ) {
	M_PROLOG
	close_and_invalidate( _in );
	return;
	M_EPILOG
}

void HPipedChild::close_out( void ) {
	M_PROLOG
	close_and_invalidate( _out );
	return;
	M_EPILOG
}

void HPipedChild::close_err( void ) {
	M_PROLOG
	close_and_invalidate( _err );
	return;
	M_EPILOG
}

HPipedChild::process_group_t::iterator HPipedChild::wait_for_process_group( process_group_t& processGroup_ ) {
	M_PROLOG
	if ( processGroup_.is_empty() ) {
		return ( processGroup_.end() );
	}
	int leader( processGroup_.front()->_processGroupId );
#ifdef __DEBUG__
	for ( process_group_t::iterator it( processGroup_.begin() ), end( processGroup_.end() ); it != end; ++ it ) {
		M_ASSERT( (*it)->_processGroupId == leader );
	}
#endif
	int status( 0 );
	int pid( 0 );
	M_ENSURE( ( pid = ::waitpid( -leader, &status, WUNTRACED | WCONTINUED ) ) != -1 );
	for ( process_group_t::iterator it( processGroup_.begin() ), end( processGroup_.end() ); it != end; ++ it ) {
		if ( (*it)->_pid == pid ) {
			(*it)->update_status( status );
			return ( it );
		}
	}
	return ( processGroup_.end() );
	M_EPILOG
}

}

}

