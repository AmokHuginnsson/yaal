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
M_CVSID ( "$CVSHeader$" );
#include "hthread.h"

namespace stdhapi
{

namespace hcore
{

HThread::HThread ( void )
	: f_eStatus ( D_DEAD ), f_sAttributes ( ), f_xThread ( ), f_oMutex ( )
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
		M_IRV ( finish ( ) );
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
	while ( ( f_eStatus != D_ALIVE ) && ( f_eStatus != D_ZOMBIE ) )
		;
	return ( 0 );
	M_EPILOG
	}

int HThread::finish ( void )
	{
	M_PROLOG
	M_CRITICAL_SECTION ( );
	void * l_pvReturn = NULL;
	if ( ( f_eStatus != D_ALIVE ) && ( f_eStatus != D_ZOMBIE ) )
		M_THROW ( _ ( "thread is not running" ), static_cast < int > ( f_eStatus ) );
	M_ENSURE ( pthread_cancel ( f_xThread ) == 0 );
	M_ENSURE ( pthread_join ( f_xThread, & l_pvReturn ) == 0 );
	f_eStatus = D_DEAD;
	return ( reinterpret_cast < int > ( l_pvReturn ) );
	M_EPILOG
	}

void * HThread::SPAWN ( void * a_pvThread )
	{
	M_PROLOG
	void * l_pvReturn = NULL;
	HThread * l_poThread = reinterpret_cast < HThread * > ( a_pvThread );
	M_ENSURE ( pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, NULL ) == 0 );
	M_ENSURE ( pthread_setcanceltype ( PTHREAD_CANCEL_DEFERRED, NULL ) == 0 );
	l_poThread->f_eStatus = D_ALIVE;
	l_pvReturn = reinterpret_cast < void * > ( l_poThread->run ( ) );
	l_poThread->f_eStatus = D_ZOMBIE;
	return ( l_pvReturn );
	M_EPILOG
	}

HThread::status_t HThread::listen ( void ) const
	{
	M_PROLOG
	M_ENSURE ( pthread_setcancelstate ( PTHREAD_CANCEL_ENABLE, NULL ) == 0 );
	pthread_testcancel ( );
	M_ENSURE ( pthread_setcancelstate ( PTHREAD_CANCEL_DISABLE, NULL ) == 0 );
	return ( f_eStatus );
	M_EPILOG
	}

bool HThread::is_alive ( void ) const
	{
	M_PROLOG
	return ( f_eStatus == D_ALIVE );
	M_EPILOG
	}

HMutex::HMutex ( bool a_bRecursive ) : f_bRecursive ( a_bRecursive ),
																			 f_sAttributes ( ), f_xMutex ( )
	{
	M_PROLOG
	M_IRV ( pthread_mutexattr_init ( & f_sAttributes ) );
	M_ENSURE ( pthread_mutexattr_settype ( & f_sAttributes,
				f_bRecursive ? PTHREAD_MUTEX_RECURSIVE_NP : PTHREAD_MUTEX_ERRORCHECK_NP ) != EINVAL );
	M_IRV ( pthread_mutex_init ( & f_xMutex, & f_sAttributes ) );
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
	M_IRV ( pthread_mutexattr_destroy ( & f_sAttributes ) );
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

}

}

