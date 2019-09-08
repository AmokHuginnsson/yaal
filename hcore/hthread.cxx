/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <ctime> /* clock_gettime */
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <libintl.h>

#include "config.hxx"
#if defined( HAVE_PTHREAD_SET_NAME_NP )
#include <pthread_np.h>
#elif ! defined( HAVE_PTHREAD_SETNAME_NP ) && defined( HAVE_PRCTL ) /* #if defined( HAVE_PTHREAD_SET_NAME_NP ) */
#include <sys/prctl.h>
#endif /* #elif ! defined( HAVE_PTHREAD_SETNAME_NP ) && defined( HAVE_PRCTL ) #if defined( HAVE_PTHREAD_SET_NAME_NP ) */

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hthread.hxx"
#include "memory.hxx"
#include "si.hxx"
#include "hlog.hxx"

namespace yaal {

namespace hcore {

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
	: _status( DEAD )
	, _buf( chunk_size<pthread_t>( 1 ) + chunk_size<pthread_attr_t>( 1 ) )
	, _mutex( HMutex::TYPE::RECURSIVE )
	, _semaphore()
	, _resGuard()
	, _call()
	, _id( INVALID )
	, _exceptionInfo() {
	M_PROLOG
	pthread_attr_t* attr( static_cast<pthread_attr_t*>( static_cast<void*>( _buf.get<char>() + sizeof ( pthread_t ) ) ) );
	M_ENSURE( ::pthread_attr_init( attr ) == 0 );
	size_t stackSize( 0 );
	M_ENSURE( pthread_attr_getstacksize( attr, &stackSize ) == 0 );
	log( LOG_LEVEL::DEBUG ) << "Default thread stack size: " << stackSize
		<< " and we will use: " << ( _threadStackSize ? _threadStackSize : static_cast<int>( stackSize ) ) << endl;
	if ( _threadStackSize > 0 ) {
		M_ENSURE( pthread_attr_setstacksize( attr, static_cast<size_t>( _threadStackSize ) ) == 0 );
	}
	HResource<void, void (*)( void* )> res( attr, do_pthread_attr_destroy );
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
		lock.unlock();
		finish();
		lock.lock();
	}
	M_ASSERT( _status == DEAD );
	return;
	M_DESTRUCTOR_EPILOG
}

void HThread::spawn( call_t call_ ) {
	M_PROLOG
	HLock lock( _mutex );
	if ( _status != DEAD ) {
		M_THROW( _( "thread is already running or spawning" ), _status );
	}
	_status = SPAWNING;
	_call = call_;
	M_ENSURE( ::pthread_create( _buf.get<pthread_t>(),
				static_cast<pthread_attr_t*>( static_cast<void*>( _buf.get<char>() + sizeof ( pthread_t ) ) ),
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

void HThread::finish( void ) {
	M_PROLOG
	HLock lock( _mutex );
	if ( ( _status != ALIVE ) && ( _status != ZOMBIE ) && ( _status != SPAWNING ) ) {
		M_THROW( _( "thread is not running" ), _status );
	}
	schedule_finish();
	lock.unlock();
	_semaphore.wait();
	lock.lock();
	void* returnValue( nullptr );
	M_ENSURE( ::pthread_join( *_buf.get<pthread_t>(), &returnValue ) == 0 );
	_status = DEAD;
	if ( _exceptionInfo._stacked ) {
		throw HThreadException( _exceptionInfo._message, _exceptionInfo._code );
	}
	return;
	M_EPILOG
}

void HThread::join( void ) {
	M_PROLOG
	finish();
	return;
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
	 * there is a chance to continue normal execution, so we finish gracefully
	 * via kill_interior().
	 *
	 * In case of `exception in thread' there is no chance to continue
	 * normal execution without library intervention,
	 * so we die via appropriate fatal signal.
	 */
	try {
		reinterpret_cast<HThread*>( thread_ )->control();
	} catch ( HException const& e ) {
		log( LOG_LEVEL::ERROR ) << "Uncaught exception: `" << e.what() << "' in thread!" << endl;
		throw;
	} catch ( ... ) {
		log( LOG_LEVEL::ERROR ) << "Unknown uncaught exception in thread!" << endl;
		throw;
	}
	return ( nullptr );
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif /* #ifdef __HOST_OS_TYPE_SOLARIS__ */
void HThread::control( void ) {
	M_PROLOG
	M_ENSURE( ::pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, nullptr ) == 0 );
	M_ENSURE( ::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, nullptr ) == 0 );
	/* pthread_cleanup_push and pthread_cleanup_pop are macros that need
	 * to be treated as { and } brackets which means that variables
	 * declared between pthread_cleanup_push and pthread_cleanup_pop
	 * are not visible after pthread_cleanup_pop.
	 */
	pthread_cleanup_push( CLEANUP, this );
	_status = ALIVE;
	_id = get_current_thread_id();
	_semaphore.signal();
	_call();
	pthread_cleanup_pop( 0 );
	_status = ZOMBIE;
	_id = INVALID;
	_semaphore.signal();
	return;
	M_EPILOG
}
#ifdef __HOST_OS_TYPE_SOLARIS__
#pragma GCC diagnostic pop
#endif /* #ifdef __HOST_OS_TYPE_SOLARIS__ */

bool HThread::is_alive( void ) const {
	M_PROLOG
	HLock lock( _mutex );
	return ( _status == ALIVE );
	M_EPILOG
}

HThread::id_t HThread::get_id( void ) const {
	return ( _id );
}

HThread::id_t HThread::get_current_thread_id( void ) {
	return ( reinterpret_cast<id_t>( reinterpret_cast<void*>( pthread_self() ) ) );
}

void HThread::set_name( char const* name_ ) {
	char name[16];
	strncpy( name, name_, 16 );
	name[15] = 0;
#if defined( HAVE_PTHREAD_SETNAME_NP )
#ifdef __HOST_OS_TYPE_DARWIN__
	::pthread_setname_np( name );
#else /* #ifdef __HOST_OS_TYPE_DARWIN__ */
	::pthread_setname_np( ::pthread_self(), name );
#endif /* #else #ifdef __HOST_OS_TYPE_DARWIN__ */
#elif defined( HAVE_PTHREAD_SET_NAME_NP ) /* #if defined( HAVE_PTHREAD_SETNAME_NP ) */
	::pthread_set_name_np( ::pthread_self(), name );
#elif defined( HAVE_PRCTL ) /* #elif defined( HAVE_PTHREAD_SET_NAME_NP ) #if defined( HAVE_PTHREAD_SETNAME_NP ) */
	::prctl( PR_SET_NAME, name, 0, 0, 0 );
#else /* #elif defined( HAVE_PRCTL ) #elif defined( HAVE_PTHREAD_SET_NAME_NP ) #if defined( HAVE_PTHREAD_SETNAME_NP ) */
	log( LOG_LEVEL::WARNING ) << "Setting thread name (`" << name_ << "') not supported on your platform." << endl;
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

HMutex::HMutex( TYPE type_ )
	: _type( type_ )
	, _buf( chunk_size<pthread_mutex_t>( 1 ) + chunk_size<pthread_mutexattr_t>( 1 ) )
	, _resGuard()
	, _owner( HThread::INVALID ) {
	M_PROLOG
	if ( _type == TYPE::DEFAULT ) {
		_type = TYPE::NON_RECURSIVE;
	}
	pthread_mutexattr_t* attr( static_cast<pthread_mutexattr_t*>( static_cast<void*>( _buf.get<char>() + sizeof ( pthread_mutex_t ) ) ) );
	::pthread_mutexattr_init( attr );
	HResource<void, void (*)( void* )> res( attr, do_pthread_mutexattr_destroy );
	_resGuard.swap( res );
	M_ENSURE( ::pthread_mutexattr_settype( attr,
				_type == TYPE::RECURSIVE ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK ) != EINVAL );
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
	if ( _type != TYPE::RECURSIVE ) {
		M_ENSURE( error != EDEADLK );
	}
	_owner = HThread::get_current_thread_id();
	return;
	M_EPILOG
}

void HMutex::unlock( void ) {
	M_PROLOG
	_owner = HThread::INVALID;
	M_ENSURE( ::pthread_mutex_unlock( _buf.get<pthread_mutex_t>() ) == 0 );
	return;
	M_EPILOG
}

bool HMutex::is_owned( void ) const {
	M_PROLOG
	return ( _owner == HThread::get_current_thread_id() );
	M_EPILOG
}

void HMutex::reown( void ) {
	M_PROLOG
	_owner = HThread::get_current_thread_id();
	return;
	M_EPILOG
}

HLock::HLock( HMutex& mutex_ )
	: _mutex( &mutex_ )
	, _locked( false ) {
	M_PROLOG
	_mutex->lock();
	_locked = true;
	return;
	M_EPILOG
}

HLock::HLock( HLock&& other_ ) noexcept
	: _mutex( other_._mutex )
	, _locked( other_._locked ) {
	other_._mutex = nullptr;
	other_._locked = false;
	return;
}

HLock::~HLock( void ) {
	M_PROLOG
	if ( _locked ) {
		_mutex->unlock();
		_locked = false;
	}
	return;
	M_DESTRUCTOR_EPILOG
}

HLock& HLock::operator = ( HLock&& other_ ) noexcept {
	if ( &other_ != this ) {
		_mutex = other_._mutex;
		_locked = other_._locked;
		other_._mutex = nullptr;
		other_._locked = false;
	}
	return ( *this );
}

bool HLock::owns_lock( void ) const noexcept {
	return ( _locked );
}

void HLock::lock( void ) {
	M_PROLOG
	M_ASSERT( _mutex && ! _locked );
	_mutex->lock();
	_locked = true;
	return;
	M_EPILOG
}

void HLock::unlock( void ) {
	M_PROLOG
	M_ASSERT( _locked );
	_mutex->unlock();
	_locked = false;
	return;
	M_EPILOG
}

HSemaphore::HSemaphore( int value_ )
	: _mutex()
	, _cond( _mutex )
	, _count( value_ ) {
	return;
}

void HSemaphore::wait( void ) {
	M_PROLOG
	HLock l( _mutex );
	while ( _count == 0 ) {
		/* Inside cond.wait() mutex is unlocked. */
		_cond.wait_for(/* ever */);
	}
	-- _count;
	return;
	M_EPILOG
}

void HSemaphore::signal( void ) {
	M_PROLOG
	HLock l( _mutex );
	++ _count;
	_cond.signal();
	return;
	M_EPILOG
}

void HSemaphore::reset( int value_ ) {
	M_PROLOG
	HLock l( _mutex );
	_count = value_;
	if ( _count > 0 ) {
		_cond.broadcast();
	}
	return;
	M_EPILOG
}

HCondition::HCondition( HMutex& mutex_ )
	: _buf( chunk_size<pthread_cond_t>( 1 ) + chunk_size<pthread_condattr_t>( 1 ) )
	, _mutex( mutex_ ) {
	M_PROLOG
	pthread_condattr_t* attr( static_cast<pthread_condattr_t*>( static_cast<void*>( _buf.get<char>() + sizeof ( pthread_cond_t ) ) ) );
	::pthread_condattr_init( attr );
	M_ENSURE( ::pthread_cond_init( _buf.get<pthread_cond_t>(), attr ) == 0 );
	return;
	M_EPILOG
}

HCondition::~HCondition( void ) {
	M_PROLOG
	M_ENSURE( ::pthread_cond_destroy( _buf.get<pthread_cond_t>() ) == 0 );
	::pthread_condattr_destroy( static_cast<pthread_condattr_t*>( static_cast<void*>( _buf.get<char>() + sizeof ( pthread_cond_t ) ) ));
	return;
	M_DESTRUCTOR_EPILOG
}

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int const FWD_CLOCK_REALTIME = CLOCK_REALTIME;
#pragma GCC diagnostic pop
}

/*
 * Read it or die!
 *
 * When ::pthread_cond_wait() is invoked, calling thread releases mutex
 * and goes to sleep, then scheduler (when she sees it as appropriate)
 * switches tasks. In the second task ::pthread_cond_signal() is called ...
 * Here comes important part: scheduler does not necessarily switches task
 * right back, it means that more than one call to ::pthread_cond_signal()
 * may occur in sequence, and all signals, that are send before scheduler
 * switches back to task one, when ::pthread_cond_wait() may lock again,
 * are lost!
 */

HCondition::status_t HCondition::do_wait( time_t timeOutSeconds_,
		int long timeOutNanoSeconds_ ) {
	M_PROLOG
	M_ASSERT( _mutex.is_owned() );
	int error( 0 );
	timespec timeOut{ timeOutSeconds_, timeOutNanoSeconds_ };
	error = ::pthread_cond_timedwait( _buf.get<pthread_cond_t>(),
				_mutex._buf.get<pthread_mutex_t>(), &timeOut );
	/*
	 * We need to make sure that mutex owner is reset to proper value,
	 * as HCondition::signal() invoker undoubtedly modified it.
	 */
	_mutex.reown();
	/* Error code is not stored in errno but is explicitly returned. */
	HScopedValueReplacement<int> saveErrno( errno, error );
	M_ENSURE( ( error == 0 ) || ( error == EINTR ) || ( error == ETIMEDOUT ) );
	return ( ( error == 0 ) ? SIGNALED : ( ( error == EINTR ) ? INTERRUPT : TIMEOUT ) );
	M_EPILOG
}

HCondition::status_t HCondition::wait_until( HTime const& time_ ) {
	M_PROLOG
	return ( do_wait( yaal_epoch_to_unix_epoch( time_.raw() ), 0 ) );
	M_EPILOG
}

HCondition::status_t HCondition::wait_for( time::duration_t duration_ ) {
	M_PROLOG
	timespec timeOut;
	::memset( &timeOut, 0, sizeof ( timeOut ) );
	M_ENSURE( clock_gettime( FWD_CLOCK_REALTIME, &timeOut ) == 0 );
	timeOut.tv_sec += static_cast<time_t>( ( duration_ / si::NANO_IN_WHOLE ).raw() );
	timeOut.tv_nsec += static_cast<int long>( ( duration_ % si::NANO_IN_WHOLE ).raw() );
	if ( timeOut.tv_nsec >= si::NANO_IN_WHOLE ) {
		++ timeOut.tv_sec;
		timeOut.tv_nsec -= static_cast<int long>( si::NANO_IN_WHOLE );
	}
	return ( do_wait( timeOut.tv_sec, timeOut.tv_nsec ) );
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
	: _mutex(), _condition( _mutex ), _signaled( false ) {
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
	while ( ! _signaled ) {
		_condition.wait_for( /* ever */ );
	}
	_signaled = false;
	return;
	M_EPILOG
}

void HEvent::signal( void ) {
	M_PROLOG
	HLock l( _mutex );
	_signaled = true;
	_condition.signal();
	return;
	M_EPILOG
}

HReadWriteLock::HReadWriteLock( void )
	: _mutex()
	, _buf( chunk_size<pthread_rwlock_t>( 1 ) + chunk_size<pthread_rwlockattr_t>( 1 ) )
	, _lockInfo() {
	pthread_rwlockattr_t* attr( static_cast<pthread_rwlockattr_t*>( static_cast<void*>( _buf.get<char>() + sizeof ( pthread_rwlock_t ) ) ) );
	M_ENSURE( ::pthread_rwlockattr_init( attr ) == 0 );
	M_ENSURE( ::pthread_rwlock_init( _buf.get<pthread_rwlock_t>(), attr ) == 0 );
	return;
}

HReadWriteLock::~HReadWriteLock( void ) {
	M_PROLOG
	M_ENSURE( ::pthread_rwlock_destroy( _buf.get<pthread_rwlock_t>() ) == 0 );
	pthread_rwlockattr_t* attr( static_cast<pthread_rwlockattr_t*>( static_cast<void*>( _buf.get<char>() + sizeof ( pthread_rwlock_t ) ) ) );
	M_ENSURE( ::pthread_rwlockattr_destroy( attr ) == 0 );
	return;
	M_DESTRUCTOR_EPILOG
}

void HReadWriteLock::lock_read( void ) {
	M_PROLOG
	OLockInfo* lockInfo( nullptr );
	/* scope for _lockInfo access */ {
		HLock l( _mutex );
		lockInfo = &_lockInfo[ HThread::get_current_thread_id() ];
	}
	/*
	 * `lockInfo' is a memory reference that is valid
	 * even if other thread accesses and modifies _lockInfo map.
	 */
	if ( ! lockInfo->_count ) {
		lockInfo->_firstLockRead = true;
		M_ENSURE( ::pthread_rwlock_rdlock( _buf.get<pthread_rwlock_t>() ) == 0 );
	}
	++ lockInfo->_count;
	return;
	M_EPILOG
}

void HReadWriteLock::lock_write( void ) {
	M_PROLOG
	OLockInfo* lockInfo( nullptr );
	/* scope for _lockInfo access */ {
		HLock l( _mutex );
		lockInfo = &_lockInfo[ HThread::get_current_thread_id() ];
	}
	/*
	 * `lockInfo' is a memory reference that is valid
	 * even if other thread accesses and modifies _lockInfo map.
	 */
	M_ASSERT( ! lockInfo->_firstLockRead );
	if ( ! lockInfo->_count ) {
		M_ENSURE( ::pthread_rwlock_wrlock( _buf.get<pthread_rwlock_t>() ) == 0 );
	}
	++ lockInfo->_count;
	return;
	M_EPILOG
}

void HReadWriteLock::unlock( void ) {
	M_PROLOG
	bool last( false );
	/* scope for _lockInfo access */ {
		HLock l( _mutex );
		lock_info_t::iterator it( _lockInfo.find( HThread::get_current_thread_id() ) );
		M_ENSURE( it != _lockInfo.end() );
		-- it->second._count;
		if ( ! it->second._count ) {
			last = true;
			_lockInfo.erase( it );
		}
	}
	if ( last ) {
		M_ENSURE( ::pthread_rwlock_unlock( _buf.get<pthread_rwlock_t>() ) == 0 );
	}
	return;
	M_EPILOG
}

HReadWriteLockReadLock::HReadWriteLockReadLock( HReadWriteLock& rwLock_ )
	: _rwLock( &rwLock_ ) {
	M_PROLOG
	_rwLock->lock_read();
	return;
	M_EPILOG
}

HReadWriteLockReadLock::HReadWriteLockReadLock( HReadWriteLockReadLock&& other_ ) noexcept
	: _rwLock( other_._rwLock ) {
	other_._rwLock = nullptr;
}

HReadWriteLockReadLock& HReadWriteLockReadLock::operator = ( yaal::hcore::HReadWriteLockReadLock&& other_ ) noexcept {
	if ( &other_ != this ) {
		_rwLock = other_._rwLock;
		other_._rwLock = nullptr;
	}
	return ( *this );
}

HReadWriteLockReadLock::~HReadWriteLockReadLock( void ) {
	M_PROLOG
	if ( _rwLock ) {
		_rwLock->unlock();
	}
	return;
	M_DESTRUCTOR_EPILOG
}

HReadWriteLockWriteLock::HReadWriteLockWriteLock( HReadWriteLock& rwLock_ )
	: _rwLock( &rwLock_ ) {
	M_PROLOG
	_rwLock->lock_write();
	return;
	M_EPILOG
}

HReadWriteLockWriteLock::HReadWriteLockWriteLock( HReadWriteLockWriteLock&& other_ ) noexcept
	: _rwLock( other_._rwLock ) {
	other_._rwLock = nullptr;
}

HReadWriteLockWriteLock& HReadWriteLockWriteLock::operator = ( yaal::hcore::HReadWriteLockWriteLock&& other_ ) noexcept {
	if ( &other_ != this ) {
		_rwLock = other_._rwLock;
		other_._rwLock = nullptr;
	}
	return ( *this );
}

HReadWriteLockWriteLock::~HReadWriteLockWriteLock( void ) {
	M_PROLOG
	if ( _rwLock ) {
		_rwLock->unlock();
	}
	return;
	M_DESTRUCTOR_EPILOG
}

}

}

