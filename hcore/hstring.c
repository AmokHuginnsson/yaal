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

namespace stdhapi
{

namespace hcore
{

HString::HString ( void ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
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

HString::HString ( const HString & a_roString ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
	( * this ) = a_roString;
	return;
	M_EPILOG
	}

HString::HString ( const size_t a_lSize ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
	hs_realloc ( a_lSize + 1 );
	memset ( f_pcBuffer, 0, a_lSize + 1 );
	return;
	M_EPILOG
	}

void HString::hs_realloc ( const size_t a_iSize )
	{
	M_PROLOG
	int l_iOldLength = 0;
	if ( a_iSize < 1 )
		M_THROW ( "bad new size", a_iSize );
	if ( a_iSize > f_iSize )
		{
		l_iOldLength = f_iSize;
		f_iSize = 1;
		while ( f_iSize < a_iSize )f_iSize <<= 1;
		f_pcBuffer = xrealloc ( f_pcBuffer, f_iSize, char );
		memset ( f_pcBuffer + l_iOldLength, 0, f_iSize - l_iOldLength );
		}
	return;
	M_EPILOG
	}

HString::HString ( char const * a_pcStr ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
	if ( a_pcStr )
		{
		hs_realloc ( strlen ( a_pcStr ) + 1 );
		strcpy ( f_pcBuffer, a_pcStr );
		}
	return;
	M_EPILOG
	}

HString::HString ( char a_cChar ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
	hs_realloc ( 2 );
	f_pcBuffer [ 0 ] = a_cChar;
	f_pcBuffer [ 1 ] = 0;
	return;
	M_EPILOG
	}

HString::HString ( int a_iInt ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%d", a_iInt ) + 1;
	hs_realloc ( l_iSize );
	snprintf ( f_pcBuffer, l_iSize, "%d", a_iInt );
	return;
	M_EPILOG
	}

HString::HString ( long int a_lLong ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%ld", a_lLong ) + 1;
	hs_realloc ( l_iSize );
	snprintf ( f_pcBuffer, l_iSize, "%ld", a_lLong );
	return;
	M_EPILOG
	}

HString::HString ( double a_dDouble ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%f", a_dDouble ) + 1;
	hs_realloc ( l_iSize );
	snprintf ( f_pcBuffer, l_iSize, "%f", a_dDouble );
	return;
	M_EPILOG
	}

HString::HString ( const void * a_pvPtrVoid ) : f_pcBuffer ( NULL ), f_iSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%p", a_pvPtrVoid ) + 1;
	hs_realloc ( l_iSize );
	snprintf ( f_pcBuffer, l_iSize, "%p", a_pvPtrVoid );
	return;
	M_EPILOG
	}

HString & HString::operator = ( const HString & a_roString )
	{
	M_PROLOG
	int l_iLenght = 0;
	if ( ( this != & a_roString ) && a_roString.f_pcBuffer )
		{
		l_iLenght = strlen ( a_roString.f_pcBuffer ) + 1;
		hs_realloc ( l_iLenght );
		strcpy ( f_pcBuffer, a_roString.f_pcBuffer );
		}
	return ( * this );
	M_EPILOG
	}

HString HString::operator + ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( ( * this ) + static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

HString HString::operator + ( char const * a_pcStr ) const
	{
	M_PROLOG
	int l_iLength = 0;
	HString str;
	l_iLength =	( f_pcBuffer ? get_length () : 0 ) 
							+ ( a_pcStr ? strlen ( a_pcStr ) : 0 );
	if ( ! l_iLength )
		return ( str = "" );
	str.hs_realloc ( l_iLength + 1 );
	str.f_pcBuffer [ 0 ] = 0;
	if ( f_pcBuffer )
		strcpy ( str.f_pcBuffer, f_pcBuffer );
	if ( a_pcStr )
		strcat ( str.f_pcBuffer, a_pcStr );
	return ( str );
	M_EPILOG
	}

HString HString::operator + ( char const a_cChar ) const
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

HString HString::operator + ( const int a_iInt ) const
	{
	M_PROLOG
	return ( * this + static_cast < long > ( a_iInt ) );
	M_EPILOG
	}

HString HString::operator + ( const long int a_lLong ) const
	{
	M_PROLOG
	HString l_oStr;
	HString l_oLong;
	l_oStr = ( f_pcBuffer ? static_cast < char const * > ( * this ) : "" );
	l_oLong = a_lLong;
	return ( l_oStr + l_oLong );
	M_EPILOG
	}

HString HString::operator + ( const double a_dDouble ) const
	{
	M_PROLOG
	HString l_oStr;
	HString l_oDouble;
	l_oStr = ( f_pcBuffer ? static_cast < char const * > ( * this ) : "" );
	l_oDouble = a_dDouble;
	return ( l_oStr + l_oDouble );
	M_EPILOG
	}

HString HString::operator + ( const void * a_pvVoidPtr ) const
	{
	M_PROLOG
	HString l_oStr;
	HString l_oVoidPtr;
	l_oStr = ( f_pcBuffer ? static_cast < char const * > ( * this ) : "" );
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

HString & HString::operator <<= ( const int a_iShift )
	{
	int l_iCtr = 0;
	if ( a_iShift < 0 )
		M_THROW ( "bad shift lenght", a_iShift );
	if ( a_iShift )
		{
		while ( f_pcBuffer [ l_iCtr + a_iShift ] )
			{
			f_pcBuffer [ l_iCtr ] = f_pcBuffer [ l_iCtr + a_iShift ];
			l_iCtr ++;
			}
		f_pcBuffer [ l_iCtr ] = 0;
		}
	return ( * this );
	}

char & HString::operator [ ] ( const int a_iIndex ) const
	{
	M_PROLOG
	int l_iLength = get_length ( );
	if  ( ( static_cast < size_t > ( a_iIndex ) >= f_iSize ) || ( a_iIndex > l_iLength ) )
		M_THROW ( "index out of bound", a_iIndex );
	return ( f_pcBuffer [ a_iIndex ] );
	M_EPILOG
	}

bool HString::operator == ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this == static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator == ( char const * a_pcStr ) const
	{
	M_PROLOG
	return ( ! strcmp ( f_pcBuffer, a_pcStr ) );
	M_EPILOG
	}

bool HString::operator != ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this != static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator != ( char const * a_pcStr ) const
	{
	M_PROLOG
	return ( strcmp ( f_pcBuffer, a_pcStr ) ? true : false );
	M_EPILOG
	}

bool HString::operator >= ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this >= static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator >= ( char const * a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = strcmp ( f_pcBuffer, a_pcStr );
	if ( l_iCmp >= 0 )
		return ( true );
	else
		return ( false );
	M_EPILOG
	}

bool HString::operator <= ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this <= static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator <= ( char const * a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = strcmp ( f_pcBuffer, a_pcStr );
	if ( l_iCmp <= 0 )
		return ( true );
	else
		return ( false );
	M_EPILOG
	}

bool HString::operator > ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this > static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator > ( char const * a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = strcmp ( f_pcBuffer, a_pcStr );
	if ( l_iCmp > 0 )
		return ( true );
	else
		return ( false );
	M_EPILOG
	}

bool HString::operator < ( const HString & a_roString ) const
	{
	M_PROLOG
	return ( * this < static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator < ( char const * a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = strcmp ( f_pcBuffer, a_pcStr );
	if ( l_iCmp < 0 )
		return ( true );
	else
		return ( false );
	M_EPILOG
	}

HString::operator char * ( void ) const
	{
	M_PROLOG
	return ( f_pcBuffer );
	M_EPILOG
	}

bool HString::is_empty ( void ) const
	{
	return ( ! ( f_pcBuffer && f_pcBuffer [ 0 ] ) );
	}

int HString::get_length ( void ) const
	{
	M_PROLOG
	int l_iLength = 0;
	if ( ! f_pcBuffer )
		M_THROW ( "no buffer.", reinterpret_cast < int > ( f_pcBuffer ) );
	l_iLength = strlen ( f_pcBuffer );
	if ( static_cast < size_t > ( l_iLength ) > f_iSize )
		M_THROW ( "no terminating null!", l_iLength );
	return ( l_iLength );
	M_EPILOG
	}

HString & HString::format ( char const * a_pcFormat, ... )
	{
	M_PROLOG
	int l_iSize = 0;
	va_list ap;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	va_start ( ap, a_pcFormat );
	l_iSize = vsnprintf ( l_pcMeasureBuffer, 1, a_pcFormat, ap );
	if ( l_iSize < 1 )
		return ( * this );
	l_iSize ++;
	va_end ( ap );
	hs_realloc ( l_iSize );
	va_start ( ap, a_pcFormat );
	vsprintf ( f_pcBuffer, a_pcFormat, ap );
	va_end ( ap );
	return ( * this );
	M_EPILOG
	}

int HString::find ( char a_cChar, int a_iAfter ) const
	{
	M_PROLOG
	if ( ( a_iAfter < 0 ) || ( a_iAfter >= static_cast < int > ( strlen ( f_pcBuffer ) ) ) )
		return ( -1 );
	char * l_pcStr = strchr ( f_pcBuffer + a_iAfter, a_cChar );
	if ( ! l_pcStr )
		return ( - 1 );
	return ( static_cast < int > ( ( l_pcStr - f_pcBuffer ) ) );
	M_EPILOG
	}

int HString::find ( char const * a_pcPattern, int a_iAfter ) const
	{
	M_PROLOG
	if ( ( ! a_pcPattern ) || ( a_iAfter < 0 ) )
		return ( -1 );
	if ( ( ! strlen ( a_pcPattern ) )
			|| ( static_cast < int > ( strlen ( f_pcBuffer ) ) <= a_iAfter ) )
		return ( -1 );
	char * l_pcStr = strstr ( f_pcBuffer + a_iAfter, a_pcPattern );
	if ( ! l_pcStr )
		return ( -1 );
	return ( static_cast < int > ( ( l_pcStr - f_pcBuffer ) ) );
	M_EPILOG
	}

int HString::reverse_find ( char a_cChar, int a_iBefore ) const
	{
	M_PROLOG
	
	if ( ( a_iBefore < 0 )
			|| ( a_iBefore >= static_cast < int > ( strlen ( f_pcBuffer ) ) ) )
		return ( - 1 );
	char l_cTmp = * ( f_pcBuffer + a_iBefore );
	* ( f_pcBuffer + a_iBefore ) = 0;
	char * l_pcStr = strchr ( f_pcBuffer, a_cChar );
	* ( f_pcBuffer + a_iBefore ) = l_cTmp;
	if ( ! l_pcStr )
		return ( -1 );
	return ( static_cast < int > ( ( l_pcStr - f_pcBuffer ) ) );
	M_EPILOG
	}

HString & HString::replace ( char const * a_pcPattern, char const * a_pcWith )
	{
	M_PROLOG
	char * l_pcTmp = 0, * l_pcStr;
	int l_iLenNew, l_iLenOld, l_iLenPattern, l_iLenWith, l_iSubWP, l_iCursor = 0, l_iIndex = 0;
	if ( ! a_pcPattern )
		return ( * this );
	l_iLenPattern = strlen ( a_pcPattern );
	if ( l_iLenPattern < 1 )
		return ( * this );
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
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr++ )
		f_pcBuffer [ l_iCtr ] = toupper ( f_pcBuffer [ l_iCtr ] );
	return ( * this );
	M_EPILOG
	}

HString & HString::lower ( void )
	{
	M_PROLOG
	int l_iCtr, l_iLenght = strlen ( f_pcBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr++ )
		f_pcBuffer [ l_iCtr ] = tolower ( f_pcBuffer [ l_iCtr ] );
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
		if ( a_cChar == f_pcBuffer [ l_iCtr ] )
			f_pcBuffer [ l_iCtr ] = tolower ( f_pcBuffer [ l_iCtr ] );
		else
			f_pcBuffer [ l_iCtr ] = a_cChar;
		}
	return ( * this );
	M_EPILOG
	}

HString HString::left ( int a_iTo ) const
	{
	M_PROLOG
	int l_iLenght = strlen ( f_pcBuffer );
	HString l_oStr;
	if ( a_iTo < 1 )
		return ( l_oStr );
	l_iLenght = ( a_iTo < l_iLenght ? a_iTo : l_iLenght );
	l_oStr.hs_realloc ( l_iLenght + 1 );
	strncpy ( l_oStr.f_pcBuffer, f_pcBuffer, l_iLenght );
	l_oStr.f_pcBuffer [ l_iLenght ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString HString::mid ( int a_iFrom, int a_iLenght ) const
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

HString HString::right ( int a_iFromEnd ) const
	{
	M_PROLOG
	int l_iLenght = strlen ( f_pcBuffer );
	HString l_oStr;
	if ( a_iFromEnd < 1 )
		return ( l_oStr );
	a_iFromEnd = ( a_iFromEnd < l_iLenght ? a_iFromEnd : l_iLenght );
	l_oStr.hs_realloc ( a_iFromEnd + 1 );
	strncpy ( l_oStr.f_pcBuffer, f_pcBuffer + l_iLenght - a_iFromEnd, l_iLenght );
	l_oStr.f_pcBuffer [ a_iFromEnd ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString & HString::trim_left ( char const * a_pcSet )
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

HString & HString::trim_right( char const * a_pcSet )
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

HString HString::split ( char const * a_pcAt, int a_iPart ) const
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

HString operator + ( char const * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( HString ( a_pcStr ) + a_roString );
	M_EPILOG
	}

HString operator + ( char const a_cChar, const HString & a_roString )
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

bool operator == ( char const * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString == a_pcStr );
	M_EPILOG
	}

bool operator != ( char const * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString != a_pcStr );
	M_EPILOG
	}

bool operator >= ( char const * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString <= a_pcStr );
	M_EPILOG
	}

bool operator <= ( char const * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString >= a_pcStr );
	M_EPILOG
	}

bool operator > ( char const * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString < a_pcStr );
	M_EPILOG
	}

bool operator < ( char const * a_pcStr, const HString & a_roString )
	{
	M_PROLOG
	return ( a_roString > a_pcStr );
	M_EPILOG
	}

/* all str* and mem* functions takes const pointer as argument and returns
	 non const pointer */
char * strrnpbrk ( char const * a_pcBuffer, char const * a_pcStopSet,
		int a_iIndex )
	{
	int l_iCtr = 0;
	int l_iStopSetSize = strlen ( a_pcStopSet );
	while ( a_iIndex -- )
		for ( l_iCtr = 0; l_iCtr < l_iStopSetSize; l_iCtr ++ )
			if ( a_pcBuffer [ a_iIndex ] == a_pcStopSet [ l_iCtr ] )
				return ( const_cast < char * > ( a_pcBuffer + a_iIndex ) );
	return ( NULL );
	}

char const * strrpbrk ( char const * a_pcBuffer, char const * a_pcStopSet )
	{
	return ( strrnpbrk ( a_pcBuffer, a_pcStopSet, strlen ( a_pcBuffer ) ) );
	}

size_t strrnspn ( char const * a_pcBuffer, char const * a_pcSkipSet,
		int a_iLenght )
	{
	int l_iCtr = 0;
	int l_iSkipSetSize = strlen ( a_pcSkipSet );
	int l_iIndex = a_iLenght;
	while ( l_iIndex -- )
		{
		for ( l_iCtr = 0; l_iCtr < l_iSkipSetSize; l_iCtr ++ )
			if ( a_pcBuffer [ l_iIndex ] == a_pcSkipSet [ l_iCtr ] )
				break;
		if ( l_iCtr >= l_iSkipSetSize )
			return ( a_iLenght - l_iIndex );
		}
	return ( 0 );
	}

size_t strrspn ( char const * a_pcBuffer, char const * a_pcSkipSet )
	{
	return ( strrnspn ( a_pcBuffer, a_pcSkipSet, strlen ( a_pcBuffer ) ) );
	}

}

}

