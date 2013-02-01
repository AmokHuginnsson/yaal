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
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "algorithm.hxx"
#include "hnumber.hxx"
#include "hfile.hxx"

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
int const DECIMAL_DIGITS_IN_LEAF_CONST = 1;
int const SPECIAL_CHARS = 3; /* minus, dot, nil */
char const VALID_CHARACTERS[] = "-.0123456789";
int const JUST_DIGITS = 2;
int const HARDCODED_MINIMUM_PRECISION = 16;
int const A_MINUS = 0;
int const A_DOT = 1;
int const A_ZERO = 2;
int const NUMBER_START = 3;
int const KARATSUBA_THRESHOLD = 20; /* FIXME: 20 is fine */
i32_t const DECIMAL_SHIFT[] = {
	1l,
	10l,
	100l,
	1000l,
	10000l,
	100000l,
	1000000l,
	10000000l,
	100000000l,
	1000000000l
};
i32_t const LEAF = meta::power<10, DECIMAL_DIGITS_IN_LEAF_CONST>::value;
i32_t const LEAF_SQ = static_cast<i32_t>( ::sqrt( LEAF ) );
char ZFORMAT[] = "%00u";

char unused = ZFORMAT[2] = static_cast<char>( '0' + DECIMAL_DIGITS_IN_LEAF_CONST );

inline i32_t leafcmp( i32_t const* left_, i32_t const* right_, int long len_ ) {
	i32_t cmp( 0 );
	for ( int long i( 0 ); ! cmp && ( i < len_ ); ++ i )
		cmp = left_[ i ] - right_[ i ];
	return ( cmp );
}

static HNumber const _one_( 1 );

}

int const HNumber::DECIMAL_DIGITS_IN_LEAF = DECIMAL_DIGITS_IN_LEAF_CONST;
int HNumber::DEFAULT_PRECISION = 100;

HNumber::HNumber( void )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	return;
	M_EPILOG
}

HNumber::~HNumber( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

HNumber::HNumber( double long number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_floating_point( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( double number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_floating_point( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long long number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long long number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int long number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( int number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_integer( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( double long number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_floating_point( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( double number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_floating_point( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( char const* const number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( char const* const number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HString const& number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HString const& number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_leafCount( 0 ), _integralPartSize( 0 ),
	_canonical(), _cache(), _negative( false ) {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HNumber const& source )
	: _precision( source._precision ),
	_leafCount( source._leafCount ),
	_integralPartSize( source._integralPartSize ),
	_canonical(), _cache(),
	_negative( source._negative ) {
	M_PROLOG
	if ( source._leafCount ) {
		_canonical.realloc( chunk_size<i32_t>( source._leafCount ), HChunk::STRATEGY::EXACT );
		::memcpy( _canonical.raw(), source._canonical.raw(), chunk_size<i32_t>( source._leafCount ) );
		if ( source._cache.get_size() > 0 ) {
			_cache.realloc( source._cache.get_size(), HChunk::STRATEGY::EXACT );
			::memcpy( _cache.raw(), source._cache.raw(), source._cache.get_size() );
		}
	}
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
	int long long number( abs( number_ ) );
	if ( number_ >= ( LEAF * LEAF ) ) {
		_leafCount = 3;
		_canonical.realloc( chunk_size<i32_t>( _leafCount ) );
		i32_t* data( _canonical.get<i32_t>() );
		data[ 0 ] = static_cast<i32_t>( number / ( LEAF * LEAF ) );
		data[ 1 ] = static_cast<i32_t>( ( number % ( LEAF * LEAF ) ) / LEAF );
		data[ 2 ] = static_cast<i32_t>( number % LEAF );
	} else if ( number_ >= LEAF ) {
		_leafCount = 2;
		_canonical.realloc( chunk_size<i32_t>( _leafCount ) );
		i32_t* data( _canonical.get<i32_t>() );
		data[ 0 ] = static_cast<i32_t>( number / LEAF );
		data[ 1 ] = static_cast<i32_t>( number % LEAF );
	} else {
		_leafCount = 1;
		_canonical.realloc( chunk_size<i32_t>( _leafCount ) );
		_canonical.get<i32_t>()[ 0 ] = static_cast<i32_t>( number );
	}
	_integralPartSize = _leafCount;
	_negative = ( number_ < 0 );
	return;
	M_EPILOG
}

void HNumber::from_string( HString const& number_ ) {
	M_PROLOG
	/* ! - represent known but invalid character, ? - represent unknown character */
	int long start( number_.find_one_of( VALID_CHARACTERS ) );
	M_ENSURE( start != HString::npos ); /* exclude "!!!!" */
	char const* const src = number_.raw();
	_negative = ( src[ start ] == VALID_CHARACTERS[ A_MINUS ] ); /* "!!!-???" */
	if ( _negative )
		++ start;
	int long len( number_.get_length() );
	M_ENSURE( start < len ); /* exclude "!!-" */
	M_ENSURE( number_.find_one_of( VALID_CHARACTERS + A_DOT, start ) == start ); /* exclude "--" and "-!!" */
	int long idx( number_.find_other_than( "0", start ) ); /* skip leading 0s */
	int long end( start + 1 );
	_integralPartSize = 0;
	_leafCount = 0;
	if ( idx != HString::npos ) do /* "!!![-][.1-9]???" or "000." */ {
		int long firstValid( start );
		start = idx;
		int long dot( number_.find( VALID_CHARACTERS[ A_DOT ], start ) );
		idx = number_.find_other_than( VALID_CHARACTERS + A_DOT, start );
		if ( ( idx != HString::npos ) && ( idx < dot ) ) /* "!!232.!!!" */
			dot = HString::npos;
		int long digit( number_.find_one_of( VALID_CHARACTERS + A_ZERO, start ) );
		if ( ( digit == HString::npos ) && ( firstValid < start ) )
			break;
		M_ENSURE( digit != HString::npos ); /* must have digit */
		M_ENSURE( ( digit - start ) <= 1 ); /* exclude "-..!!" and "..!!" */
		end = number_.find_other_than( VALID_CHARACTERS + ( dot >= 0 ? A_ZERO : A_DOT ), dot >= 0 ? dot + 1 : start );
		( end != HString::npos ) || ( end = len );
		if ( dot != HString::npos ) {
			idx = number_.reverse_find_other_than( "0", len - end );
			end = ( idx != HString::npos ) ? len - idx : start + 1;
		}
		M_ASSERT( ( dot == HString::npos ) || ( ( end - dot ) > 0 ) );
		_integralPartSize = ( dot != HString::npos ? ( ( dot - start ) + ( DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) ) / DECIMAL_DIGITS_IN_LEAF_CONST : ( ( end - start ) + ( DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) ) / DECIMAL_DIGITS_IN_LEAF_CONST );
		int long fractionalPart( dot != HString::npos ? ( ( end - ( dot + 1 ) ) + ( DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) ) / DECIMAL_DIGITS_IN_LEAF_CONST : 0 );
		_leafCount = _integralPartSize + fractionalPart;
		if ( _leafCount > 0 )
			_canonical.realloc( chunk_size<i32_t>( _leafCount ) );
		i32_t* dst( _canonical.get<i32_t>() );
		i32_t leaf( 0 );
		int digitInLeaf( 0 );
		if ( dot != HString::npos ) /* scan fractional part */ {
			idx = _integralPartSize;
			for ( int long i( dot + 1 ); i < end; ++ i, ++ digitInLeaf ) {
				M_ASSERT( src[ i ] >= VALID_CHARACTERS[ A_ZERO ] );
				if ( digitInLeaf == DECIMAL_DIGITS_IN_LEAF_CONST ) {
					dst[idx ++] = leaf;
					digitInLeaf = 0;
					leaf = 0;
				}
				leaf += ( ( src[ i ] - VALID_CHARACTERS[ A_ZERO ] ) * DECIMAL_SHIFT[ ( DECIMAL_DIGITS_IN_LEAF_CONST - digitInLeaf ) - 1 ] );
			}
			if ( idx < _leafCount )
				dst[idx] = leaf;
		}
		if ( _integralPartSize > 0 ) {
			idx = _integralPartSize - 1;
			leaf = 0;
			digitInLeaf = 0;
			for ( int long i( ( dot != HString::npos ? dot : end ) - 1 ); i >= start; -- i, ++ digitInLeaf ) {
				M_ASSERT( src[ i ] >= VALID_CHARACTERS[ A_ZERO ] );
				if ( digitInLeaf == DECIMAL_DIGITS_IN_LEAF_CONST ) {
					M_ASSERT( idx >= 0 );
					dst[idx --] = leaf;
					digitInLeaf = 0;
					leaf = 0;
				}
				leaf += ( ( src[ i ] - VALID_CHARACTERS[ A_ZERO ] ) * DECIMAL_SHIFT[ digitInLeaf ] );
			}
			if ( idx >= 0 )
				dst[idx] = leaf;
		}
		if ( dot == HString::npos )
			_integralPartSize = _leafCount;
		else if ( ( end - dot - 1 ) >= _precision )
			_precision = end - dot;
	} while ( 0 );
	if ( _leafCount == 0 )
		_negative = false;
	return;
	M_EPILOG
}

HString HNumber::to_string( void ) const {
	M_PROLOG
	i32_t const* const src( _canonical.get<i32_t>() );
	_cache.realloc( _leafCount * DECIMAL_DIGITS_IN_LEAF_CONST + 3 ); /* + 1 for '.', + 1 for '-' and + 1 for terminating NIL */
	char* ptr( _cache.get<char>() );
	if ( _negative )
		*ptr ++ = VALID_CHARACTERS[ A_MINUS ];
	int leaf( 0 );
	for ( ; leaf < _integralPartSize; ++ leaf )
		ptr += snprintf( ptr, DECIMAL_DIGITS_IN_LEAF_CONST + 1, leaf ? ZFORMAT : "%u", src[ leaf ] ); /* + 1 for terminating NIL */
	if ( _leafCount > _integralPartSize )
		*ptr ++ = VALID_CHARACTERS[ A_DOT ];
	for ( ; leaf < _leafCount; ++ leaf )
		ptr += snprintf( ptr, DECIMAL_DIGITS_IN_LEAF_CONST + 1, ZFORMAT, src[ leaf ] );
	if ( ! _leafCount )
		*ptr ++ = '0';
	else if ( _leafCount > _integralPartSize ) {
		while ( *( ptr - 1 ) == '0' )
			-- ptr;
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
	if ( _integralPartSize > 3 )
		throw HNumberException( "integer overflow" );
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
	return ( value );
	M_EPILOG
}

int long HNumber::get_precision( void ) const {
	return ( _precision );
}

void HNumber::set_precision( int long precision_ ) {
	M_PROLOG
	if ( ( precision_ >= HARDCODED_MINIMUM_PRECISION )
			&& ( ( precision_ <= _precision ) || is_exact() ) )
		_precision = precision_;
	int long leafPrecision( ( _precision + DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) / DECIMAL_DIGITS_IN_LEAF_CONST );
	if ( ( _integralPartSize + leafPrecision ) < _leafCount )
		_leafCount = _integralPartSize + leafPrecision;
	return;
	M_EPILOG
}

int long HNumber::fractional_length( void ) const {
	return ( _leafCount - _integralPartSize );
}

int long HNumber::fractional_decimal_digits( void ) const {
	int long fractionalDecimalDigits( fractional_length() * DECIMAL_DIGITS_IN_LEAF_CONST );
	i32_t lastLeaf( _canonical.get<i32_t>()[ _leafCount - 1 ] );
	for ( int i( 1 ); i < ( DECIMAL_DIGITS_IN_LEAF_CONST - 1 ); ++ i ) {
		if ( lastLeaf % DECIMAL_SHIFT[i] )
			break;
		-- fractionalDecimalDigits;
	}
	return ( fractionalDecimalDigits );
}

bool HNumber::is_exact( void ) const {
	M_PROLOG
	bool exact( true );
	if ( fractional_length() > 0 ) {
		int long fractionalDecimalDigits( fractional_decimal_digits() );
		M_ASSERT( fractionalDecimalDigits <= _precision );
		exact = fractionalDecimalDigits < _precision;
	}
	return ( exact );
	M_EPILOG
}

int long HNumber::absolute_lower( HNumber const& other ) const {
	M_PROLOG
	i32_t const* p1( _canonical.get<i32_t>() );
	i32_t const* p2( other._canonical.get<i32_t>() );
	int long cmp( 1 );
	if ( _integralPartSize < other._integralPartSize )
		cmp = -1;
	else if ( _integralPartSize == other._integralPartSize ) {
		int long len = min( _leafCount, other._leafCount );
		cmp = leafcmp( p1, p2, len );
		if ( ! cmp )
			cmp = _leafCount - other._leafCount;
	}
	return ( cmp );
	M_EPILOG
}

bool HNumber::operator == ( HNumber const& other ) const {
	return ( ( _negative == other._negative )
			&& ( _leafCount == other._leafCount )
			&& ( _integralPartSize == other._integralPartSize )
			&& ! ::memcmp( _canonical.raw(), other._canonical.raw(), chunk_size<i32_t>( _leafCount ) ) );
}

bool HNumber::operator != ( HNumber const& other ) const {
	return ( ! operator == ( other ) );
}

bool HNumber::operator < ( HNumber const& other ) const {
	bool lower = false;
	if ( _negative && ! other._negative )
		lower = true;
	else if ( ( _negative && other._negative ) || ( ! ( _negative || other._negative ) ) ) {
		int long cmp = absolute_lower( other );
		lower = cmp < 0;
		if ( cmp && _negative && other._negative )
			lower = ! lower;
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

bool HNumber::mutate_addition( i32_t* res, int long ressize,
		i32_t const* const addends[], int long* missingIntegral, int long* missingFractional, bool sub, bool swp ) const {
	i32_t e[ 2 ];
	int long lmx[] = { missingIntegral ? ( swp ? missingIntegral[ 1 ] : missingIntegral[ 0 ] ) : 0, missingIntegral ? ( swp ? missingIntegral[ 0 ] : missingIntegral[ 1 ] ) : 0 };
	int long rmx[] = { missingFractional ? ( swp ? missingFractional[ 1 ] : missingFractional[ 0 ] ) : 0, missingFractional ? ( swp ? missingFractional[ 0 ] : missingFractional[ 1 ] ) : 0 };
	i32_t const* const epx[] = {
		swp ? addends[ 1 ] - lmx[ 0 ] - 1 : addends[ 0 ] - lmx[ 0 ] - 1,
		swp ? addends[ 0 ] - lmx[ 1 ] - 1 : addends[ 1 ] - lmx[ 1 ] - 1
	};
	int long idx( ressize - 1 ); /* index of first processed leaf */
	int side( ( rmx[ 0 ] > rmx[ 1 ] ) ? 1 : 0 );
	int long off( rmx[ 1 - side ] );
	i32_t const* src( epx[ side ] );
	e[ 1 - side ] = 0;
	i32_t carrier( 0 );
	while ( off -- && ( idx > 0 ) ) {
		e[ side ] = src[ idx ];
		if ( sub ) {
			i32_t x( e[ 0 ] - carrier );
			if ( x < e[ 1 ] ) {
				x += LEAF;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = x - e[ 1 ];
		} else {
		 	i32_t x( e[ 0 ] + e[ 1 ] + carrier );
			if ( x >= LEAF ) {
				x -= LEAF;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = x;
		}
		-- idx;
	}
	side = ( lmx[ 0 ] > lmx[ 1 ] ) ? 1 : 0;
	off = lmx[ 1 - side ];
	while ( idx > off ) {
		if ( sub ) {
			i32_t x( epx[ 0 ][ idx ] - carrier );
			if ( x < epx[ 1 ][ idx ] ) {
				x += LEAF;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = x - epx[ 1 ][ idx ];
		} else {
			i32_t x( epx[ 0 ][ idx ] + epx[ 1 ][ idx ] + carrier );
			if ( x >= LEAF ) {
				x -= LEAF;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = x;
		}
		-- idx;
	}
	src = epx[ side ];
	e[ 1 - side ] = 0;
	while ( idx > 0 ) {
		e[ side ] = src[ idx ];
		if ( sub ) {
			i32_t x( e[ 0 ] - carrier );
			if ( x < e[ 1 ] ) {
				x += LEAF;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = x - e[ 1 ];
		} else {
			i32_t x( e[ 0 ] + e[ 1 ] + carrier );
			if ( x >= LEAF ) {
				x -= LEAF;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = x;
		}
		-- idx;
	}
	if ( carrier && ! sub )
		++ res[ 0 ];
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
	int long ips = max( _integralPartSize, addend_._integralPartSize );
	int long dps = max( fractional_length(), addend_.fractional_length() );
	bool augendExact( is_exact() );
	bool addendExact( addend_.is_exact() );
	if ( fractional_length() < addend_.fractional_length() )
		_precision = augendExact ? addend_._precision : _precision;
	else
		_precision = addendExact ? _precision : addend_._precision;
	( dps <= _precision ) || ( dps = _precision );
	int long ressize = ips + dps + 1; /* + 1 for possible carrier */
	_cache.realloc( chunk_size<i32_t>( ressize ) );
	::memset( _cache.raw(), 0, _cache.get_size() );
	i32_t* res( _cache.get<i32_t>() );
	i32_t const* addend1( _canonical.get<i32_t>() );
	i32_t const* addend2( addend_._canonical.get<i32_t>() );
	int long missingIntegral[] = { ips - _integralPartSize, ips - addend_._integralPartSize };
	int long missingFractional[] = { dps - fractional_length(), dps - addend_.fractional_length() };
	( missingFractional[ 0 ] >= 0 ) || ( missingFractional[ 0 ] = 0 );
	( missingFractional[ 1 ] >= 0 ) || ( missingFractional[ 1 ] = 0 );
	i32_t const* addends[] = { addend1, addend2 };
	bool sub = ( ( _negative && ! addend_._negative ) || ( ! _negative && addend_._negative ) );
	bool swp = sub && ( absolute_lower( addend_ ) < 0 );
	mutate_addition( res, ressize, addends, missingIntegral, missingFractional, sub, swp );
	_integralPartSize = ips;
	if ( ressize > 0 ) {
		_negative = sub ? ( _negative ? ! swp : swp ) : ( _negative && addend_._negative );
		++ _integralPartSize;
	}
	_leafCount = ressize;
	_canonical.swap( _cache );
	normalize( augendExact && addendExact );
	if ( _leafCount == 0 )
		_negative = false;
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
	if ( ! _leafCount )
		_negative = false;
	operator += ( subtrahend_ );
	_negative = ! _negative;
	if ( ! _leafCount )
		_negative = false;
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
		::memset( _cache.raw(), 0, _cache.get_size() );
		karatsuba( _cache,
				n._canonical.get<i32_t>(), n._leafCount,
				factor._canonical.get<i32_t>(), factor._leafCount );
		_leafCount += factor_._leafCount;
		_integralPartSize += factor_._integralPartSize;
		_canonical.swap( _cache );
		normalize( multiplierExact && factorExact );
		_negative = ! ( ( n._negative && factor._negative ) || ! ( n._negative || factor._negative ) );
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

HNumber& HNumber::operator /= ( HNumber const& divisor_ ) {
	M_PROLOG
	M_ENSURE( divisor_._leafCount > 0 );
	if ( _leafCount ) {
		/*
		 * We use "long division" pen and paper algorithm.
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
		i32_t const* divisor( divisor_._canonical.get<i32_t>() );
		int long divisorLeafCount( divisor_._leafCount );
		int long lshift( 0 );
		while ( ! divisor[ lshift ] )
			++ lshift;
		divisor += lshift;
		divisorLeafCount -= lshift;
		int long rshift( 0 );
		while ( ! divisor[ divisorLeafCount - 1 ] ) {
			-- divisorLeafCount;
			++ rshift;
		}
		M_ASSERT( ( divisorLeafCount > 0 ) && ! ( lshift && rshift ) );
		/*
		 * Pre-allocate memory for result.
		 */
		_cache.realloc( chunk_size<i32_t>( _leafCount ) );
		/*
		 * We need memory buffer for dividend sample,
		 * and another one for multiplier sample.
		 * Assuming divisor has n leafs, a multiplier sample's buffer must
		 * be able to accomodate n + 1 leafs ( + 1 for possible carrier),
		 * dividendSample also must provide space for n + 1 leafs.
		 */
		HChunk buffer( chunk_size<i32_t>( ( divisorLeafCount + 1 ) * 2 ) );
		/*
		 * Buffer for trial&error multiplications done while looking for next quotient leaf.
		 */
		i32_t* multiplierSample( buffer.get<i32_t>() );
		i32_t* dividendSample( buffer.get<i32_t>() + divisorLeafCount + 1 );
		i32_t* dividend( _canonical.get<i32_t>() );
		int long newPrecision( 0 );
		bool dividendExact( is_exact() );
		bool divisorExact( divisor_.is_exact() );
		if ( dividendExact && divisorExact )
			newPrecision = _precision + divisor_._precision;
		else if ( ! ( dividendExact || divisorExact ) )
			newPrecision = min( _precision, divisor_._precision );
		else if ( dividendExact )
			newPrecision = divisor_._precision;
		else
			newPrecision = _precision;
		int long precision( ( newPrecision + DECIMAL_DIGITS_IN_LEAF_CONST - 1 ) / DECIMAL_DIGITS_IN_LEAF_CONST );
		int long shift( 0 );
		while ( ( shift < _leafCount ) && ! dividend[ shift ] )
			++ shift;
		int long divSampleLen( min( _leafCount - shift, divisorLeafCount ) ); /* Index of the next leaf to process */
		::memcpy( dividendSample, dividend + shift, chunk_size<i32_t>( divSampleLen ) );
		int long dividendLeafNo( divSampleLen + shift );
		/*
		 * Number of leafs in quotient before leaf point.
		 * Variable used to stop calculations based on maximum precision.
		 */
		int long integralPart( _integralPartSize - divisor_._integralPartSize + ( divisor_._integralPartSize > 0 ? 0 : lshift ) - shift );
		int long quotientLeafNo( 0 ); /* Index of currently guessed quotient leaf. */
		if ( integralPart < 0 ) {
			_cache.realloc( chunk_size<i32_t>( quotientLeafNo - integralPart ) );
			::memset( _cache.get<i32_t>() + quotientLeafNo, 0, chunk_size<i32_t>( -integralPart ) );
			quotientLeafNo -= integralPart;
			integralPart = 0;
		}
		++ integralPart;
		while ( ! dividend[ _leafCount - 1 ] )
			-- _leafCount;
		bool firstLeaf( true );
		bool exact( false );
		do {
			/*
			 * Using binary search algorithm we have to guess next leaf value.
			 * Assuming that "correctLeaf" is proper next leaf value to be guessed,
			 * following relation holds:
			 * leafHi > correctLeaf >= leafLow
			 * and correct leaf is found when leafHi == ( leafLow + 1 ).
			 */
			i32_t leafHi( LEAF ); /* binary search algo upper bound  */
			i32_t leaf( LEAF_SQ ); /* guessed leaf */
			i32_t leafLow( 0 ); /* binary search algo lower bound */

			int long zfill( dividendLeafNo );
			/*
			 * Fix possible underflow in dividendSample.
			 */
			shift = 0;
			while ( ( shift < divSampleLen ) && ! dividendSample[ shift ] )
				++ shift;
			if ( shift > 0 ) {
				divSampleLen -= shift;
				if ( divSampleLen > 0 )
					::memmove( dividendSample, dividendSample + shift, chunk_size<i32_t>( divSampleLen ) );
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
						while ( ( ( dividendLeafNo + shift ) < _leafCount ) && ! dividend[ dividendLeafNo + shift ] )
							++ shift;
						dividendLeafNo += shift;
					}
					int long compensationLen( min( _leafCount - dividendLeafNo, divisorLeafCount - divSampleLen ) );
					::memcpy( dividendSample + divSampleLen, dividend + dividendLeafNo, chunk_size<i32_t>( compensationLen ) );
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
				int long compensationLen( divisorLeafCount - divSampleLen );
				::memset( dividendSample + divSampleLen, 0, chunk_size<i32_t>( compensationLen ) );
				divSampleLen = divisorLeafCount;
				dividendLeafNo += compensationLen;
//				if ( firstLeaf ) {
//					integralPart -= compensationLen;
//					( integralPart >= 0 ) || ( integralPart = 0 ); /* too hackish? */
//				}
			}

			/*
			 * If dividendSample is still not enough to produce next quotient leaf
			 * then fetch next leaf from divident to dividendSample.
			 */
			if ( leafcmp( dividendSample, divisor, divSampleLen ) < 0 ) /* We establish that divisor_ <= multiplierSample. */ {
				dividendSample[divSampleLen] = dividendLeafNo < _leafCount ? dividend[dividendLeafNo] : 0;
				++ divSampleLen;
				++ dividendLeafNo;
				if ( firstLeaf )
					-- integralPart;
			}

			if ( ! firstLeaf ) {
				zfill = dividendLeafNo - zfill - 1;
				if ( zfill > 0 ) {
					_cache.realloc( chunk_size<i32_t>( quotientLeafNo + zfill ) );
					::memset( _cache.get<i32_t>() + quotientLeafNo, 0, chunk_size<i32_t>( zfill ) );
					quotientLeafNo += zfill;
				}
			}
			firstLeaf = false;

			leafLow = 1;
			/* Binary search loop. */
			while ( leafHi > ( leafLow + 1 ) ) {
				/*
				 * Multiply current guess about current leaf by divisor.
				 */
				::memset( multiplierSample, 0, chunk_size<i32_t>( divisorLeafCount + 1 ) );
				karatsuba( buffer, &leaf, 1, divisor, divisorLeafCount );
				/*
				 * If product has more leafs than divisor than it is bigger.
				 */
				i32_t cmp( 0 );
				int off( multiplierSample[0] ? 0 : 1 );
				if ( ( divSampleLen > divisorLeafCount ) && ! multiplierSample[0] )
					cmp = -1;
				else if ( multiplierSample[0] && ( divSampleLen == divisorLeafCount ) )
					cmp = 1;
				if ( ! cmp )
					cmp = leafcmp( multiplierSample + off, dividendSample, divSampleLen );
				if ( cmp > 0 )
					leafHi = leaf;
				else if ( cmp < 0 )
					leafLow = leaf;
				else
					break;
				leaf = ( leafHi + leafLow ) / 2;
			}
			/*
			 * A helper for mutate_addition.
			 * The substraction is done `in place'.
			 */
			::memset( multiplierSample, 0, chunk_size<i32_t>( divisorLeafCount + 1 ) );
			karatsuba( buffer, &leaf, 1, divisor, divisorLeafCount );
			i32_t const* addends[] = { dividendSample, multiplierSample + ( divSampleLen == divisorLeafCount ? 1 : 0 ) };
			if ( mutate_addition( dividendSample - 1, divSampleLen + 1, addends, NULL, NULL, true, false ) ) {
				dividendSample[ -- divSampleLen ] = 0;
			}
			_cache.realloc( chunk_size<i32_t>( quotientLeafNo + 1 ) );
			_cache.get<i32_t>()[ quotientLeafNo ++ ] = leaf;
		} while ( ( quotientLeafNo - integralPart ) < precision );
		_integralPartSize = integralPart;
		_leafCount = ( integralPart > quotientLeafNo ? integralPart : quotientLeafNo );
		if ( quotientLeafNo < _leafCount )
			::memset( _cache.get<i32_t>() + quotientLeafNo, 0, chunk_size<i32_t>( _leafCount - quotientLeafNo ) );
		_negative = ! ( ( _negative && divisor_._negative ) || ! ( _negative || divisor_._negative ) );
		_precision = newPrecision;
		_canonical.swap( _cache );
		normalize( exact && dividendExact && divisorExact );
	}
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator - ( void ) const {
	M_PROLOG
	HNumber n( *this );
	if ( _leafCount )
		n._negative = ! n._negative;
	return ( n );
	M_EPILOG
}

HNumber HNumber::operator ^ ( int long unsigned exp ) const {
	M_PROLOG
	HNumber n( *this );
	n ^= exp;
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator ^= ( int long unsigned exp ) {
	M_PROLOG
	int long unsigned p( exp >> 1 );
	HNumber n( *this );
	if ( p > 2 )
		operator ^= ( p );
	else
		while ( -- p )
			operator *= ( n );
	operator *= ( *this );
	if ( exp % 2 )
		operator *= ( n );
	return ( *this );
	M_EPILOG
}

HNumber& HNumber::operator ++ ( void ) {
	M_PROLOG
	operator += ( _one_ );
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
	operator -= ( _one_ );
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
	int shift( 0 );
	while ( ( shift < _integralPartSize ) && ( res[ shift ] == 0 ) )
		++ shift;
	if ( shift ) {
		_integralPartSize -= shift;
		_leafCount -= shift;
		::memmove( res, res + shift, chunk_size<i32_t>( _leafCount ) );
	}
	while ( ( fractional_length() > 0 ) && ( res[ _leafCount - 1 ] == 0 ) )
		-- _leafCount;
	if ( updatePrecision_ && ( fractional_decimal_digits() == _precision ) )
		++ _precision;
	return;
	M_EPILOG
}

int long HNumber::karatsuba( HChunk& result, i32_t const* fx, int long fxs, i32_t const* fy, int long fys ) {
	int long shift( 0 );
	while ( ( shift < fxs ) && ! fx[ shift ] )
		++ shift;
	int long fxrl( fxs - shift );
	int long totalShift( shift );
	shift = 0;
	while ( ( shift < fys ) && ! fy[ shift ] )
		++ shift;
	int long fyrl( fys - shift );
	totalShift += shift;
	int long leafCount( 0 );
	if ( ( ( fxrl > KARATSUBA_THRESHOLD ) || ( fyrl > KARATSUBA_THRESHOLD ) ) && ( fxrl > 0 ) && ( fyrl > 0 ) ) {
		fx += ( fxs - fxrl );
		fy += ( fys - fyrl );
		fxs = fxrl;
		fys = fyrl;
		result.realloc( chunk_size<i32_t>( leafCount = fxs + fys + totalShift ) );

		int long const fs( max( fxs, fys ) );
		int long const fl( min( fxs, fys ) );
		int long const m( ( fs / 2 ) + ( fs & 1 ) ); /* Size of upper/lower half of number */
		HChunk r2m; /* intermediate result ( fx1 * fx2 ( * B ^ 2m ) ) + 1 for carrier */
		int long const r2ms( karatsuba( r2m, fx, fxs - m, fy, fys - m ) );
		HChunk r; /* intermediate result ( fx2 * fy2 ) + 1 for carrier */
		int long const rs( karatsuba( r, fx + ( fxs > m ? fxs - m : 0 ), min( fxs, m ), fy + ( fys > m ? fys - m : 0 ), min( fys, m ) ) );
		int long const size( fxs + fys + 1 ); 
		HChunk buffer( chunk_size<i32_t>( max( size + 1, ( m + 1 ) * 2 ) ) ); /* + 1 for carrier */
		i32_t* hx( buffer.get<i32_t>() );
		i32_t* hy( buffer.get<i32_t>() + m + 1 );
		/* preparation of hx and hy */
		/* hx = fx / B^m + fx % B^m */
		int long missingIntegral[] = { 2 * m - fxs, 0 };
		i32_t const* addends[] = { fx, fx + fxs - m };
		if ( fxs > m )
			mutate_addition( hx, m + 1, addends, missingIntegral, NULL, false, false );
		else
			::memcpy( hx + m + 1 - fxs, fx, chunk_size<i32_t>( fxs ) );
		/* hy */
		missingIntegral[ 0 ] = 2 * m - fys;
		addends[ 0 ] = fy;
		addends[ 1 ] = fy + fys - m;
		if ( fys > m )
			mutate_addition( hy, m + 1, addends, missingIntegral, NULL, false, false );
		else
			::memcpy( hy + m + 1 - fys, fy, chunk_size<i32_t>( fys ) );
		/* find Z */
		HChunk Z;
		int long Zs( karatsuba( Z, hx, m + 1, hy, m + 1 ) );
		/* combine all results */
		i32_t* const res( buffer.get<i32_t>() + 1 ); /* Z*B^m can possibly (front)overflow buffer of size = fxs + fys, hence + 1 in allocation and here. */
		::memset( res, 0, chunk_size<i32_t>( size ) );

		/* res = Z*B^m + r */
		i32_t const* p( Z.get<i32_t>() );
		shift = 0;
		while ( ( shift < Zs ) && ! p[ shift ] )
			++ shift;
		Zs -= shift;
		p += shift;
		addends[ 0 ] = r.get<i32_t>();
		addends[ 1 ] = p;
		missingIntegral[ 0 ] = Zs - rs + m;
		missingIntegral[ 1 ] = 0;
		if ( addends[ 0 ] ) {
			::memcpy( res + size - rs, r.raw(), chunk_size<i32_t>( rs ) );
			mutate_addition( res + size - m - Zs - 1, Zs + 1, addends, missingIntegral, NULL, false, false );
		} else
			::memcpy( res + size - m - Zs, p, chunk_size<i32_t>( Zs ) );

		/* res += r2m*B^2m */
		Zs = r2ms;
		shift = 0;
		p = r2m.get<i32_t>();
		missingIntegral[ 0 ] = 0;
		if ( p ) {
			while ( ( shift < Zs ) && ! p[ shift ] )
				++ shift;
			Zs -= shift;
			p += shift;
			addends[ 0 ] = res + 1 + shift;
			addends[ 1 ] = p;
			missingIntegral[ 1 ] = 0;
			mutate_addition( res + shift, Zs + 1, addends, missingIntegral, NULL, false, false );
		}

		/* res -= r2m*B^m, res -= r*B^m */

		int ncar( ( fl > m ? 1 : 0 ) );
		int car( 1 - ncar );

		addends[ 0 ] = res + ncar;
		addends[ 1 ] = r2m.get<i32_t>();
		if ( addends[ 1 ] ) {
			missingIntegral[ 1 ] = size - r2ms - m - ncar;
			mutate_addition( res - car, size - m + car, addends, missingIntegral, NULL, true, false );
		}
		addends[ 1 ] = r.get<i32_t>();
		if ( addends[ 1 ] ) {
			missingIntegral[ 1 ] = size - rs - m - ncar;
			mutate_addition( res - car, size - m + car, addends, missingIntegral, NULL, true, false );
		}
		::memcpy( result.get<i32_t>() + totalShift, res + 1, chunk_size<i32_t>( leafCount - totalShift ) );
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
				int long inner( fys ); /* index of last leaf in second factor */
				while ( -- inner >= 0 ) {
					int long pos( inner + 1 );
					i64_t x( static_cast<i64_t>( fx[ fxs ] ) * fy[ inner ] + carrier );
					if ( x >= LEAF ) {
						carrier = static_cast<i32_t>( x / LEAF );
						x %= LEAF;
					} else
						carrier = 0;
					e[ pos ] = static_cast<i32_t>( x );
				}
				e[ 0 ] = carrier;
				addends[ 0 ] = res + 1;
				mutate_addition( res, fys + 1 + 1, addends, NULL, NULL, false, false );
				-- res;
			} else
				-- res;
		}
	}
	return ( leafCount );
}

}

}

