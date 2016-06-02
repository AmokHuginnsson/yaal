/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  sleep.cxx - this file is integral part of `yaal' project.

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

#include <cerrno>
#include <cstdlib>
#include <sys/time.h> /* timeval (for FreeBSD) */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "sleep.hxx"
#include "hcore/hclock.hxx"
#include "hcore/si.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

bool sleep_for( yaal::hcore::time::duration_t duration_, bool ignoreInterrupts_ ) {
	HClock c;
	int err( 0 );
	timeval wait{ 0, 0 };
	do {
		i64_t left( duration_.get() - c.get_time_elapsed( time::UNIT::NANOSECOND ) );
		if ( left <= 0 ) {
			break;
		}
		wait = {
			static_cast<time_t>( left / si::NANO_IN_WHOLE ),
			static_cast<suseconds_t>( ( left % si::NANO_IN_WHOLE ) / si::NANO_IN_MICRO )
		};
	} while ( ( ( err = ::select( 0, nullptr, nullptr, nullptr, &wait ) ) == -1 ) && ( errno == EINTR ) && ignoreInterrupts_ );
	return ( ( c.get_time_elapsed( time::UNIT::NANOSECOND ) >= duration_ ) && ( err == 0 ) );
}

}

}

