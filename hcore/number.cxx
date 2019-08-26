/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
#include "hhashcontainer.hxx"
#include "hsingleton.hxx"

using namespace yaal::hcore;
using namespace yaal::math;
using namespace yaal::hcore::number;

namespace yaal {

namespace hcore {

namespace number {

HString const VALID_CHARACTERS( "-.0123456789" );
HString const DIGITS_AND_DOT( VALID_CHARACTERS.substr( A_DOT ) );
HString const DIGITS( VALID_CHARACTERS.substr( A_ZERO ) );

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
		do {
			if ( value_._negative ) {
				throw HNumberException( "square root from negative" );
			}
			/*
			 * Prepare first guess.
			 * 1. For argument greater than 1 count number of decimal digits.
			 *    For argument lower then 1 count number of leading zeros in fractional decimal digits part.
			 * 2. Take this number and divide it by 2 to get starting guess digit count.
			 * 3. Significant digit (leading or trailing) is decided by oddness of number found in step 1.
			 */
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
				s.fillz( '0'_ycp, 0, digits );
				s.set_at( 0, odd ? '2'_ycp : '6'_ycp );
			} else {
				s.fillz( '0'_ycp, 0, digits + 1 );
				s.set_at( 0, '.'_ycp );
				s.set_at( digits, odd ? '2'_ycp : '6'_ycp );
			}
			/* Newton's method of finding square root. */
			n = s;
			HNumber::integer_t precision( max( n.fractional_decimal_digits() + 1, value_.fractional_decimal_digits(), HARDCODED_MINIMUM_PRECISION ) );
			HNumber tmp;
			while ( ! _isKilled_ ) {
				tmp = value_;
				n.set_precision( precision );
				tmp.set_precision( precision );
				n._precision = tmp._precision = precision + 1;
				tmp /= n;
				n += tmp;
				n.set_precision( precision + 1 );
				n._precision = precision + 1;
				n *= number::N0_5;
				HNumber::integer_t converged( differs_at( n, tmp ) );
				HNumber::integer_t fdd( max( n.fractional_decimal_digits(), tmp.fractional_decimal_digits() ) - 1 );
				if ( converged >= fdd ) {
					if ( precision >= value_.get_precision() ) {
						n.round( converged );
						break;
					}
					precision = min( precision * 3, value_.get_precision() + 1 );
				}
			}
		} while ( false );
		n.set_precision( value_.get_precision() );
		return ( n );
		M_EPILOG
	}
	static yaal::hcore::HNumber natural_exponential( yaal::hcore::HNumber const& value_ ) {
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
		for ( HNumber i( number::N2, precision ); true; ++ i ) {
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
			e = v;
			v = number::N1;
			v.set_precision( e.get_precision() );
			v /= e;
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
		for ( HNumber i( number::N3, precision ); true; i += number::N2 ) {
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
		if ( v > number::N1 ) {
			v = number::N1;
		} else if ( v < number::N_1 ) {
			v = number::N_1;
		}
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
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
	/*
	 * arcsin(x) power series converges extremely slowly near 1.
	 * we will use arctan(x) to get result faster.
	 * arcsin(x) = arctan(x/sqrt(1-x^2));
	 * for 1 and -1 we return values from definition
	 */
	static yaal::hcore::HNumber arcus_sinus( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		if ( ( value_ > number::N1 ) || ( value_ < number::N_1 ) ) {
			throw HNumberException( "argument not in arcus sinus domain" );
		}
		HNumber v;
		if ( ( value_ < number::N1 ) && ( value_ > number::N_1 ) ) {
			HNumber input( value_, value_.get_precision() + 9 );
			HNumber d( square_root( number::N1 - ( input ^ 2 ) ) );
			v = arcus_tangens( input / d );
		} else {
			v = number::PI( value_.get_precision() );
			v *= number::N0_5;
			v._negative = value_._negative;
		}

		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber arcus_cosinus( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		if ( ( value_ > number::N1 ) || ( value_ < number::N_1 ) ) {
			throw HNumberException( "argument not in arcus cosinus domain" );
		}
		HNumber v;
		if ( value_ != number::N0 ) {
			HNumber input( value_, value_.get_precision() + 9 );
			HNumber d( square_root( number::N1 - ( input ^ 2 ) ) );
			v = arcus_tangens( d / input );
			if ( value_._negative ) {
				v += number::PI( value_.get_precision() );
			}
		} else {
			v = number::PI( value_.get_precision() );
			v *= number::N0_5;
		}

		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
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
	static yaal::hcore::HNumber arcus_cotangens( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber v( number::PI( value_.get_precision() ) );
		v *= number::N0_5;
		HNumber input( value_, value_.get_precision() + 1 );
		v -= arcus_tangens( input );
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber hyperbolic_sinus( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber v( value_, value_.get_precision() + 1 );
		v = natural_exponential( v ) - natural_exponential( -v );
		v /= number::N2;
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber hyperbolic_cosinus( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber v( value_, value_.get_precision() + 1 );
		v = natural_exponential( v ) + natural_exponential( -v );
		v /= number::N2;
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber hyperbolic_tangens( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber v( value_, value_.get_precision() + 10 );
		HNumber p( natural_exponential( v ) );
		HNumber n( natural_exponential( -v ) );
		v = p - n;
		v /= ( p + n );
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber hyperbolic_cotangens( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		HNumber v( value_, value_.get_precision() + 10 );
		HNumber p( natural_exponential( v ) );
		HNumber n( natural_exponential( -v ) );
		v = p + n;
		v /= ( p - n );
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber sigmoid( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		static HNumber const CUTOFF( 11357 * 2 );
		HNumber v( value_, value_.get_precision() + 1 );
		if ( v.absolute_lower( CUTOFF ) < 0 ) {
			v /= number::N2;
			v = hyperbolic_tangens( v );
			++ v;
			v /= number::N2;
			if ( v > number::N1 ) {
				v = number::N1;
			} else if ( v < number::N_1 ) {
				v = number::N_1;
			}
		} else if ( v > number::N0 ) {
			v = number::N1;
		} else {
			v = number::N_1;
		}
		v.round( value_.get_precision() );
		v.set_precision( value_.get_precision() );
		return ( v );
		M_EPILOG
	}
	static yaal::hcore::HNumber error_function( yaal::hcore::HNumber const& value_ ) {
		M_PROLOG
		return ( math::error_function( value_.to_floating_point() ) );
		M_EPILOG
	}
	static hash_value_t hash( HNumber const& value_ ) {
		hash_value_t h( 0 );
		i32_t const* src( value_._canonical.get<i32_t>() );
		for ( int i( 0 ); i < value_._leafCount; ++ i ) {
#if SIZEOF_INT_LONG == 8
			h = h ^ ( static_cast<hash_value_t>( src[i] ) << ( ( i % 2 ) ? 32 : 0 ) );
#else
			h = h ^ static_cast<hash_value_t>( src[i] );
#endif
		}
		h = h ^ static_cast<hash_value_t>( value_._precision );
		h = h ^ static_cast<hash_value_t>( value_._leafCount );
		h = h ^ static_cast<hash_value_t>( value_._integralPartSize );
		h += ( value_._negative ? 1 : 0 );
		return ( h );
	}
};

template<>
hash<HNumber>::hash_value_type hash<HNumber>::operator () ( HNumber const& number_ ) const {
	return ( HNumber::ElementaryFunctions::hash( number_ ) );
}

namespace number {

class HFactorialCache : public yaal::hcore::HSingleton<HFactorialCache> {
public:
	typedef HFactorialCache this_type;
	typedef HResource<HNumber> number_t;
	typedef HArray<number_t> consecutive_t;
	typedef HMap<int long long, number_t> isolated_t;
	static int const FORCED_CONSECUTIVE_SIZE = 4096;
	static int const CONSECUTIVE_STRETCH_SIZE = 128;
	static int const DIVIDE_AND_CONQUER_CUT_OFF = 32;
private:
	consecutive_t _consecutive;
	isolated_t _isolated;
	HMutex _mutex;
public:
	static HNumber mul_range( int long long from_, int long long to_ ) {
		M_PROLOG
		HNumber n;
		if ( ( to_ - from_ ) >= DIVIDE_AND_CONQUER_CUT_OFF ) {
			int long long mid( ( from_ + to_ ) / 2 );
			n = mul_range( from_, mid ) * mul_range( mid + 1, to_ );
		} else {
			for ( n = from_, ++ from_; from_ <= to_; ++ from_ ) {
				n *= from_;
			}
		}
		return ( n );
		M_EPILOG
	}
	yaal::hcore::HNumber const& factorial( int long long value_ ) {
		M_ENSURE( value_ >= 0 );
		HNumber const* res( nullptr );
		HLock l( _mutex );
		if ( value_ >= _consecutive.get_size() ) {
			if ( ( value_ <= FORCED_CONSECUTIVE_SIZE ) || ( value_ <= ( _consecutive.get_size() + CONSECUTIVE_STRETCH_SIZE ) ) ) {
				for ( int long long i( _consecutive.get_size() ); i <= value_; ++ i ) {
					number_t n( make_resource<HNumber>( *_consecutive.back() ) );
					*n *=i;
					_consecutive.push_back( yaal::move( n ) );
				}
				res = _consecutive[static_cast<int long>( value_ )].raw();
			} else {
				int long long baseArg( 0 );
				HNumber const* baseVal( nullptr );
				isolated_t::iterator baseIt( _isolated.lower_bound( value_ ) );
				if ( baseIt == _isolated.end() ) {
					if ( ! _isolated.is_empty() ) {
						baseArg = _isolated.rbegin()->first;
						baseVal = _isolated.rbegin()->second.raw();
					} else {
						baseArg = _consecutive.get_size() - 1;
						baseVal = _consecutive.back().raw();
					}
				} else if ( baseIt->first != value_ ) {
					if ( baseIt != _isolated.begin() ) {
						-- baseIt;
						baseArg = baseIt->first;
						baseVal = baseIt->second.raw();
					} else {
						baseArg = _consecutive.get_size() - 1;
						baseVal = _consecutive.back().raw();
					}
				} else {
					baseArg = baseIt->first;
					baseVal = baseIt->second.raw();
				}
				M_ASSERT( baseArg <= value_ );
				if ( baseArg == value_ ) {
					res = baseVal;
				} else {
					res = _isolated.insert( make_pair( value_, make_resource<HNumber>( *baseVal * mul_range( baseArg + 1LL, value_ ) ) ) ).first->second.raw();
				}
			}
		} else {
			res = _consecutive[static_cast<int long>( value_ )].raw();
		}
		return ( *res );
	}
private:
	HFactorialCache( void )
		: _consecutive()
		, _isolated()
		, _mutex() {
		_consecutive.push_back( make_resource<HNumber>( N1 ) );
		_consecutive.push_back( make_resource<HNumber>( N1 ) );
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
yaal::hcore::HNumber const N_1( -1 );
namespace {
yaal::hcore::HNumber find_pi( yaal::hcore::HNumber::integer_t precision_ ) {
	static HNumber const a( 545140134_yn );
	static HNumber const b( 13591409_yn );
	static HNumber const c( 640320_yn ^ 3 );
	static HNumber const k( 10368 );

	bool plus( true );

	HNumber va;
	HNumber vb;
	HNumber e( number::N1 );
	HNumber denominator( number::N1 );
	HNumber a1( number::N0 );
	HNumber k1( 120 );
	HNumber k2( 5424 );
	HNumber k3( 15552 );

	HNumber::integer_t extraPrec( precision_ / 125 );
	for ( HNumber i( number::N0 ); true; ++ i ) {
		e.set_precision( precision_ + extraPrec + 6 );
		denominator.set_precision( precision_ + extraPrec + 6 );
		e /= denominator;
		if ( e == number::N0 ) {
			break;
		}
		if ( plus ) {
			va += ( e * i );
			vb += e;
		} else {
			va -= ( e * i );
			vb -= e;
		}
		e *= k1;
		denominator = ( c * ( ( i + 1 ) ^ 3 ) );
		k1 += k2;
		k2 += k3;
		k3 += k;
		a1 += a;
		plus = !plus;
	}
	e = va * a + vb * b;
	e *= square_root( HNumber( 10005, precision_ + 6 ) );
	HNumber v( 4270934400_yn );
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
			return ( natural_exponential( yaal::hcore::HNumber( 1, precision_ ) ) );
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

inline yaal::hcore::HNumber greatest_common_divisor( yaal::hcore::HNumber&& a_, yaal::hcore::HNumber&& b_ ) {
	a_.abs();
	b_.abs();
	while ( a_ && b_ ) {
		if ( a_ > b_ ) {
			a_ %= b_;
		} else {
			b_ %= a_;
		}
	}
	return ( a_ ? a_ : b_ );
}

}

}

namespace math {

yaal::hcore::HNumber square_root( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::square_root( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber natural_exponential( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::natural_exponential( value_ ) );
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

yaal::hcore::HNumber arcus_sinus( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::arcus_sinus( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber arcus_cosinus( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::arcus_cosinus( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber arcus_tangens( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::arcus_tangens( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber arcus_cotangens( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::arcus_cotangens( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber hyperbolic_sinus( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::hyperbolic_sinus( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber hyperbolic_cosinus( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::hyperbolic_cosinus( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber hyperbolic_tangens( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::hyperbolic_tangens( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber hyperbolic_cotangens( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::hyperbolic_cotangens( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber sigmoid( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::sigmoid( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber error_function( yaal::hcore::HNumber const& value_ ) {
	M_PROLOG
	return ( yaal::hcore::HNumber::ElementaryFunctions::error_function( value_ ) );
	M_EPILOG
}

yaal::hcore::HNumber greatest_common_divisor( yaal::hcore::HNumber a_, yaal::hcore::HNumber b_ ) {
	M_PROLOG
	return ( yaal::hcore::number::greatest_common_divisor( yaal::move( a_ ), yaal::move( b_ ) ) );
	M_EPILOG
}

}

template<>
int long long lexical_cast( HNumber const& val_ ) {
	return ( val_.to_integer() );
}

}

