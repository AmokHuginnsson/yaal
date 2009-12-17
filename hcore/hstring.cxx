/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hstring.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "algorithm.hxx"
#include "hstring.hxx"
#include "xalloc.hxx"
#include "hpool.hxx"

namespace yaal
{

namespace hcore
{

namespace string_helper
	{
	enum
		{
		OK = 0,
		NULL_PTR,
		UNINITIALIZED
		};

/* Useful helpers */
char * strrnpbrk( char const* const, char const* const,
		int long /* no const - used */ );
int long strrnspn( char const* const, char const* const, int long const );
int long kmpsearch( char const* const, int long const&, char const* const, int long const& );
	}

char const n_pcWhiteSpace[] = "\a\b \t\v\f\r\n";
char const* n_ppcErrMsgHString[ 3 ] =
	{
	_( "ok" ),
	_( "NULL pointer used for string operations" ),
	_( "use of uninitialized string" )
	};

HString::HString( void ) : f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HString::~HString( void )
	{
	M_PROLOG
	if ( f_pcBuffer )
		xfree( f_pcBuffer );
	return;
	M_EPILOG
	}

HString::HString( HString const& a_roString )
	: f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( a_roString.f_lSize )
	{
	M_PROLOG
	if ( f_lSize )
		{
		hs_realloc( f_lSize + 1 );
		::std::strcpy( f_pcBuffer, a_roString.f_pcBuffer );
		}
	return;
	M_EPILOG
	}

HString::HString( int const a_lPreallocate, bool const )
	: f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	hs_realloc( a_lPreallocate );
	::std::memset( f_pcBuffer, 0, a_lPreallocate );
	return;
	M_EPILOG
	}

void HString::hs_realloc( int long const a_lPreallocate )
	{
	M_PROLOG
	if ( a_lPreallocate < 1 )
		M_THROW( _( "bad new buffer size requested" ), a_lPreallocate );
	if ( a_lPreallocate > f_lAllocatedBytes )
		{
		int long l_lOldAllocation = f_lAllocatedBytes;
		f_lAllocatedBytes = 1;
		while ( f_lAllocatedBytes < a_lPreallocate )
			f_lAllocatedBytes <<= 1;
		f_pcBuffer = xrealloc<char>( f_pcBuffer, f_lAllocatedBytes );
		::std::memset( f_pcBuffer + l_lOldAllocation, 0, f_lAllocatedBytes - l_lOldAllocation );
		}
	return;
	M_EPILOG
	}

void HString::materialize( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HString::HString( char const* const a_pcStr )
	: f_pcBuffer ( NULL ), f_lAllocatedBytes ( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	if ( a_pcStr )
		{
		f_lSize = ::std::strlen( a_pcStr );
		hs_realloc( f_lSize + 1 );
		::std::strcpy( f_pcBuffer, a_pcStr );
		}
	return;
	M_EPILOG
	}

HString::HString( char const a_cChar ) : f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 1 )
	{
	M_PROLOG
	hs_realloc( 2 );
	f_pcBuffer[ 0 ] = a_cChar;
	f_pcBuffer[ 1 ] = 0;
	return;
	M_EPILOG
	}

HString::HString( int const a_iInt ) : f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	f_lSize = ::snprintf( NULL, 0, "%d", a_iInt );
	hs_realloc( f_lSize + 1 );
	M_ENSURE( ::snprintf( f_pcBuffer, f_lSize + 1, "%d", a_iInt ) == f_lSize );
	return;
	M_EPILOG
	}

HString::HString( int long const a_lLong ) : f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	f_lSize = ::snprintf( NULL, 0, "%ld", a_lLong );
	hs_realloc( f_lSize + 1 );
	M_ENSURE( ::snprintf( f_pcBuffer, f_lSize + 1, "%ld", a_lLong ) == f_lSize );
	return;
	M_EPILOG
	}

HString::HString( int long unsigned const a_lLong ) : f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	f_lSize = ::snprintf( NULL, 0, "%lu", a_lLong );
	hs_realloc( f_lSize + 1 );
	M_ENSURE( ::snprintf( f_pcBuffer, f_lSize + 1, "%lu", a_lLong ) == f_lSize );
	return;
	M_EPILOG
	}

HString::HString( int unsigned const a_ulLong ) : f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	f_lSize = ::snprintf( NULL, 0, "%u", a_ulLong );
	hs_realloc( f_lSize + 1 );
	M_ENSURE( ::snprintf( f_pcBuffer, f_lSize + 1, "%u", a_ulLong ) == f_lSize );
	return;
	M_EPILOG
	}

HString::HString( double const a_dDouble ) : f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	f_lSize = ::snprintf( NULL, 0, "%f", a_dDouble );
	hs_realloc( f_lSize + 1 );
	M_ENSURE( ::snprintf( f_pcBuffer, f_lSize + 1, "%f", a_dDouble ) == f_lSize );
	return;
	M_EPILOG
	}

HString::HString( double long const a_dDouble ) : f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	f_lSize = ::snprintf( NULL, 0, "%.12Lf", a_dDouble );
	hs_realloc( f_lSize + 1 );
	M_ENSURE( ::snprintf( f_pcBuffer, f_lSize + 1, "%.12Lf", a_dDouble ) == f_lSize );
	return;
	M_EPILOG
	}

HString::HString( void const* const a_pvPtrVoid )
	: f_pcBuffer( NULL ), f_lAllocatedBytes( 0 ), f_lSize( 0 )
	{
	M_PROLOG
	/*
	 * Solaris libc omits 0x in %p conversion.
	 * Well, that sucks.
	 */
	f_lSize = ::snprintf( NULL, 0, "0x%lx", reinterpret_cast<int long>( a_pvPtrVoid ) );
	hs_realloc( f_lSize + 1 );
	M_ENSURE( ::snprintf( f_pcBuffer, f_lSize + 1, "0x%lx", reinterpret_cast<int long>( a_pvPtrVoid ) ) == f_lSize );
	return;
	M_EPILOG
	}

HString& HString::operator = ( HString const& a_roString )
	{
	M_PROLOG
	if ( this != &a_roString )
		{
		f_lSize = a_roString.f_lSize;
		if ( f_lSize >= f_lAllocatedBytes )
			hs_realloc( f_lSize + 1 );
		if ( f_lSize )
			::std::strcpy( f_pcBuffer, a_roString.f_pcBuffer );
		if( f_pcBuffer )
			f_pcBuffer[ f_lSize ] = 0;
		}
	return ( *this );
	M_EPILOG
	}

HString operator + ( HString const& a_roString, HString const& right )
	{
	M_PROLOG
	HString str( a_roString );
	return ( str += right );
	M_EPILOG
	}

HString& HString::operator += ( HString const& a_roString )
	{
	M_PROLOG
	if ( a_roString.f_lSize )
		{
		int long l_lOldLength = f_lSize;
		f_lSize += a_roString.f_lSize;
		hs_realloc( f_lSize + 1 );
		::std::strcpy( f_pcBuffer + l_lOldLength, a_roString.f_pcBuffer );
		}
	return ( *this );
	M_EPILOG
	}

char HString::operator[] ( int const a_iIndex ) const
	{
	return ( operator[]( static_cast<int long>( a_iIndex ) ) );
	}

char HString::operator[] ( int long const a_lIndex ) const
	{
	M_PROLOG
	if ( a_lIndex >= f_lAllocatedBytes )
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
	if ( ! a_cChar )
		f_lSize = a_lIndex;
	return ( a_cChar );
	M_EPILOG
	}

bool operator == ( HString const& left, HString const& right )
	{
	M_PROLOG
	int long len = left.get_length();
	return ( ( &left == &right )
			|| ( ( len == right.get_length() )
				&& ( ! ( len && ::std::strcmp( left.raw(), right.raw() ) ) ) ) );
	M_EPILOG
	}

bool operator != ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ! operator == ( left, right ) );
	M_EPILOG
	}

bool operator >= ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ::std::strcoll( left.raw(), right.raw() ) >= 0 );
	M_EPILOG
	}

bool operator <= ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ::std::strcoll( left.raw(), right.raw() ) <= 0 );
	M_EPILOG
	}

bool operator > ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ! operator <= ( left, right ) );
	M_EPILOG
	}

bool operator < ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ! operator >= ( left, right ) );
	M_EPILOG
	}

char const* HString::raw( void ) const
	{
	return ( f_pcBuffer ? f_pcBuffer : "" );
	}

HString::const_iterator HString::begin( void ) const
	{
	return ( f_pcBuffer );
	}

HString::const_iterator HString::end( void ) const
	{
	return ( f_pcBuffer + get_length() );
	}

bool HString::empty( void ) const
	{
	return ( is_empty() );
	}

bool HString::is_empty( void ) const
	{
	return ( ! f_lSize );
	}

bool HString::operator ! ( void ) const
	{
	return ( is_empty() );
	}

void HString::clear( void )
	{
	if ( f_pcBuffer )
		f_pcBuffer[ 0 ] = 0;
	f_lSize = 0;
	return;
	}

int long HString::get_length( void ) const
	{
	M_PROLOG
	M_ASSERT( ( ! f_pcBuffer && ( f_lSize == 0 ) && ( f_lAllocatedBytes == 0 ) ) || ( f_lSize < f_lAllocatedBytes ) );
	return ( f_lSize );
	M_EPILOG
	}

int long HString::length( void ) const
	{
	M_PROLOG
	return ( get_length() );
	M_EPILOG
	}

int long HString::size( void ) const
	{
	M_PROLOG
	return ( get_length() );
	M_EPILOG
	}

int long HString::capacity( void ) const
	{
	M_PROLOG
	return ( f_lAllocatedBytes - 1 );
	M_EPILOG
	}

void HString::swap( HString& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( f_pcBuffer, other.f_pcBuffer );
		swap( f_lAllocatedBytes, other.f_lAllocatedBytes );
		swap( f_lSize, other.f_lSize );
		}
	return;
	}

HString& HString::format( char const* const a_pcFormat, ... )
	{
	M_PROLOG
	::std::va_list ap;
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

HString& HString::vformat( char const* const a_pcFormat, void* a_xAp )
	{
	M_PROLOG
	if ( ! a_pcFormat )
		M_THROW( n_ppcErrMsgHString[ string_helper::NULL_PTR ], errno );
	::std::va_list orig;
	__va_copy( orig, *static_cast< ::std::va_list*>( a_xAp ) );
	f_lSize = vsnprintf( NULL, 0, a_pcFormat, *static_cast< ::std::va_list*>( a_xAp ) );
	hs_realloc( f_lSize + 1 );
	M_ENSURE ( vsnprintf( f_pcBuffer, f_lSize + 1, a_pcFormat, orig ) == f_lSize );
	va_end( orig );
	return ( *this );
	M_EPILOG
	}

int long HString::find( char const a_cChar, int long a_lAfter ) const
	{
	M_PROLOG
	if ( a_lAfter >= f_lSize )
		return ( -1 );
	if ( a_lAfter < 0 )
		a_lAfter = 0;
	char* l_pcStr = static_cast<char*>( ::std::memchr( f_pcBuffer + a_lAfter, a_cChar, f_lSize - a_lAfter ) );
	if ( ! l_pcStr )
		return ( - 1 );
	return ( l_pcStr - f_pcBuffer );
	M_EPILOG
	}

int long HString::find( HString const& a_oPattern, int long a_lAfter ) const
	{
	M_PROLOG
	if ( a_oPattern.is_empty() )
		return ( -1 );
	return ( nfind( a_oPattern, a_oPattern.get_length(), a_lAfter ) );
	M_EPILOG
	}

int long HString::nfind( HString const& a_oPattern, int long a_iPatternLength, int long a_lAfter ) const
	{
	M_PROLOG
	if ( a_oPattern.is_empty() )
		return ( -1 );
	if ( a_lAfter < 0 )
		a_lAfter = 0;
	if ( ( ! a_iPatternLength )
			|| ( f_lSize < ( a_lAfter + a_iPatternLength ) ) )
		return ( -1 );
	int long idx = string_helper::kmpsearch( f_pcBuffer + a_lAfter,
			f_lSize - a_lAfter, a_oPattern.raw(), a_iPatternLength );
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
	if ( ( ! ::std::strlen( a_pcSet ) )
			|| ( f_lSize <= a_lAfter ) )
		return ( - 1 );
	char * l_pcStr = ::std::strpbrk( f_pcBuffer + a_lAfter, a_pcSet );
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
	if ( ( ! ::std::strlen( a_pcSet ) ) || ( f_lSize <= a_iBefore ) )
		return ( - 1 );
	char* l_pcStr = string_helper::strrnpbrk( f_pcBuffer,
			a_pcSet, f_lSize - a_iBefore );
	if ( ! l_pcStr )
		return ( - 1 );
	return ( ( f_lSize - 1 ) - ( l_pcStr - f_pcBuffer ) );
	M_EPILOG
	}

int long HString::find_other_than( char const* const a_pcSet,
		int long a_lAfter ) const
	{
	M_PROLOG
	if ( ! a_pcSet )
		return ( 0 );
	if ( a_lAfter < 0 )
		a_lAfter = 0;
	if ( ! ::std::strlen( a_pcSet ) )
		return ( 0 );
	if ( f_lSize <= a_lAfter )
		return ( -1 );
	int long l_iIndex = ::std::strspn( f_pcBuffer + a_lAfter, a_pcSet );
	if ( ( l_iIndex + a_lAfter ) >= f_lSize )
		return ( - 1 );
	return ( l_iIndex + a_lAfter );
	M_EPILOG
	}

int long HString::reverse_find_other_than( char const* const a_pcSet,
		int long a_iBefore ) const
	{
	M_PROLOG
	if ( ! a_pcSet )
		return ( 0 );
	if ( a_iBefore < 0 )
		a_iBefore = 0;
	if ( ! ::std::strlen( a_pcSet ) )
		return ( 0 );
	if ( f_lSize <= a_iBefore )
		return ( -1 );
	int long l_iIndex = string_helper::strrnspn( f_pcBuffer,
			a_pcSet, f_lSize - a_iBefore );
	if ( l_iIndex >= ( f_lSize - a_iBefore ) )
		return ( - 1 );
	return ( ( f_lSize - 1 ) - l_iIndex );
	M_EPILOG
	}

int long HString::reverse_find( char const a_cChar, int long a_iBefore ) const
	{
	M_PROLOG
	if ( a_iBefore >= f_lSize )
		return ( -1 );
	if ( a_iBefore < 0 )
		a_iBefore = 0;
	char* l_pcStr = static_cast<char*>( ::memrchr( f_pcBuffer, a_cChar, f_lSize - a_iBefore ) );
	if ( ! l_pcStr )
		return ( -1 );
	return ( ( f_lSize - 1 ) - ( l_pcStr - f_pcBuffer ) );
	M_EPILOG
	}

HString& HString::replace( HString const& a_oPattern,
		HString const& a_oWith )
	{
	M_PROLOG
	if ( a_oPattern.is_empty() )
		return ( *this );
	int long l_iLenPattern = a_oPattern.get_length();
	int long l_iLenWith = a_oWith.get_length();
	int long l_iSubWP = l_iLenWith - l_iLenPattern;
	int long l_iIndex = 0;
	if ( l_iSubWP == 0 ) /* replacement is equals to pattern */
		{
		while ( ( l_iIndex = find( a_oPattern, l_iIndex ) ) > -1 )
			{
			::std::strncpy( f_pcBuffer + l_iIndex, a_oWith.raw(), l_iLenWith );
			l_iIndex += l_iLenPattern;
			}
		}
	else 
		{
		int long l_lSize = f_lSize;
		while ( ( l_iIndex = find( a_oPattern, l_iIndex ) ) > -1 )
			{
			l_lSize += l_iSubWP;
			l_iIndex += l_iLenPattern;
			}
		HString s;
		HString* src = NULL;
		if ( l_iSubWP > 0 ) /* replacement is longer than pattern */
			{
			s = *this;
			s.materialize();
			hs_realloc( l_lSize + 1 );
			src = &s;
			}
		else /* replacement is shorter than pattern */
			src = this;
		int long oldIdx = 0;
		int long newIdx = 0;
		l_iIndex = 0;
		while ( ( l_iIndex = src->find( a_oPattern, l_iIndex ) ) > -1 )
			{
			if ( newIdx && ( ( l_iIndex - oldIdx ) != l_iLenPattern ) )
				{
				::std::memmove( f_pcBuffer + newIdx, src->f_pcBuffer + oldIdx + l_iLenPattern, ( l_iIndex - oldIdx ) - l_iLenPattern );
				newIdx += ( ( l_iIndex - oldIdx ) - l_iLenPattern );
				}
			else if ( ! newIdx )
				newIdx = l_iIndex;
			oldIdx = l_iIndex;
			::std::memmove( f_pcBuffer + newIdx, a_oWith.raw(), l_iLenWith );
			newIdx += l_iLenWith;
			l_iIndex += l_iLenPattern;
			}
		if ( newIdx && ( ( f_lSize - oldIdx ) != l_iLenPattern ) )
			::std::memmove( f_pcBuffer + newIdx, src->f_pcBuffer + oldIdx + l_iLenPattern, ( f_lSize - oldIdx ) - l_iLenPattern );
		using yaal::swap;
		swap( f_lSize, l_lSize );
		}
	f_pcBuffer[ f_lSize ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::upper( void )
	{
	M_PROLOG
	for ( int long l_iCtr = 0; l_iCtr < f_lSize; l_iCtr++ )
		f_pcBuffer[ l_iCtr ] = static_cast<char>( ::std::toupper( f_pcBuffer[ l_iCtr ] ) );
	return ( *this );
	M_EPILOG
	}

HString& HString::lower( void )
	{
	M_PROLOG
	for ( int long l_iCtr = 0; l_iCtr < f_lSize; l_iCtr++ )
		f_pcBuffer[ l_iCtr ] = static_cast<char>( ::std::tolower( f_pcBuffer[ l_iCtr ] ) );
	return ( *this );
	M_EPILOG
	}

HString& HString::reverse( void )
	{
	M_PROLOG
	char a_cChar;
	for ( int long l_iCtr = 0; l_iCtr < f_lSize; l_iCtr++ )
		{
		a_cChar = static_cast<char>( ::std::toupper( f_pcBuffer[ l_iCtr ] ) );
		if ( a_cChar == f_pcBuffer[ l_iCtr ] )
			f_pcBuffer[ l_iCtr ] = static_cast<char>( ::std::tolower( f_pcBuffer[ l_iCtr ] ) );
		else
			f_pcBuffer[ l_iCtr ] = a_cChar;
		}
	return ( * this );
	M_EPILOG
	}

HString HString::left( int long const& a_iTo ) const
	{
	M_PROLOG
	HString l_oStr;
	if ( a_iTo < 1 )
		return ( l_oStr );
	l_oStr.f_lSize = ( a_iTo < f_lSize ? a_iTo : f_lSize );
	l_oStr.hs_realloc( l_oStr.f_lSize + 1 );
	::std::strncpy( l_oStr.f_pcBuffer, f_pcBuffer, l_oStr.f_lSize );
	l_oStr.f_pcBuffer[ l_oStr.f_lSize ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString HString::mid( int long a_iFrom, int long a_iLength ) const
	{
	M_PROLOG
	HString l_oStr;
	if ( a_iFrom < 0 )
		a_iLength += a_iFrom, a_iFrom = 0;
	if ( ( a_iLength <= 0 ) || ( a_iFrom >= f_lSize ) )
		return ( l_oStr );
	l_oStr.f_lSize = ( a_iLength < f_lSize ? a_iLength : f_lSize );
	if ( ( l_oStr.f_lSize + a_iFrom ) > f_lSize )
		l_oStr.f_lSize = f_lSize - a_iFrom;
	l_oStr.hs_realloc( l_oStr.f_lSize + 1 );
	::std::strncpy( l_oStr.f_pcBuffer, f_pcBuffer + a_iFrom, l_oStr.f_lSize );
	l_oStr.f_pcBuffer[ l_oStr.f_lSize ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString HString::right( int long const& a_iFromEnd ) const
	{
	M_PROLOG
	HString l_oStr;
	if ( a_iFromEnd < 1 )
		return ( l_oStr );
	l_oStr.f_lSize = ( a_iFromEnd < f_lSize ? a_iFromEnd : f_lSize );
	l_oStr.hs_realloc( l_oStr.f_lSize + 1 );
	::std::strncpy( l_oStr.f_pcBuffer, f_pcBuffer + f_lSize - l_oStr.f_lSize,
			l_oStr.f_lSize );
	l_oStr.f_pcBuffer[ l_oStr.f_lSize ] = 0;
	return ( l_oStr );
	M_EPILOG
	}

HString& HString::trim_left( char const* const a_pcSet )
	{
	M_PROLOG
	M_ASSERT( f_pcBuffer );
	int l_iCut = 0;
	while ( f_pcBuffer[ l_iCut ] && ::std::strchr( a_pcSet, f_pcBuffer[ l_iCut ] ) )
		++ l_iCut;
	if ( l_iCut )
		shift_left( l_iCut );
	return ( *this );
	M_EPILOG
	}

HString& HString::trim_right( char const* const a_pcSet )
	{
	M_PROLOG
	M_ASSERT( f_pcBuffer );
	int long l_iCut = 0;
	while ( ( l_iCut < f_lSize )
			&& ::std::strchr( a_pcSet, f_pcBuffer[ f_lSize - ( l_iCut + 1 ) ] ) )
		++ l_iCut;
	if ( l_iCut ) 
		{
		f_lSize -= l_iCut;
		f_pcBuffer[ f_lSize ] = 0;
		}
	M_ASSERT( f_lSize >= 0 );
	return ( *this );
	M_EPILOG
	}

HString& HString::shift_left( int long const& a_iShift )
	{
	M_PROLOG
	if ( a_iShift < 0 )
		M_THROW( "bad left shift lenght", a_iShift );
	if ( a_iShift )
		{
		if ( a_iShift < f_lSize )
			{
			f_lSize -= a_iShift;
			::std::memmove( f_pcBuffer, f_pcBuffer + a_iShift, f_lSize + 1 );
			}
		else
			{
			f_lSize = 0;
			f_pcBuffer[ f_lSize ] = 0;
			}
		}
	return ( *this );
	M_EPILOG
	}

HString& HString::shift_right( int long const& a_iShift, char const a_cFiller )
	{
	M_PROLOG
	if ( a_iShift < 0 )
		M_THROW( "bad right shift lenght", a_iShift );
	if ( a_iShift )
		{
		int long oldSize = f_lSize;
		f_lSize += a_iShift;
		hs_realloc( f_lSize + 1 );
		::std::memmove( f_pcBuffer + a_iShift, f_pcBuffer, oldSize + 1 );
		fill( a_cFiller, 0, a_iShift );
		f_lSize = oldSize + a_iShift;
		}
	return ( * this );
	M_EPILOG
	}

HString& HString::fill( char a_cFiller, int long a_iOffset, int long a_iCount )
	{
	M_PROLOG
	if ( a_iCount < 0 )
		M_THROW( _( "bad length" ), a_iCount );
	if ( a_iOffset < 0 )
		M_THROW( _( "bad offset" ), a_iOffset );
	if ( ( a_iOffset + a_iCount ) >= f_lAllocatedBytes )
		M_THROW( _( "overflow" ), a_iOffset + a_iCount );
	if ( a_iCount == 0 )
		a_iCount = ( f_lAllocatedBytes - a_iOffset ) - 1; /* we maintain zero terminator (even though it is not fillz()) hence - 1 */
	if ( ( a_iCount + a_iOffset ) > f_lSize )
		f_lSize = a_iCount + a_iOffset;
	::std::memset( f_pcBuffer + a_iOffset, a_cFiller, a_iCount );
	f_pcBuffer[ f_lSize ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::fillz( char a_cFiller, int long a_iOffset, int long a_iCount )
	{
	M_PROLOG
	fill( a_cFiller, a_iOffset, a_iCount );
	f_pcBuffer[ a_iCount + a_iOffset ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::erase( int long a_iFrom, int long a_iLength )
	{
	M_PROLOG
	if ( a_iFrom < 0 )
		a_iLength += a_iFrom, a_iFrom = 0;
	if ( ( a_iFrom + a_iLength ) >= f_lSize )
		a_iLength = f_lSize - a_iFrom;
	if ( ( a_iLength > 0 ) && ( a_iFrom < f_lSize ) )
		{
		::std::memmove( f_pcBuffer + a_iFrom, f_pcBuffer + a_iFrom + a_iLength, f_lSize - ( a_iFrom + a_iLength ) );
		f_lSize -= a_iLength;
		f_pcBuffer[ f_lSize ] = 0;
		}
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
			if ( static_cast< ::std::size_t>( -a_iFrom ) > ::std::strlen( a_pcChunk ) )
				M_THROW( "negative offset caused chunk overflow", a_iFrom );
			a_pcChunk += -a_iFrom;
			}
		a_iFrom = 0;
		}
	if ( ( a_iLength > 0 ) && ( a_iFrom <= f_lSize ) )
		{
		int long chunkLen = a_pcChunk ? static_cast<int long>( ::std::strlen( a_pcChunk ) ) : 0;
		if ( a_pcChunk && ( a_iLength > chunkLen ) )
			M_THROW( "length too big for this chunk (by)", a_iLength - chunkLen );
		int long oldSize = f_lSize;
		f_lSize += a_iLength;
		hs_realloc( f_lSize + 1 );
		::std::memmove( f_pcBuffer + a_iFrom + a_iLength, f_pcBuffer + a_iFrom, ( oldSize + 1 ) - a_iFrom );
		if ( a_pcChunk )
			::std::strncpy( f_pcBuffer + a_iFrom, a_pcChunk, a_iLength );
		}
	return ( *this );
	M_EPILOG
	}

HString& HString::insert( int long a_iFrom, char const* a_pcChunk )
	{
	M_PROLOG
	return ( insert( a_iFrom, ::strlen( a_pcChunk ), a_pcChunk ) );
	M_EPILOG
	}

HString& HString::insert( int long a_iFrom, char const* a_pcChunk, int long a_iLength )
	{
	M_PROLOG
	return ( insert( a_iFrom, a_iLength, a_pcChunk ) );
	M_EPILOG
	}

HString& HString::insert( int long a_iFrom, int long a_iLength, char const& a_cChar )
	{
	M_PROLOG
	insert( a_iFrom, a_iLength );
	return ( fill( a_cChar, a_iFrom, a_iLength ) );
	M_EPILOG
	}

namespace string_helper
{

/* all str* and mem* functions takes const pointer as argument and returns
	 non const pointer */
char* strrnpbrk( char const* const a_pcBuffer,
		char const* const a_pcStopSet, int long a_iLength )
	{
	M_PROLOG
	if ( a_iLength < 1 )
		return ( NULL );
	int long l_iStopSetSize = ::std::strlen( a_pcStopSet );
	int long l_iIndex = a_iLength - 1;
	while ( l_iIndex )
		{
		if ( ::std::memchr( a_pcStopSet, a_pcBuffer[ l_iIndex ], l_iStopSetSize ) )
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
	int long l_iSkipSetSize = ::std::strlen( a_pcSkipSet );
	int long l_iIndex = a_iLength - 1;
	while ( l_iIndex )
		{
		if ( ! ::std::memchr( a_pcSkipSet, a_pcBuffer[ l_iIndex ], l_iSkipSetSize ) )
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

int long kmpcasesearch( char const* const str, int long const& lenstr, char const* const pat, int long const& lenpat )
	{
	typedef HPool<int> kmpnext_t;
	kmpnext_t KMPnext( lenpat + 1 );
	int* next = KMPnext.raw();
	int b = next[ 0 ] = -1;
	for ( int i = 1; i <= lenpat; ++ i )
		{
		while ( ( b > -1 ) && ( tolower( pat[ b ] ) != tolower( pat[ i - 1 ] ) ) )
			b = next[ b ];
		++ b;
		next[ i ] = ( tolower( pat[ i ] ) == tolower( pat[ b ] ) ) ? next[ b ] : b;
		}
	int start = -1;
	b = 0;
	for ( int i = 0; i < lenstr; ++ i )
		{
		while ( ( b > -1 ) && ( tolower( pat[ b ] ) != tolower( str[ i ] ) ) )
			b = next[ b ];
		if ( ++ b < lenpat )
			continue;
		start = i - b + 1;
		break;
		}
	return ( start );
	}

}

int strcasecmp( HString const& left, HString const& right )
	{
	return ( ::strcasecmp( left.raw(), right.raw() ) );
	}

}

}

