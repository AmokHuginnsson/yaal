/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hnumber.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <cstdio>
#include <cmath>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hnumber.hxx"
#include "number.hxx"
#include "algorithm.hxx"

using namespace yaal::hcore;
using namespace yaal::math;
using namespace yaal::hcore::number;

/*
 * Naming conventions:
 *
 *   addend + addend = sum
 *   (or alternative form)
 *   augend + addend = sum
 *
 *   minuend - subtrahend = difference
 *
 *   multiplicand * factor = product
 *   (or alternative form)
 *   multiplicand * multiplier = product
 *
 *   dividend / divisor = quotient
 *
 *   dividend % divisor = remainder
 */

namespace yaal {

namespace hcore {

namespace {

int const KARATSUBA_THRESHOLD = 20; /* FIXME: 20 is fine */

/*
 * ( leaf % DECIMAL_SHIFT[DECIMAL_DIGITS_IN_LEAF_CONST] ) == leaf
 */

i32_t const LEAF = meta::power<10, DECIMAL_DIGITS_IN_LEAF_CONST>::value;
char ZFORMAT[] = "%00u";

namespace {
char unused __attribute__((used)) = ZFORMAT[2] = static_cast<char>( '0' + DECIMAL_DIGITS_IN_LEAF_CONST );
}

inline i32_t leafcmp( i32_t const* left_, i32_t const* right_, HNumber::integer_t len_ ) {
	i32_t cmp( 0 );
	for ( HNumber::integer_t i( 0 ); ! cmp && ( i < len_ ); ++ i )
		cmp = left_[ i ] - right_[ i ];
	return ( cmp );
}

}

int const HNumber::DECIMAL_DIGITS_IN_LEAF = DECIMAL_DIGITS_IN_LEAF_CONST;
int HNumber::DEFAULT_PRECISION = 100;

HNumber::HNumber( void )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	return;
	M_EPILOG
}

HNumber::~HNumber( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

HNumber::HNumber( double long number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_floating_point( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( double number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_floating_point( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long long number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long long unsigned number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_unsigned_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long long number_, integer_t precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long number_, integer_t precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int number_, integer_t precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( double long number_, integer_t precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_floating_point( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( double number_, integer_t precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_floating_point( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( char const* const number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( char const* const number_, integer_t precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HString const& number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HString const& number_, integer_t precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HNumber const& source )
	: _precision( source._precision )
	, _leafCount( source._leafCount )
	, _integralPartSize( source._integralPartSize )
	, _canonical()
	, _cache()
	, _negative( source._negative ) {
	M_PROLOG
	if ( source._leafCount ) {
		_canonical.realloc( chunk_size<i32_t>( source._leafCount ), HChunk::STRATEGY::EXACT );
		::memcpy( _canonical.raw(), source._canonical.raw(), static_cast<size_t>( chunk_size<i32_t>( source._leafCount ) ) );
		if ( source._cache.get_size() > 0 ) {
			_cache.realloc( source._cache.get_size(), HChunk::STRATEGY::EXACT );
			::memcpy( _cache.raw(), source._cache.raw(), static_cast<size_t>( source._cache.get_size() ) );
		}
	}
	return;
	M_EPILOG
}

HNumber::HNumber( HNumber const& source, integer_t precision_ )
	: _precision( source._precision )
	, _leafCount( source._leafCount )
	, _integralPartSize( source._integralPartSize )
	, _canonical()
	, _cache()
	, _negative( source._negative ) {
	M_PROLOG
	if ( source._leafCount ) {
		_canonical.realloc( chunk_size<i32_t>( source._leafCount ), HChunk::STRATEGY::EXACT );
		::memcpy( _canonical.raw(), source._canonical.raw(), static_cast<size_t>( chunk_size<i32_t>( source._leafCount ) ) );
		if ( source._cache.get_size() > 0 ) {
			_cache.realloc( source._cache.get_size(), HChunk::STRATEGY::EXACT );
			::memcpy( _cache.raw(), source._cache.raw(), static_cast<size_t>( source._cache.get_size() ) );
		}
	}
	set_precision( precision_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HNumber&& number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION )
	, _leafCount( 0 )
	, _integralPartSize( 0 )
	, _canonical()
	, _cache()
	, _negative( false ) {
	M_PROLOG
	swap( number_ );
	return;
	M_EPILOG
}

HNumber& HNumber::operator = ( HNumber const& source ) {
	M_PROLOG
	if ( &source != this ) {
		HNumber tmp( source );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

HNumber& HNumber::operator = ( HNumber&& source ) {
	M_PROLOG
	if ( &source != this ) {
		swap( source );
		source._precision = ( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION ? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION );
		source._leafCount = 0;
		source._integralPartSize = 0;
		source._negative = false;
	}
	return ( *this );
	M_EPILOG
}

void HNumber::swap( HNumber& other ) {
	M_PROLOG
	if ( &other != this ) {
		using yaal::swap;
		swap( _precision, other._precision );
		swap( _leafCount, other._leafCount );
		swap( _integralPartSize, other._integralPartSize );
		swap( _canonical, other._canonical );
		swap( _cache, other._cache );
		swap( _negative, other._negative );
	}
	return;
	M_EPILOG
}

void HNumber::from_floating_point( double long number_ ) {
	M_PROLOG
	HString source( number_ );
	from_string( source );
	return;
	M_EPILOG
}

void HNumber::from_integer( int long long number_ ) {
	M_PROLOG
	from_unsigned_integer( static_cast<int long long unsigned>( yaal::abs( number_ ) ) );
	_negative = ( number_ < 0 );
	return;
	M_EPILOG
}

void HNumber::from_unsigned_integer( int long long unsigned number_ ) {
	M_PROLOG
	if ( number_ >= ( static_cast<int long long unsigned>( LEAF ) * LEAF ) ) {
		_leafCount = 3;
		_canonical.realloc( chunk_size<i32_t>( _leafCount ) );
		i32_t* data( _canonical.get<i32_t>() );
		data[ 0 ] = static_cast<i32_t>( number_ / ( static_cast<int long long unsigned>( LEAF ) * LEAF ) );
		data[ 1 ] = static_cast<i32_t>( ( number_ % ( static_cast<int long long unsigned>( LEAF ) * LEAF ) ) / LEAF );
		data[ 2 ] = static_cast<i32_t>( number_ % LEAF );
	} else if ( number_ >= static_cast<int long long unsigned>( LEAF ) ) {
		_leafCount = 2;
		_canonical.realloc( chunk_size<i32_t>( _leafCount ) );
		i32_t* data( _canonical.get<i32_t>() );
		data[ 0 ] = static_cast<i32_t>( number_ / LEAF );
		data[ 1 ] = static_cast<i32_t>( number_ % LEAF );
	} else if ( number_ != 0 ) {
		_leafCount = 1;
		_canonical.realloc( chunk_size<i32_t>( _leafCount ) );
		_canonical.get<i32_t>()[ 0 ] = static_cast<i32_t>( number_ );
	}
	_integralPartSize = _leafCount;
	return;
	M_EPILOG
}

void HNumber::from_string( HString const& number_ ) {
	M_PROLOG
	/* ! - represent known but invalid character, ? - represent unknown character */
	integer_t start( static_cast<integer_t>( number_.find_one_of( VALID_CHARACTERS ) ) );
	M_ENSURE( start != HString::npos ); /* exclude "!!!!" */
	char const* const src = number_.raw();
	_negative = ( src[ start ] == VALID_CHARACTERS[ A_MINUS ] ); /* "!!!-???" */
	if ( _negative ) {
		++ start;
	}
	integer_t len( static_cast<integer_t>( number_.get_length() ) );
	M_ENSURE( start < len ); /* exclude "!!-" */
	M_ENSURE( number_.find_one_of( VALID_CHARACTERS + A_DOT, start ) == start ); /* exclude "--" and "-!!" */
	integer_t idx( static_cast<integer_t>( number_.find_other_than( "0", start ) ) ); /* skip leading 0s */
	_integralPartSize = 0;
	_leafCount = 0;
	if ( idx != HString::npos ) {
		do { /* "!!![-][.1-9]???" or "000." */
			integer_t firstValid( start );
			start = idx;
			integer_t dot( static_cast<integer_t>( number_.find( VALID_CHARACTERS[ A_DOT ], start ) ) );
			idx = static_cast<integer_t>( number_.find_other_than( VALID_CHARACTERS + A_DOT, start ) );
			if ( ( idx != HString::npos ) && ( idx < dot ) ) { /* "!!232.!!!" */
				dot = HString::npos;
			}
			integer_t digit( static_cast<integer_t>( number_.find_one_of( VALID_CHARACTERS + A_ZERO, start ) ) );
			if ( ( digit == HString::npos ) && ( firstValid < start ) ) {
				break;
			}
			M_ENSURE( digit != HString::npos ); /* must have digit */
			M_ENSURE( ( digit - start ) <= 1 ); /* exclude "-..!!" and "..!!" */
			integer_t end( static_cast<integer_t>( number_.find_other_than( VALID_CHARACTERS + ( dot >= 0 ? A_ZERO : A_DOT ), dot >= 0 ? dot + 1 : start ) ) );
			( end != HString::npos ) || ( end = len );
			if ( dot != HString::npos ) {
				idx = static_cast<integer_t>( number_.reverse_find_other_than( "0", len - end ) );
				end = ( idx != HString::npos ) ? len - idx : start + 1;
			}
			M_ASSERT( ( dot == HString::npos ) || ( ( end - dot ) > 0 ) );
			_integralPartSize = ( dot != HString::npos ? ( ( dot - start ) + ( DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) ) / DECIMAL_DIGITS_IN_LEAF_CONST : ( ( end - start ) + ( DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) ) / DECIMAL_DIGITS_IN_LEAF_CONST );
			integer_t fractionalPart( dot != HString::npos ? ( ( end - ( dot + 1 ) ) + ( DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) ) / DECIMAL_DIGITS_IN_LEAF_CONST : 0 );
			_leafCount = _integralPartSize + fractionalPart;
			if ( _leafCount > 0 ) {
				_canonical.realloc( chunk_size<i32_t>( _leafCount ) );
			}
			i32_t* dst( _canonical.get<i32_t>() );
			i32_t leaf( 0 );
			int digitInLeaf( 0 );
			if ( dot != HString::npos ) { /* scan fractional part */
				idx = _integralPartSize;
				for ( integer_t i( dot + 1 ); i < end; ++ i, ++ digitInLeaf ) {
					M_ASSERT( src[ i ] >= VALID_CHARACTERS[ A_ZERO ] );
					if ( digitInLeaf == DECIMAL_DIGITS_IN_LEAF_CONST ) {
						dst[idx ++] = leaf;
						digitInLeaf = 0;
						leaf = 0;
					}
					leaf += ( ( src[ i ] - VALID_CHARACTERS[ A_ZERO ] ) * DECIMAL_SHIFT[ ( DECIMAL_DIGITS_IN_LEAF_CONST - digitInLeaf ) - 1 ] );
				}
				if ( idx < _leafCount ) {
					dst[idx] = leaf;
				}
			}
			if ( _integralPartSize > 0 ) {
				idx = _integralPartSize - 1;
				leaf = 0;
				digitInLeaf = 0;
				for ( integer_t i( ( dot != HString::npos ? dot : end ) - 1 ); i >= start; -- i, ++ digitInLeaf ) {
					M_ASSERT( src[ i ] >= VALID_CHARACTERS[ A_ZERO ] );
					if ( digitInLeaf == DECIMAL_DIGITS_IN_LEAF_CONST ) {
						M_ASSERT( idx >= 0 );
						dst[idx --] = leaf;
						digitInLeaf = 0;
						leaf = 0;
					}
					leaf += ( ( src[ i ] - VALID_CHARACTERS[ A_ZERO ] ) * DECIMAL_SHIFT[ digitInLeaf ] );
				}
				if ( idx >= 0 ) {
					dst[idx] = leaf;
				}
			}
			if ( dot == HString::npos ) {
				_integralPartSize = _leafCount;
			} else if ( ( end - dot - 1 ) >= _precision ) {
				_precision = end - dot;
			}
		} while ( 0 );
	}
	if ( _leafCount == 0 ) {
		_negative = false;
	}
	return;
	M_EPILOG
}

HString HNumber::to_string( void ) const {
	M_PROLOG
	i32_t const* const src( _canonical.get<i32_t>() );
	_cache.realloc( _leafCount * DECIMAL_DIGITS_IN_LEAF_CONST + 3 ); /* + 1 for '.', + 1 for '-' and + 1 for terminating NIL */
	char* ptr( _cache.get<char>() );
	if ( _negative ) {
		*ptr ++ = VALID_CHARACTERS[ A_MINUS ];
	}
	integer_t leaf( 0 );
	for ( ; leaf < _integralPartSize; ++ leaf ) {
		ptr += snprintf( ptr, DECIMAL_DIGITS_IN_LEAF_CONST + 1, leaf ? ZFORMAT : "%u", src[ leaf ] ); /* + 1 for terminating NIL */
	}
	if ( _leafCount > _integralPartSize ) {
		if ( ! _integralPartSize ) {
			*ptr ++ = VALID_CHARACTERS[ A_ZERO ];
		}
		*ptr ++ = VALID_CHARACTERS[ A_DOT ];
	}
	for ( ; leaf < _leafCount; ++ leaf ) {
		ptr += snprintf( ptr, DECIMAL_DIGITS_IN_LEAF_CONST + 1, ZFORMAT, src[ leaf ] );
	}
	if ( ! _leafCount ) {
		*ptr ++ = '0';
	} else if ( _leafCount > _integralPartSize ) {
		while ( *( ptr - 1 ) == '0' ) {
			-- ptr;
		}
	}
	*ptr = 0;
	char const* s( _cache.get<char>() );
	HString str( s, ptr - s );
	return ( str );
	M_EPILOG
}

double long HNumber::to_floating_point( void ) const {
	M_PROLOG
	return ( lexical_cast<double long>( to_string() ) );
	M_EPILOG
}

int long long HNumber::to_integer( void ) const {
	M_PROLOG
	if ( _integralPartSize > 3 ) {
		throw HNumberException( "integer overflow" );
	}
	int long long value( 0 );
	if ( _integralPartSize > 0 ) {
		i32_t const* data( _canonical.get<i32_t>() );
		value = data[ 0 ];
		if ( _integralPartSize > 1 ) {
			value *= LEAF;
			value += data[ 1 ];
		}
		if ( _integralPartSize > 2 ) {
			value *= LEAF;
			value += data[ 2 ];
		}
	}
	return ( ! _negative ? value : -value );
	M_EPILOG
}

HNumber::integer_t HNumber::get_precision( void ) const {
	return ( _precision );
}

void HNumber::set_precision( integer_t precision_ ) {
	M_PROLOG
	if ( precision_ < HARDCODED_MINIMUM_PRECISION )
		precision_ = HARDCODED_MINIMUM_PRECISION;
	if ( ( precision_ < _precision ) || ( ( precision_ > _precision ) && is_exact() ) ) {
		if ( precision_ < _precision ) {
			static_cast<void>( round( precision_ ) );
		}
		_precision = precision_;
	}
	return;
	M_EPILOG
}

HNumber::integer_t HNumber::fractional_length( void ) const {
	return ( _leafCount - _integralPartSize );
}

HNumber::integer_t HNumber::fractional_decimal_digits( void ) const {
	integer_t fractionalDecimalDigits( fractional_length() * DECIMAL_DIGITS_IN_LEAF_CONST );
	if ( fractionalDecimalDigits > 0 ) {
		i32_t lastLeaf( _canonical.get<i32_t>()[ _leafCount - 1 ] );
		for ( int i( 1 ); i < DECIMAL_DIGITS_IN_LEAF_CONST; ++ i ) {
			if ( lastLeaf % DECIMAL_SHIFT[i] ) {
				break;
			}
			-- fractionalDecimalDigits;
		}
	}
	return ( fractionalDecimalDigits );
}

bool HNumber::is_exact( void ) const {
	M_PROLOG
	bool exact( true );
	if ( fractional_length() > 0 ) {
		integer_t fractionalDecimalDigits( fractional_decimal_digits() );
		M_ASSERT( fractionalDecimalDigits <= _precision );
		exact = fractionalDecimalDigits < _precision;
	}
	return ( exact );
	M_EPILOG
}

HNumber::integer_t HNumber::absolute_lower( HNumber const& other ) const {
	M_PROLOG
	i32_t const* p1( _canonical.get<i32_t>() );
	i32_t const* p2( other._canonical.get<i32_t>() );
	integer_t cmp( 1 );
	if ( _integralPartSize < other._integralPartSize ) {
		cmp = -1;
	} else if ( _integralPartSize == other._integralPartSize ) {
		integer_t len = min( _leafCount, other._leafCount );
		cmp = leafcmp( p1, p2, len );
		if ( ! cmp ) {
			cmp = _leafCount - other._leafCount;
		}
	}
	return ( cmp );
	M_EPILOG
}

bool HNumber::operator == ( HNumber const& other ) const {
	return ( ( _negative == other._negative )
			&& ( _leafCount == other._leafCount )
			&& ( _integralPartSize == other._integralPartSize )
			&& ! ::memcmp( _canonical.raw(), other._canonical.raw(), static_cast<size_t>( chunk_size<i32_t>( _leafCount ) ) ) );
}

bool HNumber::operator != ( HNumber const& other ) const {
	return ( ! operator == ( other ) );
}

bool HNumber::operator < ( HNumber const& other ) const {
	bool lower = false;
	if ( _negative && ! other._negative ) {
		lower = true;
	} else if ( ( _negative && other._negative ) || ( ! ( _negative || other._negative ) ) ) {
		integer_t cmp = absolute_lower( other );
		lower = cmp < 0;
		if ( cmp && _negative && other._negative ) {
			lower = ! lower;
		}
	}
	return ( lower );
}

bool HNumber::operator > ( HNumber const& other ) const {
	return ( other < *this );
}

bool HNumber::operator <= ( HNumber const& other ) const {
	return ( ( operator == ( other ) ) || ( operator < ( other ) ) );
}

bool HNumber::operator >= ( HNumber const& other ) const {
	return ( other <= *this );
}

bool HNumber::mutate_addition( i32_t* res, integer_t ressize,
		i32_t const* const addends[], integer_t* missingIntegral, integer_t* missingFractional, bool sub ) const {
	i32_t e[ 2 ];
	i32_t const* const epx[] = {
		addends[ 0 ] - ( missingIntegral ? missingIntegral[ 0 ] : 0 ) - 1,
		addends[ 1 ] - ( missingIntegral ? missingIntegral[ 1 ] : 0 ) - 1
	};
	integer_t idx( ressize - 1 ); /* index of first processed leaf */
	integer_t side( 0 );
	integer_t off( 0 );
	if ( missingFractional ) {
		side = ( missingFractional[ 0 ] > missingFractional[ 1 ] ) ? 1 : 0;
		off = missingFractional[ 1 - side ];
	}
	i32_t const* src( epx[ side ] );
	e[ 1 - side ] = 0;
	i32_t carrier( 0 );
	if ( sub ) {
		while ( off -- && ( idx > 0 ) ) {
			e[ side ] = src[ idx ];
			i32_t x( e[ 0 ] - carrier );
			if ( x < e[ 1 ] ) {
				x += LEAF;
				carrier = 1;
			} else {
				carrier = 0;
			}
			res[ idx ] = x - e[ 1 ];
			-- idx;
		}
	} else {
		while ( off -- && ( idx > 0 ) ) {
			e[ side ] = src[ idx ];
			i32_t x( e[ 0 ] + e[ 1 ] + carrier );
			if ( x >= LEAF ) {
				x -= LEAF;
				carrier = 1;
			} else {
				carrier = 0;
			}
			res[ idx ] = x;
			-- idx;
		}
	}
	if ( missingIntegral ) {
		side = ( missingIntegral[ 0 ] > missingIntegral[ 1 ] ) ? 1 : 0;
		off = missingIntegral[ 1 - side ];
	} else {
		side = 0;
		off = 0;
	}
	if ( sub ) {
		while ( idx > off ) {
			i32_t x( epx[ 0 ][ idx ] - carrier );
			if ( x < epx[ 1 ][ idx ] ) {
				x += LEAF;
				carrier = 1;
			} else {
				carrier = 0;
			}
			res[ idx ] = x - epx[ 1 ][ idx ];
			-- idx;
		}
	} else {
		while ( idx > off ) {
			i32_t x( epx[ 0 ][ idx ] + epx[ 1 ][ idx ] + carrier );
			if ( x >= LEAF ) {
				x -= LEAF;
				carrier = 1;
			} else {
				carrier = 0;
			}
			res[ idx ] = x;
			-- idx;
		}
	}
	src = epx[ side ];
	e[ 1 - side ] = 0;
	if ( sub ) {
		while ( idx > 0 ) {
			e[ side ] = src[ idx ];
			i32_t x( e[ 0 ] - carrier );
			if ( x < e[ 1 ] ) {
				x += LEAF;
				carrier = 1;
			} else {
				carrier = 0;
			}
			res[ idx ] = x - e[ 1 ];
			-- idx;
		}
	} else {
		while ( idx > 0 ) {
			e[ side ] = src[ idx ];
			i32_t x( e[ 0 ] + e[ 1 ] + carrier );
			if ( x >= LEAF ) {
				x -= LEAF;
				carrier = 1;
			} else {
				carrier = 0;
			}
			res[ idx ] = x;
			-- idx;
		}
	}
	if ( carrier && ! sub ) {
		++ res[ 0 ];
	}
	return ( carrier ? true : false );
}

HNumber HNumber::operator + ( HNumber const& addend_ ) const {
	M_PROLOG
	HNumber n( *this );
	n += addend_;
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator += ( HNumber const& addend_ ) {
	M_PROLOG
	integer_t ips = max( _integralPartSize, addend_._integralPartSize );
	integer_t dps = max( fractional_length(), addend_.fractional_length() );
	bool augendExact( is_exact() );
	bool addendExact( addend_.is_exact() );
	if ( fractional_length() < addend_.fractional_length() ) {
		_precision = augendExact ? addend_._precision : _precision;
	} else {
		_precision = addendExact ? _precision : addend_._precision;
	}
	( dps <= _precision ) || ( dps = _precision );
	integer_t ressize = ips + dps + 1; /* + 1 for possible carrier */
	_cache.realloc( chunk_size<i32_t>( ressize ) );
	::memset( _cache.raw(), 0, static_cast<size_t>( _cache.get_size() ) );
	i32_t* res( _cache.get<i32_t>() );
	i32_t const* addend1( _canonical.get<i32_t>() );
	i32_t const* addend2( addend_._canonical.get<i32_t>() );
	integer_t missingIntegral[] = { ips - _integralPartSize, ips - addend_._integralPartSize };
	integer_t missingFractional[] = { dps - fractional_length(), dps - addend_.fractional_length() };
	( missingFractional[ 0 ] >= 0 ) || ( missingFractional[ 0 ] = 0 );
	( missingFractional[ 1 ] >= 0 ) || ( missingFractional[ 1 ] = 0 );
	i32_t const* addends[] = { addend1, addend2 };
	bool sub = ( ( _negative && ! addend_._negative ) || ( ! _negative && addend_._negative ) );
	bool swp( false );
	if ( sub && ( absolute_lower( addend_ ) < 0 ) ) {
		using yaal::swap;
		swap( missingIntegral[0], missingIntegral[1] );
		swap( missingFractional[0], missingFractional[1] );
		swap( addends[0], addends[1] );
		swp = true;
	}
	mutate_addition( res, ressize, addends, missingIntegral, missingFractional, sub );
	_integralPartSize = ips;
	if ( ressize > 0 ) {
		_negative = sub ? ( _negative ? ! swp : swp ) : ( _negative && addend_._negative );
		++ _integralPartSize;
	}
	_leafCount = ressize;
	_canonical.swap( _cache );
	normalize( augendExact && addendExact );
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator - ( HNumber const& subtrahend_ ) const {
	M_PROLOG
	HNumber n( *this );
	n -= subtrahend_;
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator -= ( HNumber const& subtrahend_ ) {
	M_PROLOG
	_negative = ! _negative;
	if ( ! _leafCount ) {
		_negative = false;
	}
	operator += ( subtrahend_ );
	_negative = ! _negative;
	if ( ! _leafCount ) {
		_negative = false;
	}
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator * ( HNumber const& factor_ ) const {
	M_PROLOG
	HNumber n( *this );
	n *= factor_;
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator *= ( HNumber const& factor_ ) {
	M_PROLOG
	HNumber const& n( factor_._leafCount < _leafCount ? factor_ : *this );
	HNumber const& factor( factor_._leafCount < _leafCount ? *this : factor_ );
	if ( n._leafCount && factor._leafCount ) {
		bool multiplierExact( n.is_exact() );
		bool factorExact( factor.is_exact() );
		::memset( _cache.raw(), 0, static_cast<size_t>( _cache.get_size() ) );
		karatsuba( _cache,
				n._canonical.get<i32_t>(), n._leafCount,
				factor._canonical.get<i32_t>(), factor._leafCount );
		_leafCount += factor_._leafCount;
		_integralPartSize += factor_._integralPartSize;
		_canonical.swap( _cache );
		_negative = ! ( ( n._negative && factor._negative ) || ! ( n._negative || factor._negative ) );
		normalize( multiplierExact && factorExact );
	} else {
		_leafCount = _integralPartSize = 0;
		_negative = false;
	}
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator / ( HNumber const& divisor_ ) const {
	M_PROLOG
	HNumber n( *this );
	n /= divisor_;
	return ( n );
	M_EPILOG
}

i32_t HNumber::multiply_by_leaf_low( i32_t* data_, integer_t leafCount_, i32_t leaf_ ) {
	M_ASSERT( ( leaf_ >= 0 ) && ( leaf_ < LEAF ) );
	i32_t carrier( 0 );
	for ( integer_t i( leafCount_ - 1 ); i >= 0; -- i ) {
		i64_t x( static_cast<i64_t>( data_[ i ] ) * leaf_ + carrier );
		if ( x >= LEAF ) {
			carrier = static_cast<i32_t>( x / LEAF );
			x %= LEAF;
		} else {
			carrier = 0;
		}
		data_[ i ] = static_cast<i32_t>( x );
	}
	return ( carrier );
}

void HNumber::multiply_by_leaf( i32_t leaf_ ) {
	M_PROLOG
	M_ASSERT( ( leaf_ >= 0 ) && ( leaf_ < LEAF ) );
	if ( _leafCount > 0 ) {
		i32_t carrier( multiply_by_leaf_low( _canonical.get<i32_t>(), _leafCount, leaf_ ) );
		i32_t* data( _canonical.get<i32_t>() );
		while ( ( _leafCount > _integralPartSize ) && ! data[_leafCount - 1] ) {
			-- _leafCount;
		}
		if ( carrier ) {
			_canonical.realloc( chunk_size<i32_t>( _leafCount + 1 ) );
			data = _canonical.get<i32_t>();
			::memmove( data + 1, data, static_cast<size_t>( chunk_size<i32_t>( _leafCount ) ) );
			++ _integralPartSize;
			++ _leafCount;
			data[0] = carrier;
		}
	}
	return;
	M_EPILOG
}

void HNumber::divide_by_leaf( i32_t leaf_, integer_t shift_ ) {
	M_PROLOG
	i32_t* data( _canonical.get<i32_t>() );
	i64_t remainder( 0 );
	bool negative( leaf_ < 0 );
	leaf_ = yaal::abs( leaf_ );
	M_ASSERT( leaf_ < LEAF );
	if ( shift_ ) {
		_integralPartSize += shift_;
		if ( _integralPartSize < 0 ) {
			_canonical.realloc( chunk_size<i32_t>( _leafCount - _integralPartSize ) );
			data = _canonical.get<i32_t>();
			::memmove( data - _integralPartSize, data, static_cast<size_t>( chunk_size<i32_t>( _leafCount ) ) );
			::memset( data, 0, static_cast<size_t>( chunk_size<i32_t>( -_integralPartSize ) ) );
			_leafCount -= _integralPartSize;
			_integralPartSize = 0;
		} else if ( _integralPartSize > _leafCount ) {
			_canonical.realloc( chunk_size<i32_t>( _leafCount = _integralPartSize ) );
			data = _canonical.get<i32_t>();
		}
	}
	for ( integer_t i( 0 ); i < _leafCount; ++ i ) {
		i64_t x( remainder * LEAF + data[i] );
		data[i] = static_cast<i32_t>( x / leaf_ );
		remainder = x % leaf_;
	}
	if ( ! data[0] && ( _integralPartSize > 0 ) ) {
		::memmove( data, data + 1, static_cast<size_t>( chunk_size<i32_t>( _leafCount - 1 ) ) );
		-- _integralPartSize;
		-- _leafCount;
	}
	integer_t precision( ( _precision + DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) / DECIMAL_DIGITS_IN_LEAF_CONST );
	while ( remainder && ( ( _leafCount - _integralPartSize ) < precision ) ) { /* we need more digits */
		_canonical.realloc( chunk_size<i32_t>( _leafCount + 1 ) );
		data = _canonical.get<i32_t>();
		i64_t x( remainder * LEAF );
		data[_leafCount] = static_cast<i32_t>( x / leaf_ );
		remainder = x % leaf_;
		++ _leafCount;
	}
	_negative = ! ( ( _negative && negative ) || ! ( _negative || negative ) );
	normalize( ! remainder );
	return;
	M_EPILOG
}

HNumber& HNumber::operator /= ( HNumber const& divisor_ ) {
	M_PROLOG
	if ( ! ( divisor_._leafCount > 0 ) ) {
		throw HNumberException( "division by zero" );
	}
	if ( _leafCount ) {
		/*
		 * We use "long division" pen and paper algorithm
		 * with smart guessing of quotient digits taken from TAOCP.
		 *
		 * Eamples of special cases that have to be handled.
		 * (each digit is one leaf)
		 *
		 * 1 / 3
		 * 1 / 13
		 * 10 / 3
		 * 10 / 13
		 * 10 / 13131313
		 * 0.1 / 3
		 * 0.1 / 13
		 * 0.1 / 131313
		 * 130000 / 13
		 * 0.0001 / 3
		 * 131313 / 13
		 * 131313.131313 / 13
		 */
		/*
		 * Find proper part of divisor, i.e.:
		 * .00123 -> 123
		 * 123000 -> 123
		 */
		if ( divisor_._leafCount == 1 ) {
			divide_by_leaf( divisor_._canonical.get<i32_t>()[0] * ( divisor_._negative ? -1 : 1 ), divisor_._integralPartSize > 0 ? 0 : 1 );
		} else {
			i32_t const* divisorTest( divisor_._canonical.get<i32_t>() );
			integer_t test( 0 );
			while ( ! divisorTest[ test ] ) {
				++ test;
			}
			i32_t normalizator( LEAF / ( divisorTest[ test ] + 1 ) );
			HNumber divisorCopy;
			if ( normalizator != 1 ) {
				multiply_by_leaf( normalizator );
				divisorCopy = divisor_;
				divisorCopy.multiply_by_leaf( normalizator );
			}
			HNumber const& normalizedDivisor( normalizator != 1 ? divisorCopy : divisor_ );

			i32_t const* divisor( normalizedDivisor._canonical.get<i32_t>() );
			integer_t divisorLeafCount( normalizedDivisor._leafCount );
			integer_t lshift( 0 );
			while ( ! divisor[ lshift ] ) {
				++ lshift;
			}
			divisor += lshift;
			divisorLeafCount -= lshift;
			integer_t rshift( 0 );
			while ( ! divisor[ divisorLeafCount - 1 ] ) {
				-- divisorLeafCount;
				++ rshift;
			}
			M_ASSERT( ( divisorLeafCount > 0 ) && ! ( lshift && rshift ) );
			if ( divisorLeafCount == 1 ) {
				divide_by_leaf( divisor[0] * ( divisor_._negative ? -1 : 1 ), ( lshift - rshift ) + ( normalizedDivisor._integralPartSize > 0 ? 0 : 1 ) );
			} else {
				/*
				 * Pre-allocate memory for result.
				 */
				_cache.realloc( chunk_size<i32_t>( _leafCount ) );
				/*
				 * We need memory buffer for dividend sample,
				 * and another one for multiplier sample.
				 * Assuming divisor has n leafs, a multiplier sample's buffer must
				 * be able to accommodate n + 1 leafs ( + 1 for possible carrier),
				 * dividendSample also must provide space for n + 1 leafs.
				 */
				HChunk buffer( chunk_size<i32_t>( ( divisorLeafCount + 1 ) * 2 ) );
				/*
				 * Buffer for trial&error multiplications done while looking for next quotient leaf.
				 */
				i32_t* multiplierSample( buffer.get<i32_t>() );
				i32_t* dividendSample( buffer.get<i32_t>() + divisorLeafCount + 1 );
				i32_t* dividend( _canonical.get<i32_t>() );
				integer_t newPrecision( 0 );
				bool dividendExact( is_exact() );
				bool divisorExact( normalizedDivisor.is_exact() );
				if ( dividendExact && divisorExact ) {
					newPrecision = _precision + normalizedDivisor._precision;
				} else {
					_precision = newPrecision = min( _precision, normalizedDivisor._precision );
				}
				integer_t precision( ( newPrecision + DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) / DECIMAL_DIGITS_IN_LEAF_CONST );
				integer_t shift( 0 );
				while ( ( shift < _leafCount ) && ! dividend[ shift ] ) {
					++ shift;
				}
				integer_t divSampleLen( min( _leafCount - shift, divisorLeafCount ) ); /* Index of the next leaf to process */
				::memcpy( dividendSample, dividend + shift, static_cast<size_t>( chunk_size<i32_t>( divSampleLen ) ) );
				integer_t dividendLeafNo( divSampleLen + shift );
				/*
				 * Number of leafs in quotient before leaf point.
				 * Variable used to stop calculations based on maximum precision.
				 */
				integer_t integralPart( _integralPartSize - normalizedDivisor._integralPartSize + ( normalizedDivisor._integralPartSize > 0 ? 0 : lshift ) - shift );
				integer_t quotientLeafNo( 0 ); /* Index of currently guessed quotient leaf. */
				if ( integralPart < 0 ) {
					_cache.realloc( chunk_size<i32_t>( quotientLeafNo - integralPart ) );
					::memset( _cache.get<i32_t>() + quotientLeafNo, 0, static_cast<size_t>( chunk_size<i32_t>( -integralPart ) ) );
					quotientLeafNo -= integralPart;
					integralPart = 0;
				}
				++ integralPart;
				while ( ! dividend[ _leafCount - 1 ] ) {
					-- _leafCount;
				}
				bool firstLeaf( true );
				bool exact( false );
				do {
					integer_t zfill( dividendLeafNo );
					/*
					 * Fix possible underflow in dividendSample.
					 */
					shift = 0;
					while ( ( shift < divSampleLen ) && ! dividendSample[ shift ] ) {
						++ shift;
					}
					if ( shift > 0 ) {
						divSampleLen -= shift;
						if ( divSampleLen > 0 )
							::memmove( dividendSample, dividendSample + shift, static_cast<size_t>( chunk_size<i32_t>( divSampleLen ) ) );
					}

					/*
					 * Compensate for missing leafs.
					 */
					if ( divSampleLen < divisorLeafCount ) {
						if ( dividendLeafNo < _leafCount ) {
							/*
							 * If we have cleared dividendSample (dividendSample == 0),
							 * skip following zeros from dividend.
							 */
							if ( divSampleLen == 0 ) {
								shift = 0;
								while ( ( ( dividendLeafNo + shift ) < _leafCount ) && ! dividend[ dividendLeafNo + shift ] ) {
									++ shift;
								}
								dividendLeafNo += shift;
							}
							integer_t compensationLen( min( _leafCount - dividendLeafNo, divisorLeafCount - divSampleLen ) );
							::memcpy( dividendSample + divSampleLen, dividend + dividendLeafNo, static_cast<size_t>( chunk_size<i32_t>( compensationLen ) ) );
							dividendLeafNo += compensationLen;
							divSampleLen += compensationLen;
						} else if ( divSampleLen == 0 ) {
							exact = true;
							break;
						}
					}

					/*
					 * Fill rest of dividendSample with 0.
					 */
					if ( divSampleLen < divisorLeafCount ) {
						integer_t compensationLen( divisorLeafCount - divSampleLen );
						::memset( dividendSample + divSampleLen, 0, static_cast<size_t>( chunk_size<i32_t>( compensationLen ) ) );
						divSampleLen = divisorLeafCount;
						dividendLeafNo += compensationLen;
					}

					/*
					 * If dividendSample is still not enough to produce next quotient leaf
					 * then fetch next leaf from divident to dividendSample.
					 */
					bool extra( false );
					if ( leafcmp( dividendSample, divisor, divSampleLen ) < 0 ) { /* We establish that normalizedDivisor <= multiplierSample. */
						dividendSample[divSampleLen] = dividendLeafNo < _leafCount ? dividend[dividendLeafNo] : 0;
						++ divSampleLen;
						++ dividendLeafNo;
						if ( firstLeaf )
							-- integralPart;
						extra = true;
					}

					if ( ! firstLeaf ) {
						zfill = dividendLeafNo - zfill - 1;
						if ( zfill > 0 ) {
							_cache.realloc( chunk_size<i32_t>( quotientLeafNo + zfill ) );
							::memset( _cache.get<i32_t>() + quotientLeafNo, 0, static_cast<size_t>( chunk_size<i32_t>( zfill ) ) );
							quotientLeafNo += zfill;
						}
					}

					/*
					 * Stright from The Art of Computer Programming,
					 * 4.3. MULTIPLE-PRECISION ARITHMETIC,
					 * 4.3.1. The Classical Algorithms,
					 * Algorithm D, step B3:
					 *
					 * If dividend[0] = divisor[0], set Q to BASE-1;
					 * otherwise set Q to (dividend[0]+dividend[1])/divisor[0].
					 * Now test if divisor[1]*Q > (dividend[0]*BASE + dividend[1] - Q*divisor[0])*BASE + dividend[2];
					 * if so, decrease Q by 1 and repeat this test.
					 */
					i64_t leaf( 0 ); /* guessed leaf */
					i64_t u0( 0 );
					i64_t u1( 0 );
					i64_t u2( 0 );
					i64_t v1( divisor[0] );
					i64_t v2( divisor[1] );
					if ( extra ) {
						u0 = ( dividendSample[0] );
						u1 = ( dividendSample[1] );
						u2 = ( dividendSample[2] );
					} else {
						u1 = ( dividendSample[0] );
						u2 = ( dividendSample[1] );
					}
					if ( u0 == v1 ) {
						leaf = LEAF - 1;
					} else {
						leaf = ( u0 * LEAF + u1 ) / v1;
					}
					if ( ( v2 * leaf ) > ( ( ( ( ( u0 * LEAF ) + u1 ) - leaf * v1 ) * LEAF ) + u2 ) ) {
						-- leaf;
						if ( ( v2 * leaf ) > ( ( ( ( ( u0 * LEAF ) + u1 ) - leaf * v1 ) * LEAF ) + u2 ) )
							-- leaf;
					}
					/*
					 * A helper for mutate_addition.
					 * The subtraction is done `in place'.
					 */
					::memcpy( multiplierSample + 1, divisor, static_cast<size_t>( chunk_size<i32_t>( divisorLeafCount ) ) );
					multiplierSample[0] = multiply_by_leaf_low( multiplierSample + 1, divisorLeafCount, static_cast<i32_t>( leaf ) );
					if ( ( multiplierSample[0] && ! extra ) || ( leafcmp( divisor, multiplierSample + ( divSampleLen == divisorLeafCount ? 1 : 0 ), divisorLeafCount ) < 0 ) ) {
						-- leaf;
						::memcpy( multiplierSample + 1, divisor, static_cast<size_t>( chunk_size<i32_t>( divisorLeafCount ) ) );
						multiplierSample[0] = multiply_by_leaf_low( multiplierSample + 1, divisorLeafCount, static_cast<i32_t>( leaf ) );
					}
					i32_t const* addends[] = { dividendSample, multiplierSample + ( divSampleLen == divisorLeafCount ? 1 : 0 ) };
					if ( mutate_addition( dividendSample - 1, divSampleLen + 1, addends, NULL, NULL, true ) ) {
						dividendSample[ -- divSampleLen ] = 0;
					}
					_cache.realloc( chunk_size<i32_t>( quotientLeafNo + 1 ) );
					_cache.get<i32_t>()[ quotientLeafNo ++ ] = static_cast<i32_t>( leaf );
					firstLeaf = false;
				} while ( ( quotientLeafNo - integralPart ) < precision );
				_integralPartSize = integralPart;
				_leafCount = ( integralPart > quotientLeafNo ? integralPart : quotientLeafNo );
				if ( quotientLeafNo < _leafCount )
					::memset( _cache.get<i32_t>() + quotientLeafNo, 0, static_cast<size_t>( chunk_size<i32_t>( _leafCount - quotientLeafNo ) ) );
				_negative = ! ( ( _negative && normalizedDivisor._negative ) || ! ( _negative || normalizedDivisor._negative ) );
				_canonical.swap( _cache );
				normalize( exact && dividendExact && divisorExact );
			}
		}
	}
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator - ( void ) const {
	M_PROLOG
	HNumber n( *this );
	if ( _leafCount > 0 ) {
		n._negative = ! n._negative;
	}
	return ( n );
	M_EPILOG
}

HNumber HNumber::operator ^ ( int long long exp ) const {
	M_PROLOG
	HNumber n( *this );
	n ^= exp;
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator ^= ( int long long exp_ ) {
	M_PROLOG
	if ( _leafCount > 0 ) {
		int long long unsigned exp( static_cast<int long long unsigned>( yaal::abs( exp_ ) ) );
		if ( exp > 1 ) {
			int long long unsigned p( exp >> 1 );
			HNumber n( *this );
			if ( p > 2 ) {
				operator ^= ( static_cast<int long long>( p ) );
			} else {
				while ( -- p ) {
					operator *= ( n );
				}
			}
			operator *= ( *this );
			if ( exp % 2 ) {
				operator *= ( n );
			}
			if ( exp_ < 0 ) {
				n = *this;
				from_integer( 1 );
				operator /= ( n );
			}
		} else if ( exp == 0 ) {
			from_integer( 1 );
		}
	} else if ( exp_ == 0 ) {
		throw HNumberException( "indeterminate form 0^0" );
	}
	return ( *this );
	M_EPILOG
}

HNumber& HNumber::operator ++ ( void ) {
	M_PROLOG
	operator += ( number::N1 );
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator ++ ( int ) {
	M_PROLOG
	HNumber n( *this );
	operator ++ ();
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator -- ( void ) {
	M_PROLOG
	operator -= ( number::N1 );
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator -- ( int ) {
	M_PROLOG
	HNumber n( *this );
	operator -- ();
	return ( n );
	M_EPILOG
}

void HNumber::normalize( bool updatePrecision_ ) {
	M_PROLOG
	i32_t* res( _canonical.get<i32_t>() );
	integer_t shift( 0 );
	while ( ( shift < _integralPartSize ) && ( res[ shift ] == 0 ) ) {
		++ shift;
	}
	if ( shift ) {
		_integralPartSize -= shift;
		_leafCount -= shift;
		::memmove( res, res + shift, static_cast<size_t>( chunk_size<i32_t>( _leafCount ) ) );
	}
	while ( ( _leafCount > _integralPartSize ) && ( res[ _leafCount - 1 ] == 0 ) ) {
		-- _leafCount;
	}
	integer_t fractionalDecimalDigits( fractional_decimal_digits() );
	if ( updatePrecision_ && ( fractionalDecimalDigits >= _precision ) ) {
		_precision = fractionalDecimalDigits + 1;
	}
	integer_t leafPrecision( ( _precision + DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) / DECIMAL_DIGITS_IN_LEAF_CONST );
	if ( ( _leafCount > 0 ) && ( fractionalDecimalDigits > _precision ) && ( leafPrecision <= fractional_length() ) ) {
		_leafCount -= ( fractional_length() - leafPrecision );
		i32_t& lastLeaf( res[ _leafCount - 1 ] );
		lastLeaf -= ( lastLeaf % DECIMAL_SHIFT[ DECIMAL_DIGITS_IN_LEAF_CONST - ( _precision % DECIMAL_DIGITS_IN_LEAF_CONST ) ] );
	}
	while ( ( _leafCount > _integralPartSize ) && ( res[ _leafCount - 1 ] == 0 ) ) {
		-- _leafCount;
	}
	if ( ! _leafCount ) {
		_negative = false;
	}
	return;
	M_EPILOG
}

HNumber::integer_t HNumber::karatsuba( HChunk& result, i32_t const* fx, integer_t fxs, i32_t const* fy, integer_t fys ) {
	integer_t shift( 0 );
	while ( ( shift < fxs ) && ! fx[ shift ] ) {
		++ shift;
	}
	integer_t fxrl( fxs - shift );
	integer_t totalShift( shift );
	shift = 0;
	while ( ( shift < fys ) && ! fy[ shift ] ) {
		++ shift;
	}
	integer_t fyrl( fys - shift );
	totalShift += shift;
	integer_t leafCount( 0 );
	if ( ( ( fxrl > KARATSUBA_THRESHOLD ) || ( fyrl > KARATSUBA_THRESHOLD ) ) && ( fxrl > 0 ) && ( fyrl > 0 ) ) {
		fx += ( fxs - fxrl );
		fy += ( fys - fyrl );
		fxs = fxrl;
		fys = fyrl;
		result.realloc( chunk_size<i32_t>( leafCount = fxs + fys + totalShift ) );

		integer_t const fs( max( fxs, fys ) );
		integer_t const fl( min( fxs, fys ) );
		integer_t const m( ( fs / 2 ) + ( fs & 1 ) ); /* Size of upper/lower half of number */
		HChunk r2m; /* intermediate result ( fx1 * fx2 ( * B ^ 2m ) ) + 1 for carrier */
		integer_t const r2ms( karatsuba( r2m, fx, fxs - m, fy, fys - m ) );
		HChunk r; /* intermediate result ( fx2 * fy2 ) + 1 for carrier */
		integer_t const rs( karatsuba( r, fx + ( fxs > m ? fxs - m : 0 ), min( fxs, m ), fy + ( fys > m ? fys - m : 0 ), min( fys, m ) ) );
		integer_t const size( fxs + fys + 1 );
		HChunk buffer( chunk_size<i32_t>( max( size + 1, ( m + 1 ) * 2 ) ) ); /* + 1 for carrier */
		i32_t* hx( buffer.get<i32_t>() );
		i32_t* hy( buffer.get<i32_t>() + m + 1 );
		/* preparation of hx and hy */
		/* hx = fx / B^m + fx % B^m */
		integer_t missingIntegral[] = { 2 * m - fxs, 0 };
		i32_t const* addends[] = { fx, fx + fxs - m };
		if ( fxs > m ) {
			mutate_addition( hx, m + 1, addends, missingIntegral, NULL, false );
		} else {
			::memcpy( hx + m + 1 - fxs, fx, static_cast<size_t>( chunk_size<i32_t>( fxs ) ) );
		}
		/* hy */
		missingIntegral[ 0 ] = 2 * m - fys;
		addends[ 0 ] = fy;
		addends[ 1 ] = fy + fys - m;
		if ( fys > m ) {
			mutate_addition( hy, m + 1, addends, missingIntegral, NULL, false );
		} else {
			::memcpy( hy + m + 1 - fys, fy, static_cast<size_t>( chunk_size<i32_t>( fys ) ) );
		}
		/* find Z */
		HChunk Z;
		integer_t Zs( karatsuba( Z, hx, m + 1, hy, m + 1 ) );
		/* combine all results */
		i32_t* const res( buffer.get<i32_t>() + 1 ); /* Z*B^m can possibly (front)overflow buffer of size = fxs + fys, hence + 1 in allocation and here. */
		::memset( res, 0, static_cast<size_t>( chunk_size<i32_t>( size ) ) );

		/* res = Z*B^m + r */
		i32_t const* p( Z.get<i32_t>() );
		shift = 0;
		while ( ( shift < Zs ) && ! p[ shift ] ) {
			++ shift;
		}
		Zs -= shift;
		p += shift;
		addends[ 0 ] = r.get<i32_t>();
		addends[ 1 ] = p;
		missingIntegral[ 0 ] = Zs - rs + m;
		missingIntegral[ 1 ] = 0;
		if ( addends[ 0 ] ) {
			::memcpy( res + size - rs, r.raw(), static_cast<size_t>( chunk_size<i32_t>( rs ) ) );
			mutate_addition( res + size - m - Zs - 1, Zs + 1, addends, missingIntegral, NULL, false );
		} else {
			::memcpy( res + size - m - Zs, p, static_cast<size_t>( chunk_size<i32_t>( Zs ) ) );
		}

		/* res += r2m*B^2m */
		Zs = r2ms;
		shift = 0;
		p = r2m.get<i32_t>();
		missingIntegral[ 0 ] = 0;
		if ( p ) {
			while ( ( shift < Zs ) && ! p[ shift ] ) {
				++ shift;
			}
			Zs -= shift;
			p += shift;
			addends[ 0 ] = res + 1 + shift;
			addends[ 1 ] = p;
			missingIntegral[ 1 ] = 0;
			mutate_addition( res + shift, Zs + 1, addends, missingIntegral, NULL, false );
		}

		/* res -= r2m*B^m, res -= r*B^m */

		int ncar( ( fl > m ? 1 : 0 ) );
		int car( 1 - ncar );

		addends[ 0 ] = res + ncar;
		addends[ 1 ] = r2m.get<i32_t>();
		if ( addends[ 1 ] ) {
			missingIntegral[ 1 ] = size - r2ms - m - ncar;
			mutate_addition( res - car, size - m + car, addends, missingIntegral, NULL, true );
		}
		addends[ 1 ] = r.get<i32_t>();
		if ( addends[ 1 ] ) {
			missingIntegral[ 1 ] = size - rs - m - ncar;
			mutate_addition( res - car, size - m + car, addends, missingIntegral, NULL, true );
		}
		::memcpy( result.get<i32_t>() + totalShift, res + 1, static_cast<size_t>( chunk_size<i32_t>( leafCount - totalShift ) ) );
	} else if ( ( fxrl > 0 ) && ( fyrl > 0 ) ) {
/* variables for mutate_addition() */
		result.realloc( chunk_size<i32_t>( leafCount = fxs + fys ) );
		HChunk element( chunk_size<i32_t>( fys + 1 ) );
		i32_t* e( element.get<i32_t>() );
		i32_t* res( ( result.get<i32_t>() + fxs ) - 2 ); /* - 1 for carrier */
		i32_t const* addends[] = { res, e };
/* the end of variables for mutate_addition() */
		while ( -- fxs >= 0 ) {
			if ( fx[ fxs ] ) {
				i32_t carrier( 0 );
				integer_t inner( fys ); /* index of last leaf in second factor */
				while ( -- inner >= 0 ) {
					integer_t pos( inner + 1 );
					i64_t x( static_cast<i64_t>( fx[ fxs ] ) * fy[ inner ] + carrier );
					if ( x >= LEAF ) {
						carrier = static_cast<i32_t>( x / LEAF );
						x %= LEAF;
					} else {
						carrier = 0;
					}
					e[ pos ] = static_cast<i32_t>( x );
				}
				e[ 0 ] = carrier;
				addends[ 0 ] = res + 1;
				mutate_addition( res, fys + 1 + 1, addends, NULL, NULL, false );
				-- res;
			} else {
				-- res;
			}
		}
	}
	return ( leafCount );
}

HNumber& HNumber::abs( void ) {
	M_PROLOG
	_negative = false;
	return ( *this );
	M_EPILOG
}

HNumber& HNumber::round( integer_t significant_ ) {
	M_PROLOG
	/* !!!
	 *
	 * significant_ is not an index, significant_ is an amount!
	 *
	 */
	M_ENSURE( significant_ >= 0 );
	if ( significant_ < fractional_decimal_digits() ) {
		integer_t newFractionalLeafCount( ( significant_ + DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) / DECIMAL_DIGITS_IN_LEAF_CONST );
		/*
		 * Trim total leaf count.
		 */
		if ( ( _integralPartSize + newFractionalLeafCount ) < _leafCount ) {
			_leafCount = _integralPartSize + newFractionalLeafCount;
		}
		M_ASSERT( newFractionalLeafCount == fractional_length() );
		i32_t* data( _canonical.get<i32_t>() );

		/*
		 * Get digit that is next to last significant digit.
		 * From now on nts stands for next-to-significant.
		 */
		integer_t ntsLeafNo( significant_ / DECIMAL_DIGITS_IN_LEAF_CONST );
		int ntsDigitInLeafNo( significant_ % DECIMAL_DIGITS_IN_LEAF_CONST );
		i32_t ntsDigit( data[_integralPartSize + ntsLeafNo] );
		ntsDigit /= DECIMAL_SHIFT[ DECIMAL_DIGITS_IN_LEAF_CONST - 1 - ntsDigitInLeafNo ];
		ntsDigit %= 10;
		if ( ( _leafCount > 0 ) && ( significant_ % DECIMAL_DIGITS_IN_LEAF_CONST ) ) {
			i32_t& lastLeaf( data[ _leafCount - 1 ] );
			lastLeaf -= ( lastLeaf % DECIMAL_SHIFT[ DECIMAL_DIGITS_IN_LEAF_CONST - significant_ % DECIMAL_DIGITS_IN_LEAF_CONST ] );
			if ( ntsDigit >= 5 ) {
				add_leaf_low( _leafCount - 1, DECIMAL_SHIFT[ DECIMAL_DIGITS_IN_LEAF_CONST - ntsDigitInLeafNo ] );
			}
		} else {
			if ( ntsDigit >= 5 ) {
				add_leaf_low( _leafCount - 1, 1 );
			}
		}

		while ( ( _leafCount > _integralPartSize ) && ! data[ _leafCount - 1 ] )
			-- _leafCount;
		if ( ! _leafCount )
			_negative = false;
	}
	return ( *this );
	M_EPILOG
}

HNumber& HNumber::floor( void ) {
	M_PROLOG
	int long leafCount( _leafCount );
	_leafCount = _integralPartSize;
	if ( _negative && ( leafCount > _integralPartSize ) ) {
		add_leaf_low( _integralPartSize - 1, 1 );
	}
	return ( *this );
	M_EPILOG
}

HNumber& HNumber::ceil( void ) {
	M_PROLOG
	int long leafCount( _leafCount );
	_leafCount = _integralPartSize;
	if ( ! _negative && ( leafCount > _integralPartSize ) ) {
		add_leaf_low( _integralPartSize - 1, 1 );
	}
	return ( *this );
	M_EPILOG
}

HNumber& HNumber::trunc( void ) {
	M_PROLOG
	_leafCount = _integralPartSize;
	return ( *this );
	M_EPILOG
}

void HNumber::add_leaf_low( integer_t from_, i32_t leaf_ ) {
	M_PROLOG
	i32_t carrier( leaf_ );
	i32_t* data( _canonical.get<i32_t>() );
	M_ASSERT( ( leaf_ >= 0 ) && ( leaf_ < LEAF ) );
	/*
	 * If number is in (-1;1) range the integral part size is equal 0,
	 * hence from_ can be equal to -1.
	 */
	M_ASSERT( ( from_ >= -1 ) && ( from_ < _leafCount ) );
	int long idx( from_ );
	while ( ( idx >= 0 ) && ( carrier > 0 ) ) {
		i32_t& x( data[idx] );
		x += carrier;
		if ( x >= LEAF ) {
			x -= LEAF;
			carrier = 1;
		} else {
			carrier = 0;
		}
		-- idx;
	}
	if ( carrier > 0 ) {
		_canonical.realloc( chunk_size<i32_t>( _leafCount + 1 ) );
		data = _canonical.get<i32_t>();
		::memmove( data + 1, data, static_cast<size_t>( chunk_size<i32_t>( _leafCount ) ) );
		++ _integralPartSize;
		++ _leafCount;
		data[0] = carrier;
	}
	return;
	M_EPILOG
}

HNumber operator "" _yn ( char const* str_, size_t len_ ) {
	return ( HString( str_, static_cast<int>( len_ ) ) );
}

HNumber operator "" _yn ( double long val_ ) {
	return ( val_ );
}

HNumber operator "" _yn ( int long long unsigned val_ ) {
	return ( val_ );
}

}

}

