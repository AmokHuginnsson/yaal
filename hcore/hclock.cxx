/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hclock.cxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hclock.cxx
 * \brief Implementation of HClock class.
 */

#include <ctime>

#include "config.hxx"

#ifndef HAVE_CLOCK_GETTIME
#include "compat.hxx"
#endif /* #ifndef HAVE_CLOCK_GETTIME */

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hclock.hxx"
#include "hexception.hxx"
#include "numeric.hxx"

using namespace yaal::meta;

namespace yaal {

namespace hcore {

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int const FWD_CLOCK_REALTIME = CLOCK_REALTIME;
#if defined( __HOST_OS_TYPE_SOLARIS__ ) || defined( __HOST_OS_TYPE_WINDOWS__ )
#undef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID CLOCK_REALTIME
#endif /* #if defined( __HOST_OS_TYPE_SOLARIS__ ) || defined( __HOST_OS_TYPE_WINDOWS__ ) */
static int const FWD_CLOCK_THREAD_CPUTIME_ID = CLOCK_THREAD_CPUTIME_ID;
#pragma GCC diagnostic error "-Wold-style-cast"
}

HClock::HClock( TYPE::type_t type_ )
	: _moment(), _type( type_ ) {
	M_PROLOG
	reset();
	return;
	M_EPILOG
}

i64_t HClock::get_time_elapsed( UNIT::unit_t unit_ ) const {
	M_PROLOG
	static int long const NANO_IN_WHOLE = power<10, 9>::value;
	static int long const MICRO_IN_WHOLE = power<10, 6>::value;
	static int long const NANO_IN_MILI = power<10, 6>::value;
	static int long const MILI_IN_WHOLE = power<10, 3>::value;
	static int long const NANO_IN_MICRO = power<10, 3>::value;
	timespec time;
	timespec now;
	clockid_t cid( _type == TYPE::REAL ? FWD_CLOCK_REALTIME : FWD_CLOCK_THREAD_CPUTIME_ID );
	M_ENSURE( clock_gettime( cid, &now ) == 0 );
	time.tv_sec = static_cast<time_t>( now.tv_sec - _moment[ UNIT::SECOND ] );
	if ( now.tv_nsec < _moment[ UNIT::NANOSECOND ] ) {
		-- time.tv_sec;
		time.tv_nsec = NANO_IN_WHOLE - static_cast<int long>( _moment[ UNIT::NANOSECOND ] - now.tv_nsec );
	} else
		time.tv_nsec = now.tv_nsec - static_cast<int long>( _moment[ UNIT::NANOSECOND ] );
	i64_t elapsed( 0 );
	switch ( unit_ ) {
		case ( UNIT::SECOND ): elapsed = time.tv_sec; break;
		case ( UNIT::NANOSECOND ): elapsed = static_cast<i64_t>( time.tv_sec ) * NANO_IN_WHOLE + time.tv_nsec; break;
		case ( UNIT::MICROSECOND ): elapsed = static_cast<i64_t>( time.tv_sec ) * MICRO_IN_WHOLE + time.tv_nsec / NANO_IN_MICRO; break;
		case ( UNIT::MILISECOND ): elapsed = static_cast<i64_t>( time.tv_sec ) * MILI_IN_WHOLE + time.tv_nsec / NANO_IN_MILI; break;
		default: M_ASSERT( 0 && "bad HClock::UNIT!" ); break;
	}
	return ( elapsed );
	M_EPILOG
}

void HClock::reset( void ) {
	M_PROLOG
	timespec time;
	clockid_t cid( _type == TYPE::REAL ? FWD_CLOCK_REALTIME : FWD_CLOCK_THREAD_CPUTIME_ID );
	M_ENSURE( clock_gettime( cid, &time ) == 0 );
	_moment[ UNIT::SECOND ] = time.tv_sec;
	_moment[ UNIT::NANOSECOND ] = time.tv_nsec;
	return;
	M_EPILOG
}

}

}

