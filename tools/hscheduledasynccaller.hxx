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

#include "tools/hasynccaller.hxx"

namespace yaal {

namespace tools {

/*! \brief Task scheduler.
 *
 * HScheduledAsyncCaller is capable of invocation of any method of any class
 * at precisely specified moment in time.
 */
class M_YAAL_TOOLS_PUBLIC_API HScheduledAsyncCaller : public HAbstractAsyncCaller, public yaal::hcore::HSingleton<HScheduledAsyncCaller> {
public:
	typedef HAbstractAsyncCaller base_type;
	typedef HScheduledAsyncCaller this_type;
private:
	yaal::hcore::HCondition _condition;
	HScheduledAsyncCaller( void );
	virtual ~HScheduledAsyncCaller( void );
	virtual void do_work( void );
	virtual void do_signal( void );
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HScheduledAsyncCaller>;
	friend class yaal::hcore::HDestructor<HScheduledAsyncCaller>;
};

typedef yaal::hcore::HExceptionT<HScheduledAsyncCaller, HAbstractAsyncCallerException> HScheduledAsyncCallerException;

}

}

#endif /* #ifndef YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED */

