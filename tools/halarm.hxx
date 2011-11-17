/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/halarm.hxx - this file is integral part of yaal project.

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
/*! \file tools/halarm.hxx
 * \brief Declaration of HAlarm class.
 */

#ifndef YAAL_TOOLS_HALARM_HXX_INCLUDED
#define YAAL_TOOLS_HALARM_HXX_INCLUDED 1

#include <ctime>

#include "hcore/hexception.hxx"
#include "hcore/hthread.hxx"

namespace yaal {

namespace tools {

/*! \brief HAlarm class provides automatic registration and deregistration of system alerts.
 */
class HAlarm {
private:
	timer_t _timer;
	yaal::hcore::HLock _lock;
	M_YAAL_TOOLS_PUBLIC_API static yaal::hcore::HMutex _mutex;
public:
	typedef HAlarm this_type;
	/*! \brief Register an alarm.
	 *
	 * miliseconds_ - alarm shall come in miliseconds_ miliseconds.
	 */
	HAlarm( int long miliseconds_ );

	/*! \brief Deregister alarm if it has not came.
	 */
	~HAlarm( void );
private:
	HAlarm( HAlarm const& );
	HAlarm& operator = ( HAlarm const& );
};

typedef yaal::hcore::HExceptionT<HAlarm> HAlarmException;

}

}

#endif /* #ifndef YAAL_TOOLS_HALARM_HXX_INCLUDED */

