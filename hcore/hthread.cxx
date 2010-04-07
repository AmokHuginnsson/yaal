/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hthread.cxx - this file is integral part of `yaal' project.

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

#include <pthread.h>
#include <semaphore.h>
#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hthread.hxx"
#include "hlog.hxx"
#include "xalloc.hxx"

namespace yaal
{

namespace hcore
{

void do_pthread_attr_destroy( void* attr )
	{
	M_PROLOG
	M_ENSURE( ::pthread_attr_destroy( static_cast<pthread_attr_t*>( attr ) ) == 0 );
	return;
	M_EPILOG
	}

HThread::HThread( void )
	: f_eStatus( DEAD ), _buf( chunk_size<pthread_t>( 1 ) + chunk_size<pthread_attr_t>( 1 ) ),
	f_oMutex( HMutex::TYPE::RECURSIVE ), f_oSemaphore(), _resGuard()
	{
	M_PROLOG
	pthread_attr_t* attr( static_cast<pthread_attr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_t ) ) ) );
	M_ENSURE( ::pthread_attr_init( attr ) == 0 );
	HResource<void> res( attr, do_pthread_attr_destroy );
	_resGuard.swap( res );
	M_ENSURE( ::pthread_attr_setdetachstate( attr, PTHREAD_CREATE_JOINABLE ) == 0 );
	M_ENSURE( ::pthread_attr_setinheritsched( attr, PTHREAD_INHERIT_SCHED ) == 0 );
	return;
	M_EPILOG
	}

HThread::~HThread( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( f_eStatus != DEAD )
		{
		f_oMutex.unlock();
		finish();
		f_oMutex.lock();
		}
	M_ASSERT( f_eStatus == DEAD );
	return;
	M_EPILOG
	}

int HThread::spawn( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( f_eStatus != DEAD )
		M_THROW( _( "thread is already running or spawning" ), f_eStatus );
	f_eStatus = SPAWNING;
	M_ENSURE( ::pthread_create( _buf.get<pthread_t>(),
				static_cast<pthread_attr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_t ) ) ),
				SPAWN, this ) == 0 );
	f_oSemaphore.wait();
	return ( 0 );
	M_EPILOG
	}

void HThread::schedule_finish( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( f_eStatus != DEAD )
		f_eStatus = ZOMBIE;
	return;
	M_EPILOG
	}

int long HThread::finish( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( ( f_eStatus != ALIVE ) && ( f_eStatus != ZOMBIE ) && ( f_eStatus != SPAWNING ) )
		M_THROW( _( "thread is not running" ), f_eStatus );
	schedule_finish();
	f_oMutex.unlock();
	f_oSemaphore.wait();
	f_oMutex.lock();
	void* l_pvReturn = NULL;
	M_ENSURE( ::pthread_join( *_buf.get<pthread_t>(), &l_pvReturn ) == 0 );
	f_eStatus = DEAD;
	return ( reinterpret_cast<int long>( l_pvReturn ) );
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

void HThread::CLEANUP( void* a_pvThread )
	{
	M_PROLOG
	reinterpret_cast<HThread*>( a_pvThread )->do_cleanup();
	return;
	M_EPILOG
	}

void* HThread::control( void )
	{
	M_PROLOG
	M_ENSURE( ::pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL ) == 0 );
	M_ENSURE( ::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL ) == 0 );
	/* pthread_cleanup_push and pthread_cleanup_pop are macros that need
	 * to be treated as { and } brackets which means that variables
	 * declared between pthread_cleanup_push and pthread_cleanup_pop
	 * are not visible after pthread_cleanup_pop.
	 */
	void* l_pvReturn( NULL );
	pthread_cleanup_push( CLEANUP, this );
	f_eStatus = ALIVE;
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
	 * That is why we have to deliver interface that allows users to not specify
	 * any useful or meaningful int run();
	 */
	l_pvReturn = reinterpret_cast<void*>( do_run() );
	pthread_cleanup_pop( 0 );
	f_eStatus = ZOMBIE;
	f_oSemaphore.signal();
	return ( l_pvReturn );
	M_EPILOG
	}

bool HThread::is_alive( void ) const
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	return ( f_eStatus == ALIVE );
	M_EPILOG
	}

int HThread::do_run( void )
	{
	M_PROLOG
	/*
	 * Just avoid crash on two subsequent spawn() calls.
	 */
	log << "Pure virtual call - ignored." << endl;
	return( -1 );
	M_EPILOG
	}

void HThread::do_cleanup( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int long HThread::get_id( void )
	{
	return ( reinterpret_cast<int long>( reinterpret_cast<void*>( pthread_self() ) ) );
	}

void do_pthread_mutexattr_destroy( void* attr )
	{
	::pthread_mutexattr_destroy( static_cast<pthread_mutexattr_t*>( attr ) );
	}

HMutex::HMutex( TYPE::mutex_type_t const a_eType ) : f_eType( a_eType ),
	_buf( chunk_size<pthread_mutex_t>( 1 ) + chunk_size<pthread_mutexattr_t>( 1 ) ),
	_resGuard()
	{
	M_PROLOG
	if ( f_eType == TYPE::DEFAULT )
		f_eType = TYPE::NON_RECURSIVE;
	pthread_mutexattr_t* attr( static_cast<pthread_mutexattr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_mutex_t ) ) ) );
	::pthread_mutexattr_init( attr );
	HResource<void> res( attr, do_pthread_mutexattr_destroy );
	_resGuard.swap( res );
	M_ENSURE( ::pthread_mutexattr_settype( attr,
				f_eType & TYPE::RECURSIVE ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK ) != EINVAL );
	::pthread_mutex_init( _buf.get<pthread_mutex_t>(), attr );
	return;
	M_EPILOG
	}

HMutex::~HMutex( void )
	{
	M_PROLOG
	int l_iError = 0;
	while ( ( l_iError = ::pthread_mutex_destroy( _buf.get<pthread_mutex_t>() ) ) == EBUSY )
		;
	M_ENSURE( l_iError == 0 );
	return;
	M_EPILOG
	}

void HMutex::lock( void )
	{
	M_PROLOG
	int l_iError = ::pthread_mutex_lock( _buf.get<pthread_mutex_t>() );
	if ( ! ( f_eType & TYPE::RECURSIVE ) )
		M_ENSURE( l_iError != EDEADLK );
	return;
	M_EPILOG
	}

void HMutex::unlock( void )
	{
	M_PROLOG
	int l_iError = ::pthread_mutex_unlock( _buf.get<pthread_mutex_t>() );
	if ( ! ( f_eType & TYPE::RECURSIVE ) )
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
	: f_oSemaphore( chunk_size<sem_t>( 1 ) )
	{
	M_PROLOG
	M_ENSURE( ::sem_init( f_oSemaphore.get<sem_t>(), 0, 0 ) == 0 );
	return;
	M_EPILOG
	}

HSemaphore::~HSemaphore( void )
	{
	M_PROLOG
	M_ENSURE( ::sem_destroy( f_oSemaphore.get<sem_t>() ) == 0 );
	return;
	M_EPILOG
	}

void HSemaphore::wait( void )
	{
	M_PROLOG
	::sem_wait( f_oSemaphore.get<sem_t>() ); /* Always returns 0. */
	return;
	M_EPILOG
	}

void HSemaphore::signal( void )
	{
	M_PROLOG
	M_ENSURE( ::sem_post( f_oSemaphore.get<sem_t>() ) == 0 );
	return;
	M_EPILOG
	}

HCondition::HCondition( HMutex& a_roMutex )
	: _buf( chunk_size<pthread_cond_t>( 1 ) + chunk_size<pthread_condattr_t>( 1 ) ), f_roMutex( a_roMutex )
	{
	M_PROLOG
	pthread_condattr_t* attr( static_cast<pthread_condattr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_cond_t ) ) ) );
	::pthread_condattr_init( attr );
	::pthread_cond_init( _buf.get<pthread_cond_t>(), attr );
	return;
	M_EPILOG
	}

HCondition::~HCondition( void )
	{
	M_PROLOG
	M_ENSURE( ::pthread_cond_destroy( _buf.get<pthread_cond_t>() ) == 0 );
	::pthread_condattr_destroy( static_cast<pthread_condattr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_cond_t ) ) ));
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
	static int long const NANO_IN_WHOLE = meta::power<10, 9>::value;
	l_sTimeOut.tv_sec += a_ulTimeOutSeconds;
	l_sTimeOut.tv_nsec += a_ulTimeOutNanoSeconds;
	if ( l_sTimeOut.tv_nsec >= NANO_IN_WHOLE )
		{
		++ l_sTimeOut.tv_sec;
		l_sTimeOut.tv_nsec -= NANO_IN_WHOLE;
		}
	l_iError = ::pthread_cond_timedwait( _buf.get<pthread_cond_t>(),
				f_roMutex._buf.get<pthread_mutex_t>(), &l_sTimeOut );
	M_ENSURE ( ( l_iError == 0 ) || ( l_iError == EINTR ) || ( l_iError == ETIMEDOUT ) );
	return ( ( l_iError == 0 ) ? OK : ( ( l_iError == EINTR ) ? INTERRUPT : TIMEOUT ) );
	M_EPILOG
	}

void HCondition::signal( void )
	{
	M_PROLOG
	HLock l_oLock( f_roMutex );
	::pthread_cond_signal( _buf.get<pthread_cond_t>() );
	return;
	M_EPILOG
	}

}

}

