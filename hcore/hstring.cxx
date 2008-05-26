/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hstring.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <libintl.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hstring.h"
#include "xalloc.h"
#include "hpool.h"

namespace yaal
{

namespace hcore
{

namespace
	{
	enum
		{
		E_OK = 0,
		E_NULL_PTR,
		E_UNINITIALIZED
		};

/* Useful helpers */
char * strrnpbrk( char const* const, char const* const,
		int long /* no const - used */ );
int long strrnspn( char const* const, char const* const, int long const );
int long kmpsearch( char const* const, int long const&, char const* const, int long const& );
	}

char const n_pcWhiteSpace [ ] = " \t\n\v\f\r";
char const * n_ppcErrMsgHString [ 3 ] =
	{
	_ ( "ok" ),
	_ ( "NULL pointer used for string operations" ),
	_ ( "use of uninitialized string" )
	};

HString::HString ( void ) : f_pcBuffer ( NULL ), f_lSize ( 0 )
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

HString::HString ( HString const & a_roString )
	: f_pcBuffer ( NULL ), f_lSize ( 0 )
	{
	M_PROLOG
	( * this ) = a_roString;
	return;
	M_EPILOG
	}

HString::HString ( int const a_iSize, bool const )
	: f_pcBuffer ( NULL ), f_lSize ( 0 )
	{
	M_PROLOG
	hs_realloc ( a_iSize + 1 );
	memset ( f_pcBuffer, 0, a_iSize + 1 );
	return;
	M_EPILOG
	}

void HString::hs_realloc ( int long const a_iSize )
	{
	M_PROLOG
	int long l_iOldLength = 0;
	if ( a_iSize < 1 )
		M_THROW ( _ ( "bad new buffer size requested" ), a_iSize );
	if ( a_iSize > f_lSize )
		{
		l_iOldLength = f_lSize;
		f_lSize = 1;
		while ( f_lSize < a_iSize )
			f_lSize <<= 1;
		f_pcBuffer = xrealloc<char>( f_pcBuffer, f_lSize );
		::memset( f_pcBuffer + l_iOldLength, 0, f_lSize - l_iOldLength );
		}
	return;
	M_EPILOG
	}

HString::HString ( char const * const a_pcStr )
	: f_pcBuffer ( NULL ), f_lSize ( 0 )
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

HString::HString ( char const a_cChar ) : f_pcBuffer ( NULL ), f_lSize ( 0 )
	{
	M_PROLOG
	hs_realloc ( 2 );
	f_pcBuffer [ 0 ] = a_cChar;
	f_pcBuffer [ 1 ] = 0;
	return;
	M_EPILOG
	}

HString::HString ( int const a_iInt ) : f_pcBuffer ( NULL ), f_lSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf( l_pcMeasureBuffer, 1, "%d", a_iInt ) + 1;
	hs_realloc ( l_iSize );
	M_ENSURE( ::snprintf( f_pcBuffer, l_iSize, "%d", a_iInt ) < l_iSize );
	return;
	M_EPILOG
	}

HString::HString ( int long const a_lLong ) : f_pcBuffer ( NULL ), f_lSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%ld", a_lLong ) + 1;
	hs_realloc ( l_iSize );
	M_ENSURE ( snprintf ( f_pcBuffer, l_iSize, "%ld", a_lLong ) < l_iSize );
	return;
	M_EPILOG
	}

HString::HString ( int long unsigned const a_lLong ) : f_pcBuffer ( NULL ), f_lSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%lu", a_lLong ) + 1;
	hs_realloc ( l_iSize );
	M_ENSURE ( snprintf ( f_pcBuffer, l_iSize, "%lu", a_lLong ) < l_iSize );
	return;
	M_EPILOG
	}

HString::HString ( int unsigned const a_ulLong ) : f_pcBuffer ( NULL ), f_lSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%u", a_ulLong ) + 1;
	hs_realloc ( l_iSize );
	M_ENSURE ( snprintf ( f_pcBuffer, l_iSize, "%u", a_ulLong ) < l_iSize );
	return;
	M_EPILOG
	}

HString::HString( double const a_dDouble ) : f_pcBuffer( NULL ), f_lSize( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer[ 3 ] = "\0\0";
	l_iSize = ::snprintf( l_pcMeasureBuffer, 1, "%f", a_dDouble ) + 1;
	hs_realloc( l_iSize );
	M_ENSURE( ::snprintf( f_pcBuffer, l_iSize, "%f", a_dDouble ) < l_iSize );
	return;
	M_EPILOG
	}

HString::HString( double long const a_dDouble ) : f_pcBuffer( NULL ), f_lSize( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer[ 3 ] = "\0\0";
	l_iSize = ::snprintf( l_pcMeasureBuffer, 1, "%.12Lf", a_dDouble ) + 1;
	hs_realloc( l_iSize );
	M_ENSURE( ::snprintf( f_pcBuffer, l_iSize, "%.12Lf", a_dDouble ) < l_iSize );
	return;
	M_EPILOG
	}

HString::HString ( void const * const a_pvPtrVoid )
	: f_pcBuffer ( NULL ), f_lSize ( 0 )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	l_iSize = snprintf ( l_pcMeasureBuffer, 1, "%p", a_pvPtrVoid ) + 1;
	hs_realloc ( l_iSize );
	M_ENSURE ( snprintf ( f_pcBuffer, l_iSize, "%p", a_pvPtrVoid ) < l_iSize );
	return;
	M_EPILOG
	}

HString & HString::operator = ( HString const & a_roString )
	{
	M_PROLOG
	if ( this != & a_roString )
		{
		if ( a_roString.f_pcBuffer )
			{
			hs_realloc ( strlen ( a_roString.f_pcBuffer ) + 1 );
			strcpy ( f_pcBuffer, a_roString.f_pcBuffer );
			}
		else
			( * this ) = "";
		}
	return ( * this );
	M_EPILOG
	}

HString HString::operator + ( HString const & a_roString ) const
	{
	M_PROLOG
	return ( ( * this ) + static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

HString HString::operator + ( char const * const a_pcStr ) const
	{
	M_PROLOG
	int long l_iLength = 0;
	HString str;
	l_iLength =	( f_pcBuffer ? get_length () : 0 ) 
							+ ( a_pcStr ? strlen ( a_pcStr ) : 0 );
	if ( ! l_iLength )
		return ( "" );
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
	int long l_iLength = 0;
	HString l_oStr;
	l_iLength = ( f_pcBuffer ? strlen ( f_pcBuffer ) : 0 ) + 2;
	l_oStr = * this;
	l_oStr.hs_realloc ( l_iLength );
	l_iLength -= 2;
	l_oStr.f_pcBuffer [ l_iLength ] = a_cChar;
	l_oStr.f_pcBuffer [ l_iLength + 1 ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString HString::operator + ( int const a_iInt ) const
	{
	M_PROLOG
	return ( operator + ( static_cast<long>( a_iInt ) ) );
	M_EPILOG
	}

HString HString::operator + ( int long const a_lLong ) const
	{
	M_PROLOG
	HString l_oStr;
	HString l_oLong;
	l_oStr = ( f_pcBuffer ? f_pcBuffer : "" );
	l_oLong = a_lLong;
	return ( l_oStr + l_oLong );
	M_EPILOG
	}

HString HString::operator + ( size_t const a_ulLong ) const
	{
	M_PROLOG
	HString l_oStr;
	HString l_oLong;
	l_oStr = ( f_pcBuffer ? f_pcBuffer : "" );
	l_oLong = a_ulLong;
	return ( l_oStr + l_oLong );
	M_EPILOG
	}

HString HString::operator + ( double const a_dDouble ) const
	{
	M_PROLOG
	HString l_oStr;
	HString l_oDouble;
	l_oStr = ( f_pcBuffer ? f_pcBuffer : "" );
	l_oDouble = a_dDouble;
	return ( l_oStr + l_oDouble );
	M_EPILOG
	}

HString HString::operator + ( double long const a_dDouble ) const
	{
	M_PROLOG
	HString l_oStr;
	HString l_oDouble;
	l_oStr = ( f_pcBuffer ? f_pcBuffer : "" );
	l_oDouble = a_dDouble;
	return ( l_oStr + l_oDouble );
	M_EPILOG
	}

HString HString::operator + ( void const * const a_pvVoidPtr ) const
	{
	M_PROLOG
	HString l_oStr;
	HString l_oVoidPtr;
	l_oStr = ( f_pcBuffer ? f_pcBuffer : "" );
	l_oVoidPtr = a_pvVoidPtr;
	return ( l_oStr + l_oVoidPtr );
	M_EPILOG
	}

HString & HString::operator += ( HString const & a_roString )
	{
	M_PROLOG
	return ( ( *this ) = ( ( *this ) + a_roString ) );
	M_EPILOG
	}

char HString::operator[] ( int const a_iIndex )
	{
	return ( operator[]( static_cast<int long>( a_iIndex ) ) );
	}

char HString::operator[] ( int long const a_lIndex )
	{
	M_PROLOG
	if ( a_lIndex >= f_lSize )
		M_THROW( "index out of bound", a_lIndex );
	return ( f_pcBuffer[ a_lIndex ] );
	M_EPILOG
	}

char HString::set_at( int long const a_lIndex, char a_cChar )
	{
	M_PROLOG
	if ( a_lIndex >= f_lSize )
		M_THROW( "index out of bound", a_lIndex );
	f_pcBuffer[ a_lIndex ] = a_cChar;
	return ( a_cChar );
	M_EPILOG
	}

bool HString::operator == ( HString const & a_roString ) const
	{
	M_PROLOG
	return ( * this == static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator == ( char const * const a_pcStr ) const
	{
	M_PROLOG
	if ( f_pcBuffer && a_pcStr )
		return ( ! ::strcoll( f_pcBuffer, a_pcStr ) );
	if ( f_pcBuffer )
		return ( ! f_pcBuffer [ 0 ] );
	return ( ! a_pcStr [ 0 ] );
	M_EPILOG
	}

bool HString::operator != ( HString const & a_roString ) const
	{
	M_PROLOG
	return ( * this != static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator != ( char const * const a_pcStr ) const
	{
	M_PROLOG
	return ( ! operator == ( a_pcStr ) );
	M_EPILOG
	}

bool HString::operator >= ( HString const & a_roString ) const
	{
	M_PROLOG
	return ( * this >= static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator >= ( char const * const a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = 0;
	if ( ! ( f_pcBuffer && a_pcStr ) )
		M_THROW ( n_ppcErrMsgHString [ E_NULL_PTR ], errno );
	l_iCmp = ::strcoll( f_pcBuffer, a_pcStr );
	if ( l_iCmp >= 0 )
		return ( true );
	else
		return ( false );
	M_EPILOG
	}

bool HString::operator <= ( HString const & a_roString ) const
	{
	M_PROLOG
	return ( * this <= static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator <= ( char const * const a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = 0;
	if ( ! ( f_pcBuffer && a_pcStr ) )
		M_THROW ( n_ppcErrMsgHString [ E_NULL_PTR ], errno );
	l_iCmp = ::strcoll( f_pcBuffer, a_pcStr );
	if ( l_iCmp <= 0 )
		return ( true );
	else
		return ( false );
	M_EPILOG
	}

bool HString::operator > ( HString const & a_roString ) const
	{
	M_PROLOG
	return ( * this > static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator > ( char const * const a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = 0;
	if ( ! ( f_pcBuffer && a_pcStr ) )
		M_THROW ( n_ppcErrMsgHString [ E_NULL_PTR ], errno );
	l_iCmp = ::strcoll( f_pcBuffer, a_pcStr );
	if ( l_iCmp > 0 )
		return ( true );
	else
		return ( false );
	M_EPILOG
	}

bool HString::operator < ( HString const & a_roString ) const
	{
	M_PROLOG
	return ( * this < static_cast < char const * > ( a_roString ) );
	M_EPILOG
	}

bool HString::operator < ( char const* const a_pcStr ) const
	{
	M_PROLOG
	int l_iCmp = 0;
	if ( ! ( f_pcBuffer && a_pcStr ) )
		M_THROW( n_ppcErrMsgHString[ E_NULL_PTR ], errno );
	l_iCmp = ::strcoll( f_pcBuffer, a_pcStr );
	if ( l_iCmp < 0 )
		return ( true );
	else
		return ( false );
	M_EPILOG
	}

HString::operator char const* ( void ) const
	{
	M_PROLOG
	return ( f_pcBuffer );
	M_EPILOG
	}

char const* HString::raw( void ) const
	{
	return ( f_pcBuffer );
	}

bool HString::empty( void ) const
	{
	return ( is_empty() );
	}

bool HString::is_empty( void ) const
	{
	return ( ! ( f_pcBuffer && f_pcBuffer[ 0 ] ) );
	}

void HString::clear( void )
	{
	if ( f_pcBuffer )
		f_pcBuffer[ 0 ] = 0;
	return;
	}

int long HString::get_length( void ) const
	{
	M_PROLOG
	int long l_iLength = 0;
	if ( ! f_pcBuffer )
		M_THROW ( "no buffer.", reinterpret_cast<int long>( f_pcBuffer ) );
	l_iLength = ::strlen( f_pcBuffer );
	M_ASSERT( l_iLength < f_lSize );
	return ( l_iLength );
	M_EPILOG
	}

void HString::swap( HString& left, HString& right )
	{
	if ( &left != &right )
		{
		yaal::swap( left.f_lSize, right.f_lSize );
		yaal::swap( left.f_pcBuffer, right.f_pcBuffer );
		}
	return;
	}

HString& HString::format( char const* const a_pcFormat, ... )
	{
	M_PROLOG
	va_list ap;
	va_start( ap, a_pcFormat );
	try
		{
		vformat( a_pcFormat, &ap );
		}
	catch ( ... )
		{
		va_end( ap );
		throw;
		}
	va_end( ap );
	return ( *this );
	M_EPILOG
	}

HString & HString::vformat( char const * const a_pcFormat, void* a_xAp )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcMeasureBuffer [ 3 ] = "\0\0";
	if ( ! a_pcFormat )
		M_THROW ( n_ppcErrMsgHString [ E_NULL_PTR ], errno );
	va_list orig;
	__va_copy( orig, *static_cast<va_list*>( a_xAp ) );
	l_iSize = vsnprintf( l_pcMeasureBuffer, 1, a_pcFormat, *static_cast<va_list*>( a_xAp ) ) + 1;
	hs_realloc ( l_iSize );
	M_ENSURE ( vsnprintf( f_pcBuffer, l_iSize, a_pcFormat, orig ) < l_iSize );
	va_end( orig );
	return ( *this );
	M_EPILOG
	}

int long HString::find( char const a_cChar, int long a_lAfter ) const
	{
	M_PROLOG
	int long l_iLength = ::strlen( f_pcBuffer );
	if ( a_lAfter >= l_iLength )
		return ( -1 );
	if ( a_lAfter < 0 )
		a_lAfter = 0;
	char* l_pcStr = static_cast<char*>( memchr( f_pcBuffer + a_lAfter, a_cChar, l_iLength - a_lAfter ) );
	if ( ! l_pcStr )
		return ( - 1 );
	return ( l_pcStr - f_pcBuffer );
	M_EPILOG
	}

int long HString::find( char const* const a_pcPattern, int long a_lAfter ) const
	{
	M_PROLOG
	if ( ! a_pcPattern )
		return ( -1 );
	return ( nfind( a_pcPattern, ::strlen( a_pcPattern ), a_lAfter ) );
	M_EPILOG
	}

int long HString::nfind( char const* const a_pcPattern, int long a_iPatternLength, int long a_lAfter ) const
	{
	M_PROLOG
	if ( ! a_pcPattern )
		return ( -1 );
	if ( a_lAfter < 0 )
		a_lAfter = 0;
	int long len = 0;
	if ( ( ! a_iPatternLength )
			|| ( ( len = ::strlen( f_pcBuffer ) ) < ( a_lAfter + a_iPatternLength ) ) )
		return ( -1 );
	int long idx = kmpsearch( f_pcBuffer + a_lAfter, len - a_lAfter, a_pcPattern, a_iPatternLength );
	return ( idx >= 0 ? idx + a_lAfter : -1 );
	M_EPILOG
	}

int long HString::find_one_of( char const* const a_pcSet,
		int long a_lAfter ) const
	{
	M_PROLOG
	if ( ! a_pcSet )
		return ( - 1 );
	if ( a_lAfter < 0 )
		a_lAfter = 0;
	if ( ( ! ::strlen ( a_pcSet ) )
			|| ( static_cast<int>( ::strlen( f_pcBuffer ) ) <= a_lAfter ) )
		return ( - 1 );
	char * l_pcStr = ::strpbrk( f_pcBuffer + a_lAfter, a_pcSet );
	if ( ! l_pcStr )
		return ( - 1 );
	return ( l_pcStr - f_pcBuffer );
	M_EPILOG
	}

int long HString::reverse_find_one_of( char const* const a_pcSet,
		int long a_iBefore ) const
	{
	M_PROLOG
	if ( ! a_pcSet )
		return ( - 1 );
	if ( a_iBefore < 0 )
		a_iBefore = 0;
	int long l_iLength = ::strlen( f_pcBuffer );
	if ( ( ! ::strlen( a_pcSet ) ) || ( l_iLength <= a_iBefore ) )
		return ( - 1 );
	char* l_pcStr = strrnpbrk( f_pcBuffer, a_pcSet, l_iLength - a_iBefore );
	if ( ! l_pcStr )
		return ( - 1 );
	return ( ( l_iLength - 1 ) - ( l_pcStr - f_pcBuffer ) );
	M_EPILOG
	}

int long HString::find_other_than( char const* const a_pcSet,
		int long a_lAfter ) const
	{
	M_PROLOG
	int long l_iLength = 0, l_iIndex = 0;
	if ( ! a_pcSet )
		return ( 0 );
	if ( a_lAfter < 0 )
		a_lAfter = 0;
	if ( ! strlen( a_pcSet ) )
		return ( 0 );
	l_iLength = ::strlen( f_pcBuffer );
	if ( l_iLength <= a_lAfter )
		return ( -1 );
	l_iIndex = ::strspn( f_pcBuffer + a_lAfter, a_pcSet );
	if ( ( l_iIndex + a_lAfter ) >= l_iLength )
		return ( - 1 );
	return ( l_iIndex + a_lAfter );
	M_EPILOG
	}

int long HString::reverse_find_other_than( char const* const a_pcSet,
		int long a_iBefore ) const
	{
	M_PROLOG
	int long l_iLength = 0, l_iIndex = 0;
	if ( ! a_pcSet )
		return ( 0 );
	if ( a_iBefore < 0 )
		a_iBefore = 0;
	if ( ! strlen( a_pcSet ) )
		return ( 0 );
	l_iLength = ::strlen ( f_pcBuffer );
	if ( l_iLength <= a_iBefore )
		return ( -1 );
	l_iIndex = strrnspn( f_pcBuffer, a_pcSet, l_iLength - a_iBefore );
	if ( l_iIndex >= ( l_iLength - a_iBefore ) )
		return ( - 1 );
	return ( ( l_iLength - 1 ) - l_iIndex );
	M_EPILOG
	}

int long HString::reverse_find( char const a_cChar, int long a_iBefore ) const
	{
	M_PROLOG
	int long l_iLength = ::strlen( f_pcBuffer );
	if ( a_iBefore >= l_iLength )
		return ( -1 );
	if ( a_iBefore < 0 )
		a_iBefore = 0;
	char* l_pcStr = static_cast<char*>( ::memrchr( f_pcBuffer, a_cChar, l_iLength - a_iBefore ) );
	if ( ! l_pcStr )
		return ( -1 );
	return ( ( l_iLength - 1 ) - ( l_pcStr - f_pcBuffer ) );
	M_EPILOG
	}

HString& HString::replace( char const* const a_pcPattern,
		char const* const a_pcWith )
	{
	M_PROLOG
	int long l_iLenNew, l_iLenOld, l_iCursor = 0, l_iIndex = 0;
	if ( ! a_pcPattern )
		return ( *this );
	int long l_iLenPattern = ::strlen( a_pcPattern );
	if ( l_iLenPattern < 1 )
		return ( *this );
	int long l_iLenWith = ::strlen( a_pcWith );
	int long l_iSubWP = l_iLenWith - l_iLenPattern;
	l_iLenOld = l_iLenNew = ::strlen( f_pcBuffer );
	while ( ( l_iIndex = find( a_pcPattern, l_iIndex ) ) > -1 )
		{
		l_iLenNew += l_iSubWP;
		l_iIndex ++;
		}
	hs_realloc( l_iLenNew + 1 );
	char* l_pcTmp = xstrdup( f_pcBuffer );
	char* l_pcStr = f_pcBuffer;
	f_pcBuffer = l_pcTmp;
	l_iIndex = 0;
	::memset( l_pcStr, 0, l_iLenNew );
	while ( ( l_iIndex = find( a_pcPattern, l_iIndex ) ) > -1 )
		{
		::strncat( l_pcStr, f_pcBuffer + l_iCursor, l_iIndex - l_iCursor );
		::strcat( l_pcStr, a_pcWith );
		l_iCursor = l_iIndex + l_iLenPattern;
		l_iIndex ++;
		}
	::strncat( l_pcStr, f_pcBuffer + l_iCursor, l_iLenOld - l_iCursor );
	l_pcStr[ l_iLenNew ] = 0;
	l_pcTmp = f_pcBuffer;
	f_pcBuffer = l_pcStr;
	xfree( l_pcTmp );
	return ( *this );
	M_EPILOG
	}

HString& HString::upper ( void )
	{
	M_PROLOG
	int long l_iCtr = 0, l_iLenght = strlen ( f_pcBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr++ )
		f_pcBuffer [ l_iCtr ] = static_cast < char > ( toupper ( f_pcBuffer [ l_iCtr ] ) );
	return ( * this );
	M_EPILOG
	}

HString& HString::lower ( void )
	{
	M_PROLOG
	int long l_iCtr = 0, l_iLenght = strlen ( f_pcBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr++ )
		f_pcBuffer [ l_iCtr ] = static_cast < char > ( tolower ( f_pcBuffer [ l_iCtr ] ) );
	return ( * this );
	M_EPILOG
	}

HString& HString::reverse ( void )
	{
	M_PROLOG
	char a_cChar;
	int long l_iCtr = 0, l_iLenght = strlen ( f_pcBuffer );
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr++ )
		{
		a_cChar = static_cast < char > ( toupper ( f_pcBuffer [ l_iCtr ] ) );
		if ( a_cChar == f_pcBuffer [ l_iCtr ] )
			f_pcBuffer [ l_iCtr ] = static_cast < char > ( tolower ( f_pcBuffer [ l_iCtr ] ) );
		else
			f_pcBuffer [ l_iCtr ] = a_cChar;
		}
	return ( * this );
	M_EPILOG
	}

HString HString::left ( int const a_iTo ) const
	{
	M_PROLOG
	int long l_iLenght = ::strlen( f_pcBuffer );
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

HString HString::mid( int long a_iFrom, int long /* reused */ a_iLength ) const
	{
	M_PROLOG
	int long l_iLenOrig = ::strlen( f_pcBuffer );
	HString l_oStr;
	if ( a_iFrom < 0 )
		a_iLength += a_iFrom, a_iFrom = 0;
	if ( ( a_iLength <= 0 ) || ( a_iFrom >= l_iLenOrig ) )
		return ( l_oStr );
	a_iLength = ( a_iLength < l_iLenOrig ? a_iLength : l_iLenOrig );
	if ( ( a_iLength + a_iFrom ) > l_iLenOrig )
		a_iLength = l_iLenOrig - a_iFrom;
	l_oStr.hs_realloc ( a_iLength + 1 );
	strncpy ( l_oStr.f_pcBuffer, f_pcBuffer + a_iFrom, a_iLength );
	l_oStr.f_pcBuffer [ a_iLength ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString HString::right( int long /* reused */ a_iFromEnd ) const
	{
	M_PROLOG
	int long l_iLenght = ::strlen( f_pcBuffer );
	HString l_oStr;
	if ( a_iFromEnd < 1 )
		return ( l_oStr );
	a_iFromEnd = ( a_iFromEnd < l_iLenght ? a_iFromEnd : l_iLenght );
	l_oStr.hs_realloc( a_iFromEnd + 1 );
	::strncpy( l_oStr.f_pcBuffer, f_pcBuffer + l_iLenght - a_iFromEnd,
			a_iFromEnd );
	l_oStr.f_pcBuffer[ a_iFromEnd ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString& HString::trim_left( char const* const a_pcSet )
	{
	M_PROLOG
	int l_iCut = 0;
	while ( ::strchr( a_pcSet, f_pcBuffer[ l_iCut ] ) )
		++ l_iCut;
	if ( l_iCut )
		shift_left( l_iCut );
	return ( *this );
	M_EPILOG
	}

HString& HString::trim_right( char const* const a_pcSet )
	{
	M_PROLOG
	int long l_iCut = 0, l_iLenght;
	l_iLenght = ::strlen( f_pcBuffer );
	while ( ::strchr( a_pcSet, f_pcBuffer[ l_iLenght - l_iCut ] ) )
		l_iCut ++;
	if ( l_iCut )
		f_pcBuffer[ ( l_iLenght - l_iCut ) + 1 ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::shift_left( int long const a_iShift )
	{
	int long l_iLenght = 0;
	if ( a_iShift < 0 )
		M_THROW ( "bad left shift lenght", a_iShift );
	if ( a_iShift )
		{
		l_iLenght = get_length();
		if ( a_iShift < l_iLenght )
			::memmove( f_pcBuffer, f_pcBuffer + a_iShift, l_iLenght + 1 - a_iShift );
		else
			f_pcBuffer[ 0 ] = 0;
		}
	return ( *this );
	}

HString& HString::shift_right( int long const a_iShift, char const a_cFiller )
	{
	int long l_iLenght = 0;
	if ( a_iShift < 0 )
		M_THROW ( "bad right shift lenght", a_iShift );
	if ( a_iShift )
		{
		hs_realloc( l_iLenght + a_iShift + 1 );
		l_iLenght = get_length();
		::memmove( f_pcBuffer + a_iShift, f_pcBuffer, l_iLenght + 1 );
		fill( a_cFiller, a_iShift );
		}
	return ( * this );
	}

HString HString::split( char const* const a_pcAt, int const a_iPart ) const
	{
	M_PROLOG
	if ( f_lSize )
		{
		int long l_iBegining = 0;
		for ( int l_iCtr = 0;	f_pcBuffer[ l_iBegining ] && ( l_iCtr < a_iPart ); ++ l_iCtr )
			l_iBegining += strcspn( f_pcBuffer + l_iBegining, a_pcAt ) + 1;
		if ( f_pcBuffer[ l_iBegining ] )
			{
			int long l_iSize = ::strcspn( f_pcBuffer + l_iBegining, a_pcAt );
			return ( mid( l_iBegining, l_iSize ) );
			}
		}
	return ( "" );
	M_EPILOG
	}

HString& HString::fill( char a_cFiller, int long a_iLength, int long a_iOffset )
	{
	M_PROLOG
	if ( a_iLength < 0 )
		M_THROW( _( "bad length" ), a_iLength );
	if ( a_iOffset < 0 )
		M_THROW( _( "bad offset" ), a_iOffset );
	if ( ( a_iOffset + a_iLength ) >= f_lSize )
		M_THROW( _( "overflow" ), a_iOffset + a_iLength );
	if ( a_iLength == 0 )
		a_iLength = f_lSize - a_iOffset;
	::memset( f_pcBuffer + a_iOffset, a_cFiller, a_iLength );
	return ( *this );
	M_EPILOG
	}

HString& HString::fillz( char a_cFiller, int long a_iLength, int long a_iOffset )
	{
	M_PROLOG
	fill( a_cFiller, a_iLength, a_iOffset );
	f_pcBuffer [ a_iLength != 0 ? a_iLength + a_iOffset : f_lSize ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::erase( int long a_iFrom, int long a_iLength )
	{
	M_PROLOG
	if ( a_iFrom < 0 )
		a_iLength += a_iFrom, a_iFrom = 0;
	if ( ( a_iFrom + a_iLength ) >= f_lSize )
		a_iLength = ( f_lSize - a_iFrom ) - 1;
	if ( ( a_iLength > 0 ) && ( a_iFrom < f_lSize ) )
		::memmove( f_pcBuffer + a_iFrom, f_pcBuffer + a_iFrom + a_iLength, f_lSize - ( a_iFrom + a_iLength ) );
	return ( *this );
	M_EPILOG
	}

HString& HString::insert( int long a_iFrom, int long a_iLength, char const* a_pcChunk )
	{
	M_PROLOG
	if ( a_iFrom < 0 )
		{
		a_iLength += a_iFrom;
		if ( a_pcChunk )
			{
			if ( static_cast<size_t>( -a_iFrom ) > ::strlen( a_pcChunk ) )
				M_THROW( "negative offset caused chunk overflow", a_iFrom );
			a_pcChunk += -a_iFrom;
			}
		a_iFrom = 0;
		}
	if ( ( a_iLength > 0 ) && ( a_iFrom < f_lSize ) )
		{
		if ( a_pcChunk && ( static_cast<size_t>( a_iLength ) > ::strlen( a_pcChunk ) ) )
			M_THROW( "length too big for this chunk", a_iLength );
		int long l_iLength = get_length();
		hs_realloc( l_iLength + a_iLength + 1 );
		::memmove( f_pcBuffer + a_iFrom + a_iLength, f_pcBuffer + a_iFrom, ( l_iLength + 1 ) - a_iFrom );
		if ( a_pcChunk )
			::strncpy( f_pcBuffer + a_iFrom, a_pcChunk, a_iLength );
		}
	return ( *this );
	M_EPILOG
	}

HString operator + ( char const * const a_pcStr, HString const & a_roString )
	{
	M_PROLOG
	return ( HString ( a_pcStr ) + a_roString );
	M_EPILOG
	}

HString operator + ( char const a_cChar, HString const & a_roString )
	{
	M_PROLOG
	return ( HString ( a_cChar ) + a_roString );
	M_EPILOG
	}

HString operator + ( int const a_iInt , HString const & a_roString )
	{
	M_PROLOG
	return ( HString ( a_iInt ) + a_roString );
	M_EPILOG
	}

HString operator + ( int long const a_lLong, HString const & a_roString )
	{
	M_PROLOG
	return ( HString ( a_lLong ) + a_roString );	
	M_EPILOG
	}

HString operator + ( double const a_dDouble, HString const& a_roString )
	{
	M_PROLOG
	return ( HString( a_dDouble ) + a_roString );
	M_EPILOG
	}

HString operator + ( double long const a_dDouble, HString const& a_roString )
	{
	M_PROLOG
	return ( HString( a_dDouble ) + a_roString );
	M_EPILOG
	}

HString operator + ( void const * const a_pvVoidPtr,
		HString const & a_roString )
	{
	M_PROLOG
	return ( HString ( a_pvVoidPtr ) + a_roString );
	M_EPILOG
	}

bool operator == ( char const * const a_pcStr, HString const & a_roString )
	{
	M_PROLOG
	return ( a_roString == a_pcStr );
	M_EPILOG
	}

bool operator != ( char const * const a_pcStr, HString const & a_roString )
	{
	M_PROLOG
	return ( a_roString != a_pcStr );
	M_EPILOG
	}

bool operator >= ( char const * const a_pcStr, HString const & a_roString )
	{
	M_PROLOG
	return ( a_roString <= a_pcStr );
	M_EPILOG
	}

bool operator <= ( char const * const a_pcStr, HString const & a_roString )
	{
	M_PROLOG
	return ( a_roString >= a_pcStr );
	M_EPILOG
	}

bool operator > ( char const * const a_pcStr, HString const & a_roString )
	{
	M_PROLOG
	return ( a_roString < a_pcStr );
	M_EPILOG
	}

bool operator < ( char const * const a_pcStr, HString const & a_roString )
	{
	M_PROLOG
	return ( a_roString > a_pcStr );
	M_EPILOG
	}

namespace
{

/* all str* and mem* functions takes const pointer as argument and returns
	 non const pointer */
char* strrnpbrk( char const* const a_pcBuffer,
		char const* const a_pcStopSet, int long a_iLength )
	{
	M_PROLOG
	if ( a_iLength < 1 )
		return ( NULL );
	int long l_iStopSetSize = ::strlen( a_pcStopSet );
	int long l_iIndex = a_iLength - 1;
	while ( l_iIndex )
		{
		if ( ::memchr( a_pcStopSet, a_pcBuffer[ l_iIndex ], l_iStopSetSize ) )
			return ( const_cast<char*>( a_pcBuffer + l_iIndex ) );
		-- l_iIndex;
		}
	return ( NULL );
	M_EPILOG
	}

int long strrnspn( char const* const a_pcBuffer, char const* const a_pcSkipSet,
		int const long a_iLength )
	{
	M_PROLOG
	int long l_iSkipSetSize = ::strlen( a_pcSkipSet );
	int long l_iIndex = a_iLength - 1;
	while ( l_iIndex )
		{
		if ( ! ::memchr( a_pcSkipSet, a_pcBuffer[ l_iIndex ], l_iSkipSetSize ) )
			return ( l_iIndex );
		-- l_iIndex;
		}
	return ( a_iLength );
	M_EPILOG
	}

int long kmpsearch( char const* const str, int long const& lenstr, char const* const pat, int long const& lenpat )
	{
	typedef HPool<int> kmpnext_t;
	kmpnext_t KMPnext( lenpat + 1 );
	int* next = KMPnext.raw();
	int b = next[ 0 ] = -1;
	for ( int i = 1; i <= lenpat; ++ i )
		{
		while ( ( b > -1 ) && ( pat[ b ] != pat[ i - 1 ] ) )
			b = next[ b ];
		++ b;
		next[ i ] = ( pat[ i ] == pat[ b ] ) ? next[ b ] : b;
		}
	int start = -1;
	b = 0;
	for ( int i = 0; i < lenstr; ++ i )
		{
		while ( ( b > -1 ) && ( pat[ b ] != str[ i ] ) )
			b = next[ b ];
		if ( ++ b < lenpat )
			continue;
		start = i - b + 1;
		break;
		}
	return ( start );
	}

}

}

}

