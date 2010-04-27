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
#include <cstdio>	   /* perror function */
#include <csignal>   /* signal handling */
#include <unistd.h>  /* kill function */
#include <cstdlib>   /* exit */
#include <pthread.h> /* pthread_sigmask (in FreeBSD) */

#include "config.hxx"

#include "hcore/base.hxx" /* M_PROLOG, M_EPILOG */
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore/xalloc.hxx"
#include "hcore/hlog.hxx"       /* log object */
#include "hcore/hstring.hxx"    /* HString class */
#include "signals.hxx"
#include "tools.hxx"                /* tools namespace */

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
	typedef HBaseSignalHandlers self_t;
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

int HSignalService::f_iExitStatus = 0;
HSignalService::HSignalService( void )
	: f_bLoop( true ), f_oLocker( chunk_size<sigset_t>( 1 ) ),
	_thread(), f_oMutex(), f_oHandlers()
	{
	M_PROLOG
	M_ENSURE( sigemptyset( f_oLocker.get<sigset_t>() ) == 0 );
	if ( n_iDebugLevel < DEBUG_LEVEL::GDB )
		register_handler( SIGINT, bound_call( &HBaseSignalHandlers::signal_INT, _1 ) );
	register_handler( SIGHUP, bound_call( &HBaseSignalHandlers::signal_HUP, _1 ) );
	register_handler( SIGTERM, bound_call( &HBaseSignalHandlers::signal_TERM, _1 ) );
	register_handler( SIGQUIT, bound_call( &HBaseSignalHandlers::signal_QUIT, _1 ) );
	register_handler( SIGTSTP, bound_call( &HBaseSignalHandlers::signal_TSTP, _1 ) );
	register_handler( SIGCONT, bound_call( &HBaseSignalHandlers::signal_CONT, _1 ) );
	register_handler( SIGUSR1, bound_call( &HBaseSignalHandlers::signal_USR1, _1 ) );
	handler_t fatal( bound_call( &HBaseSignalHandlers::signal_fatal, _1 ) );
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
	_thread.spawn( bound_call( &HSignalService::run, this ) );
	return;
	M_EPILOG
	}

HSignalService::~HSignalService( void )
	{
	M_PROLOG
	f_bLoop = false;
	/*
	 * man for raise() is full of shit
	 * raise( SIG_NO ) is NOT equivalent for kill( getpid(), SIG_NO )
	 * with respect to multi-thread environment at least
	 * all hail to IBM Signal Managment documentation
	 */
	M_ENSURE( kill( getpid(), SIGURG ) == 0 );
	_thread.finish();
	return;
	M_EPILOG
	}

void* HSignalService::run( void )
	{
	M_PROLOG
	while ( f_bLoop && _thread.is_alive() )
		{
		int l_iSigNo = 0;
		M_ENSURE( sigwait( f_oLocker.get<sigset_t>(), &l_iSigNo ) == 0 );
		HLock l_oLock( f_oMutex );
		handlers_t::iterator it;
		if ( ( it = f_oHandlers.find( l_iSigNo ) ) != f_oHandlers.end() )
			{
			for ( ; ( it != f_oHandlers.end() ) && ( (*it).first == l_iSigNo ); ++ it )
				{
				handler_t handler( (*it).second );
				M_ASSERT( !! handler );
				int status = handler->invoke( l_iSigNo );
				if ( status > 0 )
					break; /* signal was entirely consumed */
				else if ( status < 0 )
					{
					f_bLoop = false;
					schedule_exit( -1 - status );
					}
				}
			}
		else
			M_ENSURE( ( l_iSigNo == SIGURG ) || ( l_iSigNo == SIGPIPE ) );
		}
	return ( 0 );
	M_EPILOG
	}

void HSignalService::register_handler( int a_iSigNo, handler_t a_oHandler )
	{
	M_PROLOG
	f_oHandlers.push_front( a_iSigNo, a_oHandler );
	lock_on( a_iSigNo );
	M_ENSURE( kill( getpid(), SIGURG ) == 0 );
	return;
	M_EPILOG
	}

void HSignalService::lock_on( int a_iSigNo )
	{
	M_PROLOG
	M_ENSURE( sigaddset( f_oLocker.get<sigset_t>(), a_iSigNo ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_BLOCK, f_oLocker.get<sigset_t>(), NULL ) == 0 );

	/*
	 * FreeBSD does not wake sigwait on signal with installed
	 * default hadlers with INGORE action.
	 * FreeBSD does not wake sigwait even if one specify INGNORED
	 * signal as blocked.
	 */
	struct sigaction act;
	M_ENSURE( sigemptyset( &act.sa_mask ) == 0 );
	M_ENSURE( sigaddset( &act.sa_mask, a_iSigNo ) == 0 );
	act.sa_handler = dummy_signal_handler;
	act.sa_flags = SA_RESTART;
	M_ENSURE( sigaction( a_iSigNo, &act, NULL ) == 0 );
	return;
	M_EPILOG
	}

void HSignalService::schedule_exit( int a_iExitStatus )
	{
	M_PROLOG
	f_iExitStatus = a_iExitStatus;
	M_ENSURE( signal( SIGALRM, HSignalService::exit ) != SIG_ERR );
	alarm( 1 );
	M_EPILOG
	}

void HSignalService::exit( int )
	{
	::exit( f_iExitStatus );
	}

int HSignalService::life_time( int )
	{
	return ( 100 );
	}

namespace
{

/* singnal handler definitions */

void HBaseSignalHandlers::unlock( int a_iSigNo )
	{
	M_PROLOG
	sigset_t set;
	M_ENSURE( sigemptyset( &set ) == 0 );
	M_ENSURE( sigaddset( &set, a_iSigNo ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_UNBLOCK, &set, NULL ) == 0 );
	return;
	M_EPILOG
	}

void HBaseSignalHandlers::lock( int a_iSigNo )
	{
	M_PROLOG
	sigset_t set;
	M_ENSURE( sigemptyset( &set ) == 0 );
	M_ENSURE( sigaddset( &set, a_iSigNo ) == 0 );
	M_ENSURE( pthread_sigmask( SIG_BLOCK, &set, NULL ) == 0 );
	return;
	M_EPILOG
	}

int HBaseSignalHandlers::signal_INT ( int a_iSignum )
	{
	M_PROLOG
	if ( tools::n_bIgnoreSignalSIGINT )
		return ( 0 );
	HString l_oMessage;
	l_oMessage = "Interrupt signal caught, process broken: ";
	l_oMessage += ::strsignal( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	return ( -1 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_HUP( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "Unhandled HUP received: ";
	l_oMessage += ::strsignal( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	return ( -1 );
	M_EPILOG
	}
	
int HBaseSignalHandlers::signal_TERM( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "Process was explictly killed: ";
	l_oMessage += strsignal( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	return ( -2 );
	M_EPILOG
	}
	
int HBaseSignalHandlers::signal_QUIT ( int a_iSignum )
	{
	M_PROLOG
	if ( tools::n_bIgnoreSignalSIGQUIT )
		return ( 0 );
	HString l_oMessage;
	l_oMessage = "Abnormal program quit forced: ";
	l_oMessage += ::strsignal( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	abort();
	return ( 0 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_TSTP( int a_iSignum )
	{
	M_PROLOG
	if ( tools::n_bIgnoreSignalSIGTSTP )
		return ( 0 );
	HString l_oMessage;
	l_oMessage = "Stop signal caught, process suspended: ";
	l_oMessage += ::strsignal( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	unlock( SIGTSTP );
	raise( SIGTSTP );
	return ( 0 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_CONT( int a_iSignum )
	{
	M_PROLOG
	lock( SIGTSTP );
	HString l_oMessage;
	l_oMessage = "Process was resurected: ";
	l_oMessage += ::strsignal( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	return ( 0 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_fatal( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "Process caused FATAL ERROR: ";
	l_oMessage += ::strsignal( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	abort();
	M_EPILOG
	}

int HBaseSignalHandlers::signal_USR1( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "\nDo you play with the mouse under FreeBSD ? ";
	l_oMessage += ::strsignal( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	return ( -3 );
	M_EPILOG
	}

/* end of signal handler definitions */

}

}

}

