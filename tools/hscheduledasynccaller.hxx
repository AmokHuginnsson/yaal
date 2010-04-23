/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hscheduledasynccaller.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED
#define YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED

#include "tools/hasynccaller.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Task scheduler.
 *
 * HScheduledAsyncCaller is capable of invocation of any method of any class
 * at precisely specified moment in time.
 */
class HScheduledAsyncCaller : public HAbstractAsyncCaller
	{
	yaal::hcore::HCondition f_oCondition;
	HScheduledAsyncCaller( void );
	virtual ~HScheduledAsyncCaller( void );
	virtual void* do_work( void );
	virtual void do_signal( void );
	friend class yaal::hcore::HSingleton<HScheduledAsyncCaller>;
	friend class yaal::hcore::HDestructor<HScheduledAsyncCaller>;
	};

typedef yaal::hcore::HSingleton<HScheduledAsyncCaller> HScheduledAsyncCallerService;

}

}

#endif /* not YAAL_TOOLS_HSCHEDULEDASYNCCALLER_HXX_INCLUDED */

