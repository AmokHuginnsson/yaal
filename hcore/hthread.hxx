/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hthread.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hthread.hxx
 * \brief Declaration of threading related classes.
 *
 * HThread, HMutex, HLock, HSemaphore, HCondition, HEvent.
 */

#ifndef YAAL_HCORE_HTHREAD_HXX_INCLUDED
#define YAAL_HCORE_HTHREAD_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/huniquemovable.hxx"
#include "hcore/hmap.hxx"

namespace yaal {

namespace hcore {

class HCondition;
/*! \brief Implementation of multi-threaded synchronizing prymitive - Mutex.
 */
class HMutex {
public:
	typedef HMutex this_type;
	/*! \brief Mutex types.
	 */
	struct TYPE {
		/*! \brief Mutex types.
		 */
		typedef enum {
			DEFAULT = 0,      /*!< Default mutex type (non-recursive). */
			RECURSIVE = 1,    /*!< Recursive mutex type. */
			NON_RECURSIVE = 2 /*!< Implicit non-recursive mutex type. */
		} mutex_type_t;
	};
private:
	/*{*/
	TYPE::mutex_type_t _type;
	HChunk _buf;
	HResource<void> _resGuard;
	int long _owner;
	/*}*/
public:
	/*{*/
	HMutex( TYPE::mutex_type_t const /* recursive | fair */ = TYPE::DEFAULT );
	virtual ~HMutex( void );
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

class HSemaphoreImplementationInterface {
	typedef HSemaphoreImplementationInterface this_type;
public:
	HSemaphoreImplementationInterface( void ) {}
	virtual ~HSemaphoreImplementationInterface( void ) {}
	void wait( void );
	void signal( void );
private:
	virtual void do_wait( void ) = 0;
	virtual void do_signal( void ) = 0;
	HSemaphoreImplementationInterface( HSemaphoreImplementationInterface const& );
	HSemaphoreImplementationInterface& operator = ( HSemaphoreImplementationInterface const& );
};

typedef HExceptionT<HMutex> HMutexException;

/*! \brief Multi-threaded synchronizing prymitive - Semaphore.
 */
class HSemaphore {
public:
	typedef HSemaphore this_type;
	typedef HResource<HSemaphoreImplementationInterface> semaphore_implementation_t;
	struct TYPE {
		typedef enum {
			POSIX,
			YAAL
		} type_t;
	};
	M_YAAL_HCORE_PUBLIC_API static TYPE::type_t DEFAULT;
private:
	semaphore_implementation_t _impl;
public:
	HSemaphore( TYPE::type_t = DEFAULT );
	virtual ~HSemaphore( void );
	void wait( void );
	void signal( void );
private:
	HSemaphore( HSemaphore const& );
	HSemaphore& operator = ( HSemaphore const& );
};

typedef HExceptionT<HSemaphore> HSemaphoreException;

/*! \brief Core multi-threading primitive - Thread.
 */
class HThread {
public:
	typedef HThread this_type;
	M_YAAL_HCORE_PUBLIC_API static int _threadStackSize;
	static int long const INVALID = -1;
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
		OExceptionInfo( void ) : _stacked( false ), _code( 0 ), _message() {}
	};
	status_t _status;
	HChunk _buf;
	mutable HMutex _mutex;
	HSemaphore _semaphore;
	HResource<void> _resGuard;
	typedef HBoundCall<> call_t;
	call_t _call;
	OExceptionInfo _exceptionInfo;
public:
	HThread( void );
	virtual ~HThread( void );
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
	/*! \brief Store information about exception that occured.
	 *
	 * Using this method cause HThread::finish() to throw a HThreadException.
	 *
	 * \param message_ - exception message.
	 * \param code_ - additional excepion information code.
	 */
	void stack_exception( yaal::hcore::HString const& message_, int code_ = 0 );
	/*! \brief Get operationg system level working thread ID.
	 *
	 * \return Working thread ID.
	 */
	static int long get_id( void );
	/*! \brief Set thread name.
	 *
	 * \param name_ - new name for working thread.
	 */
	static void set_name( char const* name_ );
private:
	/*! \brief Pass control to user specified call in working thread.
	 */
	void control( void );
	/*! \brief Spawn working thread and execute HThead::control().
	 */
	static void* SPAWN( void* );
	static void CLEANUP( void* );
	HThread( HThread const& );
	HThread& operator = ( HThread const& );
};

typedef HExceptionT<HThread> HThreadException;

/*! \brief HLock Implementats automatic multi-threaded synchronizing prymitive.
 *
 * Scope based automation of locking and unlocking of Mutexes.
 */
class HLock {
	HMutex& _mutex;
public:
	explicit HLock( HMutex& );
	virtual ~HLock( void );
private:
	HLock( HLock const& );
	HLock& operator = ( HLock const& );
};
typedef HUniqueMovable<HLock> external_lock_t;

/*! \brief Multi-threaded synchronizing prymitive - Conditional Variable.
 */
class HCondition {
	typedef HCondition this_type;
	HChunk _buf;
	HMutex& _mutex;
public:
	typedef enum {
		OK,
		TIMEOUT,
		INTERRUPT
	} status_t;
	HCondition( HMutex& );
	virtual ~HCondition( void );
	status_t wait( int long unsigned, int long unsigned );
	void signal( void );
	void broadcast( void );
private:
	HCondition( HCondition const& );
	HCondition& operator = ( HCondition const& );
};

typedef HExceptionT<HCondition> HConditionException;

/*! \brief Asynchronous notification mechanizm.
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
class HReadWriteLock {
public:
	typedef HReadWriteLock this_type;
private:
	struct OLockInfo {
		int _count;
		bool _firstLockRead;
		OLockInfo( void )
			: _count( 0 ), _firstLockRead( false )
			{}
	};
	typedef yaal::hcore::HMap<int long, OLockInfo> lock_info_t;
	HMutex _mutex;
	HChunk _buf;
	lock_info_t _lockInfo;
public:
	HReadWriteLock( void );
	virtual ~HReadWriteLock( void );
	void lock_read( void );
	void lock_write( void );
	void unlock( void );
};

/*! \brief Read-Write Lock reader-guard functionality implementation.
 */
class HReadWriteLockReadLock {
public:
	typedef HReadWriteLockReadLock this_type;
private:
	HReadWriteLock& _rwLock;
public:
	explicit HReadWriteLockReadLock( HReadWriteLock& );
	virtual ~HReadWriteLockReadLock( void );
private:
	HReadWriteLockReadLock( HReadWriteLockReadLock const& );
	HReadWriteLockReadLock& operator = ( HReadWriteLockReadLock const& );
};
typedef HUniqueMovable<HReadWriteLockReadLock> external_read_write_lock_read_lock_t;

/*! \brief Read-Write Lock writer-guard functionality implementation.
 */
class HReadWriteLockWriteLock {
public:
	typedef HReadWriteLockWriteLock this_type;
private:
	HReadWriteLock& _rwLock;
public:
	explicit HReadWriteLockWriteLock( HReadWriteLock& );
	virtual ~HReadWriteLockWriteLock( void );
private:
	HReadWriteLockWriteLock( HReadWriteLockWriteLock const& );
	HReadWriteLockWriteLock& operator = ( HReadWriteLockWriteLock const& );
};
typedef HUniqueMovable<HReadWriteLockWriteLock> external_read_write_lock_write_lock_t;

}

}

#endif /* #ifndef YAAL_HCORE_HTHREAD_HXX_INCLUDED */

