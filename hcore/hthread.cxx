/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hthread.cxx - this file is integral part of `yaal' project.

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

#include <pthread.h>
#include <semaphore.h>
#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hthread.h"
#include "hlog.h"
#include "xalloc.h"

namespace yaal
{

namespace hcore
{

HThread::HThread( void )
	: f_eStatus( D_DEAD ), f_oAttributes( xcalloc<pthread_attr_t>( 1 ) ),
	f_oThread( xcalloc<pthread_t>( 1 ) ), f_oMutex( HMutex::TYPE::D_RECURSIVE ), f_oSemaphore()
	{
	M_PROLOG
	pthread_attr_t* attr = static_cast<pthread_attr_t*>( f_oAttributes.get() );
	M_ENSURE( ::pthread_attr_init( attr ) == 0 );
	M_ENSURE( ::pthread_attr_setdetachstate( attr, PTHREAD_CREATE_JOINABLE ) == 0 );
	M_ENSURE( ::pthread_attr_setinheritsched( attr, PTHREAD_INHERIT_SCHED ) == 0 );
	return;
	M_EPILOG
	}

HThread::~HThread( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( f_eStatus != D_DEAD )
		{
		f_oMutex.unlock();
		finish();
		f_oMutex.lock();
		}
	M_ASSERT( f_eStatus == D_DEAD );
	M_ENSURE( ::pthread_attr_destroy( static_cast<pthread_attr_t*>( f_oAttributes.get() ) ) == 0 );
	return;
	M_EPILOG
	}

int HThread::spawn( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( f_eStatus != D_DEAD )
		M_THROW( _( "thread is already running or spawning" ), f_eStatus );
	f_eStatus = D_SPAWNING;
	M_ENSURE( ::pthread_create( static_cast<pthread_t*>( f_oThread.get() ),
				static_cast<pthread_attr_t*>( f_oAttributes.get() ), SPAWN, this ) == 0 );
	f_oSemaphore.wait();
	return ( 0 );
	M_EPILOG
	}

void HThread::schedule_finish( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( f_eStatus != D_DEAD )
		f_eStatus = D_ZOMBIE;
	return;
	M_EPILOG
	}

int HThread::finish( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( ( f_eStatus != D_ALIVE ) && ( f_eStatus != D_ZOMBIE ) && ( f_eStatus != D_SPAWNING ) )
		M_THROW( _( "thread is not running" ), f_eStatus );
	schedule_finish();
	void* l_pvReturn = NULL;
	f_oMutex.unlock();
	f_oSemaphore.wait();
	f_oMutex.lock();
	M_ENSURE( ::pthread_join( *static_cast<pthread_t*>( f_oThread.get() ), &l_pvReturn ) == 0 );
	f_eStatus = D_DEAD;
	return ( reinterpret_cast<int>( l_pvReturn ) );
	M_EPILOG
	}

void* HThread::SPAWN( void* a_pvThread )
	{
	M_PROLOG
	return ( reinterpret_cast<HThread*>( a_pvThread )->control() );
	/*
	 * We cannot afford silencing uncaught exceptions here.
	 * Because of not catching an exception at the thread owner layer
	 * is a severe coding bug, application must crash in the event
	 * of uncaught exception.
	 */
	M_EPILOG
	}

void* HThread::control( void )
	{
	M_PROLOG
	void* l_pvReturn = NULL;
	M_ENSURE( ::pthread_setcancelstate( PTHREAD_CANCEL_DISABLE, NULL ) == 0 );
	M_ENSURE( ::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL ) == 0 );
	f_eStatus = D_ALIVE;
	f_oSemaphore.signal();
	/*
	 * Setting int run(); as pure virtual exposed us to undefined behavior
	 * at the event of two subsequent spawn() calls.
	 *
	 * The scenario is:
	 * There are 2 threads: T1 and T2.
	 * T1 does:
	 * spawn();
	 *
	 * T2 is created from this spawn, and invokes this function (control()).
	 * Here, between line above and line below comes task switch.
	 *
	 * T1 does:
	 * spawn();
	 *
	 * The second spawn fails with an exception and the thread object is destroyed.
	 * Here comes another task switch and although finish() waits on signal
	 * that is send after the run() call, the derived class is already destroyed
	 * and the run below is "pure virtual" with no underneath object.
	 *
	 * That is way we have to deliver interface that allows users to not specify
	 * any useful or meaningful int run();
	 */
	l_pvReturn = reinterpret_cast<void*>( run() );
	f_eStatus = D_ZOMBIE;
	f_oSemaphore.signal();
	return ( l_pvReturn );
	M_EPILOG
	}

bool HThread::is_alive( void ) const
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	return ( f_eStatus == D_ALIVE );
	M_EPILOG
	}

int HThread::run( void )
	{
	M_PROLOG
	/*
	 * Just avoid crash on two subsequent spawn() calls.
	 */
	log << "Pure virtual call - ignored." << endl;
	return( -1 );
	M_EPILOG
	}

HMutex::HMutex( TYPE::mutex_type_t const a_eType ) : f_eType ( a_eType ),
	f_oAttributes( xcalloc<pthread_mutexattr_t>( 1 ) ),
	f_oMutex( xcalloc<pthread_mutex_t>( 1 ) )
	{
	M_PROLOG
	if ( f_eType == TYPE::D_DEFAULT )
		f_eType = TYPE::D_NON_RECURSIVE;
	pthread_mutexattr_t* attr = static_cast<pthread_mutexattr_t*>( f_oAttributes.get() );
	::pthread_mutexattr_init( attr );
	M_ENSURE( ::pthread_mutexattr_settype( attr,
				f_eType & TYPE::D_RECURSIVE ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK ) != EINVAL );
	::pthread_mutex_init( static_cast<pthread_mutex_t*>( f_oMutex.get() ), attr );
	return;
	M_EPILOG
	}

HMutex::~HMutex( void )
	{
	M_PROLOG
	int l_iError = 0;
	while ( ( l_iError = ::pthread_mutex_destroy( static_cast<pthread_mutex_t*>( f_oMutex.get() ) ) ) == EBUSY )
		;
	M_ENSURE( l_iError == 0 );
	::pthread_mutexattr_destroy( static_cast<pthread_mutexattr_t*>( f_oAttributes.get() ) );
	return;
	M_EPILOG
	}

void HMutex::lock( void )
	{
	M_PROLOG
	int l_iError = ::pthread_mutex_lock( static_cast<pthread_mutex_t*>( f_oMutex.get() ) );
	if ( ! ( f_eType & TYPE::D_RECURSIVE ) )
		M_ENSURE( l_iError != EDEADLK );
	return;
	M_EPILOG
	}

void HMutex::unlock( void )
	{
	M_PROLOG
	int l_iError = ::pthread_mutex_unlock( static_cast<pthread_mutex_t*>( f_oMutex.get() ) );
	if ( ! ( f_eType & TYPE::D_RECURSIVE ) )
		M_ENSURE( l_iError != EPERM );
	return;
	M_EPILOG
	}

HLock::HLock( HMutex& a_roMutex ) : f_roMutex( a_roMutex )
	{
	M_PROLOG
	f_roMutex.lock();
	return;
	M_EPILOG
	}

HLock::~HLock( void )
	{
	M_PROLOG
	f_roMutex.unlock();
	return;
	M_EPILOG
	}

HSemaphore::HSemaphore( void )
	: f_oSemaphore( xcalloc<sem_t>( 1 ) )
	{
	M_PROLOG
	M_ENSURE( ::sem_init( static_cast<sem_t*>( f_oSemaphore.get() ), 0, 0 ) == 0 );
	return;
	M_EPILOG
	}

HSemaphore::~HSemaphore( void )
	{
	M_PROLOG
	M_ENSURE( ::sem_destroy( static_cast<sem_t*>( f_oSemaphore.get() ) ) == 0 );
	return;
	M_EPILOG
	}

void HSemaphore::wait( void )
	{
	M_PROLOG
	::sem_wait( static_cast<sem_t*>( f_oSemaphore.get() ) ); /* Always returns 0. */
	return;
	M_EPILOG
	}

void HSemaphore::signal( void )
	{
	M_PROLOG
	M_ENSURE( ::sem_post( static_cast<sem_t*>( f_oSemaphore.get() ) ) == 0 );
	return;
	M_EPILOG
	}

HCondition::HCondition( HMutex& a_roMutex )
	: f_oAttributes( xcalloc<pthread_condattr_t>( 1 ) ), f_oCondition( xcalloc<pthread_cond_t>( 1 ) ), f_roMutex( a_roMutex )
	{
	M_PROLOG
	pthread_condattr_t* attr = static_cast<pthread_condattr_t*>( f_oAttributes.get() );
	::pthread_condattr_init( attr );
	::pthread_cond_init( static_cast<pthread_cond_t*>( f_oCondition.get() ), attr );
	return;
	M_EPILOG
	}

HCondition::~HCondition( void )
	{
	M_PROLOG
	M_ENSURE( ::pthread_cond_destroy( static_cast<pthread_cond_t*>( f_oCondition.get() ) ) == 0 );
	::pthread_condattr_destroy( static_cast<pthread_condattr_t*>( f_oAttributes.get() ) );
	return;
	M_EPILOG
	}

/*
 * Read it or die!
 *
 * When ::pthread_cond_wait() is invoked, calling thread releases mutex
 * and goes to sleep, then scheduler (when she sees it as apriopriate)
 * switches tasks. In the second task ::pthread_cond_signal() is called ...
 * Here comes important part: scheduler does not neceserily switches task
 * right back, it means that more than one call to ::pthread_cond_signal()
 * may occure in sequence, and all signals, that are send before scheduler
 * switches back to task one, when ::pthread_cond_wait() may lock again,
 * are lost!
 */

HCondition::status_t HCondition::wait( int long unsigned const& a_ulTimeOutSeconds,
		int long unsigned const& a_ulTimeOutNanoSeconds )
	{
	M_PROLOG
	int l_iError = 0;
	timespec l_sTimeOut;
	clock_gettime( CLOCK_REALTIME, &l_sTimeOut );
	static int long const D_NANO_IN_WHOLE = power<10, 9>::value;
	l_sTimeOut.tv_sec += a_ulTimeOutSeconds;
	l_sTimeOut.tv_nsec += a_ulTimeOutNanoSeconds;
	if ( l_sTimeOut.tv_nsec >= D_NANO_IN_WHOLE )
		{
		++ l_sTimeOut.tv_sec;
		l_sTimeOut.tv_nsec -= D_NANO_IN_WHOLE;
		}
	l_iError = ::pthread_cond_timedwait( static_cast<pthread_cond_t*>( f_oCondition.get() ),
				static_cast<pthread_mutex_t*>( f_roMutex.f_oMutex.get() ), &l_sTimeOut );
	M_ENSURE ( ( l_iError == 0 ) || ( l_iError == EINTR ) || ( l_iError == ETIMEDOUT ) );
	return ( ( l_iError == 0 ) ? D_OK : ( ( l_iError == EINTR ) ? D_INTERRUPT : D_TIMEOUT ) );
	M_EPILOG
	}

void HCondition::signal( void )
	{
	M_PROLOG
	HLock l_oLock( f_roMutex );
	::pthread_cond_signal( static_cast<pthread_cond_t*>( f_oCondition.get() ) );
	return;
	M_EPILOG
	}

}

}

