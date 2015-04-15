/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  duration.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "duration.hxx"
#include "si.hxx"
#include "htime.hxx"

namespace yaal {

namespace hcore {

namespace time {

duration_t duration( yaal::i64_t duration_, UNIT unit_ ) {
	duration_t nanoseconds( 0 );
	switch ( unit_ ) {
		case ( UNIT::DAY ): {
			nanoseconds = duration_ * HTime::HOURS_IN_DAY * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE * si::NANO_IN_WHOLE;
		} break;
		case ( UNIT::HOUR ): {
			nanoseconds = duration_ * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE * si::NANO_IN_WHOLE;
		} break;
		case ( UNIT::MINUTE ): {
			nanoseconds = duration_ * HTime::SECONDS_IN_MINUTE * si::NANO_IN_WHOLE;
		} break;
		case ( UNIT::SECOND ): {
			nanoseconds = duration_ * si::NANO_IN_WHOLE;
		} break;
		case ( UNIT::MILISECOND ): {
			nanoseconds = duration_ * si::NANO_IN_MILI;
		} break;
		case ( UNIT::MICROSECOND ): {
			nanoseconds = duration_ * si::NANO_IN_MICRO;
		} break;
		case ( UNIT::NANOSECOND ): {
			nanoseconds = duration_;
		} break;
	}
	return ( nanoseconds );
}

}

}

}

