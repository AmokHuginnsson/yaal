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

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hnumber.h"
#include "hlog.h"

namespace yaal
{

namespace hcore
{

namespace
{
int const D_SPECIAL_CHARS = 3; /* minus, dot, nil */
char const* const D_VALID_CHARACTERS = "-.0123456789";
int const D_JUST_DIGITS = 2;
int const D_HARDCODED_MINIMUM_DEFAULT_PRECISION = 16;
int const D_A_MINUS = 0;
int const D_A_DOT = 1;
int const D_A_ZERO = 2;
int const D_NUMBER_START = 3;
}

int HNumber::D_DEFAULT_PRECISION = 100;

HNumber::HNumber( void )
	: f_iPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_bNegative( false ), f_iDigitCount( 0 ), f_iIntegralPartSize( 0 ),
	f_oCanonical( f_iDigitCount, canonical_t::D_AUTO_GROW )
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
	: f_iPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_bNegative( false ), f_iDigitCount( 0 ), f_iIntegralPartSize( 0 ),
	f_oCanonical( f_iDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_double( a_dNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( double long a_dNumber, int a_iPrecision )
	: f_iPrecision( a_iPrecision > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? a_iPrecision : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_bNegative( false ), f_iDigitCount( 0 ), f_iIntegralPartSize( 0 ),
	f_oCanonical( f_iDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_double( a_dNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( char const* const a_pcNumber )
	: f_iPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_bNegative( false ), f_iDigitCount( 0 ), f_iIntegralPartSize( 0 ),
	f_oCanonical( f_iDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_pcNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( char const* const a_pcNumber, int a_iPrecision )
	: f_iPrecision( a_iPrecision > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? a_iPrecision : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_bNegative( false ), f_iDigitCount( 0 ), f_iIntegralPartSize( 0 ),
	f_oCanonical( f_iDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_pcNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( HString const& a_oNumber )
	: f_iPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_bNegative( false ), f_iDigitCount( 0 ), f_iIntegralPartSize( 0 ),
	f_oCanonical( f_iDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_oNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( HString const& a_oNumber, int a_iPrecision )
	: f_iPrecision( a_iPrecision > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? a_iPrecision : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_bNegative( false ), f_iDigitCount( 0 ), f_iIntegralPartSize( 0 ),
	f_oCanonical( f_iDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_oNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( HNumber const& source )
	: f_iPrecision( source.f_iPrecision ),
	f_bNegative( source.f_bNegative ),
	f_iDigitCount( source.f_iDigitCount ),
	f_iIntegralPartSize( source.f_iIntegralPartSize ),
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
		f_iPrecision = source.f_iPrecision;
		f_bNegative = source.f_bNegative;
		f_iDigitCount = source.f_iDigitCount;
		f_iIntegralPartSize = source.f_iIntegralPartSize;
		f_oCanonical = source.f_oCanonical;
		}
	return ( *this );
	M_EPILOG
	}

void HNumber::swap( HNumber& source )
	{
	M_PROLOG
	if ( &source != this )
		{
		int l_iPrecision = f_iPrecision;
		bool l_bNegative = f_bNegative;
		int l_iDigitCount = f_iDigitCount;
		int l_iIntegralPartSize = f_iIntegralPartSize;
		f_iPrecision = source.f_iPrecision;
		f_bNegative = source.f_bNegative;
		f_iDigitCount = source.f_iDigitCount;
		f_iIntegralPartSize = source.f_iIntegralPartSize;
		source.f_iPrecision = l_iPrecision;
		source.f_bNegative = l_bNegative;
		source.f_iDigitCount = l_iDigitCount;
		source.f_iIntegralPartSize = l_iIntegralPartSize;
		source.f_oCanonical.swap( f_oCanonical );
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
	int start = a_oNumber.find_one_of( D_VALID_CHARACTERS );
	M_ENSURE( start >= 0 ); /* exclude "!!!!" */
	char const* const src = a_oNumber.raw();
	f_bNegative = ( src[ start ] == D_VALID_CHARACTERS[ D_A_MINUS ] ); /* "!!!-???" */
	if ( f_bNegative )
		++ start;
	int len = a_oNumber.get_length();
	M_ENSURE( start < len ); /* exclude "!!-" */
	M_ENSURE( a_oNumber.find_one_of( D_VALID_CHARACTERS + D_A_DOT, start ) == start ); /* exclude "--" and "-!!" */
	int idx = a_oNumber.find_other_than( "0", start ); /* skip leading 0s */
	int end = start + 1;
	f_iIntegralPartSize = 0;
	f_iDigitCount = 0;
	if ( idx >= 0 ) do /* "!!![-][.1-9]???" or "000." */
		{
		int first_valid = start;
		start = idx;
		int dot = a_oNumber.find( D_VALID_CHARACTERS[ D_A_DOT ], start );
		idx = a_oNumber.find_other_than( D_VALID_CHARACTERS + D_A_DOT, start );
		if ( ( idx >= 0 ) && ( idx < dot ) ) /* "!!232!!." */
			dot = -1;
		int digit = a_oNumber.find_one_of( D_VALID_CHARACTERS + D_A_ZERO, start );
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
		f_iDigitCount = end - start;
		if ( dot >= 0 )
			{
			f_iIntegralPartSize = dot - start;
			-- f_iDigitCount;
			}
		else
			f_iIntegralPartSize = f_iDigitCount;
		if ( decimal_length() > f_iPrecision )
			f_iPrecision = decimal_length();
		if ( f_iDigitCount )
			f_oCanonical.pool_realloc( f_iDigitCount );
		char* dst = f_oCanonical.raw();
		idx = 0;
		for ( int i = start; i < end; ++ i )
			{
			if ( src[ i ] == D_VALID_CHARACTERS[ D_A_DOT ] )
				continue;
			dst[ idx ++ ] = src[ i ] - D_VALID_CHARACTERS[ D_A_ZERO ];
			}
		}
	while ( 0 );
	if ( f_iDigitCount == 0 )
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
	for ( ; digit < f_iIntegralPartSize; ++ digit )
		str += static_cast<char>( src[ digit ] + D_VALID_CHARACTERS[ D_A_ZERO ] );
	if ( f_iDigitCount > f_iIntegralPartSize )
		str += D_VALID_CHARACTERS[ D_A_DOT ];
	for ( ; digit < f_iDigitCount; ++ digit )
		str += static_cast<char>( src[ digit ] + D_VALID_CHARACTERS[ D_A_ZERO ] );
	if ( ! f_iDigitCount )
		str = "0";
	return ( str );
	M_EPILOG
	}

double long HNumber::to_double( void ) const
	{
	M_PROLOG
	return ( strtold( to_string(), NULL ) );
	M_EPILOG
	}

int HNumber::get_precision( void ) const
	{
	return ( f_iPrecision );
	}

void HNumber::set_precision( int a_iPrecision )
	{
	M_PROLOG
	if ( ( a_iPrecision <= f_iPrecision ) || ( decimal_length() < f_iPrecision ) )
		f_iPrecision = a_iPrecision;
	if ( ( f_iIntegralPartSize + f_iPrecision ) < f_iDigitCount )
		f_iDigitCount = f_iIntegralPartSize + f_iPrecision;
	return;
	M_EPILOG
	}

int HNumber::decimal_length( void ) const
	{
	return ( f_iDigitCount - f_iIntegralPartSize );
	}

bool HNumber::is_exact( void ) const
	{
	M_ASSERT( ( f_iDigitCount - f_iIntegralPartSize ) <= f_iPrecision );
	return ( ( f_iDigitCount - f_iIntegralPartSize ) < f_iPrecision );
	}

int HNumber::absolute_lower( HNumber const& other ) const
	{
	char const* p1 = f_oCanonical.raw();
	char const* p2 = other.f_oCanonical.raw();
	int cmp = 1;
	if ( f_iIntegralPartSize < other.f_iIntegralPartSize )
		cmp = -1;
	else if ( f_iIntegralPartSize == other.f_iIntegralPartSize )
		{
		int len = min( f_iDigitCount, other.f_iDigitCount );
		cmp = memcmp( p1, p2, len );
		if ( ! cmp )
			cmp = f_iDigitCount - other.f_iDigitCount;
		}
	return ( cmp );
	}

bool HNumber::operator == ( HNumber const& other ) const
	{
	return ( ( f_bNegative == other.f_bNegative )
			&& ( f_iDigitCount == other.f_iDigitCount )
			&& ( f_iIntegralPartSize == other.f_iIntegralPartSize )
			&& ! ::memcmp( f_oCanonical.raw(), other.f_oCanonical.raw(), f_iDigitCount ) );
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
		int cmp = absolute_lower( other );
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

HNumber HNumber::operator + ( HNumber const& element ) const
	{
	int ips = max( f_iIntegralPartSize, element.f_iIntegralPartSize );
	int dps = max( decimal_length(), element.decimal_length() );
	HNumber n;
	if ( decimal_length() < element.decimal_length() )
		n.f_iPrecision = is_exact() ? element.f_iPrecision : f_iPrecision;
	else
		n.f_iPrecision = element.is_exact() ? f_iPrecision : element.f_iPrecision;
	( dps <= n.f_iPrecision ) || ( dps = n.f_iPrecision );
	int ressize = ips + dps + 1; /* + 1 for possible carrier */
	n.f_oCanonical.pool_realloc( ressize );
	n.f_iIntegralPartSize = ips;
	char* res = n.f_oCanonical.raw();
	bool carrier = false;
	char* ep1 = f_oCanonical.raw();
	char* ep2 = element.f_oCanonical.raw();
	int idx = ressize; /* index of first processed digit */
	int lm[] = { ips - f_iIntegralPartSize, ips - element.f_iIntegralPartSize };
	int rm[] = { dps - decimal_length(), dps - element.decimal_length() };
	( rm[ 0 ] >= 0 ) || ( rm[ 0 ] = 0 );
	( rm[ 1 ] >= 0 ) || ( rm[ 1 ] = 0 );
	int e[ 2 ];
	char* ep[] = { ep1, ep2 };
	bool sub = ( ( f_bNegative && ! element.f_bNegative ) || ( ! f_bNegative && element.f_bNegative ) );
	bool swp = sub && ( absolute_lower( element ) < 0 );
	while ( -- idx > 0 )
		{
		int ld = idx; /* left index distance */
		int rd = ( ressize - 1 ) - idx;/* right index distance */
		for ( int i = 0; i < 2; ++ i )
			{
			int k = swp ? 1 - i : i;
			e[ i ] = 0;
			if ( ep[ k ] && ( ld > lm[ k ] ) && ( rd >= rm[ k ] ) )
				e[ i ] = ep[ k ][ ( idx - lm[ k ] ) - 1 ];
			}
		if ( sub )
			{
			int x = e[ 0 ] - ( carrier ? 1 : 0 );
			if ( x < e[ 1 ] )
				{
				x += 10;
				carrier = true;
				}
			else
				carrier = false;
			res[ idx ] = x - e[ 1 ];
			}
		else
			{
			res[ idx ] =  e[ 0 ] + e[ 1 ] + ( carrier ? 1 : 0 );
			if ( res[ idx ] > 9 )
				{
				res[ idx ] -= 10;
				carrier = true;
				}
			else
				carrier = false;
			}
		}
	if ( ressize > 0 )
		{
		if ( sub )
			{
			int shift = 1;
			while ( ( shift <= n.f_iIntegralPartSize ) && ( res[ shift ] == 0 ) )
				++ shift;
			ressize -= shift;
			++ n.f_iIntegralPartSize -= shift;
			::memmove( res, res + shift , ressize );
			n.f_bNegative = f_bNegative ? ! swp : swp;
			}
		else
			{
			n.f_bNegative = f_bNegative && element.f_bNegative;
			if ( carrier )
				{
				res[ 0 ] = 1;
				++ n.f_iIntegralPartSize;
				}
			else
				{
				-- ressize;
				::memmove( res, res + 1, ressize );
				}
			}
		}
	n.f_iDigitCount = ressize;
	while ( ( n.decimal_length() > 0 ) && ( res[ n.f_iDigitCount - 1 ] == 0 ) )
		-- n.f_iDigitCount;
	if ( n.f_iDigitCount == 0 )
		n.f_bNegative = false;
	return ( n );
	}

HNumber& HNumber::operator += ( HNumber const& element )
	{
	operator = ( *this + element );
	return ( *this );
	}

HNumber HNumber::operator - ( HNumber const& element ) const
	{
	HNumber n( *this );
	n -= element;
	return ( n );
	}

HNumber& HNumber::operator -= ( HNumber const& element )
	{
	operator = ( *this + -element );
	return ( *this );
	}

HNumber HNumber::operator * ( HNumber const& factor ) const
	{
	HNumber n;
	if ( factor.f_iDigitCount < f_iDigitCount )
		n = factor * ( *this );
	else if ( f_iDigitCount && factor.f_iDigitCount )
		{
		HNumber element;
		n.f_oCanonical.pool_realloc( f_iDigitCount + factor.f_iDigitCount );
		element.f_oCanonical.pool_realloc( f_iDigitCount + factor.f_iDigitCount );
		char* e = element.f_oCanonical.raw();
		char const* const fo = f_oCanonical.raw();
		char const* const fi = factor.f_oCanonical.raw();
		element.f_iDigitCount = factor.f_iDigitCount + 1; /* one for carrier */
		int digit = f_iDigitCount; /* index of last digit in first factor */
		int decimal = decimal_length() + factor.decimal_length();
		while ( -- digit >= 0 )
			{
			int carrier = 0;
			int inner = factor.f_iDigitCount; /* index of last digit in second factor */
			e[ element.f_iDigitCount - 1 ] = 0;
			if ( fo[ digit ] )
				{
				while ( -- inner >= 0 )
					{
					int pos = inner + 1;
					e[ pos ] = fo[ digit ] * fi[ inner ] + carrier;
					if ( e[ pos ] > 9 )
						{
						carrier = e[ pos ] / 10;
						e[ pos ] %= 10;
						}
					else
						carrier = 0;
					}
				e[ 0 ] = carrier;
				int lz = 0;
				while ( e[ lz ] == 0 )
					++ lz;
				if ( lz )
					{
					element.f_iDigitCount -= lz;
					::memmove( e, e + lz, element.f_iDigitCount );
					}
				element.f_iIntegralPartSize = element.f_iDigitCount - decimal;
				n += element;
				if ( lz )
					element.f_iDigitCount += lz;
				}
			++ element.f_iDigitCount;
			}
		n.f_bNegative = ! ( ( f_bNegative && factor.f_bNegative ) || ! ( f_bNegative || factor.f_bNegative ) );
		}
	return ( n );
	}

HNumber& HNumber::operator *= ( HNumber const& factor )
	{
	operator = ( *this * factor );
	return ( *this );
	}

HNumber HNumber::operator / ( HNumber const& factor ) const
	{
	M_ENSURE( factor != "0" )
	HNumber n;
	HNumber numerator;
	HNumber denominator( factor );
	denominator.f_iIntegralPartSize = denominator.f_iDigitCount;
	numerator.f_oCanonical.pool_realloc( denominator.f_iDigitCount );
	numerator.f_iDigitCount = numerator.f_iIntegralPartSize = denominator.f_iDigitCount;
	return ( n );
	}

HNumber& HNumber::operator /= ( HNumber const& factor )
	{
	operator = ( *this / factor );
	return ( *this );
	}

HNumber HNumber::operator - ( void ) const
	{
	HNumber n( *this );
	if ( f_iDigitCount )
		n.f_bNegative = ! n.f_bNegative;
	return ( n );
	}

HNumber HNumber::operator ^ ( int long unsigned exp ) const
	{
	HNumber n( *this );
	int long unsigned p = exp >> 1;
	if ( p > 2 )
		n = n ^ p;
	else
		while ( -- p )
			n *= *this;
	return ( ( exp % 2 ) ? ( n * n * *this ) : ( n * n ) );
	}

HNumber& HNumber::operator ^= ( int long unsigned exp )
	{
	operator = ( *this ^ exp );
	return ( *this );
	}

}

}

