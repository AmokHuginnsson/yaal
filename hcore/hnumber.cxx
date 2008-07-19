/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hnumber.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>

#include "base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hnumber.h"

namespace yaal
{

namespace hcore
{

namespace
{
int const D_SPECIAL_CHARS = 3; /* minus, dot, nil */
char const* const D_VALID_CHARACTERS = "-.0123456789";
int const D_JUST_DIGITS = 2;
int const D_HARDCODED_MINIMUM_PRECISION = 16;
int const D_A_MINUS = 0;
int const D_A_DOT = 1;
int const D_A_ZERO = 2;
int const D_NUMBER_START = 3;
int const D_KARATSUBA_THRESHOLD = 20; /* FIXME: 20 is fine */
}

int HNumber::D_DEFAULT_PRECISION = 100;

HNumber::HNumber( void )
	: f_lPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_PRECISION ),
	f_bNegative( false ), f_lDigitCount( 0 ), f_lIntegralPartSize( 0 ),
	f_oCanonical( f_lDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HNumber::~HNumber( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HNumber::HNumber( double long a_dNumber )
	: f_lPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_PRECISION ),
	f_bNegative( false ), f_lDigitCount( 0 ), f_lIntegralPartSize( 0 ),
	f_oCanonical( f_lDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_double( a_dNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( double long a_dNumber, int a_iPrecision )
	: f_lPrecision( a_iPrecision > D_HARDCODED_MINIMUM_PRECISION
			? a_iPrecision : D_HARDCODED_MINIMUM_PRECISION ),
	f_bNegative( false ), f_lDigitCount( 0 ), f_lIntegralPartSize( 0 ),
	f_oCanonical( f_lDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_double( a_dNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( char const* const a_pcNumber )
	: f_lPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_PRECISION ),
	f_bNegative( false ), f_lDigitCount( 0 ), f_lIntegralPartSize( 0 ),
	f_oCanonical( f_lDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_pcNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( char const* const a_pcNumber, int a_iPrecision )
	: f_lPrecision( a_iPrecision > D_HARDCODED_MINIMUM_PRECISION
			? a_iPrecision : D_HARDCODED_MINIMUM_PRECISION ),
	f_bNegative( false ), f_lDigitCount( 0 ), f_lIntegralPartSize( 0 ),
	f_oCanonical( f_lDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_pcNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( HString const& a_oNumber )
	: f_lPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_PRECISION ),
	f_bNegative( false ), f_lDigitCount( 0 ), f_lIntegralPartSize( 0 ),
	f_oCanonical( f_lDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_oNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( HString const& a_oNumber, int a_iPrecision )
	: f_lPrecision( a_iPrecision > D_HARDCODED_MINIMUM_PRECISION
			? a_iPrecision : D_HARDCODED_MINIMUM_PRECISION ),
	f_bNegative( false ), f_lDigitCount( 0 ), f_lIntegralPartSize( 0 ),
	f_oCanonical( f_lDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_oNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( HNumber const& source )
	: f_lPrecision( source.f_lPrecision ),
	f_bNegative( source.f_bNegative ),
	f_lDigitCount( source.f_lDigitCount ),
	f_lIntegralPartSize( source.f_lIntegralPartSize ),
	f_oCanonical( source.f_oCanonical )
	{
	M_PROLOG
	f_oCanonical = source.f_oCanonical;
	return;
	M_EPILOG
	}

HNumber& HNumber::operator = ( HNumber const& source )
	{
	M_PROLOG
	if ( &source != this )
		{
		f_lPrecision = source.f_lPrecision;
		f_bNegative = source.f_bNegative;
		f_lDigitCount = source.f_lDigitCount;
		f_lIntegralPartSize = source.f_lIntegralPartSize;
		f_oCanonical = source.f_oCanonical;
		}
	return ( *this );
	M_EPILOG
	}

void HNumber::swap( HNumber& left, HNumber& right )
	{
	M_PROLOG
	if ( &left != &right )
		{
		yaal::swap( left.f_lPrecision, right.f_lPrecision );
		yaal::swap( left.f_bNegative, right.f_bNegative );
		yaal::swap( left.f_lDigitCount, right.f_lDigitCount );
		yaal::swap( left.f_lIntegralPartSize, right.f_lIntegralPartSize );
		canonical_t::swap( left.f_oCanonical, right.f_oCanonical );
		}
	return;
	M_EPILOG
	}

void HNumber::from_double( double long a_dNumber )
	{
	M_PROLOG
	HString source( a_dNumber );
	from_string( source );
	return;
	M_EPILOG
	}

void HNumber::from_string( HString const& a_oNumber )
	{
	M_PROLOG
	/* ! - represent known but invalid character, ? - represent unknown character */
	int long start = a_oNumber.find_one_of( D_VALID_CHARACTERS );
	M_ENSURE( start >= 0 ); /* exclude "!!!!" */
	char const* const src = a_oNumber.raw();
	f_bNegative = ( src[ start ] == D_VALID_CHARACTERS[ D_A_MINUS ] ); /* "!!!-???" */
	if ( f_bNegative )
		++ start;
	int long len = a_oNumber.get_length();
	M_ENSURE( start < len ); /* exclude "!!-" */
	M_ENSURE( a_oNumber.find_one_of( D_VALID_CHARACTERS + D_A_DOT, start ) == start ); /* exclude "--" and "-!!" */
	int long idx = a_oNumber.find_other_than( "0", start ); /* skip leading 0s */
	int long end = start + 1;
	f_lIntegralPartSize = 0;
	f_lDigitCount = 0;
	if ( idx >= 0 ) do /* "!!![-][.1-9]???" or "000." */
		{
		int long first_valid = start;
		start = idx;
		int long dot = a_oNumber.find( D_VALID_CHARACTERS[ D_A_DOT ], start );
		idx = a_oNumber.find_other_than( D_VALID_CHARACTERS + D_A_DOT, start );
		if ( ( idx >= 0 ) && ( idx < dot ) ) /* "!!232!!." */
			dot = -1;
		int long digit = a_oNumber.find_one_of( D_VALID_CHARACTERS + D_A_ZERO, start );
		if ( ( digit < 0 ) && ( first_valid < start ) )
			break;
		M_ENSURE( digit >= 0 ); /* must have digit */
		M_ENSURE( ( digit - start ) <= 1 ); /* exclude "-..!!" and "..!!" */
		end = a_oNumber.find_other_than( D_VALID_CHARACTERS + ( dot >= 0 ? D_A_ZERO : D_A_DOT ), dot >= 0 ? dot + 1 : start );
		( end >= 0 ) || ( end = len );
		if ( dot >= 0 )
			{
			idx = a_oNumber.reverse_find_other_than( "0", len - end );
			end = ( idx >= 0 ) ? len - idx : start + 1;
			}
		f_lDigitCount = end - start;
		if ( dot >= 0 )
			{
			f_lIntegralPartSize = dot - start;
			-- f_lDigitCount;
			}
		else
			f_lIntegralPartSize = f_lDigitCount;
		if ( decimal_length() >= f_lPrecision )
			f_lPrecision = decimal_length() + 1;
		if ( f_lDigitCount )
			f_oCanonical.pool_realloc( f_lDigitCount );
		char* dst = f_oCanonical.raw();
		idx = 0;
		for ( int long i = start; i < end; ++ i )
			{
			if ( src[ i ] == D_VALID_CHARACTERS[ D_A_DOT ] )
				continue;
			M_ASSERT( src[ i ] >= D_VALID_CHARACTERS[ D_A_ZERO ] );
			dst[ idx ++ ] = static_cast<char>( src[ i ] - D_VALID_CHARACTERS[ D_A_ZERO ] );
			}
		}
	while ( 0 );
	if ( f_lDigitCount == 0 )
		f_bNegative = false;
	return;
	M_EPILOG
	}

HString HNumber::to_string( void ) const
	{
	M_PROLOG
	HString str = "";
	if ( f_bNegative )
		str = D_VALID_CHARACTERS[ D_A_MINUS ];
	char const* const src = f_oCanonical.raw();
	int digit = 0;
	for ( ; digit < f_lIntegralPartSize; ++ digit )
		str += static_cast<char>( src[ digit ] + D_VALID_CHARACTERS[ D_A_ZERO ] );
	if ( f_lDigitCount > f_lIntegralPartSize )
		str += D_VALID_CHARACTERS[ D_A_DOT ];
	for ( ; digit < f_lDigitCount; ++ digit )
		str += static_cast<char>( src[ digit ] + D_VALID_CHARACTERS[ D_A_ZERO ] );
	if ( ! f_lDigitCount )
		str = "0";
	return ( str );
	M_EPILOG
	}

double long HNumber::to_double( void ) const
	{
	M_PROLOG
	return ( lexical_cast<double>( to_string() ) );
	M_EPILOG
	}

int long HNumber::get_precision( void ) const
	{
	return ( f_lPrecision );
	}

void HNumber::set_precision( int long a_iPrecision )
	{
	M_PROLOG
	if ( ( a_iPrecision >= D_HARDCODED_MINIMUM_PRECISION )
			&& ( ( a_iPrecision <= f_lPrecision ) || ( decimal_length() < f_lPrecision ) ) )
		f_lPrecision = a_iPrecision;
	if ( ( f_lIntegralPartSize + f_lPrecision ) < f_lDigitCount )
		f_lDigitCount = f_lIntegralPartSize + f_lPrecision;
	return;
	M_EPILOG
	}

int long HNumber::decimal_length( void ) const
	{
	return ( f_lDigitCount - f_lIntegralPartSize );
	}

bool HNumber::is_exact( void ) const
	{
	M_ASSERT( ( f_lDigitCount - f_lIntegralPartSize ) <= f_lPrecision );
	return ( ( f_lDigitCount - f_lIntegralPartSize ) < f_lPrecision );
	}

int long HNumber::absolute_lower( HNumber const& other ) const
	{
	char const* p1 = f_oCanonical.raw();
	char const* p2 = other.f_oCanonical.raw();
	int long cmp = 1;
	if ( f_lIntegralPartSize < other.f_lIntegralPartSize )
		cmp = -1;
	else if ( f_lIntegralPartSize == other.f_lIntegralPartSize )
		{
		int long len = min( f_lDigitCount, other.f_lDigitCount );
		cmp = memcmp( p1, p2, len );
		if ( ! cmp )
			cmp = f_lDigitCount - other.f_lDigitCount;
		}
	return ( cmp );
	}

bool HNumber::operator == ( HNumber const& other ) const
	{
	return ( ( f_bNegative == other.f_bNegative )
			&& ( f_lDigitCount == other.f_lDigitCount )
			&& ( f_lIntegralPartSize == other.f_lIntegralPartSize )
			&& ! ::memcmp( f_oCanonical.raw(), other.f_oCanonical.raw(), f_lDigitCount ) );
	}

bool HNumber::operator != ( HNumber const& other ) const
	{
	return ( ! operator == ( other ) );
	}

bool HNumber::operator < ( HNumber const& other ) const
	{
	bool lower = false;
	if ( f_bNegative && ! other.f_bNegative )
		lower = true;
	else if ( ( f_bNegative && other.f_bNegative ) || ( ! ( f_bNegative || other.f_bNegative ) ) )
		{
		int long cmp = absolute_lower( other );
		lower = cmp < 0;
		if ( cmp && f_bNegative && other.f_bNegative )
			lower = ! lower;
		}
	return ( lower );
	}

bool HNumber::operator > ( HNumber const& other ) const
	{
	return ( other < *this );
	}

bool HNumber::operator <= ( HNumber const& other ) const
	{
	return ( ( operator == ( other ) ) || ( operator < ( other ) ) );
	}

bool HNumber::operator >= ( HNumber const& other ) const
	{
	return ( other <= *this );
	}

bool HNumber::mutate_addition( char* res, int long ressize,
		char const* const ep[], int long* lm, int long* rm, bool sub, bool swp ) const
	{
	int carrier = 0;
	int e[ 2 ];
	int long lmx[] = { lm ? ( swp ? lm[ 1 ] : lm[ 0 ] ) : 0, lm ? ( swp ? lm[ 0 ] : lm[ 1 ] ) : 0 };
	int long rmx[] = { rm ? ( swp ? rm[ 1 ] : rm[ 0 ] ) : 0, rm ? ( swp ? rm[ 0 ] : rm[ 1 ] ) : 0 };
	char const* const epx[] =
		{
		swp ? ep[ 1 ] - lmx[ 0 ] - 1 : ep[ 0 ] - lmx[ 0 ] - 1,
		swp ? ep[ 0 ] - lmx[ 1 ] - 1 : ep[ 1 ] - lmx[ 1 ] - 1
		};
	int long idx = ressize - 1; /* index of first processed digit */
	int side = ( rmx[ 0 ] > rmx[ 1 ] ) ? 1 : 0; 
	int long off = rmx[ 1 - side ];
	char const* src = epx[ side ];
	e[ 1 - side ] = 0;
	while ( off -- && ( idx > 0 ) )
		{
		e[ side ] = src[ idx ];
		if ( sub )
			{
			int x = e[ 0 ] - carrier;
			if ( x < e[ 1 ] )
				{
				x += 10;
				carrier = 1;
				}
			else
				carrier = 0;
			res[ idx ] = static_cast<char>( x - e[ 1 ] );
			}
		else
			{
		 	int x = e[ 0 ] + e[ 1 ] + carrier;
			if ( x > 9 )
				{
				x -= 10;
				carrier = 1;
				}
			else
				carrier = 0;
			res[ idx ] = static_cast<char>( x );
			}
		-- idx;
		}
	side = ( lmx[ 0 ] > lmx[ 1 ] ) ? 1 : 0; 
	off = lmx[ 1 - side ];
	while ( idx > off )
		{
		if ( sub )
			{
			int x = epx[ 0 ][ idx ] - carrier;
			if ( x < epx[ 1 ][ idx ] )
				{
				x += 10;
				carrier = 1;
				}
			else
				carrier = 0;
			res[ idx ] = static_cast<char>( x - epx[ 1 ][ idx ] );
			}
		else
			{
			int x = epx[ 0 ][ idx ] + epx[ 1 ][ idx ] + carrier;
			if ( x > 9 )
				{
				x -= 10;
				carrier = 1;
				}
			else
				carrier = 0;
			res[ idx ] = static_cast<char>( x );
			}
		-- idx;
		}
	src = epx[ side ];
	e[ 1 - side ] = 0;
	while ( idx > 0 )
		{
		e[ side ] = src[ idx ];
		if ( sub )
			{
			int x = e[ 0 ] - carrier;
			if ( x < e[ 1 ] )
				{
				x += 10;
				carrier = 1;
				}
			else
				carrier = 0;
			res[ idx ] = static_cast<char>( x - e[ 1 ] );
			}
		else
			{
			int x = e[ 0 ] + e[ 1 ] + carrier;
			if ( x > 9 )
				{
				x -= 10;
				carrier = 1;
				}
			else
				carrier = 0;
			res[ idx ] = static_cast<char>( x );
			}
		-- idx;
		}
	if ( carrier && ! sub )
		++ res[ 0 ];
	return ( carrier );
	}

HNumber HNumber::operator + ( HNumber const& element ) const
	{
	M_PROLOG
	int long ips = max( f_lIntegralPartSize, element.f_lIntegralPartSize );
	int long dps = max( decimal_length(), element.decimal_length() );
	HNumber n;
	if ( decimal_length() < element.decimal_length() )
		n.f_lPrecision = is_exact() ? element.f_lPrecision : f_lPrecision;
	else
		n.f_lPrecision = element.is_exact() ? f_lPrecision : element.f_lPrecision;
	( dps <= n.f_lPrecision ) || ( dps = n.f_lPrecision );
	int long ressize = ips + dps + 1; /* + 1 for possible carrier */
	n.f_oCanonical.pool_realloc( ressize );
	n.f_lIntegralPartSize = ips;
	char* res = n.f_oCanonical.raw();
	char const* ep1 = f_oCanonical.raw();
	char const* ep2 = element.f_oCanonical.raw();
	int long lm[] = { ips - f_lIntegralPartSize, ips - element.f_lIntegralPartSize };
	int long rm[] = { dps - decimal_length(), dps - element.decimal_length() };
	( rm[ 0 ] >= 0 ) || ( rm[ 0 ] = 0 );
	( rm[ 1 ] >= 0 ) || ( rm[ 1 ] = 0 );
	char const* ep[] = { ep1, ep2 };
	bool sub = ( ( f_bNegative && ! element.f_bNegative ) || ( ! f_bNegative && element.f_bNegative ) );
	bool swp = sub && ( absolute_lower( element ) < 0 );
	mutate_addition( res, ressize, ep, lm, rm, sub, swp );
	if ( ressize > 0 )
		{
		n.f_bNegative = sub ? ( f_bNegative ? ! swp : swp ) : ( f_bNegative && element.f_bNegative );
		++ n.f_lIntegralPartSize;
		}
	n.f_lDigitCount = ressize;
	n.normalize();
	if ( n.f_lDigitCount == 0 )
		n.f_bNegative = false;
	return ( n );
	M_EPILOG
	}

HNumber& HNumber::operator += ( HNumber const& element )
	{
	M_PROLOG
	operator = ( *this + element );
	return ( *this );
	M_EPILOG
	}

HNumber HNumber::operator - ( HNumber const& element ) const
	{
	M_PROLOG
	HNumber n( *this );
	n -= element;
	return ( n );
	M_EPILOG
	}

HNumber& HNumber::operator -= ( HNumber const& element )
	{
	M_PROLOG
	operator = ( *this + -element );
	return ( *this );
	M_EPILOG
	}

HNumber HNumber::operator * ( HNumber const& factor ) const
	{
	M_PROLOG
	HNumber n;
	if ( factor.f_lDigitCount < f_lDigitCount )
		n = factor * ( *this );
	else if ( f_lDigitCount && factor.f_lDigitCount )
		{
		n.f_lDigitCount = f_lDigitCount + factor.f_lDigitCount;
		n.f_lIntegralPartSize = f_lIntegralPartSize + factor.f_lIntegralPartSize;
		karatsuba( n.f_oCanonical,
				f_oCanonical.raw(), f_lDigitCount,
				factor.f_oCanonical.raw(), factor.f_lDigitCount );
		n.normalize();
		n.f_bNegative = ! ( ( f_bNegative && factor.f_bNegative ) || ! ( f_bNegative || factor.f_bNegative ) );
		}
	return ( n );
	M_EPILOG
	}

HNumber& HNumber::operator *= ( HNumber const& factor )
	{
	M_PROLOG
	operator = ( *this * factor );
	return ( *this );
	M_EPILOG
	}

HNumber HNumber::operator / ( HNumber const& denominator ) const
	{
	M_PROLOG
	M_ENSURE( denominator.f_lDigitCount != 0 )
	HNumber n;
	if ( f_lDigitCount )
		{
		n.f_lPrecision = f_lPrecision + denominator.f_lPrecision;
		char const* const den = denominator.f_oCanonical.raw();
		int long shift = 0;
		while ( ( shift < denominator.f_lDigitCount ) && ( den[ shift ] == 0 ) )
			++ shift;
		int long denlen = denominator.f_lDigitCount - shift;
		canonical_t reminder( denlen + 1 ); /* + 1 for carrier */
		canonical_t pseudoden( denlen + 1 );
		char const* src = f_oCanonical.raw();
		char* pden = pseudoden.raw();
		char* rem = reminder.raw();
		char const* ep[] = { rem, pden };
		int long len = min( f_lDigitCount, denlen );
		::memcpy( pden + 1, denominator.f_oCanonical.raw() + shift, denlen );
		::memcpy( rem + 1 + denlen - len, src, len );
		shift = 0;
		while ( ( shift < f_lDigitCount ) && ( src[ shift ++ ] == 0 ) )
			n.f_oCanonical.push_back( 0 );
		shift = denominator.f_lIntegralPartSize - f_lIntegralPartSize;
		while ( -- shift > 0 )
			n.f_oCanonical.push_back( 0 );
		int cmp = 0;
		shift = 0;
		bool carrier = ( denominator.f_lIntegralPartSize - f_lIntegralPartSize - denominator.decimal_length() ) > 0;
		bool ncar = false;
		int long pred_int = f_lIntegralPartSize - denominator.f_lIntegralPartSize + denominator.f_lDigitCount - denlen;
		( pred_int >= 0 ) || ( pred_int = 0 );
		++ pred_int;
		int long size = n.f_oCanonical.size();
		do
			{
			int digit = 0;
			while ( ( cmp = memcmp( rem, pden, denlen + 1 ) ) > 0 )
				{
				mutate_addition( rem - 1, denlen + 1 + 1, ep, NULL, NULL, true, false );
				++ digit;
				}
			::memmove( rem, rem + 1, denlen );
			rem[ denlen ] = len < f_lDigitCount ? src[ len ] : '\0';
			if ( ! cmp )
				{
				::memset( rem, 0, denlen );
				++ digit;
				}
			if ( digit || shift )
				{
				n.f_oCanonical.push_back( static_cast<char>( digit ) );
				++ size;
				shift = 1;
				}
			if ( rem[ 0 ] && carrier && ! shift )
				{
				n.f_oCanonical.push_back( 0 );
				++ size;
				carrier = false;
				}
			else if ( rem[ 0 ] && ! shift )
				ncar = true;
			++ len;
			}
		while ( ( len <= f_lDigitCount ) || ( ( ( size - pred_int ) < n.f_lPrecision ) && cmp ) );
		n.f_lDigitCount = n.f_oCanonical.size();
		n.f_lIntegralPartSize = f_lIntegralPartSize - denominator.f_lIntegralPartSize + denominator.f_lDigitCount - denlen + ( ! ncar ? 1 : 0 );
		while ( n.f_lDigitCount < n.f_lIntegralPartSize )
			{
			++ n.f_lDigitCount;
			n.f_oCanonical.push_back( 0 );
			}
		char* res = n.f_oCanonical.raw();
		if ( ( n.f_lIntegralPartSize < 0 ) && ncar )
			{
			n.f_oCanonical.push_back( 0 );
			res = n.f_oCanonical.raw();
			::memmove( res + 1, res, n.f_lDigitCount );
			res[ 0 ] = 0;
			++ n.f_lDigitCount;
			}
		( n.f_lIntegralPartSize >= 0 ) || ( n.f_lIntegralPartSize = 0 );
		n.normalize();
		if ( n.f_lDigitCount > ( n.f_lIntegralPartSize + n.f_lPrecision ) )
			n.f_lDigitCount = n.f_lIntegralPartSize + n.f_lPrecision;
		M_ASSERT( n.f_lIntegralPartSize >= 0 );
		M_ASSERT( n.f_lDigitCount >= 0 );
		n.f_bNegative = ! ( ( f_bNegative && denominator.f_bNegative ) || ! ( f_bNegative || denominator.f_bNegative ) );
		}
	return ( n );
	M_EPILOG
	}

HNumber& HNumber::operator /= ( HNumber const& factor )
	{
	M_PROLOG
	operator = ( *this / factor );
	return ( *this );
	M_EPILOG
	}

HNumber HNumber::operator - ( void ) const
	{
	M_PROLOG
	HNumber n( *this );
	if ( f_lDigitCount )
		n.f_bNegative = ! n.f_bNegative;
	return ( n );
	M_EPILOG
	}

HNumber HNumber::operator ^ ( int long unsigned exp ) const
	{
	M_PROLOG
	HNumber n( *this );
	int long unsigned p = exp >> 1;
	if ( p > 2 )
		n = n ^ p;
	else
		while ( -- p )
			n *= *this;
	return ( ( exp % 2 ) ? ( n * n * *this ) : ( n * n ) );
	M_EPILOG
	}

HNumber& HNumber::operator ^= ( int long unsigned exp )
	{
	M_PROLOG
	operator = ( *this ^ exp );
	return ( *this );
	M_EPILOG
	}

void HNumber::normalize( void )
	{
	M_PROLOG
	char* res = f_oCanonical.raw();
	int shift = 0;
	while ( ( shift < f_lIntegralPartSize ) && ( res[ shift ] == 0 ) )
		++ shift;
	if ( shift )
		{
		f_lIntegralPartSize -= shift;
		f_lDigitCount -= shift;
		::memmove( res, res + shift, f_lDigitCount );
		}
	while ( ( decimal_length() > 0 ) && ( res[ f_lDigitCount - 1 ] == 0 ) )
		-- f_lDigitCount;
	if ( f_lDigitCount == ( f_lIntegralPartSize + f_lPrecision ) )
		++ f_lPrecision;
	M_EPILOG
	return;
	}

void HNumber::karatsuba( canonical_t& result, char const* fx, int long fxs, char const* fy, int long fys ) const
	{
	int long shift = 0;
	while ( ( shift < fxs ) && ! fx[ shift ] )
		++ shift;
	int long fxrl = fxs - shift;
	int long totalShift = shift;
	shift = 0;
	while ( ( shift < fys ) && ! fy[ shift ] )
		++ shift;
	int long fyrl = fys - shift;
	totalShift += shift;
	if ( ( ( fxrl > D_KARATSUBA_THRESHOLD ) || ( fyrl > D_KARATSUBA_THRESHOLD ) ) && ( fxrl > 0 ) && ( fyrl > 0 ) )
		{
		fx += ( fxs - fxrl );
		fy += ( fys - fyrl );
		fxs = fxrl;
		fys = fyrl;
		if ( ! result.size() && ( fxs + fys ) )
			result.pool_realloc( fxs + fys + totalShift );

		int long fs = max( fxs, fys );
		int long fl = min( fxs, fys );
		int long m = ( fs / 2 ) + ( fs & 1 ); /* Size of upper/lower half of number */
		canonical_t r2m( 0 ); /* intermediate result ( fx1 * fx2 ( * B ^ 2m ) ) + 1 for carrier */
		karatsuba( r2m, fx, fxs - m, fy, fys - m );
		canonical_t r( 0 ); /* intermediate result ( fx2 * fy2 ) + 1 for carrier */
		karatsuba( r, fx + ( fxs > m ? fxs - m : 0 ), min( fxs, m ), fy + ( fys > m ? fys - m : 0 ), min( fys, m ) );
		canonical_t hx( m + 1 ); /* + 1 for carrier */
		canonical_t hy( m + 1 ); 
		/* preparation of hx and hy */
		/* hx = fx / B^m + fx % B^m */
		int long lm[] = { 2 * m - fxs, 0 };
		char const* ep[] = { fx, fx + fxs - m };
		if ( fxs > m )
			mutate_addition( hx.raw(), m + 1, ep, lm, NULL, false, false );
		else
			memcpy( hx.raw() + hx.size() - fxs, fx, fxs );
		/* hy */
		lm[ 0 ] = 2 * m - fys;
		ep[ 0 ] = fy;
		ep[ 1 ] = fy + fys - m;
		if ( fys > m )
			mutate_addition( hy.raw(), m + 1, ep, lm, NULL, false, false );
		else
			memcpy( hy.raw() + hy.size() - fys, fy, fys );
		/* find Z */
		canonical_t Z( 0 );
		karatsuba( Z, hx.raw(), hx.size(), hy.raw(), hy.size() );
		/* combine all results */
		
		canonical_t tmpres( fxs + fys + 1 );
		int long size = tmpres.size();
		char* res = tmpres.raw();

		/* res = Z*B^m + r */
		int long s = Z.size();
		char const* p = Z.raw();
		shift = 0;
		while ( ( shift < s ) && ! p[ shift ] )
			++ shift;
		s -= shift;
		p += shift;
		ep[ 0 ] = r.raw();
		ep[ 1 ] = p;
		lm[ 0 ] = s - r.size() + m;
		lm[ 1 ] = 0;
		if ( ep[ 0 ] )
			{
			::memcpy( res + size - r.size(), r.raw(), r.size() );
			mutate_addition( res + size - m - s - 1, s + 1, ep, lm, NULL, false, false );
			}
		else
			::memcpy( res + size - m - s, p, s );

		/* res += r2m*B^2m */
		s = r2m.size();
		shift = 0;
		p = r2m.raw();
		lm[ 0 ] = 0;
		if ( p )
			{
			while ( ( shift < s ) && ! p[ shift ] )
				++ shift;
			s -= shift;
			p += shift;
			ep[ 0 ] = res + 1 + shift;
			ep[ 1 ] = p;
			lm[ 1 ] = 0;
			mutate_addition( res + shift, s + 1, ep, lm, NULL, false, false );
			}

		/* res -= r2m*B^m, res -= r*B^m */

		int ncar = ( fl > m ? 1 : 0 );
		int car = 1 - ncar;

		ep[ 0 ] = res + ncar;
		ep[ 1 ] = r2m.raw();
		if ( ep[ 1 ] )
			{
			lm[ 1 ] = size - r2m.size() - m - ncar;
			mutate_addition( res - car, size - m + car, ep, lm, NULL, true, false );
			}
		ep[ 1 ] = r.raw();
		if ( ep[ 1 ] )
			{
			lm[ 1 ] = size - r.size() - m - ncar;
			mutate_addition( res - car, size - m + car, ep, lm, NULL, true, false );
			}
		::memcpy( result.raw() + totalShift, tmpres.raw() + 1, result.size() - totalShift );
		}
	else if ( ( fxrl > 0 ) && ( fyrl > 0 ) )
		{
/* variables for mutate_addition() */
		if ( ! result.size() )
			result.pool_realloc( fxs + fys );
		canonical_t element( fys + 1 );
		char* e = element.raw();
		char* res = ( result.raw() + fxs ) - 2; /* - 1 for carrier */
		char const* ep[] = { res, e };
/* the end of variables for mutate_addition() */
		while ( -- fxs >= 0 )
			{
			if ( fx[ fxs ] )
				{
				int carrier = 0;
				int long inner = fys; /* index of last digit in second factor */
				while ( -- inner >= 0 )
					{
					int long pos = inner + 1;
					int x = fx[ fxs ] * fy[ inner ] + carrier;
					if ( x > 9 )
						{
						carrier = x / 10;
						x %= 10;
						}
					else
						carrier = 0;
					e[ pos ] = static_cast<char>( x );
					}
				e[ 0 ] = static_cast<char>( carrier );
				ep[ 0 ] = res + 1;
				mutate_addition( res, fys + 1 + 1, ep, NULL, NULL, false, false );
				-- res;
				}
			else
				-- res;
			}
		}
	return;
	}

}

}

