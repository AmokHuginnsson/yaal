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

namespace yaal
{

namespace hcore
{

namespace
{
int const D_SPECIAL_CHARS = 3; /* minus, dot, nil */
char const* const D_VALID_CHARACTERS = "-.0123456789";
int D_VALID_FIRST_CHARACTERS = 0;
int D_VALID_SECOND_CHARACTERS = 1;
int D_VALID_THIRD_CHARACTERS = 2;
int const D_HARDCODED_MINIMUM_DEFAULT_PRECISION = 16;
}

int HNumber::D_DEFAULT_PRECISION = 100;

HNumber::HNumber( void )
	: f_iPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_oCanonical( f_iPrecision + D_SPECIAL_CHARS, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	f_oCanonical[ 0 ] = '0';
	f_oCanonical[ 1 ] = 0;
	return;
	M_EPILOG
	}

HNumber::~HNumber( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HNumber::from_double( double a_dNumber )
	{
	M_PROLOG
	HString source( a_dNumber );
	char const* const src = source.raw();
	char* dst = f_oCanonical.raw();
	int len = source.get_length();
	( len < f_iPrecision ) || ( len = f_iPrecision );
	for ( int i = 0; i < len; ++ i )
		dst[ i ] = src[ i ];
	dst[ len ] = 0;
	return;
	M_EPILOG
	}

void HNumber::from_string( HString const& a_oNumber )
	{
	M_PROLOG
	int start = a_oNumber.find_one_of( D_VALID_CHARACTERS + D_VALID_FIRST_CHARACTERS );
	M_ENSURE( start >= 0 );
	int end = a_oNumber.find_one_of( D_VALID_CHARACTERS + D_VALID_SECOND_CHARACTERS, start );
	/*
	 * next valid character is next to last find i.e.:
	 * -5 <- valid
	 * -- <- invalid
	 * 5 <- valid
	 */
	M_ENSURE( end >= 0 );
	if ( ( end - start ) > 1 )
		start = end - 1;

	end = a_oNumber.find_one_of( D_VALID_CHARACTERS + D_VALID_THIRD_CHARACTERS, end );
	M_ENSURE( end >= 0 );
	M_ENSURE( ( end - start ) <= 2 );
	
	end = a_oNumber.find_other_than( D_VALID_CHARACTERS + D_VALID_THIRD_CHARACTERS, end );

	int len = a_oNumber.get_length();
	( end > 0 ) || ( end = len );
	len -= start;
	( len < f_iPrecision ) || ( len = f_iPrecision );
	char* dst = f_oCanonical.raw();
	char const* const src = a_oNumber.raw();
	for ( int i = 0; i < len; ++ i )
		dst[ i ] = src[ i + start ];
	dst[ len ] = 0;
	return;
	M_EPILOG
	}

HNumber::HNumber( double long a_dNumber )
	: f_iPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_oCanonical( f_iPrecision + D_SPECIAL_CHARS, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_double( a_dNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( double long a_dNumber, int a_iPrecision )
	: f_iPrecision( a_iPrecision > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? a_iPrecision : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_oCanonical( f_iPrecision + D_SPECIAL_CHARS, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_double( a_dNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( HString const& a_oNumber )
	: f_iPrecision( D_DEFAULT_PRECISION > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? D_DEFAULT_PRECISION : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_oCanonical( f_iPrecision + D_SPECIAL_CHARS, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_oNumber );
	return;
	M_EPILOG
	}

HString HNumber::to_string( void ) const
	{
	M_PROLOG
	return ( f_oCanonical.raw() );
	M_EPILOG
	}

double long HNumber::to_double( void ) const
	{
	M_PROLOG
	return ( strtold( f_oCanonical.raw(), NULL ) );
	M_EPILOG
	}

HNumber::HNumber( HString const& a_oNumber, int a_iPrecision )
	: f_iPrecision( a_iPrecision > D_HARDCODED_MINIMUM_DEFAULT_PRECISION
			? a_iPrecision : D_HARDCODED_MINIMUM_DEFAULT_PRECISION ),
	f_oCanonical( f_iPrecision + D_SPECIAL_CHARS, canonical_t::D_AUTO_GROW )
	{
	M_PROLOG
	from_string( a_oNumber );
	return;
	M_EPILOG
	}

HNumber::HNumber( HNumber const& source )
	: f_iPrecision( source.f_iPrecision ),
	f_oCanonical( f_iPrecision + D_SPECIAL_CHARS, canonical_t::D_AUTO_GROW )
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
		if ( source.f_iPrecision > f_iPrecision )
			f_iPrecision = source.f_iPrecision;
		f_oCanonical = source.f_oCanonical;
		}
	return ( *this );
	M_EPILOG
	}

int HNumber::get_precision( void ) const
	{
	return ( f_iPrecision );
	}

}

}

