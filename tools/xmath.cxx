/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "xmath.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace xmath {

namespace {
aggregate_type_t const NONE = aggregate_type_t::new_flag();
}

aggregate_type_t const AGGREGATE_TYPE::COUNT = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::MINIMUM = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::MAXIMUM = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::SUM = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::ARITHMETIC_MEAN = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::SAMPLE_VARIANCE = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::POPULATION_VARIANCE = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::SAMPLE_STANDARD_DEVIATION = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::BASIC = AGGREGATE_TYPE::COUNT
	| AGGREGATE_TYPE::MINIMUM
	| AGGREGATE_TYPE::MAXIMUM
	| AGGREGATE_TYPE::SUM
	| AGGREGATE_TYPE::ARITHMETIC_MEAN
	| AGGREGATE_TYPE::SAMPLE_VARIANCE
	| AGGREGATE_TYPE::POPULATION_VARIANCE
	| AGGREGATE_TYPE::SAMPLE_STANDARD_DEVIATION
	| AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION;
aggregate_type_t const AGGREGATE_TYPE::MEDIAN = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::INTERQUARTILE_RANGE = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::HISTOGRAM = aggregate_type_t::new_flag();

int long long AGGREGATE_TYPE::required_data_points( aggregate_type_t const& aggregateType_ ) {
	int long long rdp( 0 );
	if ( aggregateType_ & SAMPLE_STANDARD_DEVIATION ) {
		rdp = max( rdp, 1LL );
	}
	if ( aggregateType_ & SAMPLE_VARIANCE ) {
		rdp = max( rdp, 1LL );
	}
	return ( rdp );
}

int long long HModularMultiplicativeInverse::inverse( void ) const {
	if ( _greatestCommonDivisor != 1 ) {
		throw HInvalidArgumentException( "Given value does not have a modular multiplicative inverse with respect to specified modulus." );
	}
	return ( _inverse );
}

HModularMultiplicativeInverse modular_multiplicative_inverse( int long long modulo_, int long long val_ ) {
	M_ASSERT( modulo_ > 0 );
	M_ASSERT( val_ > 0 );
	M_ASSERT( modulo_ > val_ );
	int long long s( 0 );
	int long long os( 1 );
	int long long t( 1 );
	int long long ot( 0 );
	int long long r( val_ );
	int long long OR( modulo_ );
	while ( r > 0 ) {
		int long long q = OR / r;
		using yaal::swap;
		swap( s, os );
		s -= q * os;
		swap( t, ot );
		t -= q * ot;
		swap( r, OR );
		r -= q * OR;
	}
	return ( HModularMultiplicativeInverse( OR, ot >= 0 ? ot : ot + modulo_ ) );
}

int long long modular_multiplication( int long long a_, int long long b_, int long long mod_ ) {
	M_ASSERT( mod_ > 0LL );
	M_ASSERT( a_ >= 0LL );
	M_ASSERT( b_ >= 0LL );
	int long long unsigned r( 0ULL );
	int long long unsigned mod( static_cast<int long long unsigned>( mod_ ) );
	int long long unsigned ta( static_cast<int long long unsigned>( a_ ) );
	int long long unsigned tb( static_cast<int long long unsigned>( b_ ) );
	tb = tb % mod;
	ta = ta % mod;
	if ( ( tb > 0 ) && ( ta > ( meta::max_unsigned<int long long unsigned>::value / tb ) ) ) {
		while ( tb > 0ULL ) {
			if ( ( tb % 2ULL ) == 1ULL ) {
				r += ta;
				r %= mod;
			}
			ta *= 2ULL;
			ta %= mod;
			tb /= 2ULL;
		}
	} else {
		r = ta * tb;
		r %= mod;
	}
	return ( static_cast<int long long>( r ) );
}

}

}

}

