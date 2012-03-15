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

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "algorithm.hxx"
#include "hnumber.hxx"
#include "hfile.hxx"

namespace yaal {

namespace hcore {

namespace {
int const SPECIAL_CHARS = 3; /* minus, dot, nil */
char const VALID_CHARACTERS[] = "-.0123456789";
int const JUST_DIGITS = 2;
int const HARDCODED_MINIMUM_PRECISION = 16;
int const A_MINUS = 0;
int const A_DOT = 1;
int const A_ZERO = 2;
int const NUMBER_START = 3;
int const KARATSUBA_THRESHOLD = 20; /* FIXME: 20 is fine */
}

int HNumber::DEFAULT_PRECISION = 100;

HNumber::HNumber( void )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_negative( false ), _digitCount( 0 ), _integralPartSize( 0 ),
	_canonical() {
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
	_negative( false ), _digitCount( 0 ), _integralPartSize( 0 ),
	_canonical() {
	M_PROLOG
	from_double( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( double long number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_negative( false ), _digitCount( 0 ), _integralPartSize( 0 ),
	_canonical() {
	M_PROLOG
	from_double( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( char const* const number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_negative( false ), _digitCount( 0 ), _integralPartSize( 0 ),
	_canonical() {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( char const* const number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_negative( false ), _digitCount( 0 ), _integralPartSize( 0 ),
	_canonical() {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HString const& number_ )
	: _precision( DEFAULT_PRECISION > HARDCODED_MINIMUM_PRECISION
			? DEFAULT_PRECISION : HARDCODED_MINIMUM_PRECISION ),
	_negative( false ), _digitCount( 0 ), _integralPartSize( 0 ),
	_canonical() {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HString const& number_, int precision_ )
	: _precision( precision_ > HARDCODED_MINIMUM_PRECISION
			? precision_ : HARDCODED_MINIMUM_PRECISION ),
	_negative( false ), _digitCount( 0 ), _integralPartSize( 0 ),
	_canonical() {
	M_PROLOG
	from_string( number_ );
	return;
	M_EPILOG
}

HNumber::HNumber( HNumber const& source )
	: _precision( source._precision ),
	_negative( source._negative ),
	_digitCount( source._digitCount ),
	_integralPartSize( source._integralPartSize ),
	_canonical() {
	M_PROLOG
	if ( source._digitCount ) {
		_canonical.realloc( source._digitCount, HChunk::STRATEGY::EXACT );
		::memcpy( _canonical.raw(), source._canonical.raw(), _canonical.get_size() );
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
		swap( _negative, other._negative );
		swap( _digitCount, other._digitCount );
		swap( _integralPartSize, other._integralPartSize );
		swap( _canonical, other._canonical );
	}
	return;
	M_EPILOG
}

void HNumber::from_double( double long number_ ) {
	M_PROLOG
	HString source( number_ );
	from_string( source );
	return;
	M_EPILOG
}

void HNumber::from_string( HString const& number_ ) {
	M_PROLOG
	/* ! - represent known but invalid character, ? - represent unknown character */
	int long start = number_.find_one_of( VALID_CHARACTERS );
	M_ENSURE( start >= 0 ); /* exclude "!!!!" */
	char const* const src = number_.raw();
	_negative = ( src[ start ] == VALID_CHARACTERS[ A_MINUS ] ); /* "!!!-???" */
	if ( _negative )
		++ start;
	int long len = number_.get_length();
	M_ENSURE( start < len ); /* exclude "!!-" */
	M_ENSURE( number_.find_one_of( VALID_CHARACTERS + A_DOT, start ) == start ); /* exclude "--" and "-!!" */
	int long idx = number_.find_other_than( "0", start ); /* skip leading 0s */
	int long end = start + 1;
	_integralPartSize = 0;
	_digitCount = 0;
	if ( idx >= 0 ) do /* "!!![-][.1-9]???" or "000." */ {
		int long first_valid = start;
		start = idx;
		int long dot = number_.find( VALID_CHARACTERS[ A_DOT ], start );
		idx = number_.find_other_than( VALID_CHARACTERS + A_DOT, start );
		if ( ( idx >= 0 ) && ( idx < dot ) ) /* "!!232!!." */
			dot = -1;
		int long digit = number_.find_one_of( VALID_CHARACTERS + A_ZERO, start );
		if ( ( digit < 0 ) && ( first_valid < start ) )
			break;
		M_ENSURE( digit >= 0 ); /* must have digit */
		M_ENSURE( ( digit - start ) <= 1 ); /* exclude "-..!!" and "..!!" */
		end = number_.find_other_than( VALID_CHARACTERS + ( dot >= 0 ? A_ZERO : A_DOT ), dot >= 0 ? dot + 1 : start );
		( end >= 0 ) || ( end = len );
		if ( dot >= 0 ) {
			idx = number_.reverse_find_other_than( "0", len - end );
			end = ( idx >= 0 ) ? len - idx : start + 1;
		}
		_digitCount = end - start;
		if ( dot >= 0 ) {
			_integralPartSize = dot - start;
			-- _digitCount;
		} else
			_integralPartSize = _digitCount;
		if ( decimal_length() >= _precision )
			_precision = decimal_length() + 1;
		if ( _digitCount )
			_canonical.realloc( _digitCount );
		char* dst = _canonical.get<char>();
		idx = 0;
		for ( int long i = start; i < end; ++ i ) {
			if ( src[ i ] == VALID_CHARACTERS[ A_DOT ] )
				continue;
			M_ASSERT( src[ i ] >= VALID_CHARACTERS[ A_ZERO ] );
			dst[ idx ++ ] = static_cast<char>( src[ i ] - VALID_CHARACTERS[ A_ZERO ] );
		}
	} while ( 0 );
	if ( _digitCount == 0 )
		_negative = false;
	return;
	M_EPILOG
}

HString HNumber::to_string( void ) const {
	M_PROLOG
	HString str = "";
	if ( _negative )
		str = VALID_CHARACTERS[ A_MINUS ];
	char const* const src( _canonical.get<char>() );
	int digit = 0;
	for ( ; digit < _integralPartSize; ++ digit )
		str += static_cast<char>( src[ digit ] + VALID_CHARACTERS[ A_ZERO ] );
	if ( _digitCount > _integralPartSize )
		str += VALID_CHARACTERS[ A_DOT ];
	for ( ; digit < _digitCount; ++ digit )
		str += static_cast<char>( src[ digit ] + VALID_CHARACTERS[ A_ZERO ] );
	if ( ! _digitCount )
		str = "0";
	return ( str );
	M_EPILOG
}

double long HNumber::to_double( void ) const {
	M_PROLOG
	return ( lexical_cast<double>( to_string() ) );
	M_EPILOG
}

int long HNumber::get_precision( void ) const {
	return ( _precision );
}

void HNumber::set_precision( int long precision_ ) {
	M_PROLOG
	if ( ( precision_ >= HARDCODED_MINIMUM_PRECISION )
			&& ( ( precision_ <= _precision ) || ( decimal_length() < _precision ) ) )
		_precision = precision_;
	if ( ( _integralPartSize + _precision ) < _digitCount )
		_digitCount = _integralPartSize + _precision;
	return;
	M_EPILOG
}

int long HNumber::decimal_length( void ) const {
	return ( _digitCount - _integralPartSize );
}

bool HNumber::is_exact( void ) const {
	M_ASSERT( ( _digitCount - _integralPartSize ) <= _precision );
	return ( ( _digitCount - _integralPartSize ) < _precision );
}

int long HNumber::absolute_lower( HNumber const& other ) const {
	char const* p1( _canonical.get<char>() );
	char const* p2( other._canonical.get<char>() );
	int long cmp = 1;
	if ( _integralPartSize < other._integralPartSize )
		cmp = -1;
	else if ( _integralPartSize == other._integralPartSize ) {
		int long len = min( _digitCount, other._digitCount );
		cmp = memcmp( p1, p2, len );
		if ( ! cmp )
			cmp = _digitCount - other._digitCount;
	}
	return ( cmp );
}

bool HNumber::operator == ( HNumber const& other ) const {
	return ( ( _negative == other._negative )
			&& ( _digitCount == other._digitCount )
			&& ( _integralPartSize == other._integralPartSize )
			&& ! ::memcmp( _canonical.raw(), other._canonical.raw(), _digitCount ) );
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

bool HNumber::mutate_addition( char* res, int long ressize,
		char const* const ep[], int long* lm, int long* rm, bool sub, bool swp ) const {
	int carrier = 0;
	int e[ 2 ];
	int long lmx[] = { lm ? ( swp ? lm[ 1 ] : lm[ 0 ] ) : 0, lm ? ( swp ? lm[ 0 ] : lm[ 1 ] ) : 0 };
	int long rmx[] = { rm ? ( swp ? rm[ 1 ] : rm[ 0 ] ) : 0, rm ? ( swp ? rm[ 0 ] : rm[ 1 ] ) : 0 };
	char const* const epx[] = {
		swp ? ep[ 1 ] - lmx[ 0 ] - 1 : ep[ 0 ] - lmx[ 0 ] - 1,
		swp ? ep[ 0 ] - lmx[ 1 ] - 1 : ep[ 1 ] - lmx[ 1 ] - 1
	};
	int long idx = ressize - 1; /* index of first processed digit */
	int side = ( rmx[ 0 ] > rmx[ 1 ] ) ? 1 : 0;
	int long off = rmx[ 1 - side ];
	char const* src = epx[ side ];
	e[ 1 - side ] = 0;
	while ( off -- && ( idx > 0 ) ) {
		e[ side ] = src[ idx ];
		if ( sub ) {
			int x = e[ 0 ] - carrier;
			if ( x < e[ 1 ] ) {
				x += 10;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = static_cast<char>( x - e[ 1 ] );
		} else {
		 	int x = e[ 0 ] + e[ 1 ] + carrier;
			if ( x > 9 ) {
				x -= 10;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = static_cast<char>( x );
		}
		-- idx;
	}
	side = ( lmx[ 0 ] > lmx[ 1 ] ) ? 1 : 0;
	off = lmx[ 1 - side ];
	while ( idx > off ) {
		if ( sub ) {
			int x = epx[ 0 ][ idx ] - carrier;
			if ( x < epx[ 1 ][ idx ] ) {
				x += 10;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = static_cast<char>( x - epx[ 1 ][ idx ] );
		} else {
			int x = epx[ 0 ][ idx ] + epx[ 1 ][ idx ] + carrier;
			if ( x > 9 ) {
				x -= 10;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = static_cast<char>( x );
		}
		-- idx;
	}
	src = epx[ side ];
	e[ 1 - side ] = 0;
	while ( idx > 0 ) {
		e[ side ] = src[ idx ];
		if ( sub ) {
			int x = e[ 0 ] - carrier;
			if ( x < e[ 1 ] ) {
				x += 10;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = static_cast<char>( x - e[ 1 ] );
		} else {
			int x = e[ 0 ] + e[ 1 ] + carrier;
			if ( x > 9 ) {
				x -= 10;
				carrier = 1;
			} else
				carrier = 0;
			res[ idx ] = static_cast<char>( x );
		}
		-- idx;
	}
	if ( carrier && ! sub )
		++ res[ 0 ];
	return ( carrier ? true : false );
}

HNumber HNumber::operator + ( HNumber const& element ) const {
	M_PROLOG
	int long ips = max( _integralPartSize, element._integralPartSize );
	int long dps = max( decimal_length(), element.decimal_length() );
	HNumber n;
	if ( decimal_length() < element.decimal_length() )
		n._precision = is_exact() ? element._precision : _precision;
	else
		n._precision = element.is_exact() ? _precision : element._precision;
	( dps <= n._precision ) || ( dps = n._precision );
	int long ressize = ips + dps + 1; /* + 1 for possible carrier */
	n._canonical.realloc( ressize );
	n._integralPartSize = ips;
	char* res( n._canonical.get<char>() );
	char const* ep1( _canonical.get<char>() );
	char const* ep2( element._canonical.get<char>() );
	int long lm[] = { ips - _integralPartSize, ips - element._integralPartSize };
	int long rm[] = { dps - decimal_length(), dps - element.decimal_length() };
	( rm[ 0 ] >= 0 ) || ( rm[ 0 ] = 0 );
	( rm[ 1 ] >= 0 ) || ( rm[ 1 ] = 0 );
	char const* ep[] = { ep1, ep2 };
	bool sub = ( ( _negative && ! element._negative ) || ( ! _negative && element._negative ) );
	bool swp = sub && ( absolute_lower( element ) < 0 );
	mutate_addition( res, ressize, ep, lm, rm, sub, swp );
	if ( ressize > 0 ) {
		n._negative = sub ? ( _negative ? ! swp : swp ) : ( _negative && element._negative );
		++ n._integralPartSize;
	}
	n._digitCount = ressize;
	n.normalize();
	if ( n._digitCount == 0 )
		n._negative = false;
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator += ( HNumber const& element ) {
	M_PROLOG
	operator = ( *this + element );
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator - ( HNumber const& element ) const {
	M_PROLOG
	HNumber n( *this );
	n -= element;
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator -= ( HNumber const& element ) {
	M_PROLOG
	operator = ( *this + -element );
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator * ( HNumber const& factor ) const {
	M_PROLOG
	HNumber n;
	if ( factor._digitCount < _digitCount )
		n = factor * ( *this );
	else if ( _digitCount && factor._digitCount ) {
		n._digitCount = _digitCount + factor._digitCount;
		n._integralPartSize = _integralPartSize + factor._integralPartSize;
		karatsuba( n._canonical,
				_canonical.get<char>(), _digitCount,
				factor._canonical.get<char>(), factor._digitCount );
		n.normalize();
		n._negative = ! ( ( _negative && factor._negative ) || ! ( _negative || factor._negative ) );
	}
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator *= ( HNumber const& factor ) {
	M_PROLOG
	operator = ( *this * factor );
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator / ( HNumber const& denominator ) const {
	M_PROLOG
	M_ENSURE( denominator._digitCount != 0 );
	HNumber n;
	if ( _digitCount ) {
		n._precision = _precision + denominator._precision;
		char const* const den( denominator._canonical.get<char>() );
		int long shift( 0 );
		while ( ( shift < denominator._digitCount ) && ( den[ shift ] == 0 ) )
			++ shift;
		int long denlen( denominator._digitCount - shift );
		HChunk reminder( denlen + 1 ); /* + 1 for carrier */
		HChunk pseudoden( denlen + 1 );
		char const* src( _canonical.get<char>() );
		char* pden( pseudoden.get<char>() );
		char* rem( reminder.get<char>() );
		char const* ep[] = { rem, pden };
		int long len( min( _digitCount, denlen ) );
		::memcpy( pden + 1, denominator._canonical.get<char>() + shift, denlen );
		::memcpy( rem + 1 + denlen - len, src, len );
		shift = 0;
		while ( ( shift < _digitCount ) && ( src[ shift ++ ] == 0 ) )
			++ n._digitCount;
		shift = denominator._integralPartSize - _integralPartSize;
		while ( -- shift > 0 )
			++ n._digitCount;
		if ( n._digitCount )
			n._canonical.realloc( n._digitCount );
		int cmp = 0;
		shift = 0;
		bool carrier( ( denominator._integralPartSize - _integralPartSize - denominator.decimal_length() ) > 0 );
		bool ncar( false );
		int long pred_int( _integralPartSize - denominator._integralPartSize + denominator._digitCount - denlen );
		( pred_int >= 0 ) || ( pred_int = 0 );
		++ pred_int;
		do {
			int digit = 0;
			while ( ( cmp = memcmp( rem, pden, denlen + 1 ) ) > 0 ) {
				mutate_addition( rem - 1, denlen + 1 + 1, ep, NULL, NULL, true, false );
				++ digit;
			}
			::memmove( rem, rem + 1, denlen );
			rem[ denlen ] = len < _digitCount ? src[ len ] : '\0';
			if ( ! cmp ) {
				::memset( rem, 0, denlen );
				++ digit;
			}
			if ( digit || shift ) {
				n._canonical.realloc( n._digitCount + 1 );
				n._canonical.get<char>()[ n._digitCount ++ ] = static_cast<char>( digit );
				shift = 1;
			}
			if ( rem[ 0 ] && carrier && ! shift ) {
				n._canonical.realloc( ++ n._digitCount );
				carrier = false;
			} else if ( rem[ 0 ] && ! shift )
				ncar = true;
			++ len;
		} while ( ( len <= _digitCount ) || ( ( ( n._digitCount - pred_int ) < n._precision ) && cmp ) );
		n._integralPartSize = _integralPartSize - denominator._integralPartSize + denominator._digitCount - denlen + ( ! ncar ? 1 : 0 );
		while ( n._digitCount < n._integralPartSize )
			n._canonical.realloc( ++ n._digitCount );
		char* res( n._canonical.get<char>() );
		if ( ( n._integralPartSize < 0 ) && ncar ) {
			n._canonical.realloc( n._digitCount + 1 );
			res = n._canonical.get<char>();
			::memmove( res + 1, res, n._digitCount );
			res[ 0 ] = 0;
			++ n._digitCount;
		}
		( n._integralPartSize >= 0 ) || ( n._integralPartSize = 0 );
		n.normalize();
		if ( n._digitCount > ( n._integralPartSize + n._precision ) )
			n._digitCount = n._integralPartSize + n._precision;
		M_ASSERT( n._integralPartSize >= 0 );
		M_ASSERT( n._digitCount >= 0 );
		n._negative = ! ( ( _negative && denominator._negative ) || ! ( _negative || denominator._negative ) );
	}
	return ( n );
	M_EPILOG
}

HNumber& HNumber::operator /= ( HNumber const& factor ) {
	M_PROLOG
	operator = ( *this / factor );
	return ( *this );
	M_EPILOG
}

HNumber HNumber::operator - ( void ) const {
	M_PROLOG
	HNumber n( *this );
	if ( _digitCount )
		n._negative = ! n._negative;
	return ( n );
	M_EPILOG
}

HNumber HNumber::operator ^ ( int long unsigned exp ) const {
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

HNumber& HNumber::operator ^= ( int long unsigned exp ) {
	M_PROLOG
	operator = ( *this ^ exp );
	return ( *this );
	M_EPILOG
}

HNumber& HNumber::operator ++ ( void ) {
	M_PROLOG
	operator += ( 1 );
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
	operator -= ( 1 );
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

void HNumber::normalize( void ) {
	M_PROLOG
	char* res( _canonical.get<char>() );
	int shift = 0;
	while ( ( shift < _integralPartSize ) && ( res[ shift ] == 0 ) )
		++ shift;
	if ( shift ) {
		_integralPartSize -= shift;
		_digitCount -= shift;
		::memmove( res, res + shift, _digitCount );
	} while ( ( decimal_length() > 0 ) && ( res[ _digitCount - 1 ] == 0 ) )
		-- _digitCount;
	if ( _digitCount == ( _integralPartSize + _precision ) )
		++ _precision;
	return;
	M_EPILOG
}

void HNumber::karatsuba( HChunk& result, char const* fx, int long fxs, char const* fy, int long fys ) const {
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
	if ( ( ( fxrl > KARATSUBA_THRESHOLD ) || ( fyrl > KARATSUBA_THRESHOLD ) ) && ( fxrl > 0 ) && ( fyrl > 0 ) ) {
		fx += ( fxs - fxrl );
		fy += ( fys - fyrl );
		fxs = fxrl;
		fys = fyrl;
		if ( ! result.size() && ( fxs + fys ) )
			result.realloc( fxs + fys + totalShift, HChunk::STRATEGY::EXACT );

		int long fs = max( fxs, fys );
		int long fl = min( fxs, fys );
		int long m = ( fs / 2 ) + ( fs & 1 ); /* Size of upper/lower half of number */
		HChunk r2m; /* intermediate result ( fx1 * fx2 ( * B ^ 2m ) ) + 1 for carrier */
		karatsuba( r2m, fx, fxs - m, fy, fys - m );
		HChunk r; /* intermediate result ( fx2 * fy2 ) + 1 for carrier */
		karatsuba( r, fx + ( fxs > m ? fxs - m : 0 ), min( fxs, m ), fy + ( fys > m ? fys - m : 0 ), min( fys, m ) );
		HChunk hx( m + 1 ); /* + 1 for carrier */
		HChunk hy( m + 1 );
		/* preparation of hx and hy */
		/* hx = fx / B^m + fx % B^m */
		int long lm[] = { 2 * m - fxs, 0 };
		char const* ep[] = { fx, fx + fxs - m };
		if ( fxs > m )
			mutate_addition( hx.get<char>(), m + 1, ep, lm, NULL, false, false );
		else
			memcpy( hx.get<char>() + hx.size() - fxs, fx, fxs );
		/* hy */
		lm[ 0 ] = 2 * m - fys;
		ep[ 0 ] = fy;
		ep[ 1 ] = fy + fys - m;
		if ( fys > m )
			mutate_addition( hy.get<char>(), m + 1, ep, lm, NULL, false, false );
		else
			memcpy( hy.get<char>() + hy.size() - fys, fy, fys );
		/* find Z */
		HChunk Z;
		karatsuba( Z, hx.get<char>(), hx.size(), hy.get<char>(), hy.size() );
		/* combine all results */
		
		HChunk tmpres( fxs + fys + 1 );
		int long size( tmpres.size() );
		char* res( tmpres.get<char>() );

		/* res = Z*B^m + r */
		int long s( Z.size() );
		char const* p( Z.get<char>() );
		shift = 0;
		while ( ( shift < s ) && ! p[ shift ] )
			++ shift;
		s -= shift;
		p += shift;
		ep[ 0 ] = r.get<char>();
		ep[ 1 ] = p;
		lm[ 0 ] = s - r.size() + m;
		lm[ 1 ] = 0;
		if ( ep[ 0 ] ) {
			::memcpy( res + size - r.size(), r.raw(), r.size() );
			mutate_addition( res + size - m - s - 1, s + 1, ep, lm, NULL, false, false );
		} else
			::memcpy( res + size - m - s, p, s );

		/* res += r2m*B^2m */
		s = r2m.size();
		shift = 0;
		p = r2m.get<char>();
		lm[ 0 ] = 0;
		if ( p ) {
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
		ep[ 1 ] = r2m.get<char>();
		if ( ep[ 1 ] ) {
			lm[ 1 ] = size - r2m.size() - m - ncar;
			mutate_addition( res - car, size - m + car, ep, lm, NULL, true, false );
		}
		ep[ 1 ] = r.get<char>();
		if ( ep[ 1 ] ) {
			lm[ 1 ] = size - r.size() - m - ncar;
			mutate_addition( res - car, size - m + car, ep, lm, NULL, true, false );
		}
		::memcpy( result.get<char>() + totalShift, tmpres.get<char>() + 1, result.size() - totalShift );
	} else if ( ( fxrl > 0 ) && ( fyrl > 0 ) ) {
/* variables for mutate_addition() */
		if ( ! result.size() )
			result.realloc( fxs + fys, HChunk::STRATEGY::EXACT );
		HChunk element( fys + 1 );
		char* e( element.get<char>() );
		char* res( ( result.get<char>() + fxs ) - 2 ); /* - 1 for carrier */
		char const* ep[] = { res, e };
/* the end of variables for mutate_addition() */
		while ( -- fxs >= 0 ) {
			if ( fx[ fxs ] ) {
				int carrier = 0;
				int long inner = fys; /* index of last digit in second factor */
				while ( -- inner >= 0 ) {
					int long pos = inner + 1;
					int x = fx[ fxs ] * fy[ inner ] + carrier;
					if ( x > 9 ) {
						carrier = x / 10;
						x %= 10;
					} else
						carrier = 0;
					e[ pos ] = static_cast<char>( x );
				}
				e[ 0 ] = static_cast<char>( carrier );
				ep[ 0 ] = res + 1;
				mutate_addition( res, fys + 1 + 1, ep, NULL, NULL, false, false );
				-- res;
			} else
				-- res;
		}
	}
	return;
}

}

}

