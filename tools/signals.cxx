/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	signals.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>   /* strsignal */
#include <csignal>   /* signal handling */
#include <unistd.h>  /* kill function */
#include <cstdlib>   /* exit */
#include <pthread.h> /* pthread_sigmask (in FreeBSD) */

#include "config.hxx"

#include "hcore/base.hxx" /* M_PROLOG, M_EPILOG */
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/xalloc.hxx"
#include "hcore/hlog.hxx"     /* log object */
#include "hcore/hset.hxx"
#include "hcore/hstring.hxx"  /* HString class */
#include "hcore/system.hxx"   /* get_pid() */
#include "hcore/iterator.hxx"
#include "signals.hxx"
#include "tools.hxx"          /* tools namespace */

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace
{

/*
 * dummy_signal_handler is called even if signal is masked in sigwait,
 * so it is best to have this handler as simple and non-intrusive as possible.
 */
static void dummy_signal_handler( int )
	{ }

class HBaseSignalHandlers
	{
	typedef HBaseSignalHandlers this_type;
public:
	static void unlock( int );
	static void lock( int );
	static int signal_INT( int );
	static int signal_HUP( int );
	static int signal_TERM( int );
	static int signal_QUIT( int );
	static int signal_TSTP( int );
	static int signal_CONT( int );
	static int signal_fatal( int ) __attribute__(( __noreturn__ ));
	static int signal_USR1( int );
	};

}

int HSignalService::_exitStatus = 0;
HSignalService::HSignalService( void )
	: _loop( true ), _locker( chunk_size<sigset_t>( 1 ) ),
	_thread(), _mutex(), _handlers()
	{
	M_PROLOG
	M_ENSURE( sigemptyset( _locker.get<sigset_t>() ) == 0 );
	if ( _debugLevel_ < DEBUG_LEVEL::GDB )
		register_handler( SIGINT, call( &HBaseSignalHandlers::signal_INT, _1 ) );
	register_handler( SIGHUP, call( &HBaseSignalHandlers::signal_HUP, _1 ) );
	register_handler( SIGTERM, call( &HBaseSignalHandlers::signal_TERM, _1 ) );
	register_handler( SIGQUIT, call( &HBaseSignalHandlers::signal_QUIT, _1 ) );
	register_handler( SIGTSTP, call( &HBaseSignalHandlers::signal_TSTP, _1 ) );
	register_handler( SIGCONT, call( &HBaseSignalHandlers::signal_CONT, _1 ) );
	register_handler( SIGUSR1, call( &HBaseSignalHandlers::signal_USR1, _1 ) );
	handler_t fatal( call( &HBaseSignalHandlers::signal_fatal, _1 ) );
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
	lock_on( SIGPIPE );
	lock_on( SIGURG );
	_thread.spawn( call( &HSignalService::run, this ) );
	return;
	M_EPILOG
	}

HSignalService::~HSignalService( void )
	{
	M_PROLOG
	if ( _loop )
		stop();
	return;
	M_EPILOG
	}

void HSignalService::stop( void )
	{
	M_PROLOG
	if ( _loop )
		{
		_loop = false;
		/*
		 * man for raise() is full of shit
		 * raise( SIG_NO ) is NOT equivalent for kill( getpid(), SIG_NO )
		 * with respect to multi-thread environment at least
		 * all hail to IBM Signal Managment documentation.
		 */
		M_ENSURE( system::kill( system::getpid(), SIGURG ) == 0 );
		_thread.finish();
		HSet<int> signals;
		transform( _handlers.begin(), _handlers.end(), insert_iterator( signals ), select1st<handlers_t::value_type>() );
		for_each( signals.begin(), signals.end(), call( &HSignalService::reset_signal, this, _1 ) );
		}
	return;
	M_EPILOG
	}

void* HSignalService::run( void )
	{
	M_PROLOG
	while ( _loop && _thread.is_alive() )
		{
		int sigNo = 0;
		M_ENSURE( sigwait( _locker.get<sigset_t>(), &sigNo ) == 0 );
		HLock lock( _mutex );
		handlers_t::iterator it;
		if ( ( it = _handlers.find( sigNo ) ) != _handlers.end() )
			{
			for ( ; ( it != _handlers.end() ) && ( (*it).first == sigNo ); ++ it )
				{
				handler_t handler( (*it).second );
				M_ASSERT( !! handler );
				int status = handler( sigNo );
				if ( status > 0 )
					break; /* signal was entirely consumed */
				else if ( status < 0 )
					{
					_loop = false;
					schedule_exit( -1 - status );
					}
				}
			}
		else
			M_ENSURE( ( sigNo == SIGURG ) || ( sigNo == SIGPIPE ) );
		}
	return ( 0 );
	M_EPILOG
	}

void HSignalService::register_handler( int sigNo_, handler_t handler_ )
	{
	M_PROLOG
	_handlers.push_front( sigNo_, handler_ );
	lock_on( sigNo_ );
	M_ENSURE( system::kill( system::getpid(), SIGURG ) == 0 );
	return;
	M_EPILOG
	}

void HSignalService::lock_on( int sigNo_ )
	{
	M_PROLOG
	M_ENSURE( sigaddset( _locker.get<sigset_t>(), sigNo_ ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_BLOCK, _locker.get<sigset_t>(), NULL ) == 0 );

	/*
	 * FreeBSD does not wake sigwait on signal with installed
	 * default hadlers with INGORE action.
	 * FreeBSD does not wake sigwait even if one specify INGNORED
	 * signal as blocked.
	 */
	struct sigaction act;
	::memset( &act, 0, sizeof ( struct sigaction ) );
	act.sa_flags = SA_RESTART;
	act.sa_handler = dummy_signal_handler;
	M_ENSURE( sigemptyset( &act.sa_mask ) == 0 );
	M_ENSURE( sigaddset( &act.sa_mask, sigNo_ ) == 0 );
	M_ENSURE( sigaction( sigNo_, &act, NULL ) == 0 );
	return;
	M_EPILOG
	}

void HSignalService::reset_signal( int sigNo_ )
	{
	M_PROLOG
	M_ENSURE( _handlers.count( sigNo_ ) );
	struct sigaction act;
	::memset( &act, 0, sizeof ( struct sigaction ) );
	act.sa_handler = SIG_DFL;
	M_ENSURE( sigemptyset( &act.sa_mask ) == 0 );
	M_ENSURE( sigaddset( &act.sa_mask, sigNo_ ) == 0 );
	M_ENSURE( sigaction( sigNo_, &act, NULL ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_UNBLOCK, &act.sa_mask, NULL ) == 0 );
	M_ENSURE( sigdelset( _locker.get<sigset_t>(), sigNo_ ) == 0 );
	_handlers.erase( sigNo_ );
	return;
	M_EPILOG
	}

void HSignalService::schedule_exit( int exitStatus_ )
	{
	M_PROLOG
	_exitStatus = exitStatus_;
	M_ENSURE( signal( SIGALRM, HSignalService::exit ) != SIG_ERR );
	alarm( 1 );
	M_EPILOG
	}

void HSignalService::exit( int )
	{
	::exit( _exitStatus );
	}

int HSignalService::life_time( int )
	{
	return ( 100 );
	}

namespace
{

/* singnal handler definitions */

void HBaseSignalHandlers::unlock( int sigNo_ )
	{
	M_PROLOG
	sigset_t set;
	M_ENSURE( sigemptyset( &set ) == 0 );
	M_ENSURE( sigaddset( &set, sigNo_ ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_UNBLOCK, &set, NULL ) == 0 );
	return;
	M_EPILOG
	}

void HBaseSignalHandlers::lock( int sigNo_ )
	{
	M_PROLOG
	sigset_t set;
	M_ENSURE( sigemptyset( &set ) == 0 );
	M_ENSURE( sigaddset( &set, sigNo_ ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_BLOCK, &set, NULL ) == 0 );
	return;
	M_EPILOG
	}

int HBaseSignalHandlers::signal_INT ( int signum_ )
	{
	M_PROLOG
	if ( tools::_ignoreSignalSIGINT_ )
		return ( 0 );
	HString message;
	message = "Interrupt signal caught, process broken: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_TYPE::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	return ( -1 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_HUP( int signum_ )
	{
	M_PROLOG
	HString message;
	message = "Unhandled HUP received: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_TYPE::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	return ( -1 );
	M_EPILOG
	}
	
int HBaseSignalHandlers::signal_TERM( int signum_ )
	{
	M_PROLOG
	HString message;
	message = "Process was explictly killed: ";
	message += strsignal( signum_ );
	message += '.';
	log( LOG_TYPE::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	return ( -2 );
	M_EPILOG
	}
	
int HBaseSignalHandlers::signal_QUIT ( int signum_ )
	{
	M_PROLOG
	if ( tools::_ignoreSignalSIGQUIT_ )
		return ( 0 );
	HString message;
	message = "Abnormal program quit forced: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_TYPE::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	abort();
	return ( 0 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_TSTP( int signum_ )
	{
	M_PROLOG
	if ( tools::_ignoreSignalSIGTSTP_ )
		return ( 0 );
	HString message;
	message = "Stop signal caught, process suspended: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_TYPE::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	unlock( SIGTSTP );
	raise( SIGTSTP );
	return ( 0 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_CONT( int signum_ )
	{
	M_PROLOG
	lock( SIGTSTP );
	HString message;
	message = "Process was resurected: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_TYPE::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	return ( 0 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_fatal( int signum_ )
	{
	M_PROLOG
	HString message;
	message = "Process caused FATAL ERROR: ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_TYPE::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	abort();
	M_EPILOG
	}

int HBaseSignalHandlers::signal_USR1( int signum_ )
	{
	M_PROLOG
	HString message;
	message = "\nDo you play with the mouse under FreeBSD ? ";
	message += ::strsignal( signum_ );
	message += '.';
	log( LOG_TYPE::INFO ) << message << endl;
	cerr << "\n" << message << endl;
	return ( -3 );
	M_EPILOG
	}

/* end of signal handler definitions */

}

}

}

