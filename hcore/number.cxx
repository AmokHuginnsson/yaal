/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  number.cxx - this file is integral part of `yaal' project.

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
#include "number.hxx"
#include "hnumber.hxx"
#include "math.hxx"
#include "hlist.hxx"
#include "hthread.hxx"
#include "hsingleton.hxx"

using namespace yaal::hcore;
using namespace yaal::math;
using namespace yaal::hcore::number;

namespace yaal {

namespace hcore {

namespace number {

char const VALID_CHARACTERS[] = "-.0123456789";
i32_t const DECIMAL_SHIFT[] = {
	/* 0 */ 1l,
	/* 1 */ 10l,
	/* 2 */ 100l,
	/* 3 */ 1000l,
	/* 4 */ 10000l,
	/* 5 */ 100000l,
	/* 6 */ 1000000l,
	/* 7 */ 10000000l,
	/* 8 */ 100000000l,
	/* 9 */ 1000000000l
};

yaal::hcore::HNumber const& ArcTan0_2( yaal::hcore::HNumber::integer_t = 0 );

}

struct HNumber::ElementaryFunctions {
	static yaal::hcore::HNumber square_root( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber n;
		HNumber tmp;
		HNumber oldN;
		HNumber::integer_t requiredPrecision( value_.get_precision() );
		do {
			if ( value_._negative ) {
				throw HNumberException( "square root from negative" );
			}
			HString s( value_.to_string() );
			integer_t digits( 0 );
			bool aboveOne( value_ >= number::N1 );
			if ( aboveOne ) {
				digits = static_cast<integer_t>( s.find( VALID_CHARACTERS[A_DOT] ) );
				if ( digits == HString::npos ) {
					digits = static_cast<integer_t>( s.get_length() );
				}
			} else {
				static int const SKIP_ZERO_DOT( 2 );
				digits = static_cast<integer_t>( s.find_other_than( "0", SKIP_ZERO_DOT ) ); /* SKIP_ZERO_DOT - skip "0." at the beginning */
				if ( digits == HString::npos ) {
					n = number::N0;
					break;
				}
				digits -= SKIP_ZERO_DOT;
			}
			bool odd( digits & 1l );
			if ( odd ) {
				-- digits;
			} else if ( digits > 0 ) {
				digits -= 2;
			}
			digits >>= 1;
			++ digits;
			if ( aboveOne ) {
				s.fillz( '0', 0, digits );
				s.set_at( 0, odd ? '2' : '6' );
			} else {
				s.fillz( '0', 0, digits + 1 );
				s.set_at( 0, '.' );
				s.set_at( digits, odd ? '2' : '6' );
			}
			n = s;
			HNumber::integer_t precision( n.fractional_decimal_digits() + 1 );
			while ( true ) {
				tmp = value_;
				n.set_precision( precision );
				tmp.set_precision( precision );
				if ( precision >= HARDCODED_MINIMUM_PRECISION ) {
					n._precision = precision + 1;
					tmp._precision = precision + 1;
				} else {
					n._precision = HARDCODED_MINIMUM_PRECISION + 1;
					tmp._precision = HARDCODED_MINIMUM_PRECISION + 1;
				}
				tmp /= n;
				n += tmp;
				n *= number::N0_5;
				if ( n.is_exact() && ( ( n * n ) == value_ ) ) {
					break;
				}
				if ( n._integralPartSize == oldN._integralPartSize ) {
					i32_t const* a( n._canonical.get<i32_t const>() );
					i32_t const* b( oldN._canonical.get<i32_t const>() );
					HNumber::integer_t d( 0 );
					HNumber::integer_t SIZE( min( n._leafCount, oldN._leafCount ) );
					for ( ; d < SIZE; ++ d ) {
						if ( a[d] != b[d] ) {
							break;
						}
					}
					HNumber::integer_t sameFractionalLeafs( d - n._integralPartSize );
					if ( sameFractionalLeafs > 0 ) {
						HNumber::integer_t newPrecision( sameFractionalLeafs * DECIMAL_DIGITS_IN_LEAF_CONST );
						if ( d < SIZE ) {
							i32_t diff( yaal::abs( a[d] - b[d] ) );
							for ( int i( 1 ); i < DECIMAL_DIGITS_IN_LEAF_CONST; ++ i ) {
								if ( ( diff % DECIMAL_SHIFT[DECIMAL_DIGITS_IN_LEAF_CONST - i] ) != diff ) {
									break;
								}
								++ newPrecision;
							}
						}
						if ( newPrecision >= requiredPrecision ) {
							break;
						}
						newPrecision = ( newPrecision << 2 ) + ( DECIMAL_DIGITS_IN_LEAF_CONST << 1 ) - 1;
						if ( newPrecision < HARDCODED_MINIMUM_PRECISION ) {
							newPrecision = HARDCODED_MINIMUM_PRECISION;
						}
						if ( newPrecision > precision ) {
							precision = newPrecision;
						}
					}
				}
				oldN = n;
			}
		} while ( false );
		n.set_precision( requiredPrecision );
		return ( n );
		M_EPILOG
	}
	static yaal::hcore::HNumber natural_expotential( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		integer_t precision( value_.get_precision() * 2 );
		HNumber input( value_, precision );
		bool minus( false );
		/* Check the sign of input. */
		if ( input < number::N0 ) {
			minus = true;
			input = -input;
		}

		/* Precondition input. */
		int long log2( 0 );
		while ( input > number::N1 ) {
			++ log2;
			input *= number::N0_5;
		}

		/* Initialize the variables. */
		HNumber v( input, precision );
		++ v;
		HNumber a( input, precision );
		HNumber d( number::N1, precision );

		HNumber e;
		for ( HNumber i( number::N2 ); true; ++ i ) {
			e = ( a *= input ) / ( d *= i );
			a.set_precision( precision );
			d.set_precision( precision );
			if ( e == number::N0 ) {
				while ( log2 > 0 ) {
					v = v * v;
					-- log2;
				}
				break;
			}
			v += e;
		}
		if ( minus ) {
			v = number::N1 / v;
		}
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber natural_logarithm( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		if ( value_ <= number::N0 ) {
			throw HNumberException( "natural logarithm from non-positive" );
		}
		integer_t precision( value_.get_precision() * 2 );

		HNumber f( number::N2 );
		HNumber input( value_, precision );
		while ( input >= number::N2 ) {  /* for large numbers */
			f *= number::N2;
			input = square_root( input );
		}
		while ( input <= number::N0_5 ) {  /* for small numbers */
			f *= number::N2;
			input = square_root( input );
		}

		/* Set up the loop. */
		HNumber n( ( input - number::N1 ) / ( input + number::N1 ), precision );
		HNumber v( n );
		HNumber m( n * n, precision );

		/* Sum the series. */
		HNumber e;
		for ( HNumber i( number::N3 ); true; i += number::N2 ) {
			e = ( n *= m ) / i;
			n.set_precision( precision );
			if ( e == number::N0 ) {
				v *= f;
				break;
			}
			v += e;
		}
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber sinus( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		integer_t precision( value_.get_precision() * 2 );
		HNumber input( value_, precision );

		bool minus( false );
		/* Check the sign of input. */
		if ( input < number::N0 ) {
			minus = true;
			input = -input;
		}

		HNumber const& pi( number::PI( precision ) );
		HNumber n( ( input / pi ) + number::N0_5 );
		n.floor();
		input -= ( n * pi );
		if ( n.to_integer() % 2 ) {
			input = -input;
		}

		/* Do the loop. */
		HNumber e( input, precision );
		HNumber v( input );
		HNumber s( -input * input, precision );
		HNumber tmp;
		for ( HNumber i( number::N2 ); true; ++ i ) {
			tmp = i;
			++ i;
			e *= s;
			e.set_precision( precision );
			e /= ( tmp * i );
			e.set_precision( precision );
			if ( e == number::N0 ) {
				break;
			}
			v += e;
		}
		if ( minus ) {
			v = -v;
		}
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		if ( v > number::N1 ) {
			v = number::N1;
		} else if ( v < -number::N1 ) {
			v = -number::N1;
		}
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber cosinus( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber value( value_ + number::PI( value_.get_precision() * 2 ) * number::N0_5, value_.get_precision() );
		return ( sinus( value ) );
		M_EPILOG
	}
	static yaal::hcore::HNumber tangens( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber value( value_, value_.get_precision() + 6 );
		HNumber denominator( cosinus( value ) );
		if ( denominator == number::N0 ) {
			throw HNumberException( "argument not in tangens domain" );
		}
		value = sinus( value );
		value /= denominator;
		value.round( value_.get_precision() );
		value.set_precision( value_.get_precision() );
		return ( value );
		M_EPILOG
	}
	static yaal::hcore::HNumber cotangens( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber value( value_, value_.get_precision() + 6 );
		HNumber denominator( sinus( value ) );
		if ( denominator == number::N0 ) {
			throw HNumberException( "argument not in cotangens domain" );
		}
		value = cosinus( value );
		value /= denominator;
		value.round( value_.get_precision() );
		value.set_precision( value_.get_precision() );
		return ( value );
		M_EPILOG
	}
	static yaal::hcore::HNumber arcus_tangens( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		integer_t precision( value_.get_precision() * 2 );
		HNumber input( value_, precision );

		bool minus( false );
		/* Check the sign of input. */
		if ( input < number::N0 ) {
			minus = true;
			input = -input;
		}

		static HNumber const c( "0.2", precision );
		HNumber a;
		HNumber f;
		if ( input > c ) {
			a = ArcTan0_2( precision );
			while ( input > c ) {
				++ f;
				input = ( input - c ) / ( number::N1 + input * c );
			}
		}

		HNumber v( input );
		HNumber s( -input * input );
		s.set_precision( precision );
		HNumber e;
		for ( HNumber i( number::N3, precision ); true; i += 2 ) {
			e = ( input *= s ) / i;
			input.set_precision( precision );
			if ( e == number::N0 ) {
				v += ( f * a );
				break;
			}
			v += e;
		}

		if ( minus ) {
			v = -v;
		}
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
};

namespace number {

class HFactorialCache : public yaal::hcore::HSingleton<HFactorialCache> {
public:
	typedef HFactorialCache this_type;
	typedef HResource<HNumber> number_t;
	typedef HArray<number_t> cache_t;
private:
	cache_t _cache;
	HMutex _mutex;
public:
	yaal::hcore::HNumber const& factorial( int long long value_ ) {
		M_ENSURE( value_ >= 0 );
		HLock l( _mutex );
		if ( value_ >= _cache.get_size() ) {
			for ( int long long i( _cache.get_size() ); i <= value_; ++ i ) {
				number_t n( make_resource<HNumber>( *_cache.back() ) );
				*n *=i;
				_cache.push_back( yaal::move( n ) );
			}
		}
		return ( *_cache[value_] );
	}
private:
	HFactorialCache( void )
		: _cache()
		, _mutex() {
		_cache.push_back( make_resource<HNumber>( N1 ) );
		_cache.push_back( make_resource<HNumber>( N1 ) );
		return;
	}
	virtual ~HFactorialCache( void ) {
		return;
	}
	HFactorialCache( HFactorialCache const& ) = delete;
	HFactorialCache& operator = ( HFactorialCache const& ) = delete;
	friend class yaal::hcore::HSingleton<HFactorialCache>;
	friend class yaal::hcore::HDestructor<HFactorialCache>;
};

class HNumericConstantCache {
public:
	typedef HNumericConstantCache this_type;
	typedef yaal::hcore::HList<HNumber> approximation_log_t;
	typedef std::atomic<HNumber*> best_approximation_t;
	typedef yaal::hcore::HBoundCall<yaal::hcore::HNumber ( yaal::hcore::HNumber::integer_t )> approximator_t;
private:
	approximation_log_t _approximationLog;
	best_approximation_t _bestApproximation;
	approximator_t _approximator;
	yaal::hcore::HMutex _mutex;
public:
	HNumericConstantCache( approximator_t const& approximator_, HNumber&& init_ )
		: _approximationLog()
		, _bestApproximation( nullptr )
		, _approximator( approximator_ )
		, _mutex() {
		_approximationLog.push_back( yaal::move( init_ ) );
		_bestApproximation = &_approximationLog.back();
	}
	HNumber const& approximation( yaal::hcore::HNumber::integer_t precision_ ) {
		HNumber* bestApproximation( _bestApproximation.load() );
		if ( bestApproximation->get_precision() < precision_ ) {
			HLock l( _mutex );
			bestApproximation = _bestApproximation.load();
			if ( bestApproximation->get_precision() < precision_ ) {
				_approximationLog.push_back( _approximator( precision_ ) );
				_bestApproximation.store( &_approximationLog.back() );
				bestApproximation = _bestApproximation.load();
				M_ASSERT( bestApproximation->get_precision() >= precision_ );
			}
		}
		return ( *bestApproximation );
	}
};

yaal::hcore::HNumber const N0( 0 );
yaal::hcore::HNumber const N1( 1 );
yaal::hcore::HNumber const N2( 2 );
yaal::hcore::HNumber const N3( 3 );
yaal::hcore::HNumber const N4( 4 );
yaal::hcore::HNumber const N5( 5 );
yaal::hcore::HNumber const N6( 6 );
yaal::hcore::HNumber const N0_5( "0.5" );
namespace {
yaal::hcore::HNumber find_pi( yaal::hcore::HNumber::integer_t precision_ ) {
	static HNumber a( 545140134_yn );
	static HNumber b( 13591409_yn );
	static HNumber c( 640320_yn ^ 3 );

	bool plus( true );

	HNumber v;
	HNumber e;
	HNumber denominator;

	for ( int long long i( 0 ); true; ++ i ) {
		e = factorial( 6LL * i ) * ( a * i + b );
		e.set_precision( precision_ + 6 );
		denominator = factorial( 3LL * i ) * ( factorial( i ) ^ 3 ) * ( c ^ i );
		e /= denominator;
		if ( e == number::N0 ) {
			break;
		}
		if ( plus ) {
			v += e;
		} else {
			v -= e;
		}
		plus = !plus;
	}
	e = v;
	e *= square_root( HNumber( 10005_yn, precision_ + 6 ) );
	v = 4270934400_yn;
	v.set_precision( precision_ );
	v /= e;
	v.round( precision_ );
	v.set_precision( precision_ );
	return ( v );
}
}
HNumericConstantCache PiCache(
	call( &find_pi, _1 ),
	HNumber( "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384461" )
);
yaal::hcore::HNumber const& PI( yaal::hcore::HNumber::integer_t precision_ ) {
	return ( PiCache.approximation( precision_ ) );
}
HNumericConstantCache ECache(
	HNumericConstantCache::approximator_t(
		[]( yaal::hcore::HNumber::integer_t precision_ ) -> yaal::hcore::HNumber {
			return ( natural_expotential( yaal::hcore::HNumber( 1, precision_ ) ) );
		}
	),
	HNumber( "2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174136" )
);
yaal::hcore::HNumber const& E( yaal::hcore::HNumber::integer_t precision_ ) {
	return ( ECache.approximation( precision_ ) );
}
HNumericConstantCache ArcTan0_2Cache(
	HNumericConstantCache::approximator_t(
		[]( yaal::hcore::HNumber::integer_t precision_ ) -> yaal::hcore::HNumber {
			return ( arcus_tangens( yaal::hcore::HNumber( "0.2", precision_ ) ) );
		}
	),
	HNumber( "0.19739555984988075837004976519479029344758510378785210151768894024103396997824378573269782803728804411262811807369136010445647989" )
);
yaal::hcore::HNumber const& ArcTan0_2( yaal::hcore::HNumber::integer_t precision_ ) {
	return ( ArcTan0_2Cache.approximation( precision_ ) );
}

yaal::hcore::HNumber const& factorial( int long long value_ ) {
	M_PROLOG
	return ( HFactorialCache::get_instance().factorial( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber binomial_coefficient( int long long cardinal_, int long long subCardinal_ ) {
	if ( subCardinal_ > ( cardinal_ / 2 ) ) {
		subCardinal_ = cardinal_ - subCardinal_;
	}
	return ( factorial( cardinal_ ) / ( factorial( subCardinal_ ) * factorial( cardinal_ - subCardinal_ ) ) );
}

}

}

namespace math {

yaal::hcore::HNumber square_root( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::square_root( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber natural_expotential( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::natural_expotential( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber natural_logarithm( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::natural_logarithm( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber sinus( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::sinus( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber cosinus( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::cosinus( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber tangens( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::tangens( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber cotangens( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::cotangens( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber arcus_tangens( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::arcus_tangens( value_ ) );
	M_EPILOG
}

}

}
