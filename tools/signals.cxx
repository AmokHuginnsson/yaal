/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	signals.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>  /* strsignal */
#include <cstdio>	  /* perror function */
#include <csignal>  /* signal handling */
#include <unistd.h> /* kill function */
#include <cstdlib>  /* exit */

#include "config.h"

#include "hcore/hexception.h" /* M_PROLOG, M_EPILOG */
M_VCSID ( "$Id$" )
#include "hcore/xalloc.h"
#include "hcore/hlog.h"       /* log object */
#include "hcore/hstring.h"    /* HString class */
#include "signals.h"
#include "tools.h"                /* tools namespace */

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

namespace
{

class HBaseSignalHandlers : public HSignalHandlerInterface
	{
public:
	void unlock( int );
	void lock( int );
	int signal_INT( int );
	int signal_HUP( int );
	int signal_TERM( int );
	int signal_QUIT( int );
	int signal_TSTP( int );
	int signal_CONT( int );
	int signal_fatal( int ) __attribute__(( __noreturn__ ));
	int signal_USR1( int );
	};

}

int HSignalService::HHandlerGeneric::invoke( int a_iSigNo )
	{
	M_PROLOG
	return ( (get_base()->*HANDLER)( a_iSigNo ) );
	M_EPILOG
	}

HSignalHandlerInterface*	HSignalService::HHandlerInternal::get_base( void )
	{
	M_PROLOG
	return ( &*f_oBase );
	M_EPILOG
	}

HSignalHandlerInterface*	HSignalService::HHandlerExternal::get_base( void )
	{
	M_PROLOG
	return ( f_poBase );
	M_EPILOG
	}

int HSignalService::f_iExitStatus = 0;
HSignalService::HSignalService( void )
	: f_bLoop( true ), f_oLocker( xcalloc<sigset_t>( 1 ) ),
	f_oWorker( *this ), f_oMutex(), f_oHandlers()
	{
	M_PROLOG
	sigemptyset( static_cast<sigset_t*>( f_oLocker.get() ) );
	HSignalHandlerInterface::ptr_t base( new HBaseSignalHandlers() );
	register_handler( SIGINT, HHandlerGeneric::ptr_t( new HHandlerInternal( base, &HBaseSignalHandlers::signal_INT ) ) );
	register_handler( SIGHUP, HHandlerGeneric::ptr_t( new HHandlerInternal( base, &HBaseSignalHandlers::signal_HUP ) ) );
	register_handler( SIGTERM, HHandlerGeneric::ptr_t( new HHandlerInternal( base, &HBaseSignalHandlers::signal_TERM ) ) );
	register_handler( SIGQUIT, HHandlerGeneric::ptr_t( new HHandlerInternal( base, &HBaseSignalHandlers::signal_QUIT ) ) );
	register_handler( SIGTSTP, HHandlerGeneric::ptr_t( new HHandlerInternal( base, &HBaseSignalHandlers::signal_TSTP ) ) );
	register_handler( SIGCONT, HHandlerGeneric::ptr_t( new HHandlerInternal( base, &HBaseSignalHandlers::signal_CONT ) ) );
	register_handler( SIGUSR1, HHandlerGeneric::ptr_t( new HHandlerInternal( base, &HBaseSignalHandlers::signal_USR1 ) ) );
	HHandlerGeneric::ptr_t fatal( new HHandlerInternal( base, &HBaseSignalHandlers::signal_fatal ) );
	register_handler( SIGSEGV, fatal );
	register_handler( SIGBUS, fatal );
	register_handler( SIGABRT, fatal );
	register_handler( SIGILL, fatal );
	register_handler( SIGFPE, fatal );
	register_handler( SIGIOT, fatal );
	register_handler( SIGTRAP, fatal );
	register_handler( SIGSYS, fatal );
	register_handler( SIGPIPE, fatal );
	lock_on( SIGURG );
	f_oWorker.spawn();
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
	kill( getpid(), SIGURG );
	f_oWorker.finish();
	return;
	M_EPILOG
	}

int HSignalService::operator()( HThread const* )
	{
	M_PROLOG
	while ( f_bLoop && f_oWorker.is_alive() )
		{
		int l_iSigNo = 0;
		sigwait( static_cast<sigset_t*>( f_oLocker.get() ), &l_iSigNo );
		HLock l_oLock( f_oMutex );
		handlers_t::HIterator it = f_oHandlers.find( l_iSigNo );
		if ( it != f_oHandlers.end() )
			{
			handler_list_ptr_t i = it->second;
			for ( handler_list_t::iterator handlerIt = i->begin(); handlerIt != i->end(); ++ handlerIt )
				{
				HHandlerGeneric::ptr_t handler = *handlerIt;
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
			M_ENSURE( l_iSigNo == SIGURG );
		}
	return ( 0 );
	M_EPILOG
	}

void HSignalService::register_handler( int a_iSigNo, HSignalService::HHandlerGeneric::ptr_t a_oHandler )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( f_oHandlers.find( a_iSigNo ) == f_oHandlers.end() )
		f_oHandlers[ a_iSigNo ] = handler_list_ptr_t( new handler_list_t() );
	f_oHandlers[ a_iSigNo ]->push_front( a_oHandler );
	lock_on( a_iSigNo );
	kill( getpid(), SIGURG );
	return;
	M_EPILOG
	}

void HSignalService::lock_on( int a_iSigNo )
	{
	M_PROLOG
	sigaddset( static_cast<sigset_t*>( f_oLocker.get() ), a_iSigNo );
	pthread_sigmask( SIG_BLOCK, static_cast<sigset_t*>( f_oLocker.get() ), NULL );
	return;
	M_EPILOG
	}

void HSignalService::schedule_exit( int a_iExitStatus )
	{
	f_iExitStatus = a_iExitStatus;
	signal( SIGALRM, HSignalService::exit );
	alarm( 1 );
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
	sigset_t set;
	sigemptyset( &set );
	sigaddset( &set, a_iSigNo );
	pthread_sigmask( SIG_UNBLOCK, &set, NULL );
	}

void HBaseSignalHandlers::lock( int a_iSigNo )
	{
	sigset_t set;
	sigemptyset( &set );
	sigaddset( &set, a_iSigNo );
	pthread_sigmask( SIG_BLOCK, &set, NULL );
	}

int HBaseSignalHandlers::signal_INT ( int a_iSignum )
	{
	M_PROLOG
	if ( tools::n_bIgnoreSignalSIGINT )
		return ( 0 );
	HString l_oMessage;
	l_oMessage = "Interrupt signal caught, process broken: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::D_INFO ) << l_oMessage << endl;
	fprintf ( stderr, "\n%s\n", l_oMessage.raw() );
	return ( -1 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_HUP( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "Unhandled HUP received: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::D_INFO ) << l_oMessage << endl;
	fprintf ( stderr, "\n%s\n", l_oMessage.raw() );
	return ( -1 );
	M_EPILOG
	}
	
int HBaseSignalHandlers::signal_TERM( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "Process was explictly killed: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::D_INFO ) << l_oMessage << endl;
	::fprintf( stderr, "\n%s\n", l_oMessage.raw() );
	return ( -2 );
	M_EPILOG
	}
	
int HBaseSignalHandlers::signal_QUIT ( int a_iSignum )
	{
	M_PROLOG
	if ( tools::n_bIgnoreSignalSIGQUIT )
		return ( 0 );
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	l_oMessage = "Abnormal program quit forced: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
	log( LOG_TYPE::D_INFO ) << l_oMessage << endl;
	fprintf ( stderr, "\n%s\n", l_pcSignalMessage );
	abort();
	M_EPILOG
	}

int HBaseSignalHandlers::signal_TSTP ( int a_iSignum )
	{
	M_PROLOG
	if ( tools::n_bIgnoreSignalSIGTSTP )
		return ( 0 );
	HString l_oMessage;
	l_oMessage = "Stop signal caught, process suspended: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::D_INFO ) << l_oMessage << endl;
	fprintf ( stderr, "\n%s\n", l_oMessage.raw() );
	unlock( SIGTSTP );
	raise( SIGTSTP );
	return ( 0 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_CONT ( int a_iSignum )
	{
	M_PROLOG
	lock( SIGTSTP );
	HString l_oMessage;
	l_oMessage = "Process was resurected: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::D_INFO ) << l_oMessage << endl;
	fprintf ( stderr, "\n%s\n", l_oMessage.raw() );
	return ( 0 );
	M_EPILOG
	}

int HBaseSignalHandlers::signal_fatal ( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "Process caused FATAL ERROR: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::D_INFO ) << l_oMessage << endl;
	fprintf ( stderr, "\n%s\n", l_oMessage.raw() );
	abort();
	M_EPILOG
	}

int HBaseSignalHandlers::signal_USR1 ( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "\nDo you play with the mouse under FreeBSD ? ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	log( LOG_TYPE::D_INFO ) << l_oMessage << endl;
	fprintf ( stderr, "\n%s\n", l_oMessage.raw() );
	return ( -3 );
	M_EPILOG
	}

/* end of signal handler definitions */

}

}

}

