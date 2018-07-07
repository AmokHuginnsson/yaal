/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>   /* strsignal */
#include <csignal>   /* signal handling */
#include <unistd.h>  /* kill function */
#include <cstdlib>   /* exit */
#include <pthread.h> /* pthread_sigmask (in FreeBSD) */

#include "config.hxx"

#include "hcore/base.hxx" /* M_PROLOG, M_EPILOG */
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "signals.hxx"
#include "hcore/hlog.hxx"     /* log object */
#include "hcore/hset.hxx"
#include "hcore/hstring.hxx"  /* HString class */
#include "hcore/system.hxx"   /* get_pid() */
#include "hcore/iterator.hxx"
#include "tools/hthreadpool.hxx"
#include "tools.hxx"          /* tools namespace */

using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace {

/*
 * dummy_signal_handler is called even if signal is masked in sigwait,
 * so it is best to have this handler as simple and non-intrusive as possible.
 */
static void dummy_signal_handler( int )
	{ }

class HBaseSignalHandlers {
	typedef HBaseSignalHandlers this_type;
public:
	static int signal_INT( int );
	static int signal_HUP( int );
	static int signal_TERM( int );
	static int signal_QUIT( int );
	static int signal_TSTP( int );
	static int signal_CONT( int );
	static int signal_fatal( int ) __attribute__(( __noreturn__ ));
	static void signal_fatal_sync( int );
	static int signal_USR1( int );
};

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
static sighandler_t const FWD_SIG_DFL = SIG_DFL;
static sighandler_t const FWD_SIG_ERR = SIG_ERR;
#pragma GCC diagnostic pop
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
int sigaddset_fwd( sigset_t* set, int signo ) {
	return ( sigaddset( set, signo ) );
}
int sigdelset_fwd( sigset_t* set, int signo ) {
	return ( sigdelset( set, signo ) );
}
#pragma GCC diagnostic pop
}

void reset_signal_low( int sigNo_ ) {
	M_PROLOG
	struct sigaction act;
	::memset( &act, 0, sizeof ( act ) );
	act.sa_handler = FWD_SIG_DFL;
	M_ENSURE( sigemptyset( &act.sa_mask ) == 0 );
	M_ENSURE( sigaddset_fwd( &act.sa_mask, sigNo_ ) == 0 );
	sigset_t pending;
	sigemptyset( &pending );
	M_ENSURE( sigpending( &pending ) == 0 );
	if ( sigismember( &pending, sigNo_ ) ) {
		int dummy( 0 );
		M_ENSURE( sigwait( &pending, &dummy ) == 0 );
	}
	M_ENSURE( pthread_sigmask( SIG_UNBLOCK, &act.sa_mask, nullptr ) == 0 );
	M_ENSURE( sigaction( sigNo_, &act, nullptr ) == 0 );
	return;
	M_EPILOG
}

void catch_signal_low( int sigNo_ ) {
	M_PROLOG
	struct sigaction act;
	::memset( &act, 0, sizeof ( act ) );
	act.sa_flags = SA_RESTART;
	act.sa_handler = dummy_signal_handler;
	M_ENSURE( sigemptyset( &act.sa_mask ) == 0 );
	M_ENSURE( sigaddset_fwd( &act.sa_mask, sigNo_ ) == 0 );
	M_ENSURE( sigaction( sigNo_, &act, nullptr ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_BLOCK, &act.sa_mask, nullptr ) == 0 );
	return;
	M_EPILOG
}

}

int HSignalService::_killGracePeriod = 1000;
int HSignalService::_exitStatus = 0;

HSignalService::HSignalService( void )
	: _loop( true )
	, _catch( chunk_size<sigset_t>( 1 ) )
	, _mutex()
	, _handlers() {
	M_PROLOG
	M_ENSURE( sigemptyset( _catch.get<sigset_t>() ) == 0 );
	catch_signal( SIGURG );
	if ( _debugLevel_ < DEBUG_LEVEL::GDB ) {
		register_handler( SIGINT, call( &HBaseSignalHandlers::signal_INT, _1 ) );
	}
	register_handler( SIGHUP, call( &HBaseSignalHandlers::signal_HUP, _1 ) );
	register_handler( SIGTERM, call( &HBaseSignalHandlers::signal_TERM, _1 ) );
	register_handler( SIGQUIT, call( &HBaseSignalHandlers::signal_QUIT, _1 ) );
	register_handler( SIGTSTP, call( &HBaseSignalHandlers::signal_TSTP, _1 ) );
	register_handler( SIGCONT, call( &HBaseSignalHandlers::signal_CONT, _1 ) );
	register_handler( SIGUSR1, call( &HBaseSignalHandlers::signal_USR1, _1 ) );
	handler_t fatal( call( static_cast<raw_handler_t>( &HBaseSignalHandlers::signal_fatal ), _1 ) );
	register_handler( SIGSEGV, fatal );
	register_handler( SIGBUS, fatal );
	register_handler( SIGABRT, fatal );
	register_handler( SIGILL, fatal );
	register_handler( SIGFPE, fatal );
#if defined( HAVE_DECL_SIGIOT ) && ( HAVE_DECL_SIGIOT == 1 )
	register_handler( SIGIOT, fatal );
#endif /* HAVE_DECL_SIGIOT */
	register_handler( SIGTRAP, fatal );
	register_handler( SIGSYS, fatal );
	catch_signal( SIGPIPE );
	block_signal( SIGALRM );
	HThreadPool::get_instance().start_task(
		call( &HSignalService::run, this ),
		call( &HSignalService::stop, this ),
		call( &HSignalService::want_restart, this )
	);
	return;
	M_EPILOG
}

HSignalService::~HSignalService( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HSignalService::stop( void ) {
	M_PROLOG
	if ( _loop ) {
		_loop = false;
		/*
		 * man for raise() is full of shit
		 * raise( SIG_NO ) is NOT equivalent for kill( getpid(), SIG_NO )
		 * with respect to multi-thread environment at least
		 * all hail to IBM Signal Management documentation.
		 */
		M_ENSURE( hcore::system::kill( hcore::system::getpid(), SIGURG ) == 0 );
		reset_signal_low( SIGALRM );
		HSet<int> signals;
		transform( _handlers.begin(), _handlers.end(), insert_iterator( signals ), select1st<handlers_t::value_type>() );
		for ( int sigNo : signals ) {
			reset_signal( sigNo );
		}
	}
	return;
	M_EPILOG
}

void HSignalService::run( void ) {
	M_PROLOG
	HThread::set_name( "HSignalService" );
	while ( _loop ) {
		int sigNo = 0;
		M_ENSURE( sigwait( _catch.get<sigset_t>(), &sigNo ) == 0 );
		HLock lock( _mutex );
		call_handler( sigNo );
	}
	return;
	M_EPILOG
}

bool HSignalService::want_restart( void ) const {
	return ( true );
}

void HSignalService::register_handler( int sigNo_, handler_t handler_, void const* owner_ ) {
	M_PROLOG
	HLock lock( _mutex );
	_handlers.push_front( make_pair( sigNo_, make_pair( handler_, owner_ ) ) );
	if ( _handlers.count( sigNo_ ) == 1 ) {
		int SYNCHRONOUS_SIGNALS[] = { SIGSEGV, SIGBUS, SIGFPE, SIGILL };
		if ( find( SYNCHRONOUS_SIGNALS, SYNCHRONOUS_SIGNALS + yaal::size( SYNCHRONOUS_SIGNALS ), sigNo_ ) == ( SYNCHRONOUS_SIGNALS + yaal::size( SYNCHRONOUS_SIGNALS ) ) ) {
			catch_signal( sigNo_ );
			M_ENSURE( hcore::system::kill( hcore::system::getpid(), SIGURG ) == 0 );
		} else {
			struct sigaction act;
			::memset( &act, 0, sizeof ( act ) );
			act.sa_flags = SA_RESTART;
			act.sa_handler = &HBaseSignalHandlers::signal_fatal_sync;
			M_ENSURE( sigemptyset( &act.sa_mask ) == 0 );
			M_ENSURE( sigaddset_fwd( &act.sa_mask, sigNo_ ) == 0 );
			M_ENSURE( sigaction( sigNo_, &act, nullptr ) == 0 );
		}
	}
	return;
	M_EPILOG
}

void HSignalService::flush_handlers( void const* owner_ ) {
	M_PROLOG
	HLock lock( _mutex );
	for ( handlers_t::iterator it( _handlers.begin() ); it != _handlers.end(); ) {
		handlers_t::iterator del( it );
		++ it;
		if ( del->second.second == owner_ ) {
			int sigNo( del->first );
			if ( _handlers.count( sigNo ) == 1 ) {
				reset_signal( sigNo );
			}
			_handlers.erase( del );
		}
	}
	return;
	M_EPILOG
}

void HSignalService::catch_signal( int sigNo_ ) {
	M_PROLOG
	M_ENSURE( sigaddset_fwd( _catch.get<sigset_t>(), sigNo_ ) == 0 );

	/*
	 * FreeBSD does not wake sigwait on signal with installed
	 * default handlers with IGNORE action.
	 * FreeBSD does not wake sigwait even if one specify IGNORED
	 * signal as blocked.
	 */
	catch_signal_low( sigNo_ );
	return;
	M_EPILOG
}

void HSignalService::block_signal( int sigNo_ ) {
	M_PROLOG
	catch_signal_low( sigNo_ );
	return;
	M_EPILOG
}

void HSignalService::reset_signal( int sigNo_ ) {
	M_PROLOG
	HLock lock( _mutex );
	M_ENSURE( _handlers.count( sigNo_ ) );
	reset_signal_low( sigNo_ );
	M_ENSURE( sigdelset_fwd( _catch.get<sigset_t>(), sigNo_ ) == 0 );
	_handlers.erase( sigNo_ );
	return;
	M_EPILOG
}

void HSignalService::schedule_exit( int exitStatus_ ) {
	M_PROLOG
	_isKilled_ = true;
	_exitStatus = exitStatus_;
	M_ENSURE( signal( SIGALRM, HSignalService::exit ) != FWD_SIG_ERR );
	alarm( 1 );
	M_EPILOG
}

void HSignalService::exit( int ) {
	::exit( _exitStatus );
}

int HSignalService::life_time( int ) {
	return ( HThreadPool::life_time( 0 ) - 10 );
}

void HSignalService::call_handler( int sigNo_ ) {
	M_PROLOG
	handlers_t::iterator it;
	if ( ( it = _handlers.find( sigNo_ ) ) != _handlers.end() ) {
		for ( ; ( it != _handlers.end() ) && ( (*it).first == sigNo_ ); ++ it ) {
			handler_t handler( (*it).second.first );
			M_ASSERT( !! handler );
			int status = handler( sigNo_ );
			if ( status > 0 ) {
				/* signal was entirely consumed */
				break;
			} else if ( status < 0 ) {
				_loop = false;
				schedule_exit( status );
			}
		}
	} else
		M_ENSURE( ( sigNo_ == SIGURG ) || ( sigNo_ == SIGPIPE ) );
	return;
	M_EPILOG
}

namespace {

/* signal handler definitions */

int HBaseSignalHandlers::signal_INT ( int signum_ ) {
	M_PROLOG
	if ( tools::_ignoreSignalSIGINT_ ) {
		return ( 0 );
	}
	HString message;
	message = "Interrupt signal caught, process broken: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_LEVEL::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	return ( -1 );
	M_EPILOG
}

int HBaseSignalHandlers::signal_HUP( int signum_ ) {
	M_PROLOG
	HString message;
	message = "Unhandled HUP received: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_LEVEL::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	return ( -1 );
	M_EPILOG
}

int HBaseSignalHandlers::signal_TERM( int signum_ ) {
	M_PROLOG
	HString message;
	message = "Process was explicitly killed: ";
	message += strsignal( signum_ );
	message += '.';
	log( LOG_LEVEL::NOTICE ) << message << endl;
	cerr << "\n" << message << endl;
	return ( -2 );
	M_EPILOG
}

int HBaseSignalHandlers::signal_QUIT ( int signum_ ) {
	M_PROLOG
	if ( tools::_ignoreSignalSIGQUIT_ )
		return ( 0 );
	HString message;
	message = "Abnormal program quit forced: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_LEVEL::ERROR ) << message << endl;
	cerr << "\n" << message << endl;
	abort();
	return ( 0 );
	M_EPILOG
}

int HBaseSignalHandlers::signal_TSTP( int signum_ ) {
	M_PROLOG
	if ( tools::_ignoreSignalSIGTSTP_ )
		return ( 0 );
	HString message;
	message = "Stop signal caught, process suspended: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_LEVEL::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	reset_signal_low( SIGTSTP );
	raise( SIGTSTP );
	return ( 0 );
	M_EPILOG
}

int HBaseSignalHandlers::signal_CONT( int signum_ ) {
	M_PROLOG
	catch_signal_low( SIGTSTP );
	HString message;
	message = "Process was resurrected: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_LEVEL::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	return ( 0 );
	M_EPILOG
}

int HBaseSignalHandlers::signal_fatal( int signum_ ) {
	M_PROLOG
	HString message;
	message = "Process caused FATAL ERROR: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_LEVEL::CRITICAL ) << message << endl;
	cerr << "\n" << message << endl;
	abort();
#ifdef __MSVCXX__
	return ( -1 );
#endif /* #ifdef __MSVCXX__ */
	M_EPILOG
}

void HBaseSignalHandlers::signal_fatal_sync( int signum_ ) {
	M_PROLOG
	HSignalService::get_instance().call_handler( signum_ );
	M_EPILOG
}

int HBaseSignalHandlers::signal_USR1( int signum_ ) {
	M_PROLOG
	HString message;
	message = "\nDo you play with the mouse under FreeBSD ? ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_LEVEL::WARNING ) << message << endl;
	cerr << "\n" << message << endl;
	return ( -3 );
	M_EPILOG
}

/* end of signal handler definitions */

}

}

}

