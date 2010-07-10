/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hclock.cxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hclock.cxx
 * \brief Implementation of HClock class.
 */

#include <ctime>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hclock.hxx"
#include "hexception.hxx"
#include "numeric.hxx"

using namespace yaal::meta;

namespace yaal
{

namespace hcore
{

HClock::HClock( void ) : _moment()
	{
	M_PROLOG
	timespec time;
	M_ENSURE( clock_gettime( CLOCK_REALTIME, &time ) == 0 );
	_moment[ UNIT::SECOND ] = time.tv_sec;
	_moment[ UNIT::NANOSECOND ] = time.tv_nsec;
	M_EPILOG
	}

int long HClock::get_time_elapsed( UNIT::unit_t const& unit_, bool const& reset_ ) const
	{
	M_PROLOG
	static int long const NANO_IN_WHOLE = power<10, 9>::value;
	static int long const MICRO_IN_WHOLE = power<10, 6>::value;
	static int long const NANO_IN_MILI = power<10, 6>::value;
	static int long const MILI_IN_WHOLE = power<10, 3>::value;
	static int long const NANO_IN_MICRO = power<10, 3>::value;
	timespec time;
	timespec reset;
	M_ENSURE( clock_gettime( CLOCK_REALTIME, &reset ) == 0 );
	time.tv_sec = reset.tv_sec - _moment[ UNIT::SECOND ];
	if ( reset.tv_nsec < _moment[ UNIT::NANOSECOND ] )
		{
		-- time.tv_sec;
		time.tv_nsec = NANO_IN_WHOLE - ( _moment[ UNIT::NANOSECOND ] - reset.tv_nsec );
		}
	else
		time.tv_nsec = reset.tv_nsec - _moment[ UNIT::NANOSECOND ];
	int long elapsed = 0;
	switch ( unit_ )
		{
		case ( UNIT::SECOND ): elapsed = time.tv_sec; break;
		case ( UNIT::NANOSECOND ): elapsed = time.tv_sec * NANO_IN_WHOLE + time.tv_nsec; break;
		case ( UNIT::MICROSECOND ): elapsed = time.tv_sec * MICRO_IN_WHOLE + time.tv_nsec / NANO_IN_MICRO; break;
		case ( UNIT::MILISECOND ): elapsed = time.tv_sec * MILI_IN_WHOLE + time.tv_nsec / NANO_IN_MILI; break;
		default: M_ASSERT( ! "bad HClock::UNIT!" );
		}
	if ( reset_ )
		{
		_moment[ UNIT::SECOND ] = reset.tv_sec;
		_moment[ UNIT::NANOSECOND ] = reset.tv_nsec;
		}
	return ( elapsed );
	M_EPILOG
	}

}

}

