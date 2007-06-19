/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hthread.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HTHREAD_H
#define __YAAL_HCORE_HTHREAD_H

#include <pthread.h>

namespace yaal
{

namespace hcore
{

class HCondition;
class HMutex
	{
protected:
	/*{*/
	bool	f_bRecursive;
	pthread_mutexattr_t f_sAttributes;
	pthread_mutex_t f_xMutex;
	/*}*/
public:
	/*{*/
	HMutex ( bool const /* recursive */ = false );
	virtual ~HMutex ( void );
	void lock ( void );
	bool try_lock ( void );
	void unlock ( void );
	/*}*/
private:
	/*{*/
	friend class HCondition;
	HMutex ( HMutex const & );
	HMutex & operator = ( HMutex const & );
	/*}*/
	};

class HCondition
	{
	pthread_condattr_t f_sAttributes;
	pthread_cond_t f_xCondition;
	HMutex f_oMutex;
public:
	typedef enum
		{
		D_OK,
		D_TIMEOUT,
		D_INTERRUPT
		} status_t;
	HCondition ( void );
	virtual ~HCondition ( void );
	status_t wait ( int long unsigned * = NULL, int long unsigned * = NULL );
	void signal ( void );
private:
	HCondition ( HCondition const & );
	HCondition & operator = ( HCondition const & );
	};

class HThread
	{
	typedef enum
		{
		D_DEAD,
		D_ALIVE,
		D_ZOMBIE
		} status_t;
	status_t				f_eStatus;
	pthread_attr_t	f_sAttributes;
	pthread_t				f_xThread;
protected:
	mutable HMutex	f_oMutex;
	HCondition			f_oCondition;
public:
	HThread ( void );
	virtual ~HThread ( void );
	int spawn ( void );
	int finish ( void );
	bool is_alive ( void ) const;
private:
	virtual int run ( void ) = 0;
	void * control ( void );
	static void * SPAWN ( void * );
	HThread ( HThread const & );
	HThread & operator = ( HThread const & );
	};

class HLock
	{
	HMutex & f_roMutex;
public:
	explicit HLock ( HMutex & );
	virtual ~HLock ( void );
private:
	HLock ( HLock const & );
	HLock & operator = ( HLock const & );
	};

}

}

#endif /* not __YAAL_HCORE_HTHREAD_H */

