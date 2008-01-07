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
	f_bNegative( false ), f_iDigitCount( 1 ), f_iIntegralPartSize( 1 ),
	f_oCanonical( f_iDigitCount, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	f_oCanonical[ 0 ] = 0;
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
	int start = a_oNumber.find_one_of( D_VALID_CHARACTERS );
	M_ENSURE( start >= 0 ); /* exclude "!!!!" */
	char const* const src = a_oNumber.raw();
	f_bNegative = ( src[ start ] == D_VALID_CHARACTERS[ D_A_MINUS ] ); /* "!!!-???" */
	if ( f_bNegative )
		++ start;
	int len = a_oNumber.get_length();
	M_ENSURE( start < len ); /* exclude "!!-" */
	M_ENSURE( a_oNumber.find_one_of( D_VALID_CHARACTERS + D_A_DOT, start ) == start ); /* exclude "--" and "-!!" */
	int idx = a_oNumber.find_other_than( "0", start );
	int end = start + 1;
	if ( idx < 0 ) /* "!!!-0" or "00000" */
		{
		f_bNegative = false;
		f_iIntegralPartSize = 1;
		f_iDigitCount = 1;
		}
	else /* "!!![-][.1-9]???" */
		{
		start = idx;
		int dot = a_oNumber.find( D_VALID_CHARACTERS[ D_A_DOT ], start );
		log_trace << "dot1: " << dot << endl;
		idx = a_oNumber.find_other_than( D_VALID_CHARACTERS + D_A_DOT, start );
		if ( ( idx >= 0 ) && ( idx < dot ) ) /* "!!232!!." */
			dot = -1;
		int digit = a_oNumber.find_one_of( D_VALID_CHARACTERS + D_A_ZERO, start );
		M_ENSURE( digit >= 0 ); /* must have digit */
		M_ENSURE( ( digit - start ) <= 1 ); /* exclude "-..!!" and "..!!" */
		end = a_oNumber.find_other_than( D_VALID_CHARACTERS + ( dot >= 0 ? D_A_ZERO : D_A_DOT ), dot >= 0 ? dot + 1 : start );
		log_trace << "end: " << end << endl;
		if ( end < 0 )
			end = len;
		log_trace << "dot2: " << dot << endl;
		if ( dot >= 0 )
			{
			log_trace << "len - end: " << len - end << endl;
			idx = a_oNumber.reverse_find_other_than( "0", len - end );
			if ( idx >= 0 )
				end = len - idx;
			}
		f_iDigitCount = end - start;
		if ( dot >= 0 )
			{
			f_iIntegralPartSize = dot - start;
			-- f_iDigitCount;
			}
		else
			f_iIntegralPartSize = f_iDigitCount;
		}
	if ( decimal_length() > f_iPrecision )
		f_iPrecision = decimal_length();
	log_trace << a_oNumber << " " << f_iPrecision << " " << f_iDigitCount << " " << f_iIntegralPartSize << " " << decimal_length() << endl;
	f_oCanonical.pool_realloc( f_iDigitCount );
	char* dst = f_oCanonical.raw();
	idx = 0;
	for ( int i = start; i < end; ++ i )
		{
		if ( src[ i ] == D_VALID_CHARACTERS[ D_A_DOT ] )
			continue;
		dst[ idx ++ ] = src[ i ] - D_VALID_CHARACTERS[ D_A_ZERO ];
		}
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
	char const* canon_this = f_oCanonical.raw();
	char const* canon_other = other.f_oCanonical.raw();
	bool neg_this = canon_this[ 0 ] == D_VALID_CHARACTERS[ D_A_MINUS ];
	bool neg_other = canon_other[ 0 ] == D_VALID_CHARACTERS[ D_A_MINUS ];
	if ( neg_this && ! neg_other )
		return ( true );
	return ( false );
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

HNumber HNumber::operator + ( HNumber const& ) const
	{
	return ( HNumber() );
	}

}

}

