/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hthread.hxx
 * \brief Declaration of threading related classes.
 *
 * HThread, HMutex, HLock, HSemaphore, HCondition, HEvent.
 */

#ifndef YAAL_HCORE_HTHREAD_HXX_INCLUDED
#define YAAL_HCORE_HTHREAD_HXX_INCLUDED 1

#include "config.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/duration.hxx"
#include "hcore/htime.hxx"
#include "hcore/hmap.hxx"

namespace yaal {

namespace hcore {

namespace thread_helper {
typedef i64_t thread_id_t;
}

class HCondition;
/*! \brief Implementation of multi-threaded synchronization primitive - Mutex.
 */
class HMutex final {
public:
	typedef HMutex this_type;
	/*! \brief Mutex types.
	 */
	enum class TYPE {
		DEFAULT,      /*!< Default mutex type (non-recursive). */
		RECURSIVE,    /*!< Recursive mutex type. */
		NON_RECURSIVE /*!< Implicit non-recursive mutex type. */
	};
private:
	/*{*/
	TYPE _type;
	HChunk _buf;
	HResource<void, void (*)( void* )> _resGuard;
	thread_helper::thread_id_t _owner;
	/*}*/
public:
	/*{*/
	HMutex( TYPE /* recursive | fair */ = TYPE::DEFAULT );
	~HMutex( void );
	void lock( void );
	bool try_lock( void );
	void unlock( void );
	bool is_owned( void ) const;
	/*}*/
private:
	/*{*/
	void reown( void );
	HMutex( HMutex const& );
	HMutex& operator = ( HMutex const& );
	friend class HCondition;
	/*}*/
};

typedef HExceptionT<HMutex> HMutexException;

/*! \brief Multi-threaded synchronization primitive - Conditional Variable.
 */
class HCondition final {
	typedef HCondition this_type;
	HChunk _buf;
	HMutex& _mutex;
public:
	typedef enum {
		SIGNALED,
		TIMEOUT,
		INTERRUPT
	} status_t;
	HCondition( HMutex& );
	~HCondition( void );
	status_t wait_until( HTime const& );
	status_t wait_for( time::duration_t = time::duration( 0x1fffffff /* FreeBSD strange limit. */, time::UNIT::SECOND ) );
	void signal( void );
	void broadcast( void );
private:
	status_t do_wait( time_t, int long );
	HCondition( HCondition const& );
	HCondition& operator = ( HCondition const& );
};

typedef HExceptionT<HCondition> HConditionException;

/*! \brief Multi-threaded synchronization primitive - Semaphore.
 */
class HSemaphore final {
public:
	typedef HSemaphore this_type;
private:
	HMutex _mutex;
	HCondition _cond;
	int _count;
public:
	/*! \brief Create semaphore with specific value.
	 *
	 * \param value - initialize new HSemaphore object with this \p value.
	 */
	HSemaphore( int value = 0 );

	/*! \brief Wait until semaphore is signaled.
	 */
	void wait( void );

	/*! \brief Signal the semaphore.
	 */
	void signal( void );

	/*! \brief Set new value for this semaphore to hold.
	 *
	 * \param value - set new \p value to be held by this semaphore.
	 */
	void reset( int = 0 );
private:
	HSemaphore( HSemaphore const& ) = delete;
	HSemaphore& operator = ( HSemaphore const& ) = delete;
};

typedef HExceptionT<HSemaphore> HSemaphoreException;

/*! \brief Core multi-threading primitive - Thread.
 */
class HThread final {
public:
	typedef HThread this_type;
	typedef HBoundCall<> call_t;
	typedef thread_helper::thread_id_t id_t;
	M_YAAL_HCORE_PUBLIC_API static int _threadStackSize;
	static id_t const INVALID = -1;
private:
	typedef enum {
		DEAD,
		SPAWNING,
		ALIVE,
		ZOMBIE
	} status_t;
	struct OExceptionInfo {
		bool _stacked;
		int _code;
		yaal::hcore::HString _message;
		OExceptionInfo( void )
			: _stacked( false )
			, _code( 0 )
			, _message() {
		}
	};
	status_t _status;
	HChunk _buf;
	mutable HMutex _mutex;
	HSemaphore _semaphore;
	HResource<void, void (*)( void* )> _resGuard;
	call_t _call;
	id_t _id;
	OExceptionInfo _exceptionInfo;
public:
	HThread( void );
	~HThread( void );
	/*! \brief Spawn new working thread.
	 *
	 * \param call_ - a call that shall be invoked in newly spawned thread.
	 */
	void spawn( call_t call_ );
	/*! \brief Finish working thread and cleanup after after it.
	 *
	 * \return Working thread low level exit status information.
	 */
	void finish( void );
	/*! \brief An alias for finish() method.
	 */
	void join( void );
	/*! \brief Signal working thread that it should finish.
	 */
	void schedule_finish( void );
	/*! \brief Test if working thread is currently running.
	 *
	 * \return True if working thread is currently running.
	 */
 	bool is_alive( void ) const;
	/*! \brief Store information about exception that occurred.
	 *
	 * Using this method cause HThread::finish() to throw a HThreadException.
	 *
	 * \param message_ - exception message.
	 * \param code_ - additional exception information code.
	 */
	void stack_exception( yaal::hcore::HString const& message_, int code_ = 0 );
	/*! \brief Get operating system level working thread ID for this HThread object.
	 *
	 * \return Thread ID for this HThread object.
	 */
	id_t get_id( void ) const;
	/*! \brief Get operating system level working thread ID for calling thread.
	 *
	 * \return Working thread ID.
	 */
	static id_t get_current_thread_id( void );
	/*! \brief Set thread name.
	 *
	 * \param name_ - new name for working thread.
	 */
	static void set_name( char const* name_ );
private:
	/*! \brief Pass control to user specified call in working thread.
	 */
	void control( void );
	/*! \brief Spawn working thread and execute HThread::control().
	 */
	static void* SPAWN( void* );
	static void CLEANUP( void* );
	HThread( HThread const& );
	HThread& operator = ( HThread const& );
};

typedef HExceptionT<HThread> HThreadException;

/*! \brief HLock implements automatic multi-threaded synchronization primitive.
 *
 * Scope based automation of locking and unlocking of Mutexes.
 */
class HLock final {
public:
	enum class TYPE {
		DEFINITE,
		TENTATIVE
	};
	typedef HLock this_type;
private:
	HMutex* _mutex;
	bool _locked;
public:
	explicit HLock( HMutex&, TYPE = TYPE::DEFINITE );
	HLock( HLock&& ) noexcept;
	HLock& operator = ( HLock&& ) noexcept;
	~HLock( void );
	void lock( void );
	void unlock( void );
	bool owns_lock( void ) const noexcept;
private:
	HLock( HLock const& ) = delete;
	HLock& operator = ( HLock const& ) = delete;
};

/*! \brief Asynchronous notification mechanism.
 */
class HEvent {
public:
	typedef HEvent this_type;
private:
	HMutex _mutex;
	HCondition _condition;
	bool _signaled;
public:
	HEvent( void );
	~HEvent( void );
	void wait( void );
	void signal( void );
};

typedef HExceptionT<HEvent> HEventException;

/*! \brief Read-Write Lock functionality implementation.
 */
class HReadWriteLock final {
public:
	typedef HReadWriteLock this_type;
private:
	struct OLockInfo {
		int _count;
		bool _firstLockRead;
		OLockInfo( void )
			: _count( 0 )
			, _firstLockRead( false ) {
		}
	};
	typedef yaal::hcore::HMap<HThread::id_t, OLockInfo> lock_info_t;
	HMutex _mutex;
	HChunk _buf;
	lock_info_t _lockInfo;
public:
	HReadWriteLock( void );
	~HReadWriteLock( void );
	void lock_read( void );
	void lock_write( void );
	void unlock( void );
};

/*! \brief Read-Write Lock reader-guard functionality implementation.
 */
class HReadWriteLockReadLock final {
public:
	typedef HReadWriteLockReadLock this_type;
private:
	HReadWriteLock* _rwLock;
public:
	explicit HReadWriteLockReadLock( HReadWriteLock& );
	HReadWriteLockReadLock( HReadWriteLockReadLock&& ) noexcept;
	HReadWriteLockReadLock& operator = ( HReadWriteLockReadLock&& ) noexcept;
	~HReadWriteLockReadLock( void );
private:
	HReadWriteLockReadLock( HReadWriteLockReadLock const& ) = delete;
	HReadWriteLockReadLock& operator = ( HReadWriteLockReadLock const& ) = delete;
};

/*! \brief Read-Write Lock writer-guard functionality implementation.
 */
class HReadWriteLockWriteLock final {
public:
	typedef HReadWriteLockWriteLock this_type;
private:
	HReadWriteLock* _rwLock;
public:
	explicit HReadWriteLockWriteLock( HReadWriteLock& );
	HReadWriteLockWriteLock( HReadWriteLockWriteLock&& ) noexcept;
	HReadWriteLockWriteLock& operator = ( HReadWriteLockWriteLock&& ) noexcept;
	~HReadWriteLockWriteLock( void );
private:
	HReadWriteLockWriteLock( HReadWriteLockWriteLock const& ) = delete;
	HReadWriteLockWriteLock& operator = ( HReadWriteLockWriteLock const& ) = delete;
};

}

}

#endif /* #ifndef YAAL_HCORE_HTHREAD_HXX_INCLUDED */

