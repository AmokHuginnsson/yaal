/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstring.c - this file is integral part of `stdhapi' project.

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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hstring.h"
#include "xalloc.h"

HString::HString ( void )
	{
	M_PROLOG
	f_pcBuffer = 0;
	f_iSize = 0;
	return;
	M_EPILOG
	}

HString::~HString ( void )
	{
	M_PROLOG
	if ( f_pcBuffer )
		xfree ( f_pcBuffer );
	return;
	M_EPILOG
	}

HString::HString ( const HString & a_roString ) : HObject ( )
	{
	M_PROLOG
	f_pcBuffer = NULL;
	f_iSize = 0;
	( * this ) = a_roString;
	return;
	M_EPILOG
	}

HString::HString ( const size_t a_lSize )
	{
	M_PROLOG
	f_pcBuffer = 0;
	f_iSize = 0;
	hs_realloc ( a_lSize + 1 );
	memset ( f_pcBuffer, 0, a_lSize + 1 );
	return;
	M_EPILOG
	}

void HString::hs_realloc ( const size_t a_iSize )
	{
	M_PROLOG
	int l_iOldLength = 0;
	if ( a_iSize < 1 )throw new HException ( __WHERE__, "bad new size", a_iSize );
	if ( a_iSize > f_iSize )
		{
		l_iOldLength = f_iSize;
		f_iSize = 1;
		while ( f_iSize < a_iSize )f_iSize <<= 1;
		f_pcBuffer = ( char * ) xrealloc ( f_pcBuffer, f_iSize );
		memset ( f_pcBuffer + l_iOldLength, 0, f_iSize - l_iOldLength );
		}
	return;
	M_EPILOG
	}

HObject * HString::clone ( void ) const
	{
	M_PROLOG
	HString * l_poObject = 0;
	l_poObject = new HString ( * this );
	if ( ! l_poObject )
		throw new HException ( __WHERE__, "new returned", ( int ) l_poObject );
	return ( l_poObject );
	M_EPILOG
	}

HString::HString ( const char * a_pcStr )
	{
	M_PROLOG
	f_pcBuffer = 0;
	f_iSize = 0;
	if ( a_pcStr )
		{
		hs_realloc ( strlen ( a_pcStr ) + 1 );
		strcpy ( f_pcBuffer, a_pcStr );
		}
	return;
	M_EPILOG
	}

HString::HString ( char a_cChar )
	{
	M_PROLOG
	f_pcBuffer = 0;
	f_iSize = 0;
	hs_realloc ( 2 );
	f_pcBuffer [ 0 ] = a_cChar;
	f_pcBuffer [ 1 ] = 0;
	return;
	M_EPILOG
	}

HString::HString ( int a_iInt )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	f_pcBuffer = 0;
	f_iSize = 0;
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%d", a_iInt ) + 1;
	hs_realloc ( l_iSize );
	snprintf ( f_pcBuffer, l_iSize, "%d", a_iInt );
	return;
	M_EPILOG
	}

HString::HString ( long int a_lLong )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	f_pcBuffer = 0;
	f_iSize = 0;
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%ld", a_lLong ) + 1;
	hs_realloc ( l_iSize );
	snprintf ( f_pcBuffer, l_iSize, "%ld", a_lLong );
	return;
	M_EPILOG
	}

HString::HString ( double a_dDouble )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	f_pcBuffer = 0;
	f_iSize = 0;
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%f", a_dDouble ) + 1;
	hs_realloc ( l_iSize );
	snprintf ( f_pcBuffer, l_iSize, "%f", a_dDouble );
	return;
	M_EPILOG
	}

HString::HString ( const void * a_pvPtrVoid )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	f_pcBuffer = 0;
	f_iSize = 0;
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%p", a_pvPtrVoid ) + 1;
	hs_realloc ( l_iSize );
	snprintf ( f_pcBuffer, l_iSize, "%p", a_pvPtrVoid );
	return;
	M_EPILOG
	}

HString & HString::operator = ( const HString & a_roString )
	{
	M_PROLOG
	return ( * this = a_roString.f_pcBuffer );
	M_EPILOG
	}

HString & HString::operator = ( const char * a_pcStr )
	{
	M_PROLOG
	int l_iLenght = 0;
	if ( a_pcStr )
		{
		l_iLenght = strlen ( a_pcStr ) + 1;
		hs_realloc ( l_iLenght );
		strcpy ( f_pcBuffer, a_pcStr );
		}
	return ( * this );
	M_EPILOG
	}

HString & HString::operator = ( const char a_cChar )
	{
	M_PROLOG
	hs_realloc ( 2 );
	f_pcBuffer [ 0 ] = a_cChar;
	f_pcBuffer [ 1 ] = 0;
	return ( * this );
	M_EPILOG
	}

HString & HString::operator = ( const int a_iInt )
	{
	M_PROLOG
	( * this ) = HString ( a_iInt );
	return ( * this );
	M_EPILOG
	}

HString & HString::operator = ( const long int a_lLong )
	{
	M_PROLOG
	( * this ) = HString ( a_lLong );
	return ( * this );
	M_EPILOG
	}

HString & HString::operator = ( const double a_dDouble )
	{
	M_PROLOG
	( * this ) = HString ( a_dDouble );
	return ( * this );
	M_EPILOG
	}

HString & HString::operator = ( const void * a_pvVoidPtr )
	{
	M_PROLOG
	( * this ) = HString ( a_pvVoidPtr );
	return ( * this );
	M_EPILOG
	}

HString HString::operator + ( const HString & a_roString )
	{
	M_PROLOG
	return ( ( * this ) + ( const char * ) a_roString );
	M_EPILOG
	}

HString HString::operator + ( const char * a_pcStr )
	{
	M_PROLOG
	int l_iLength = 0;
	HString str;
	l_iLength =	( f_pcBuffer ? get_length () : 0 ) 
							+ ( a_pcStr ? strlen ( a_pcStr ) : 0 );
	if ( ! l_iLength )return ( str = "" );
	str.hs_realloc ( l_iLength + 1 );
	str.f_pcBuffer [ 0 ] = 0;
	if ( f_pcBuffer )strcpy ( str.f_pcBuffer, f_pcBuffer );
	if ( a_pcStr )strcat ( str.f_pcBuffer, a_pcStr );
	return ( str );
	M_EPILOG
	}

HString HString::operator + ( const char a_cChar )
	{
	M_PROLOG
	int len = 0;
	HString l_oStr;
	l_oStr = * this;
	l_oStr.hs_realloc ( ( len = ( f_pcBuffer ? strlen ( f_pcBuffer ) : 0 ) ) + 2 );
	l_oStr.f_pcBuffer [ len ] = a_cChar;
	l_oStr.f_pcBuffer [ len + 1 ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString HString::operator + ( const int a_iInt )
	{
	M_PROLOG
	return ( * this + ( long ) a_iInt );
	M_EPILOG
	}

HString HString::operator + ( const long int a_lLong )
	{
	M_PROLOG
	HString l_oStr;
	HString l_oLong;
	l_oStr = ( f_pcBuffer ? ( const char * ) * this : "" );
	l_oLong = a_lLong;
	return ( l_oStr + l_oLong );
	M_EPILOG
	}

HString HString::operator + ( const double a_dDouble )
	{
	M_PROLOG
	HString l_oStr;
	HString l_oDouble;
	l_oStr = ( f_pcBuffer ? ( const char * ) * this : "" );
	l_oDouble = a_dDouble;
	return ( l_oStr + l_oDouble );
	M_EPILOG
	}

HString HString::operator + ( const void * a_pvVoidPtr )
	{
	M_PROLOG
	HString l_oStr;
	HString l_oVoidPtr;
	l_oStr = ( f_pcBuffer ? ( const char * ) * this : "" );
	l_oVoidPtr = a_pvVoidPtr;
	return ( l_oStr + l_oVoidPtr );
	M_EPILOG
	}

HString & HString::operator += ( const HString & a_roString )
	{
	M_PROLOG
	return ( ( * this ) = ( ( * this ) + a_roString ) );
	M_EPILOG
	}

HString & HString::operator += ( const char * a_pcStr )
	{
	M_PROLOG
	return ( ( * this ) = ( ( * this ) + a_pcStr ) );
	M_EPILOG
	}

HString & HString::operator += ( const char a_cChar )
	{
	M_PROLOG
	return ( ( * this ) = ( ( * this ) + a_cChar ) );
	M_EPILOG
	}

HString & HString::operator += ( const int a_iInt )
	{
	M_PROLOG
	return ( * this = * this + a_iInt );
	M_EPILOG
	}

HString & HString::operator += ( const long int a_lLong )
	{
	M_PROLOG
	return ( * this = * this + a_lLong );
	M_EPILOG
	}

HString & HString::operator += ( const double a_dDouble )
	{
	M_PROLOG
	return ( * this = * this + a_dDouble );
	M_EPILOG
	}

HString & HString::operator += ( const void * a_pvVoidPtr )
	{
	M_PROLOG
	return ( * this = * this + a_pvVoidPtr );
	M_EPILOG
	}

char & HString::operator [ ] ( const int a_iIndex )
	{
	M_PROLOG
	int l_iLength = get_length ( );
	if  ( ( ( size_t ) a_iIndex >= f_iSize ) || ( a_iIndex > l_iLength ) )
		throw new HException ( __WHERE__, "index out of bound", a_iIndex );
	return ( f_pcBuffer [ a_iIndex ] );
	M_EPILOG
	}

bool HString::operator == ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this == ( const char * ) a_roString );
	M_EPILOG
	}

bool HString::operator == ( const char * a_pcStr ) const
	{
	M_PROLOG
	return ( ! strcmp ( f_pcBuffer, a_pcStr ) );
	M_EPILOG
	}

bool HString::operator != ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this != ( const char * ) a_roString );
	M_EPILOG
	}

bool HString::operator != ( const char * a_pcStr ) const
	{
	M_PROLOG
	return ( strcmp ( f_pcBuffer, a_pcStr ) ? true : false );
	M_EPILOG
	}

bool HString::operator >= ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this >= ( const char * ) a_roString );
	M_EPILOG
	}

bool HString::operator >= ( const char * a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = strcmp ( f_pcBuffer, a_pcStr );
	if ( l_iCmp >= 0 )return ( true );
	else return ( false );
	M_EPILOG
	}

bool HString::operator <= ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this <= ( const char * ) a_roString );
	M_EPILOG
	}

bool HString::operator <= ( const char * a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = strcmp ( f_pcBuffer, a_pcStr );
	if ( l_iCmp <= 0 )return ( true );
	else return ( false );
	M_EPILOG
	}

bool HString::operator > ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this > ( const char * ) a_roString );
	M_EPILOG
	}

bool HString::operator > ( const char * a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = strcmp ( f_pcBuffer, a_pcStr );
	if ( l_iCmp > 0 )return ( true );
	else return ( false );
	M_EPILOG
	}

bool HString::operator < ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this < ( const char * ) a_roString );
	M_EPILOG
	}

bool HString::operator < ( const char * a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = strcmp ( f_pcBuffer, a_pcStr );
	if ( l_iCmp < 0 )return ( true );
	else return ( false );
	M_EPILOG
	}

HString::operator char * ( void ) const
	{
	M_PROLOG
	return ( f_pcBuffer );
	M_EPILOG
	}

bool HString::is_empty ( void )
	{
	return ( ! ( f_pcBuffer && f_pcBuffer [ 0 ] ) );
	}

int HString::get_length ( void ) const
	{
	M_PROLOG
	int l_iLength = 0;
	if ( ! f_pcBuffer )
		throw new HException ( __WHERE__, "no buffer.", ( int ) f_pcBuffer );
	l_iLength = strlen ( f_pcBuffer );
	if ( ( size_t ) l_iLength > f_iSize )
		throw new HException ( __WHERE__, "no terminating null!", l_iLength );
	return ( l_iLength );
	M_EPILOG
	}

HString & HString::format ( const char * a_pcFormat, ... )
	{
	M_PROLOG
	int l_iSize = 0;
	va_list ap;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	va_start ( ap, a_pcFormat );
	l_iSize = vsnprintf ( l_pcMeasureBuffer, 1, a_pcFormat, ap );
	if ( l_iSize < 1 )return ( * this );
	l_iSize ++;
	va_end ( ap );
	hs_realloc ( l_iSize );
	va_start ( ap, a_pcFormat );
	vsprintf ( f_pcBuffer, a_pcFormat, ap );
	va_end ( ap );
	return ( * this );
	M_EPILOG
	}

int HString::find ( char a_cChar, int a_iAfter )
	{
	M_PROLOG
	if ( ( a_iAfter < 0 ) || ( a_iAfter >= ( int ) strlen ( f_pcBuffer ) ) ) return ( -1 );
	char * l_pcStr = strchr ( f_pcBuffer + a_iAfter, a_cChar );
	if ( ! l_pcStr )return ( -1 );
	return ( ( int ) ( l_pcStr - f_pcBuffer ) );
	M_EPILOG
	}

int HString::find ( const char * a_pcPattern, int a_iAfter )
	{
	M_PROLOG
	if ( ( ! a_pcPattern ) || ( a_iAfter < 0 ) ) return ( -1 );
	if ( ( ! strlen ( a_pcPattern ) ) || ( ( int ) strlen ( f_pcBuffer ) <= a_iAfter ) )return ( -1 );
	char * l_pcStr = strstr ( f_pcBuffer + a_iAfter, a_pcPattern );
	if ( ! l_pcStr )return ( -1 );
	return ( ( int ) ( l_pcStr - f_pcBuffer ) );
	M_EPILOG
	}

int HString::reverse_find ( char a_cChar, int a_iBefore )
	{
	M_PROLOG
	
	if ( ( a_iBefore < 0 ) || ( a_iBefore >= ( int ) strlen ( f_pcBuffer ) ) ) return ( -1 );
	char l_cTmp = * ( f_pcBuffer + a_iBefore );
	* ( f_pcBuffer + a_iBefore ) = 0;
	char * l_pcStr = strchr ( f_pcBuffer, a_cChar );
	* ( f_pcBuffer + a_iBefore ) = l_cTmp;
	if ( ! l_pcStr )return ( -1 );
	return ( ( int ) ( l_pcStr - f_pcBuffer ) );
	M_EPILOG
	}

HString & HString::replace ( const char * a_pcPattern, const char * a_pcWith )
	{
	M_PROLOG
	char * l_pcTmp = 0, * l_pcStr;
	int l_iLenNew, l_iLenOld, l_iLenPattern, l_iLenWith, l_iSubWP, l_iCursor = 0, l_iIndex = 0;
	if ( ! a_pcPattern )return ( * this );
	l_iLenPattern = strlen ( a_pcPattern );
	if ( l_iLenPattern < 1 )return ( * this );
	l_iLenWith = strlen ( a_pcWith );
	l_iSubWP = l_iLenWith - l_iLenPattern;
	l_iLenOld = l_iLenNew = strlen ( f_pcBuffer );
	while ( ( l_iIndex = find ( a_pcPattern, l_iIndex ) ) > -1 )
		{
		l_iLenNew += l_iSubWP;
		l_iIndex ++;
		}
	hs_realloc ( l_iLenNew + 1 );
	l_pcTmp = xstrdup ( f_pcBuffer );
	l_pcStr = f_pcBuffer;
	f_pcBuffer = l_pcTmp;
	l_iIndex = 0;
	memset ( l_pcStr, 0, l_iLenNew );
	while ( ( l_iIndex = find ( a_pcPattern, l_iIndex ) ) > -1 )
		{
		strncat ( l_pcStr, f_pcBuffer + l_iCursor, l_iIndex - l_iCursor );
		strcat ( l_pcStr, a_pcWith );
		l_iCursor = l_iIndex + l_iLenPattern;
		l_iIndex ++;
		}
	strncat ( l_pcStr, f_pcBuffer + l_iCursor, l_iLenOld - l_iCursor );
	l_pcStr [ l_iLenNew ] = 0;
	l_pcTmp = f_pcBuffer;
	f_pcBuffer = l_pcStr;
	xfree ( l_pcTmp );
	return ( * this );
	M_EPILOG
	}

HString & HString::upper ( void )
	{
	M_PROLOG
	int l_iCtr, l_iLenght = strlen ( f_pcBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr++ )f_pcBuffer [ l_iCtr ] = toupper ( f_pcBuffer [ l_iCtr ] );
	return ( * this );
	M_EPILOG
	}

HString & HString::lower ( void )
	{
	M_PROLOG
	int l_iCtr, l_iLenght = strlen ( f_pcBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr++ )f_pcBuffer [ l_iCtr ] = tolower ( f_pcBuffer [ l_iCtr ] );
	return ( * this );
	M_EPILOG
	}

HString & HString::reverse ( void )
	{
	M_PROLOG
	char a_cChar;
	int l_iCtr, l_iLenght = strlen ( f_pcBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr++ )
		{
		a_cChar = toupper ( f_pcBuffer [ l_iCtr ] );
		if ( a_cChar == f_pcBuffer [ l_iCtr ] )f_pcBuffer [ l_iCtr ] = tolower ( f_pcBuffer [ l_iCtr ] );
		else f_pcBuffer [ l_iCtr ] = a_cChar;
		}
	return ( * this );
	M_EPILOG
	}

HString HString::left ( int a_iTo )
	{
	M_PROLOG
	int l_iLenght = strlen ( f_pcBuffer );
	HString l_oStr;
	if ( a_iTo < 1 )return ( l_oStr );
	l_iLenght = ( a_iTo < l_iLenght ? a_iTo : l_iLenght );
	l_oStr.hs_realloc ( l_iLenght + 1 );
	strncpy ( l_oStr.f_pcBuffer, f_pcBuffer, l_iLenght );
	l_oStr.f_pcBuffer [ l_iLenght ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString HString::mid ( int a_iFrom, int a_iLenght )
	{
	M_PROLOG
	int l_iLenOrig = strlen ( f_pcBuffer );
	HString l_oStr;
	if ( ( a_iLenght < 0 ) || ( a_iFrom < 0 ) || ( a_iFrom >= l_iLenOrig ) )
		return ( "" );
	if ( ( a_iLenght == 0 ) || ( ( a_iLenght + a_iFrom ) > l_iLenOrig ) )
		a_iLenght = l_iLenOrig - a_iFrom;
	l_oStr.hs_realloc ( a_iLenght + 1 );
	strncpy ( l_oStr.f_pcBuffer, f_pcBuffer + a_iFrom, a_iLenght );
	l_oStr.f_pcBuffer [ a_iLenght ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString HString::right ( int a_iFromEnd )
	{
	M_PROLOG
	int l_iLenght = strlen ( f_pcBuffer );
	HString l_oStr;
	if ( a_iFromEnd < 1 )return ( l_oStr );
	a_iFromEnd = ( a_iFromEnd < l_iLenght ? a_iFromEnd : l_iLenght );
	l_oStr.hs_realloc ( a_iFromEnd + 1 );
	strncpy ( l_oStr.f_pcBuffer, f_pcBuffer + l_iLenght - a_iFromEnd, l_iLenght );
	l_oStr.f_pcBuffer [ a_iFromEnd ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString & HString::trim_left ( const char * a_pcSet )
	{
	M_PROLOG
	int l_iCut = 0, l_iLenght;
	while ( strchr ( a_pcSet, f_pcBuffer [ l_iCut ] ) )l_iCut ++;
	if ( l_iCut )
		{
		l_iLenght = strlen ( f_pcBuffer );
		strcpy ( f_pcBuffer, f_pcBuffer + l_iCut );
		f_pcBuffer [ l_iLenght - l_iCut ] = 0;
		}
	return ( * this );
	M_EPILOG
	}

HString & HString::trim_right( const char * a_pcSet )
	{
	M_PROLOG
	int l_iCut = 0, l_iLenght;
	l_iLenght = strlen ( f_pcBuffer );
	while ( strchr ( a_pcSet, f_pcBuffer [ l_iLenght - l_iCut ] ) )l_iCut ++;
	if ( l_iCut )
		{
		f_pcBuffer [ l_iLenght - l_iCut + 1 ] = 0;
		}
	return ( * this );
	M_EPILOG
	}

HString HString::split ( const char * a_pcAt, int a_iPart )
	{
	M_PROLOG
	int l_iBegining = 0;
	int l_iSize = 0;
	int l_iCtr = 0;
	if ( f_iSize )
		{
		for ( ;	f_pcBuffer [ l_iBegining ] &&	( l_iCtr < a_iPart );	l_iCtr ++ )
			l_iBegining += strcspn ( f_pcBuffer + l_iBegining, a_pcAt ) + 1;
		if ( f_pcBuffer [ l_iBegining ] )
			{
			l_iSize = strcspn ( f_pcBuffer + l_iBegining, a_pcAt );
			return ( mid ( l_iBegining, l_iSize ) );
			}
		}
	return ( "" );;
	M_EPILOG
	}

HString operator + ( const char * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( HString ( a_pcStr ) + a_roString );
	M_EPILOG
	}

HString operator + ( const char a_cChar, const HString & a_roString )
	{
	M_PROLOG
	return ( HString ( a_cChar ) + a_roString );
	M_EPILOG
	}

HString operator + ( const int a_iInt , const HString & a_roString )
	{
	M_PROLOG
	return ( HString ( a_iInt ) + a_roString );
	M_EPILOG
	}

HString operator + ( const long int a_lLong, const HString & a_roString )
	{
	M_PROLOG
	return ( HString ( a_lLong ) + a_roString );	
	M_EPILOG
	}

HString operator + ( const double a_dDouble, const HString & a_roString )
	{
	M_PROLOG
	return ( HString ( a_dDouble ) + a_roString );
	M_EPILOG
	}

HString operator + ( const void * a_pvVoidPtr, const HString & a_roString )
	{
	M_PROLOG
	return ( HString ( a_pvVoidPtr ) + a_roString );
	M_EPILOG
	}

bool operator == ( const char * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString == a_pcStr );
	M_EPILOG
	}

bool operator != ( const char * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString != a_pcStr );
	M_EPILOG
	}

bool operator >= ( const char * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString <= a_pcStr );
	M_EPILOG
	}

bool operator <= ( const char * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString >= a_pcStr );
	M_EPILOG
	}

bool operator > ( const char * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString < a_pcStr );
	M_EPILOG
	}

bool operator < ( const char * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString > a_pcStr );
	M_EPILOG
	}

