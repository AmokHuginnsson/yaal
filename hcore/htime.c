/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	htime.c - this file is integral part of `stdhapi' project.

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

#include <string.h>

#include "config.h"

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "htime.h"

namespace stdhapi
{

namespace hcore
{

HTime::HTime ( void ) : f_oFormat ( D_DEFAULT_TIME_FORMAT ),
	f_oBuffer ( ), f_xValue ( ), f_sBroken ( )
	{
	M_PROLOG
	set_now ( );
	return;
	M_EPILOG
	}

HTime::HTime ( char const * a_pcStrTime ) : f_oFormat ( D_DEFAULT_TIME_FORMAT ),
	f_oBuffer ( ), f_xValue ( ), f_sBroken ( )
	{
	M_PROLOG
	char * l_pcErr = strptime ( a_pcStrTime, f_oFormat, & f_sBroken );
	if ( ! l_pcErr )
		l_pcErr = strptime ( a_pcStrTime, "%F %T", & f_sBroken );
	if ( ! l_pcErr )
		M_THROW ( strerror ( g_iErrNo ), g_iErrNo );
	f_xValue = timelocal ( & f_sBroken );
	return;
	M_EPILOG
	}

HTime::HTime ( const HTime & a_roTime ) : f_oFormat ( D_DEFAULT_TIME_FORMAT ),
	f_oBuffer ( ), f_xValue ( ), f_sBroken ( )
	{
	M_PROLOG
	( * this ) = a_roTime;
	return;
	M_EPILOG
	}

HTime::HTime ( const time_t & a_rxTime ) : f_oFormat ( D_DEFAULT_TIME_FORMAT ),
	f_oBuffer ( ), f_xValue ( a_rxTime ), f_sBroken ( )
	{
	M_PROLOG
	M_IRV ( localtime_r ( & f_xValue, & f_sBroken ) );
	return;
	M_EPILOG
	}

HTime::HTime ( const int a_iYear, const int a_iMonth, const int a_iDay,
							 const int a_iHour, const int a_iMinute, const int a_iSecond )
	: f_oFormat ( D_DEFAULT_TIME_FORMAT ), f_oBuffer ( ), f_xValue ( ),
	f_sBroken ( )
	{
	M_PROLOG
	set_datetime ( a_iYear, a_iMonth, a_iDay, a_iHour, a_iMinute, a_iSecond );
	return;
	M_EPILOG
	}

HTime::~HTime ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HTime::set_now ( void )
	{
	M_PROLOG
	f_xValue = time ( NULL );
	M_IRV ( localtime_r ( & f_xValue, & f_sBroken ) );
	return;
	M_EPILOG
	}

void HTime::format ( char const * a_pcFormat )
	{
	M_PROLOG
	f_oFormat = a_pcFormat;
	return;
	M_EPILOG
	}

void HTime::set_time ( const int a_iHour, const int a_iMinute,
											 const int a_iSecond )
	{
	M_PROLOG
	if ( ( a_iHour < 0 ) || ( a_iHour > 23 ) )
		M_THROW ( "bad hour", a_iHour );
	f_sBroken.tm_hour = a_iHour;
	if ( ( a_iMinute < 0 ) || ( a_iMinute > 59 ) )
		M_THROW ( "bad minute", a_iMinute );
	f_sBroken.tm_min = a_iMinute;
	if ( ( a_iSecond < 0 ) || ( a_iSecond > 59 ) )
		M_THROW ( "bad second", a_iSecond );
	f_sBroken.tm_sec = a_iSecond;
	f_xValue = mktime ( & f_sBroken );
	return;
	M_EPILOG
	}

void HTime::set_date ( const int a_iYear, const int a_iMonth,
											 const int a_iDay )
	{
	M_PROLOG
	f_sBroken.tm_year = a_iYear - 1900;
	if ( ( a_iMonth < 1 ) || ( a_iMonth > 12 ) )
		M_THROW ( "bad month in year", a_iMonth );
	f_sBroken.tm_mon = a_iMonth - 1;
	if ( ( a_iDay < 1 ) || ( a_iDay > 31 ) )
		M_THROW ( "bad day of month", a_iDay );
	f_sBroken.tm_mday = a_iDay;
	f_xValue = mktime ( & f_sBroken );
	return;
	M_EPILOG
	}

void HTime::set_datetime ( const int a_iYear, const int a_iMonth,
											 const int a_iDay, const int a_iHour,
											 const int a_iMinute, const int a_iSecond )
	{
	M_PROLOG
	set_date ( a_iYear, a_iMonth, a_iDay );
	set_time ( a_iHour, a_iMinute, a_iSecond );
	return;
	M_EPILOG
	}

int HTime::get_year ( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_year + 1900 );
	M_EPILOG
	}

int HTime::get_month ( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_mon + 1 );
	M_EPILOG
	}

int HTime::get_day ( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_mday );
	M_EPILOG
	}

int HTime::get_hour ( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_hour );
	M_EPILOG
	}

int HTime::get_minute ( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_min );
	M_EPILOG
	}

int HTime::get_second ( void ) const
	{
	M_PROLOG
	return ( f_sBroken.tm_sec );
	M_EPILOG
	}

HTime & HTime::operator = ( const HTime & a_roTime )
	{
	M_PROLOG
	if ( this != & a_roTime )
		{
		f_oFormat = a_roTime.f_oFormat;
		f_xValue = a_roTime.f_xValue;
		f_oBuffer = a_roTime.f_oBuffer;
		memcpy ( & f_sBroken, & a_roTime.f_sBroken, sizeof ( tm ) );
		}
	return ( * this );
	M_EPILOG
	}

HTime HTime::operator - ( const HTime & a_roTime ) const
	{
	M_PROLOG
	HTime l_oTime;
	l_oTime.format ( f_oFormat );
	l_oTime.f_xValue = static_cast < time_t > ( difftime ( f_xValue, a_roTime.f_xValue ) );
	M_IRV ( gmtime_r ( & l_oTime.f_xValue, & l_oTime.f_sBroken ) );
	return ( l_oTime );
	M_EPILOG
	}

HTime & HTime::operator -= ( const HTime & a_roTime )
	{
	M_PROLOG
	( * this ) = ( * this ) - a_roTime;
	return ( * this );
	M_EPILOG
	}

bool HTime::operator == ( const time_t & a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue == a_rxTime );
	M_EPILOG
	}

bool HTime::operator != ( const time_t & a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue != a_rxTime );
	M_EPILOG
	}

bool HTime::operator <= ( const time_t & a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue <= a_rxTime );
	M_EPILOG
	}

bool HTime::operator >= ( const time_t & a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue >= a_rxTime );
	M_EPILOG
	}

bool HTime::operator < ( const time_t & a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue < a_rxTime );
	M_EPILOG
	}

bool HTime::operator > ( const time_t & a_rxTime ) const
	{
	M_PROLOG
	return ( f_xValue > a_rxTime );
	M_EPILOG
	}

HTime::operator char const * ( void ) const
	{
	M_PROLOG
	int l_iSize = 0;
#ifdef HAVE_SMART_STRFTIME
	l_iSize = static_cast < int > ( strftime ( NULL, 1024, f_oFormat, & f_sBroken ) + 1 );
	if ( l_iSize < 2 )
		M_THROW ( "bad format", g_iErrNo );
	f_oBuffer.hs_realloc ( l_iSize );
	M_IRV ( strftime ( f_oBuffer, static_cast < unsigned int > ( l_iSize ),
			f_oFormat, & f_sBroken ) );
#else /* HAVE_SMART_STRFTIME */
	f_oBuffer.hs_realloc ( 64 ); /* FIXME that is pretty dumb hack */
	l_iSize = strftime ( f_oBuffer, 63, f_oFormat, & f_sBroken ) + 1;
	if ( l_iSize < 2 )
		M_THROW ( "bad format", g_iErrNo );
#endif /* not HAVE_SMART_STRFTIME */
	return ( f_oBuffer );
	M_EPILOG
	}

HTime::operator time_t ( void ) const
	{
	return ( f_xValue );
	}

}

}

