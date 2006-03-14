/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hthread.c - this file is integral part of `stdhapi' project.

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

#include <string.h>
#include <libintl.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" )
#include "hthread.h"

namespace stdhapi
{

namespace hcore
{

HThread::HThread ( void )
	: f_eStatus ( D_DEAD ), f_sAttributes ( ), f_xThread ( ),
	f_oMutex ( ), f_oCondition ( )
	{
	M_PROLOG
	M_ENSURE ( pthread_attr_init ( & f_sAttributes ) == 0 );
	M_ENSURE ( pthread_attr_setdetachstate ( & f_sAttributes,
				PTHREAD_CREATE_JOINABLE ) == 0 );
	M_ENSURE ( pthread_attr_setinheritsched ( & f_sAttributes,
				PTHREAD_INHERIT_SCHED ) == 0 );
	return;
	M_EPILOG
	}

HThread::~HThread ( void )
	{
	M_PROLOG
	if ( f_eStatus != D_DEAD )
		finish ( );
	M_ENSURE ( pthread_attr_destroy ( & f_sAttributes ) == 0 );
	return;
	M_EPILOG
	}

int HThread::spawn ( void )
	{
	M_PROLOG
	M_CRITICAL_SECTION ( );
	if ( f_eStatus != D_DEAD )
		M_THROW ( _ ( "thread is already running" ),
				static_cast < int > ( f_eStatus ) );
	M_ENSURE ( pthread_create ( & f_xThread,
				& f_sAttributes, SPAWN, this ) == 0 );
	f_oCondition.wait ( );
	return ( 0 );
	M_EPILOG
	}

int HThread::finish ( void )
	{
	M_PROLOG
	void * l_pvReturn = NULL;
	if ( ( f_eStatus != D_ALIVE ) && ( f_eStatus != D_ZOMBIE ) )
		M_THROW ( _ ( "thread is not running" ),
				static_cast < int > ( f_eStatus ) );
	f_eStatus = D_ZOMBIE;
	M_ENSURE ( pthread_join ( f_xThread, & l_pvReturn ) == 0 );
	f_eStatus = D_DEAD;
	return ( reinterpret_cast < int > ( l_pvReturn ) );
	M_EPILOG
	}

void * HThread::SPAWN ( void * a_pvThread )
	{
	M_PROLOG
	return ( reinterpret_cast < HThread * > ( a_pvThread )->control ( ) );
	M_EPILOG
	}

void * HThread::control ( void )
	{
	M_PROLOG
	void * l_pvReturn = NULL;
	M_ENSURE ( pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, NULL ) == 0 );
	M_ENSURE ( pthread_setcanceltype ( PTHREAD_CANCEL_DEFERRED, NULL ) == 0 );
	f_eStatus = D_ALIVE;
	f_oCondition.signal ( );
/* It is possible that destructor of derived class will be called before
 * next call here (call to run()). So there will be pure virtual method called.
 * Such event result in program termination.
 * Scenario that completes behavior described above happens when
 * two calls to spawn() occur in turn.
 * To avoid this class user must ensure that derived class destructor
 * will not finish before eventual call to run is done.
 */
	l_pvReturn = reinterpret_cast < void * > ( run ( ) );
	f_eStatus = D_ZOMBIE;
	return ( l_pvReturn );
	M_EPILOG
	}

bool HThread::is_alive ( void ) const
	{
	M_PROLOG
	return ( f_eStatus == D_ALIVE );
	M_EPILOG
	}

HMutex::HMutex ( bool const a_bRecursive ) : f_bRecursive ( a_bRecursive ),
																						 f_sAttributes ( ), f_xMutex ( )
	{
	M_PROLOG
	pthread_mutexattr_init ( & f_sAttributes );
	M_ENSURE ( pthread_mutexattr_settype ( & f_sAttributes,
				f_bRecursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK ) != EINVAL );
	pthread_mutex_init ( & f_xMutex, & f_sAttributes );
	return;
	M_EPILOG
	}

HMutex::~HMutex ( void )
	{
	M_PROLOG
	int l_iError = 0;
	while ( ( l_iError = pthread_mutex_destroy ( & f_xMutex ) ) == EBUSY )
		;
	M_ENSURE ( l_iError == 0 );
	pthread_mutexattr_destroy ( & f_sAttributes );
	return;
	M_EPILOG
	}

void HMutex::lock ( void )
	{
	M_PROLOG
	int l_iError = pthread_mutex_lock ( & f_xMutex );
	if ( ! f_bRecursive )
		M_ENSURE ( l_iError != EDEADLK );
	return;
	M_EPILOG
	}

void HMutex::unlock ( void )
	{
	M_PROLOG
	int l_iError = pthread_mutex_unlock ( & f_xMutex );
	if ( ! f_bRecursive )
		M_ENSURE ( l_iError != EPERM );
	return;
	M_EPILOG
	}

HLock::HLock ( HMutex & a_roMutex ) : f_roMutex ( a_roMutex )
	{
	M_PROLOG
	f_roMutex.lock ( );
	return;
	M_EPILOG
	}

HLock::~HLock ( void )
	{
	M_PROLOG
	f_roMutex.unlock ( );
	return;
	M_EPILOG
	}

HCondition::HCondition ( void )
	: f_sAttributes ( ), f_xCondition ( ), f_oMutex ( )
	{
	M_PROLOG
	pthread_condattr_init ( & f_sAttributes );
	pthread_cond_init ( & f_xCondition, & f_sAttributes );
	f_oMutex.lock ( );
	return;
	M_EPILOG
	}

HCondition::~HCondition ( void )
	{
	M_PROLOG
	f_oMutex.unlock ( );
	M_ENSURE ( pthread_cond_destroy ( & f_xCondition ) == 0 );
	pthread_condattr_destroy ( & f_sAttributes );
	return;
	M_EPILOG
	}

HCondition::status_t HCondition::wait ( int long unsigned * a_pulTimeOutSeconds,
		int long unsigned * a_pulTimeOutNanoSeconds )
	{
	M_PROLOG
	int l_iError = 0;
	timespec l_sTimeOut;
	if ( a_pulTimeOutSeconds || a_pulTimeOutNanoSeconds )
		{
		memset ( & l_sTimeOut, 0, sizeof ( timespec ) );
		if ( a_pulTimeOutSeconds )
			l_sTimeOut.tv_sec = ( * a_pulTimeOutSeconds );
		if ( a_pulTimeOutNanoSeconds )
			l_sTimeOut.tv_nsec = ( * a_pulTimeOutNanoSeconds );
		l_iError = pthread_cond_timedwait ( & f_xCondition,
					& f_oMutex.f_xMutex, & l_sTimeOut );
		if ( a_pulTimeOutSeconds )
			( * a_pulTimeOutSeconds ) = l_sTimeOut.tv_sec;
		if ( a_pulTimeOutNanoSeconds )
			( * a_pulTimeOutNanoSeconds ) = l_sTimeOut.tv_nsec;
		M_ENSURE ( ( l_iError == 0 ) || ( l_iError == EINTR ) || ( l_iError == ETIMEDOUT ) );
		return ( ( l_iError == 0 ) ? D_OK : ( ( l_iError == EINTR ) ? D_INTERRUPT : D_TIMEOUT ) );
		}
	else
		pthread_cond_wait ( & f_xCondition, & f_oMutex.f_xMutex ); /* Always returns 0. */
	return ( D_OK );
	M_EPILOG
	}

void HCondition::signal ( void )
	{
	M_PROLOG
	M_CRITICAL_SECTION ( );
	pthread_cond_signal ( & f_xCondition );
	return;
	M_EPILOG
	}

}

}

