/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hasynccaller.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HASYNCCALLER_HXX_INCLUDED
#define YAAL_TOOLS_HASYNCCALLER_HXX_INCLUDED

#include "hcore/hboundcall.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hmultimap.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Interface for asynchronous call providers.
 */
class HAbstractAsyncCaller
	{
protected:
	typedef int long priority_t;
	typedef yaal::hcore::HBoundCallInterface<>::ptr_t call_t;
	typedef yaal::hcore::HMultiMap<priority_t, call_t> queue_t;
	queue_t f_oQueue;
	yaal::hcore::HThread _thread;
	yaal::hcore::HMutex f_oMutex;
	bool f_bLoop;
public:
	void register_call( priority_t, call_t );
	void flush( void* );
protected:
	HAbstractAsyncCaller( void );
	virtual ~HAbstractAsyncCaller( void ) {}
	void start( void );
	void stop( void );
	virtual void do_signal( void ) = 0;
	virtual void* do_work( void ) = 0;
	static int life_time( int );
private:
	void* run( void );
	};

/*! \brief Invoke function or method asynchronously.
 */
class HAsyncCaller : public HAbstractAsyncCaller
	{
	yaal::hcore::HSemaphore f_oSemaphore;
	HAsyncCaller( void );
	virtual ~HAsyncCaller( void );
	virtual void* do_work( void );
	virtual void do_signal( void );
	friend class yaal::hcore::HSingleton<HAsyncCaller>;
	friend class yaal::hcore::HDestructor<HAsyncCaller>;
	};

typedef yaal::hcore::HSingleton<HAsyncCaller> HAsyncCallerService;

}

}

#endif /* not YAAL_TOOLS_HASYNCCALLER_HXX_INCLUDED */

