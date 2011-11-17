/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hclock.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hclock.hxx
 * \brief Declaration of HClock class.
 */

#ifndef YAAL_HCORE_HCLOCK_HXX_INCLUDED
#define YAAL_HCORE_HCLOCK_HXX_INCLUDED 1

namespace yaal {

namespace hcore {

/*! \brief High resolution time mesurement tool.
 */
class HClock {
public:
	/*! \brief Resolution units for time mesurement.
	 */
	struct UNIT {
		typedef enum {
			SECOND = 0,
			NANOSECOND = 1,
			MICROSECOND = 2,
			MILISECOND = 3
		} unit_t;
	};
private:
	mutable int long _moment[2];
public:
	HClock( void );
	/*! \brief Get time that elapsed from last measurement.
	 *
	 * \param unit_ - get time given in specific units.
	 * \param reset_ - reset timer with this call.
	 * \return Amount of time that elapsed in given unit.
	 */
	int long get_time_elapsed( UNIT::unit_t unit_ = UNIT::SECOND, bool reset_ = false ) const;
};

}

}

#endif /* #ifndef YAAL_HCORE_HCLOCK_HXX_INCLUDED */

