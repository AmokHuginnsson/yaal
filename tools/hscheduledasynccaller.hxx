/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hscheduledasynccaller.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED
#define YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED 1

#include "hcore/hboundcall.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hmultimap.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal {

namespace tools {

/*! \brief Task scheduler.
 *
 * HScheduledAsyncCaller is capable of invocation of any method of any class
 * at precisely specified moment in time.
 */
class M_YAAL_TOOLS_PUBLIC_API HScheduledAsyncCaller : public yaal::hcore::HSingleton<HScheduledAsyncCaller> {
public:
	typedef HScheduledAsyncCaller this_type;
	typedef int long priority_t;
	typedef yaal::hcore::HBoundCall<> call_t;
private:
	typedef yaal::hcore::HMultiMap<priority_t, call_t> queue_t;
	queue_t _queue;
	yaal::hcore::HMutex _mutex;
	yaal::hcore::HCondition _condition;
	bool _loop;
public:
	void call_at( priority_t, call_t );
	void flush( void* );
	void stop( void );
private:
	void run( void );
	bool want_restart( void ) const;
	HScheduledAsyncCaller( void );
	virtual ~HScheduledAsyncCaller( void );
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HScheduledAsyncCaller>;
	friend class yaal::hcore::HDestructor<HScheduledAsyncCaller>;
};

typedef yaal::hcore::HExceptionT<HScheduledAsyncCaller> HScheduledAsyncCallerException;

}

}

#endif /* #ifndef YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED */

