/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hthread.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <ctime> /* clock_gettime */
#include <cstring>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <libintl.h>

#include "config.hxx"
#if defined( HAVE_PTHREAD_SET_NAME_NP )
#include <pthread_np.h>
#elif ! defined( HAVE_PTHREAD_SETNAME_NP ) && defined( HAVE_PRCTL ) /* #if defined( HAVE_PTHREAD_SET_NAME_NP ) */
#include <sys/prctl.h>
#endif /* #elif ! defined( HAVE_PTHREAD_SETNAME_NP ) && defined( HAVE_PRCTL ) #if defined( HAVE_PTHREAD_SET_NAME_NP ) */

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hthread.hxx"
#include "hcore/memory.hxx"
#include "hlog.hxx"

namespace yaal {

namespace hcore {

HSemaphore::TYPE::type_t HSemaphore::DEFAULT = HSemaphore::TYPE::POSIX;
int HThread::_threadStackSize = 0;

namespace {

void do_pthread_attr_destroy( void* attr ) {
	M_PROLOG
	M_ENSURE( ::pthread_attr_destroy( static_cast<pthread_attr_t*>( attr ) ) == 0 );
	return;
	M_EPILOG
}

}


HThread::HThread( void )
	: _status( DEAD ), _buf( chunk_size<pthread_t>( 1 ) + chunk_size<pthread_attr_t>( 1 ) ),
	_mutex( HMutex::TYPE::RECURSIVE ), _semaphore(), _resGuard(), _call(), _exceptionInfo() {
	M_PROLOG
	pthread_attr_t* attr( static_cast<pthread_attr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_t ) ) ) );
	M_ENSURE( ::pthread_attr_init( attr ) == 0 );
	size_t stackSize( 0 );
	M_ENSURE( pthread_attr_getstacksize( attr, &stackSize ) == 0 );
	log( LOG_TYPE::DEBUG ) << "Default thread stack size: " << stackSize << " and we will use: " << ( _threadStackSize ? _threadStackSize : stackSize ) << endl;
	if ( _threadStackSize > 0 )
		M_ENSURE( pthread_attr_setstacksize( attr, _threadStackSize ) == 0 );
	HResource<void> res( attr, do_pthread_attr_destroy );
	_resGuard.swap( res );
	M_ENSURE( ::pthread_attr_setdetachstate( attr, PTHREAD_CREATE_JOINABLE ) == 0 );
	M_ENSURE( ::pthread_attr_setinheritsched( attr, PTHREAD_INHERIT_SCHED ) == 0 );
	return;
	M_EPILOG
}

HThread::~HThread( void ) {
	M_PROLOG
	HLock lock( _mutex );
	if ( _status != DEAD ) {
		_mutex.unlock();
		finish();
		_mutex.lock();
	}
	M_ASSERT( _status == DEAD );
	return;
	M_DESTRUCTOR_EPILOG
}

void HThread::spawn( call_t call_ ) {
	M_PROLOG
	HLock lock( _mutex );
	if ( _status != DEAD )
		M_THROW( _( "thread is already running or spawning" ), _status );
	_status = SPAWNING;
	_call = call_;
	M_ENSURE( ::pthread_create( _buf.get<pthread_t>(),
				static_cast<pthread_attr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_t ) ) ),
				SPAWN, this ) == 0 );
	_semaphore.wait();
	return;
	M_EPILOG
}

void HThread::schedule_finish( void ) {
	M_PROLOG
	HLock lock( _mutex );
	if ( _status != DEAD )
		_status = ZOMBIE;
	return;
	M_EPILOG
}

void* HThread::finish( void ) {
	M_PROLOG
	HLock lock( _mutex );
	if ( ( _status != ALIVE ) && ( _status != ZOMBIE ) && ( _status != SPAWNING ) )
		M_THROW( _( "thread is not running" ), _status );
	schedule_finish();
	_mutex.unlock();
	_semaphore.wait();
	_mutex.lock();
	void* returnValue( NULL );
	M_ENSURE( ::pthread_join( *_buf.get<pthread_t>(), &returnValue ) == 0 );
	_status = DEAD;
	if ( _exceptionInfo._stacked )
		throw HThreadException( _exceptionInfo._message, _exceptionInfo._code );
	return ( returnValue );
	M_EPILOG
}

void* HThread::SPAWN( void* thread_ ) {
	M_PROLOG
	/*!
	 * We cannot afford silencing uncaught exceptions here.
	 * Because of not catching an exception at the thread owner layer
	 * is a severe coding bug, application must crash in the event
	 * of uncaught exception.
	 *
	 * Other possibility would be to use kill_interior() here,
	 * but as opposite of `exception in destructor' case where
	 * process possibly would not be terminated, an uncaught exception
	 * in thread would always cause process termination.
	 *
	 * So again:
	 *
	 * In case of `exception in destructor' without library intervention
	 * thery is a chance to continue normal execution, so we finish gracefuly
	 * via kill_interior().
	 *
	 * In case of `exception in thread' there is no chance to continue
	 * normal execution without library intervention,
	 * so we die via apropriate fatal signal.
	 */
	try {
		return ( reinterpret_cast<HThread*>( thread_ )->control() );
	} catch ( HException const& e ) {
		log( LOG_TYPE::ERROR ) << "Uncaught exception: `" << e.what() << "' in thread!" << endl;
		throw;
	} catch ( ... ) {
		log( LOG_TYPE::ERROR ) << "Unknown uncaught exception in thread!" << endl;
		throw;
	}
	M_EPILOG
}

void HThread::CLEANUP( void* ) {
	M_PROLOG
	return;
	M_EPILOG
}

/*
 * pthread_cleanup_push is a macro on Solaris,
 * and this macro uses old-style-casts.
 */
#ifdef __HOST_OS_TYPE_SOLARIS__
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif /* #ifdef __HOST_OS_TYPE_SOLARIS__ */
void* HThread::control( void ) {
	M_PROLOG
	M_ENSURE( ::pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL ) == 0 );
	M_ENSURE( ::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL ) == 0 );
	/* pthread_cleanup_push and pthread_cleanup_pop are macros that need
	 * to be treated as { and } brackets which means that variables
	 * declared between pthread_cleanup_push and pthread_cleanup_pop
	 * are not visible after pthread_cleanup_pop.
	 */
	void* returnValue( NULL );
	pthread_cleanup_push( CLEANUP, this );
	_status = ALIVE;
	_semaphore.signal();
	returnValue = _call();
	pthread_cleanup_pop( 0 );
	_status = ZOMBIE;
	_semaphore.signal();
	return ( returnValue );
	M_EPILOG
}
#ifdef __HOST_OS_TYPE_SOLARIS__
#pragma GCC diagnostic error "-Wold-style-cast"
#endif /* #ifdef __HOST_OS_TYPE_SOLARIS__ */

bool HThread::is_alive( void ) const {
	M_PROLOG
	HLock lock( _mutex );
	return ( _status == ALIVE );
	M_EPILOG
}

int long HThread::get_id( void ) {
	return ( reinterpret_cast<int long>( reinterpret_cast<void*>( pthread_self() ) ) );
}

void HThread::set_name( char const* name_ ) {
	char name[16];
	strncpy( name, name_, 16 );
	name[15] = 0;
#if defined( HAVE_PTHREAD_SETNAME_NP )
	::pthread_setname_np( ::pthread_self(), name );
#elif defined( HAVE_PTHREAD_SET_NAME_NP ) /* #if defined( HAVE_PTHREAD_SETNAME_NP ) */
	::pthread_set_name_np( ::pthread_self(), name );
#elif defined( HAVE_PRCTL ) /* #elif defined( HAVE_PTHREAD_SET_NAME_NP ) #if defined( HAVE_PTHREAD_SETNAME_NP ) */
	::prctl( PR_SET_NAME, name, 0, 0, 0 );
#else /* #elif defined( HAVE_PRCTL ) #elif defined( HAVE_PTHREAD_SET_NAME_NP ) #if defined( HAVE_PTHREAD_SETNAME_NP ) */
	log( LOG_TYPE::WARNING ) << "Setting thread name (`" << name_ << "') not supported on your platform." << endl;
#endif /* #else #elif defined( HAVE_PRCTL ) #elif defined( HAVE_PTHREAD_SET_NAME_NP ) #if defined( HAVE_PTHREAD_SETNAME_NP ) */
	return;
}

void HThread::stack_exception( yaal::hcore::HString const& message_, int code_ ) {
	M_PROLOG
	_exceptionInfo._stacked = true;
	_exceptionInfo._code = code_;
	_exceptionInfo._message = message_;
	return;
	M_EPILOG
}

namespace {

void do_pthread_mutexattr_destroy( void* attr ) {
	::pthread_mutexattr_destroy( static_cast<pthread_mutexattr_t*>( attr ) );
}

}

HMutex::HMutex( TYPE::mutex_type_t const type_ ) : _type( type_ ),
	_buf( chunk_size<pthread_mutex_t>( 1 ) + chunk_size<pthread_mutexattr_t>( 1 ) ),
	_resGuard(), _owner( HThread::INVALID ) {
	M_PROLOG
	if ( _type == TYPE::DEFAULT )
		_type = TYPE::NON_RECURSIVE;
	pthread_mutexattr_t* attr( static_cast<pthread_mutexattr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_mutex_t ) ) ) );
	::pthread_mutexattr_init( attr );
	HResource<void> res( attr, do_pthread_mutexattr_destroy );
	_resGuard.swap( res );
	M_ENSURE( ::pthread_mutexattr_settype( attr,
				_type & TYPE::RECURSIVE ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK ) != EINVAL );
	::pthread_mutex_init( _buf.get<pthread_mutex_t>(), attr );
	return;
	M_EPILOG
}

HMutex::~HMutex( void ) {
	M_PROLOG
	int error = 0;
	while ( ( error = ::pthread_mutex_destroy( _buf.get<pthread_mutex_t>() ) ) == EBUSY )
		;
	M_ENSURE( error == 0 );
	return;
	M_DESTRUCTOR_EPILOG
}

void HMutex::lock( void ) {
	M_PROLOG
	int error = ::pthread_mutex_lock( _buf.get<pthread_mutex_t>() );
	if ( ! ( _type & TYPE::RECURSIVE ) )
		M_ENSURE( error != EDEADLK );
	_owner = HThread::get_id();
	return;
	M_EPILOG
}

void HMutex::unlock( void ) {
	M_PROLOG
	_owner = HThread::INVALID;
	int error = ::pthread_mutex_unlock( _buf.get<pthread_mutex_t>() );
	if ( ! ( _type & TYPE::RECURSIVE ) )
		M_ENSURE( error != EPERM );
	return;
	M_EPILOG
}

bool HMutex::is_owned( void ) const {
	M_PROLOG
	return ( _owner == HThread::get_id() );
	M_EPILOG
}

void HMutex::reown( void ) {
	M_PROLOG
	_owner = HThread::get_id();
	return;
	M_EPILOG
}

HLock::HLock( HMutex& mutex_ ) : _mutex( mutex_ ) {
	M_PROLOG
	_mutex.lock();
	return;
	M_EPILOG
}

HLock::~HLock( void ) {
	M_PROLOG
	_mutex.unlock();
	return;
	M_DESTRUCTOR_EPILOG
}

class HPosixSemaphore : public HSemaphoreImplementationInterface {
public:
	typedef HPosixSemaphore this_type;
	typedef HSemaphoreImplementationInterface base_type;
private:
	sem_t _sem;
public:
	HPosixSemaphore( void );
	virtual ~HPosixSemaphore( void );
private:
	virtual void do_wait( void );
	virtual void do_signal( void );
	HPosixSemaphore( HPosixSemaphore const& );
	HPosixSemaphore& operator = ( HPosixSemaphore const& );
};

HPosixSemaphore::HPosixSemaphore( void )
	: _sem() {
	M_PROLOG
	M_ENSURE( ::sem_init( &_sem, 0, 0 ) == 0 );
	return;
	M_EPILOG
}

HPosixSemaphore::~HPosixSemaphore( void ) {
	M_PROLOG
	M_ENSURE( ::sem_destroy( &_sem ) == 0 );
	return;
	M_DESTRUCTOR_EPILOG
}

void HPosixSemaphore::do_wait( void ) {
	M_PROLOG
	::sem_wait( &_sem ); /* Always returns 0. */
	return;
	M_EPILOG
}

void HPosixSemaphore::do_signal( void ) {
	M_PROLOG
	M_ENSURE( ::sem_post( &_sem ) == 0 );
	return;
	M_EPILOG
}

void HSemaphoreImplementationInterface::wait( void ) {
	M_PROLOG
	do_wait();
	return;
	M_EPILOG
}

void HSemaphoreImplementationInterface::signal( void ) {
	M_PROLOG
	do_signal();
	return;
	M_EPILOG
}

class HYaalSemaphore : public HSemaphoreImplementationInterface {
public:
	typedef HYaalSemaphore this_type;
	typedef HSemaphoreImplementationInterface base_type;
private:
	HMutex _mutex;
	HCondition _cond;
	int long _count;
public:
	HYaalSemaphore( void );
	virtual ~HYaalSemaphore( void );
private:
	virtual void do_wait( void );
	virtual void do_signal( void );
	HYaalSemaphore( HYaalSemaphore const& );
	HYaalSemaphore& operator = ( HYaalSemaphore const& );
};

HYaalSemaphore::HYaalSemaphore( void )
	: _mutex(), _cond( _mutex ), _count( 0 ) {
	return;
}

HYaalSemaphore::~HYaalSemaphore( void ) {
	return;
}

void HYaalSemaphore::do_wait( void ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _count > 0 )
		-- _count;
	else {
		do {
			_cond.wait( 0x1fffffff, 0 );
		} while ( ! _count );
		-- _count;
	}
	return;
	M_EPILOG
}

void HYaalSemaphore::do_signal( void ) {
	M_PROLOG
	HLock l( _mutex );
	++ _count;
	_cond.signal();
	return;
	M_EPILOG
}

HSemaphore::HSemaphore( TYPE::type_t type_ )
	: _impl( type_ == TYPE::POSIX ? semaphore_implementation_t( new ( memory::yaal ) HPosixSemaphore() ) : semaphore_implementation_t( new ( memory::yaal ) HYaalSemaphore() ) ) {
}

HSemaphore::~HSemaphore( void ) {
	return;
}

void HSemaphore::wait( void ) {
	M_PROLOG
	_impl->wait();
	return;
	M_EPILOG
}

void HSemaphore::signal( void ) {
	M_PROLOG
	_impl->signal();
	return;
	M_EPILOG
}

HCondition::HCondition( HMutex& mutex_ )
	: _buf( chunk_size<pthread_cond_t>( 1 ) + chunk_size<pthread_condattr_t>( 1 ) ), _mutex( mutex_ ) {
	M_PROLOG
	pthread_condattr_t* attr( static_cast<pthread_condattr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_cond_t ) ) ) );
	::pthread_condattr_init( attr );
	M_ENSURE( ::pthread_cond_init( _buf.get<pthread_cond_t>(), attr ) == 0 );
	return;
	M_EPILOG
}

HCondition::~HCondition( void ) {
	M_PROLOG
	M_ENSURE( ::pthread_cond_destroy( _buf.get<pthread_cond_t>() ) == 0 );
	::pthread_condattr_destroy( static_cast<pthread_condattr_t*>( static_cast<void*>( _buf.raw() + sizeof ( pthread_cond_t ) ) ));
	return;
	M_DESTRUCTOR_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int const FWD_CLOCK_REALTIME = CLOCK_REALTIME;
#pragma GCC diagnostic error "-Wold-style-cast"
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

HCondition::status_t HCondition::wait( int long unsigned timeOutSeconds_,
		int long unsigned timeOutNanoSeconds_ ) {
	M_PROLOG
	M_ASSERT( _mutex.is_owned() );
	int error = 0;
	timespec timeOut;
	::memset( &timeOut, 0, sizeof ( timespec ) );
	M_ENSURE( clock_gettime( FWD_CLOCK_REALTIME, &timeOut ) == 0 );
	static int long const NANO_IN_WHOLE = meta::power<10, 9>::value;
	timeOut.tv_sec += timeOutSeconds_;
	timeOut.tv_nsec += timeOutNanoSeconds_;
	if ( timeOut.tv_nsec >= NANO_IN_WHOLE ) {
		++ timeOut.tv_sec;
		timeOut.tv_nsec -= NANO_IN_WHOLE;
	}
	error = ::pthread_cond_timedwait( _buf.get<pthread_cond_t>(),
				_mutex._buf.get<pthread_mutex_t>(), &timeOut );
	/* We need to make sure that mutex owner is reset to proper value, as HCondition::signal() invoker undoubtfuly modified it. */
	_mutex.reown();
	/* Error code is not stored in errno but is explicitly returned. */
	HScopedValueReplacement<int> saveErrno( errno, error );
	M_ENSURE( ( error == 0 ) || ( error == EINTR ) || ( error == ETIMEDOUT ) );
	return ( ( error == 0 ) ? OK : ( ( error == EINTR ) ? INTERRUPT : TIMEOUT ) );
	M_EPILOG
}

void HCondition::signal( void ) {
	M_PROLOG
	M_ASSERT( _mutex.is_owned() );
	M_ENSURE( ::pthread_cond_signal( _buf.get<pthread_cond_t>() ) == 0 );
	return;
	M_EPILOG
}

void HCondition::broadcast( void ) {
	M_PROLOG
	M_ASSERT( _mutex.is_owned() );
	M_ENSURE( ::pthread_cond_broadcast( _buf.get<pthread_cond_t>() ) == 0 );
	return;
	M_EPILOG
}

HEvent::HEvent( void )
	: _mutex(), _condition( _mutex ) {
	M_PROLOG
	_mutex.lock();
	return;
	M_EPILOG
}

HEvent::~HEvent( void ) {
	M_PROLOG
	_mutex.unlock();
	return;
	M_DESTRUCTOR_EPILOG
}

void HEvent::wait( void ) {
	M_PROLOG
	M_ASSERT( _mutex.is_owned() );
	_condition.wait( 0x1fffffff, 0 ); /* FreeBSD strange limit. */
	return;
	M_EPILOG
}

void HEvent::signal( void ) {
	M_PROLOG
	HLock l( _mutex );
	_condition.signal();
	return;
	M_EPILOG
}

}

}

