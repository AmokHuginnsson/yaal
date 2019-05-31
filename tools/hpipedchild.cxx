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
	PIPE_END otherEnd( direction_ == PIPE_END::OUT ? PIPE_END::IN : PIPE_END::OUT );
	if ( ! ref_ ) {
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
	, _foreground( false ) {
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
#pragma GCC diagnostic pop
}

HPipedChild::STATUS HPipedChild::finish( int finishIn_ ) {
	M_PROLOG
	close_and_invalidate( _err );
	close_and_invalidate( _out );
	close_and_invalidate( _in );
	STATUS s;
	if ( _pid > 0 ) {
		int status( 0 );
		int pid( 0 );
		if ( finishIn_ > 0 ) {
			HClock clock;
			int elapsed( 0 );
			while ( ( pid != _pid ) && ( ( elapsed = static_cast<int>( clock.get_time_elapsed( time::UNIT::SECOND ) ) ) < finishIn_ ) ) {
				HAlarm alarm( static_cast<int>( time::in_units<time::UNIT::MILLISECOND>( time::duration( finishIn_ - elapsed, time::UNIT::SECOND ) ) ) );
				M_ENSURE( ( ( pid = ::waitpid( _pid, &status, WUNTRACED | WCONTINUED ) ) != -1 ) || ( errno == EINTR ) );
			}
		} else {
			M_ENSURE( ( pid = ::waitpid( _pid, &status, WNOHANG | WUNTRACED | WCONTINUED ) ) != -1 );
		}
		if ( pid != _pid ) {
			M_ENSURE( hcore::system::kill( _pid, SIGTERM ) == 0 ); {
				HAlarm alarm( _killGracePeriod );
				M_ENSURE( ( ( pid = ::waitpid( _pid, &status, 0 ) ) != -1 ) || ( errno == EINTR ) );
			}
			if ( pid != _pid ) {
				M_ENSURE( hcore::system::kill( _pid, SIGKILL ) == 0 );
				M_ENSURE( ::waitpid( _pid, &status, 0 ) == _pid );
			}
		}
		if ( FWD_WIFEXITED( status ) ) {
			s.type = STATUS::TYPE::NORMAL;
			s.value = FWD_WEXITSTATUS( status );
		} else if ( FWD_WIFSIGNALED( status ) ) {
			s.type = STATUS::TYPE::ABORT;
			s.value = FWD_WTERMSIG( status );
		}
	}
	int const stdinFd( fileno( stdin ) );
	int iofds[] = { stdinFd, fileno( stdout ), fileno( stderr ) };
	if ( _foreground && is_a_tty( stdinFd ) ) {
		M_ENSURE( setpgid( 0, 0 ) == 0 );
		for ( int fd : iofds ) {
			M_ENSURE( ::tcsetpgrp( fd, getpgrp() ) == 0 );
		}
		_foreground = false;
	}
	_pid = 0;
	return ( s );
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
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	OPipeResGuard pipeIn, pipeOut, pipeErr;
	HFSItem image( image_ );
	M_ENSURE( !! image, image_ );
	if ( ! ( image.is_executable() && image.is_file() ) ) {
		throw HPipedChildException( "Not an executable: "_ys.append( image_ ) );
	}
	M_ENSURE( ( ! ::pipe( pipeIn._res ) ) && ( ! ::pipe( pipeOut._res ) ) && ( ! ::pipe( pipeErr._res ) ) );
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
			if ( foreground_ && is_a_tty( stdinFd ) ) {
				int pgid( pgid_ > 0 ? pgid_ : getpid() );
				M_ENSURE( ::setpgid( 0, pgid ) == 0 );
				M_ENSURE( signal( SIGTTOU, SIG_IGN ) != SIG_ERR );
				M_ENSURE( signal( SIGTTIN, SIG_IGN ) != SIG_ERR );
				for ( int fd : iofds ) {
					M_ENSURE( ::tcsetpgrp( fd, pgid ) == 0 );
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
				M_ENSURE( signal( s, SIG_DFL ) != SIG_ERR );
			}
			M_ENSURE( ::dup2( pipeIn[ PIPE_END::OUT ], stdinFd ) >= 0 );
			M_ENSURE( ::dup2( pipeOut[ PIPE_END::IN ], stdoutFd ) >= 0 );
			M_ENSURE( ::dup2( pipeErr[ PIPE_END::IN ], stderrFd ) >= 0 );
			M_ENSURE( ::write( message[PIPE_END::IN], "\0", 1 ) == 1 );
			OPipeResGuard* pgs[] = { &pipeIn, &pipeOut, &pipeErr, &message };
			for ( OPipeResGuard* pg : pgs ) {
				close_and_invalidate( (*pg)[PIPE_END::IN] );
				close_and_invalidate( (*pg)[PIPE_END::OUT] );
			}
			::execv( argv.get<char const*>()[ 0 ], const_cast<char* const*>( argv.get<char const*>() ) );
			M_ENSURE( !"execv"[0] );
		} catch ( ... ) {
			M_ENSURE( ::write( message[PIPE_END::IN], "\0", 1 ) == 1 );
			_exit( 1 );
		}
	} else {
		fixupFds( in_, pipeIn, PIPE_END::IN, _in );
		fixupFds( out_, pipeOut, PIPE_END::OUT, _out );
		fixupFds( err_, pipeErr, PIPE_END::OUT, _err );
		char dummy( 0 );
		M_ENSURE( ::read( message[PIPE_END::OUT], &dummy, 1 ) == 1 );
		if ( foreground_ && is_a_tty( stdinFd ) ) {
			int pgid( pgid_ > 0 ? pgid_ : _pid );
			M_ENSURE( ::setpgid( pgid, pgid ) == 0 );
			for ( int fd : iofds ) {
				M_ENSURE( ::tcsetpgrp( fd, pgid ) == 0 );
			}
		}
		_foreground = foreground_;
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

}

}

