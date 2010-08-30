/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hthread.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hthread.hxx
 * \brief Declaration of threading related classes.
 *
 * HThread, HThreadT<>, HMutex, HLock, HSemaphore, HCondition.
 */

#ifndef YAAL_HCORE_HTHREAD_HXX_INCLUDED
#define YAAL_HCORE_HTHREAD_HXX_INCLUDED

#include "hcore/hchunk.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hboundcall.hxx"

namespace yaal
{

namespace hcore
{

class HCondition;
/*! \brief Implementation of multi-threaded synchronizing prymitive - Mutex.
 */
class HMutex
	{
	typedef HMutex this_type;
public:
	/*! \brief Mutex types.
	 */
	struct TYPE
		{
		/*! \brief Mutex types.
		 */
		typedef enum
			{
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
	/*}*/
public:
	/*{*/
	HMutex( TYPE::mutex_type_t const /* recursive | fair */ = TYPE::DEFAULT );
	virtual ~HMutex( void );
	void lock( void );
	bool try_lock( void );
	void unlock( void );
	/*}*/
private:
	/*{*/
	HMutex( HMutex const& );
	HMutex& operator = ( HMutex const& );
	friend class HCondition;
	/*}*/
	};

typedef HExceptionT<HMutex> HMutexException;

/*! \brief Multi-threaded synchronizing prymitive - Semaphore.
 */
class HSemaphore
	{
	typedef HSemaphore this_type;
	HChunk _semaphore;
public:
	HSemaphore( void );
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
class HThread
	{
	typedef HThread this_type;
	typedef enum
		{
		DEAD,
		SPAWNING,
		ALIVE,
		ZOMBIE
		} status_t;
	status_t _status;
	HChunk _buf;
	mutable HMutex _mutex;
	HSemaphore _semaphore;
	HResource<void> _resGuard;
	typedef HBoundCall<void* ()> call_t;
	call_t _call;
public:
	HThread( void );
	virtual ~HThread( void );
	int spawn( call_t );
	void* finish( void );
	void schedule_finish( void );
 	bool is_alive( void ) const;
	static int long get_id( void );
private:
	void* control( void );
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
class HLock
	{
	HMutex& _mutex;
public:
	explicit HLock( HMutex& );
	virtual ~HLock( void );
private:
	HLock( HLock const& );
	HLock& operator = ( HLock const& );
	};

/*! \brief Multi-threaded synchronizing prymitive - Conditional Variable.
 */
class HCondition
	{
	typedef HCondition this_type;
	HChunk _buf;
	HMutex& _mutex;
public:
	typedef enum
		{
		OK,
		TIMEOUT,
		INTERRUPT
		} status_t;
	HCondition( HMutex& );
	virtual ~HCondition( void );
	status_t wait( int long unsigned const&, int long unsigned const& );
	void signal( void );
private:
	HCondition( HCondition const& );
	HCondition& operator = ( HCondition const& );
	};

typedef HExceptionT<HCondition> HConditionException;

/*! \brief Asynchronous notification mechanizm.
 */
class HEvent
	{
	HMutex _mutex;
	HCondition _condition;
public:
	HEvent( void );
	~HEvent( void );
	void wait( void );
	void signal( void );
	};

}

}

#endif /* not YAAL_HCORE_HTHREAD_HXX_INCLUDED */

