/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hclock.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
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
#include "hclock.hxx"

namespace yaal
{

namespace hcore
{

HClock::HClock( void ) : f_lMoment()
	{
	M_PROLOG
	timespec l_sTime;
	M_ENSURE( clock_gettime( CLOCK_REALTIME, &l_sTime ) == 0 );
	f_lMoment[ UNIT::D_SECOND ] = l_sTime.tv_sec;
	f_lMoment[ UNIT::D_NANOSECOND ] = l_sTime.tv_nsec;
	M_EPILOG
	}

int long HClock::get_time_elapsed( UNIT::unit_t const& unit, bool const& reset ) const
	{
	M_PROLOG
	static int long const D_NANO_IN_WHOLE = power<10, 9>::value;
	static int long const D_MICRO_IN_WHOLE = power<10, 6>::value;
	static int long const D_NANO_IN_MILI = power<10, 6>::value;
	static int long const D_MILI_IN_WHOLE = power<10, 3>::value;
	static int long const D_NANO_IN_MICRO = power<10, 3>::value;
	timespec l_sTime;
	timespec l_sReset;
	M_ENSURE( clock_gettime( CLOCK_REALTIME, &l_sReset ) == 0 );
	l_sTime.tv_sec = l_sReset.tv_sec - f_lMoment[ UNIT::D_SECOND ];
	if ( l_sReset.tv_nsec < f_lMoment[ UNIT::D_NANOSECOND ] )
		{
		-- l_sTime.tv_sec;
		l_sTime.tv_nsec = D_NANO_IN_WHOLE - ( f_lMoment[ UNIT::D_NANOSECOND ] - l_sReset.tv_nsec );
		}
	else
		l_sTime.tv_nsec = l_sReset.tv_nsec - f_lMoment[ UNIT::D_NANOSECOND ];
	int long elapsed = 0;
	switch ( unit )
		{
		case ( UNIT::D_SECOND ): elapsed = l_sTime.tv_sec; break;
		case ( UNIT::D_NANOSECOND ): elapsed = l_sTime.tv_sec * D_NANO_IN_WHOLE + l_sTime.tv_nsec; break;
		case ( UNIT::D_MICROSECOND ): elapsed = l_sTime.tv_sec * D_MICRO_IN_WHOLE + l_sTime.tv_nsec / D_NANO_IN_MICRO; break;
		case ( UNIT::D_MILISECOND ): elapsed = l_sTime.tv_sec * D_MILI_IN_WHOLE + l_sTime.tv_nsec / D_NANO_IN_MILI; break;
		default: M_ASSERT( ! "bad HClock::UNIT!" );
		}
	if ( reset )
		{
		f_lMoment[ UNIT::D_SECOND ] = l_sReset.tv_sec;
		f_lMoment[ UNIT::D_NANOSECOND ] = l_sReset.tv_nsec;
		}
	return ( elapsed );
	M_EPILOG
	}

}

}

