/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htime.cxx - this file is integral part of `yaal' project.

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

#include <cstring>

#include "config.hxx"

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "htime.hxx"

namespace yaal
{

namespace hcore
{

char const* const n_pcDefaultTimeFormat = "%a, %d %b %Y %H:%M:%S %z";

HTime::HTime( void ) : f_oFormat ( n_pcDefaultTimeFormat ),
	f_oCache( 1, HChunk::STRATEGY::GEOMETRIC ), f_xValue(), f_sBroken()
	{
	M_PROLOG
	set_now();
	return;
	M_EPILOG
	}

HTime::HTime( char const* const a_pcStrTime )
	: f_oFormat ( n_pcDefaultTimeFormat ),
	f_oCache( 1, HChunk::STRATEGY::GEOMETRIC ), f_xValue(), f_sBroken()
	{
	M_PROLOG
	char* l_pcErr = ::strptime( a_pcStrTime, f_oFormat.raw(), &f_sBroken );
	if ( ! l_pcErr )
		l_pcErr = ::strptime( a_pcStrTime, "%F %T", &f_sBroken );
	M_ENSURE( l_pcErr );
	f_xValue = ::mktime( &f_sBroken );
	return;
	M_EPILOG
	}

HTime::HTime( HTime const& a_roTime ) : f_oFormat( n_pcDefaultTimeFormat ),
	f_oCache( 1, HChunk::STRATEGY::GEOMETRIC ), f_xValue(), f_sBroken()
	{
	M_PROLOG
	operator = ( a_roTime );
	return;
	M_EPILOG
	}

HTime::HTime( time_t const& a_rxTime ) : f_oFormat( n_pcDefaultTimeFormat ),
	f_oCache( 1, HChunk::STRATEGY::GEOMETRIC ), f_xValue( a_rxTime ), f_sBroken()
	{
	M_PROLOG
	M_ENSURE( localtime_r( &f_xValue, &f_sBroken ) );
	return;
	M_EPILOG
	}

HTime::HTime( int const a_iYear, int const a_iMonth, int const a_iDay,
							 int const a_iHour, int const a_iMinute, int const a_iSecond )
	: f_oFormat( n_pcDefaultTimeFormat ), f_oCache( 1, HChunk::STRATEGY::GEOMETRIC ), f_xValue(),
	f_sBroken()
	{
	M_PROLOG
	set_datetime( a_iYear, a_iMonth, a_iDay, a_iHour, a_iMinute, a_iSecond );
	return;
	M_EPILOG
	}

HTime::~HTime( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HTime::set( time_t const& a_rxTime )
	{
	M_PROLOG
	f_xValue = a_rxTime;
	M_ENSURE( localtime_r( &f_xValue, &f_sBroken ) );
	return;
	M_EPILOG
	}

void HTime::set_now ( void )
	{
	M_PROLOG
	f_xValue = time( NULL );
	M_ENSURE( localtime_r( &f_xValue, &f_sBroken ) );
	return;
	M_EPILOG
	}

void HTime::format( char const * const a_pcFormat )
	{
	M_PROLOG
	f_oFormat = a_pcFormat;
	return;
	M_EPILOG
	}

void HTime::set_time( int const a_iHour, int const a_iMinute,
											 int const a_iSecond )
	{
	M_PROLOG
	if ( ( a_iHour < 0 ) || ( a_iHour > 23 ) )
		M_THROW( "bad hour", a_iHour );
	f_sBroken.tm_hour = a_iHour;
	if ( ( a_iMinute < 0 ) || ( a_iMinute > 59 ) )
		M_THROW( "bad minute", a_iMinute );
	f_sBroken.tm_min = a_iMinute;
	if ( ( a_iSecond < 0 ) || ( a_iSecond > 59 ) )
		M_THROW( "bad second", a_iSecond );
	f_sBroken.tm_sec = a_iSecond;
	f_xValue = mktime( &f_sBroken );
	return;
	M_EPILOG
	}

void HTime::set_date( int const a_iYear, int const a_iMonth,
											 int const a_iDay )
	{
	M_PROLOG
	f_sBroken.tm_year = a_iYear - 1900;
	if ( ( a_iMonth < 1 ) || ( a_iMonth > 12 ) )
		M_THROW( "bad month in year", a_iMonth );
	f_sBroken.tm_mon = a_iMonth - 1;
	if ( ( a_iDay < 1 ) || ( a_iDay > 31 ) )
		M_THROW( "bad day of month", a_iDay );
	f_sBroken.tm_mday = a_iDay;
	f_xValue = mktime( &f_sBroken );
	return;
	M_EPILOG
	}

void HTime::set_datetime( int const a_iYear, int const a_iMonth,
											 int const a_iDay, int const a_iHour,
											 int const a_iMinute, int const a_iSecond )
	{
	M_PROLOG
	set_date( a_iYear, a_iMonth, a_iDay );
	set_time( a_iHour, a_iMinute, a_iSecond );
	return;
	M_EPILOG
	}

int HTime::get_year( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_year + 1900 );
	M_EPILOG
	}

int HTime::get_month( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_mon + 1 );
	M_EPILOG
	}

int HTime::get_day( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_mday );
	M_EPILOG
	}

int HTime::get_hour( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_hour );
	M_EPILOG
	}

int HTime::get_minute( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_min );
	M_EPILOG
	}

int HTime::get_second( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_sec );
	M_EPILOG
	}

HTime& HTime::operator = ( HTime const& a_roTime )
	{
	M_PROLOG
	if ( this != & a_roTime )
		{
		f_oFormat = a_roTime.f_oFormat;
		f_xValue = a_roTime.f_xValue;
		f_oCache.raw()[ 0 ] = 0;
		memcpy ( &f_sBroken, &a_roTime.f_sBroken, sizeof ( tm ) );
		}
	return ( * this );
	M_EPILOG
	}

HTime HTime::operator - ( HTime const& a_roTime ) const
	{
	M_PROLOG
	HTime l_oTime;
	l_oTime.f_oFormat = f_oFormat;
	l_oTime.f_xValue = static_cast<time_t>( difftime( f_xValue, a_roTime.f_xValue ) );
	M_ENSURE( gmtime_r( &l_oTime.f_xValue, &l_oTime.f_sBroken ) );
	return ( l_oTime );
	M_EPILOG
	}

HTime & HTime::operator -= ( HTime const& a_roTime )
	{
	M_PROLOG
	( *this ) = ( *this ) - a_roTime;
	return ( *this );
	M_EPILOG
	}

bool HTime::operator == ( time_t const& a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue == a_rxTime );
	M_EPILOG
	}

bool HTime::operator != ( time_t const& a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue != a_rxTime );
	M_EPILOG
	}

bool HTime::operator <= ( time_t const& a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue <= a_rxTime );
	M_EPILOG
	}

bool HTime::operator >= ( time_t const& a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue >= a_rxTime );
	M_EPILOG
	}

bool HTime::operator < ( time_t const& a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue < a_rxTime );
	M_EPILOG
	}

bool HTime::operator > ( time_t const& a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue > a_rxTime );
	M_EPILOG
	}

HTime::operator char const* ( void ) const
	{
	M_PROLOG
	int long l_iSize = 0;
#ifdef HAVE_SMART_STRFTIME
	l_iSize = ::strftime( NULL, 1024, f_oFormat.raw(), &f_sBroken ) + 1;
	if ( l_iSize < 2 )
		M_THROW( "bad format", errno );
	f_oCache.realloc( l_iSize );
	M_ENSURE( static_cast<int>( ::strftime( f_oCache.raw(),
					l_iSize, f_oFormat.raw(), &f_sBroken ) ) < l_iSize );
#else /* HAVE_SMART_STRFTIME */
	f_oCache.realloc( 64 ); /* FIXME that is pretty dumb hack */
	l_iSize = ::strftime( f_oCache.raw(), 63, f_oFormat.raw(), &f_sBroken ) + 1;
	if ( l_iSize < 2 )
		M_THROW( "bad format", errno );
#endif /* not HAVE_SMART_STRFTIME */
	return ( f_oCache.raw() );
	M_EPILOG
	}

HTime::operator time_t ( void ) const
	{
	return ( f_xValue );
	}

}

}

