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

#include "htime.h"

#include "hexception.h"

HTime::HTime ( const char * a_pcFormat )
	{
	M_PROLOG
	f_oFormat = a_pcFormat;
	f_xValue = time ( NULL );
	localtime_r ( & f_xValue, & f_sBroken );
	return;
	M_EPILOG
	}

HTime::HTime ( const HTime & a_roTime, int ) : HObject ( )
	{
	M_PROLOG
	( * this ) = a_roTime;
	return;
	M_EPILOG
	}

HTime::HTime ( const int a_iYear, const int a_iMonth, const int a_iDay,
							 const int a_iHour, const int a_iMinute, const int a_iSecond )
	{
	M_PROLOG
	f_oFormat = "%a, %d %b %Y %T %z";
	set_time ( a_iYear, a_iMonth, a_iDay, a_iHour, a_iMinute, a_iSecond );
	return;
	M_EPILOG
	}

HTime::~HTime ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HObject * HTime::clone ( void ) const
	{
	M_PROLOG
	HObject * l_poObject = NULL;
	l_poObject = new HTime ( * this );
	if ( ! l_poObject )
		throw new HException ( __WHERE__, "new returned", ( int ) l_poObject );
	return ( l_poObject );
	M_EPILOG
	}

void HTime::format ( const char * a_pcFormat )
	{
	M_PROLOG
	f_oFormat = a_pcFormat;
	return;
	M_EPILOG
	}

void HTime::set_time ( const int a_iYear, const int a_iMonth,
											 const int a_iDay, const int a_iHour,
											 const int a_iMinute, const int a_iSecond )
	{
	M_PROLOG
	f_sBroken.tm_year = a_iYear - 1900;
	if ( ( a_iMonth < 1 ) || ( a_iMonth > 12 ) )
		throw new HException ( __WHERE__, "bad month in year", a_iMonth );
	f_sBroken.tm_mon = a_iMonth - 1;
	if ( ( a_iDay < 1 ) || ( a_iDay > 31 ) )
		throw new HException ( __WHERE__, "bad day of month", a_iDay );
	f_sBroken.tm_mday = a_iDay;
	if ( ( a_iHour < 0 ) || ( a_iHour > 23 ) )
		throw new HException ( __WHERE__, "bad hour", a_iHour );
	f_sBroken.tm_hour = a_iHour;
	if ( ( a_iMinute < 0 ) || ( a_iMinute > 59 ) )
		throw new HException ( __WHERE__, "bad minute", a_iMinute );
	f_sBroken.tm_min = a_iMinute;
	if ( ( a_iSecond < 0 ) || ( a_iSecond > 59 ) )
		throw new HException ( __WHERE__, "bad second", a_iSecond );
	f_sBroken.tm_sec = a_iSecond;
	f_xValue = mktime ( & f_sBroken );
	return;
	M_EPILOG
	}

int HTime::get_year ( void )
	{
	M_PROLOG
	return ( f_sBroken.tm_year + 1900 );
	M_EPILOG
	}

int HTime::get_month ( void )
	{
	M_PROLOG
	return ( f_sBroken.tm_mon + 1 );
	M_EPILOG
	}

int HTime::get_day ( void )
	{
	M_PROLOG
	return ( f_sBroken.tm_mday );
	M_EPILOG
	}

int HTime::get_hour ( void )
	{
	M_PROLOG
	return ( f_sBroken.tm_hour );
	M_EPILOG
	}

int HTime::get_minute ( void )
	{
	M_PROLOG
	return ( f_sBroken.tm_min );
	M_EPILOG
	}

int HTime::get_second ( void )
	{
	M_PROLOG
	return ( f_sBroken.tm_sec );
	M_EPILOG
	}

HTime & HTime::operator = ( const HTime & a_roTime )
	{
	M_PROLOG
	f_oFormat = a_roTime.f_oFormat;
	f_xValue = a_roTime.f_xValue;
	memcpy ( & f_sBroken, & a_roTime.f_sBroken, sizeof ( tm ) );
	return ( * this );
	M_EPILOG
	}

const char * HTime::operator = ( const char * a_pcStrTime )
	{
	M_PROLOG
	char * l_pcErr = strptime ( a_pcStrTime, f_oFormat, & f_sBroken );
	if ( ! l_pcErr )l_pcErr = strptime ( a_pcStrTime, "%F %T", & f_sBroken );
	if ( ! l_pcErr )
		throw new HException ( __WHERE__, strerror ( g_iErrNo ), g_iErrNo );
	f_xValue = timelocal ( & f_sBroken );
	return ( a_pcStrTime );
	M_EPILOG
	}

time_t HTime::operator = ( const time_t & a_rxTime )
	{
	M_PROLOG
	f_xValue = a_rxTime;
	localtime_r ( & f_xValue, & f_sBroken );
	return ( f_xValue );
	M_EPILOG
	}

bool HTime::operator == ( const time_t & a_rxTime )
	{
	M_PROLOG
	return ( f_xValue == a_rxTime );
	M_EPILOG
	}

bool HTime::operator != ( const time_t & a_rxTime )
	{
	M_PROLOG
	return ( f_xValue != a_rxTime );
	M_EPILOG
	}

bool HTime::operator <= ( const time_t & a_rxTime )
	{
	M_PROLOG
	return ( f_xValue <= a_rxTime );
	M_EPILOG
	}

bool HTime::operator >= ( const time_t & a_rxTime )
	{
	M_PROLOG
	return ( f_xValue >= a_rxTime );
	M_EPILOG
	}

bool HTime::operator < ( const time_t & a_rxTime )
	{
	M_PROLOG
	return ( f_xValue < a_rxTime );
	M_EPILOG
	}

bool HTime::operator > ( const time_t & a_rxTime )
	{
	M_PROLOG
	return ( f_xValue > a_rxTime );
	M_EPILOG
	}

HTime::operator HString & ( void )
	{
	M_PROLOG
	int l_iSize = 0;
	l_iSize = strftime ( NULL, 1024, f_oFormat, & f_sBroken ) + 1;
	if ( l_iSize < 2 )throw new HException ( __WHERE__, "bad format", g_iErrNo );
	f_oBuffer.hs_realloc ( l_iSize );
	strftime ( f_oBuffer, l_iSize, f_oFormat, & f_sBroken );
	return ( f_oBuffer );
	M_EPILOG
	}

HTime::operator const char * ( void )
	{
	M_PROLOG
	return ( ( HString & ) * this );
	M_EPILOG
	}

HTime::operator time_t ( void )
	{
	return ( f_xValue );
	}

