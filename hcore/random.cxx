/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <sys/time.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "random.hxx"
#include "hexception.hxx"
#include "math.hxx"
#include "algorithm.hxx"
#include "system.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace random {

namespace {
	static int const MM = HRandomNumberGenerator::STATE_SIZE / 2;
	static u64_t const MATRIX_A = 0xB5026F5AA96619E9ULL;
	static u64_t const UM = 0xFFFFFFFF80000000ULL; /* Most significant 33 bits */
	static u64_t const LM = 0x7FFFFFFFULL; /* Least significant 31 bits */
}

class HSimpleRandom {
public:
	typedef HSimpleRandom this_type;
private:
	u32_t _z;
	u32_t _w;
	u32_t _jsr;
	u32_t _jcong;
public:
	HSimpleRandom( u32_t seed_ )
		: _z( seed_ ^ 362436069 )
		, _w( seed_ ^ 521288629 )
		, _jsr( seed_ ^ 123456789 )
		, _jcong( seed_ ^ 380116160 ) {
	}
	u32_t operator()( void ) {
		return ( ( mwc() ^ cong() ) + shr3() );
	}
private:
	u32_t cong( void ) {
		return ( _jcong = 69069 * _jcong + 1234567 );
	}
	u32_t shr3( void ) {
		_jsr ^= ( _jsr >> 13 );
		_jsr ^= ( _jsr << 17 );
		_jsr ^= ( _jsr << 5 );
		return ( _jsr );
	}
	u32_t znew( void ) {
		return ( _z = 36969 * ( _z & 65535 ) + ( _z >> 16 ) );
	}
	u32_t wnew( void ) {
		return ( _w = 18000 * ( _w & 65535 ) + ( _w >> 16 ) );
	}
	u32_t mwc( void ) {
		return ( ( znew() << 16 ) + wnew() );
	}
};

namespace {
u64_t entropy( void ) {
	struct timeval tv;
	M_ENSURE( gettimeofday( &tv, nullptr ) == 0 );
	system::HResourceInfo ri( system::get_memory_size_info() );
	u64_t pid( static_cast<u64_t>( system::getpid() ) );
	u64_t hb( 1ULL << 63 );
	while ( pid && ! ( pid & hb ) ) {
		pid <<= 1;
	}
	u64_t time( static_cast<u64_t>( tv.tv_sec + tv.tv_usec ) );
	int shift( 0 );
	while ( time && ! ( time & hb ) ) {
		time <<= 1;
		++ shift;
	}
	time >>= ( shift / 2 );
	u64_t mem( static_cast<u64_t>( ri.free() ) );
	return ( pid ^ time ^ mem );
}
}

HRandomNumberGenerator::HRandomNumberGenerator( void )
	: _index( STATE_SIZE + 1 )
	, _state() {
	M_PROLOG
	set_seed( entropy() );
	return;
	M_EPILOG
}

HRandomNumberGenerator::HRandomNumberGenerator( u64_t seed_ )
	: _index( STATE_SIZE + 1 )
	, _state() {
	M_PROLOG
	set_seed( seed_ );
	return;
	M_EPILOG
}

HRandomNumberGenerator::HRandomNumberGenerator( u64_t const* stateFirst_, u64_t const* stateLast_ )
	: _index( STATE_SIZE + 1 )
	, _state() {
	M_PROLOG
	M_ASSERT( stateFirst_ && stateLast_ );
	M_ASSERT( stateFirst_ != stateLast_ );
	init( stateFirst_, static_cast<int>( stateLast_ - stateFirst_ ) );
	return;
	M_EPILOG
}

void HRandomNumberGenerator::swap( HRandomNumberGenerator& randomizer_ ) {
	M_PROLOG
	if ( &randomizer_ != this ) {
		using yaal::swap;
		swap( _index, randomizer_._index );
		swap_ranges( _state, _state + yaal::size( _state ), randomizer_._state );
	}
	return;
	M_EPILOG
}

void HRandomNumberGenerator::set_seed( u64_t seed_ ) {
	M_PROLOG
	HSimpleRandom sr( static_cast<u32_t>( ( seed_ >> 32 ) ^ seed_ ) );
	u64_t state[STATE_SIZE];
	for ( int i( 0 ); i < STATE_SIZE; ++ i ) {
		u64_t hi( sr() );
		u64_t lo( sr() );
		state[i] = hi << 32 | lo;
	}
	init( state, STATE_SIZE );
	return;
	M_EPILOG
}

void HRandomNumberGenerator::init( u64_t seed_ ) {
	M_PROLOG
	_state[0] = seed_;
	for ( _index = 1; _index < STATE_SIZE; ++ _index ) {
		_state[_index] = 6364136223846793005ULL * ( _state[_index - 1] ^ ( _state[_index - 1] >> 62 ) );
		_state[_index] += static_cast<u64_t>( _index );
	}
	return;
	M_EPILOG
}

void HRandomNumberGenerator::init( u64_t const* state_, int stateSize_ ) {
	M_PROLOG
	init( 19650218ULL );
	int i( 1 );
	int j( 0 );
	int k( ( STATE_SIZE > stateSize_ ? STATE_SIZE : stateSize_ ) );
	for ( ; k; -- k ) {
		_state[i] = ( _state[i] ^ ( ( _state[i - 1] ^ ( _state[i - 1] >> 62 ) ) * 3935559000370003845ULL ) ) + state_[j];
		_state[i] += static_cast<u64_t>( j ); /* non linear */
		++ i;
		++ j;
		if ( i >= STATE_SIZE ) {
			_state[0] = _state[STATE_SIZE - 1];
			i = 1;
		}
		if ( j >= stateSize_ )
			j = 0;
	}
	for ( k = STATE_SIZE - 1; k; -- k ) {
		_state[i] = ( _state[i] ^ ( (_state[i - 1] ^ ( _state[i - 1] >> 62 ) ) * 2862933555777941757ULL ) );
		_state[i] -= static_cast<u64_t>( i ); /* non linear */
		++ i;
		if ( i >= STATE_SIZE ) {
			_state[0] = _state[STATE_SIZE - 1];
			i = 1;
		}
	}
	_state[0] = 1ULL << 63; /* MSB is 1; assuring non-zero initial array */
	return;
	M_EPILOG
}

u64_t HRandomNumberGenerator::operator()( void ) {
	M_PROLOG
	u64_t x( 0 );
	u64_t const mag01[ 2 ] = { 0ULL, MATRIX_A };
	if ( _index >= STATE_SIZE ) { /* generate STATE_SIZE words at one time */
		int i( 0 );
		for ( ; i < STATE_SIZE - MM; ++ i ) {
			x = ( _state[i] & UM ) | ( _state[ i + 1 ] & LM );
			_state[i] = _state[i + MM] ^ ( x >> 1 ) ^ mag01[static_cast<int>( x & 1ULL )];
		}
		for ( ; i < STATE_SIZE - 1; ++ i ) {
			x = ( _state[i] & UM ) | ( _state[ i + 1 ] & LM );
			_state[i] = _state[i + ( MM - STATE_SIZE )] ^ ( x >> 1 ) ^ mag01[static_cast<int>( x & 1ULL )];
		}
		x = ( _state[ STATE_SIZE - 1 ] & UM ) | ( _state[0] & LM );
		_state[ STATE_SIZE - 1 ] = _state[ MM - 1 ] ^ ( x >> 1 ) ^ mag01[static_cast<int>( x & 1ULL )];
		_index = 0;
	}

	x = _state[ _index ++ ];
	x ^= ( x >> 29 ) & 0x5555555555555555ULL;
	x ^= ( x << 17 ) & 0x71D67FFFEDA60000ULL;
	x ^= ( x << 37 ) & 0xFFF7EEE000000000ULL;
	x ^= ( x >> 43 );
	return ( x );
	M_EPILOG
}

namespace distribution {

HDistribution::HDistribution( void )
	: _rng( make_pointer<HRandomNumberGenerator>() ) {
	return;
}

void HDistribution::set_generator( generator_t const& generator_ ) {
	M_PROLOG
	_rng = generator_;
	return;
	M_EPILOG
}

HDistribution::generator_t const& HDistribution::generator( void ) const {
	M_PROLOG
	return ( _rng );
	M_EPILOG
}

HDistribution::generator_t HDistribution::generator( void ) {
	M_PROLOG
	return ( _rng );
	M_EPILOG
}

HDiscrete::HDiscrete( yaal::i64_t from_, yaal::i64_t to_ )
	: HDistribution()
	, _base( from_ )
	, _num( 0 ) {
	M_ASSERT( to_ >= from_ );
	if ( ( from_ * to_ ) >= 0 ) {
		_num = static_cast<u64_t>( to_ - from_ ) + 1;
	} else {
		_num = static_cast<u64_t>( to_ ) + static_cast<u64_t>( -from_ ) + 1;
	}
	return;
}

yaal::i64_t HDiscrete::to( void ) const {
	yaal::i64_t t( 0 );
	if ( _base > 0 ) {
		t = _base + static_cast<yaal::i64_t>( _num - 1 );
	} else if ( ( _num - 1 ) <= static_cast<u64_t>( meta::max_signed<yaal::i64_t>::value ) ) {
		t = _base + static_cast<yaal::i64_t>( _num - 1 );
	} else {
		t = static_cast<yaal::i64_t>( _num - static_cast<yaal::u64_t>( -_base ) - 1 );
	}
	return ( t );
}

yaal::i64_t HDiscrete::operator()( void ) {
	return ( static_cast<i64_t>( (*_rng)() % _num ) + _base );
}

yaal::i64_t HDiscrete::do_next_discrete( void ) {
	return ( operator()() );
}

double long HDiscrete::do_next_continuous( void ) {
	M_ASSERT( !"Invalid use of discrete distribution."[0] );
	return ( 0.0L );
}

inline double long to_standard_uniform( yaal::u64_t x_ ) {
	double long numerator( static_cast<double long>( x_ ) );
	double long denominator( static_cast<double long>( meta::max_unsigned<yaal::u64_t>::value ) );
	return ( min( max( 0.0L, numerator / denominator ), 1.0L ) );
}

HUniform::HUniform( double long lower_, double long upper_ )
	: HDistribution()
	, _base( lower_ )
	, _range( upper_ - lower_ ) {
	M_ASSERT( upper_ > lower_ );
	return;
}

double long HUniform::operator()( void ) {
	double long x( to_standard_uniform( (*_rng)() ) );
	x *= _range;
	x += _base;
	return ( x );
}

yaal::i64_t HUniform::do_next_discrete( void ) {
	M_ASSERT( !"Invalid use of continuous distribution."[0] );
	return ( 0LL );
}

double long HUniform::do_next_continuous( void ) {
	return ( operator()() );
}

HTriangle::HTriangle( double long infimum_, double long supremum_, double long mode_ )
	: HDistribution()
	, _infimum( infimum_ )
	, _supremum( supremum_ )
	, _mode( mode_ )
	, _modeValue( 0.0L )
	, _lowerMod( 0.0L )
	, _upperMod( 0.0L ) {
	M_ASSERT( ( supremum_ > infimum_ ) && ( mode_ >= infimum_ ) && ( mode_ <= supremum_ ) );
	_modeValue = ( _mode - _infimum ) / ( _supremum - _infimum );
	_lowerMod = ( _supremum - _infimum ) * ( _mode - _infimum );
	_upperMod = ( _supremum - _infimum ) * ( _supremum - _mode );
	return;
}

double long HTriangle::operator()( void ) {
	double long x( to_standard_uniform( (*_rng)() ) );
	if ( x < _modeValue ) {
		x = _infimum + math::square_root( x * _lowerMod );
	} else {
		x = _supremum - math::square_root( ( 1 - x ) * _upperMod );
	}
	return ( x );
}

yaal::i64_t HTriangle::do_next_discrete( void ) {
	M_ASSERT( !"Invalid use of continuous distribution."[0] );
	return ( 0LL );
}

double long HTriangle::do_next_continuous( void ) {
	return ( operator()() );
}

HNormal::HNormal( double long mu_, double long sigma_ )
	: HDistribution()
	, _mu( mu_ )
	, _sigma( sigma_ )
	, _cache( 0.0L )
	, _cached( false ) {
	M_ASSERT( sigma_ > 0 );
	return;
}

double long HNormal::operator()( void ) {
	double long x( 0.0L );
	if ( _cached ) {
		_cached = false;
		x = _cache;
	} else {
		double long s( 2.0L );
		double long u( 0.0L );
		double long v( 0.0L );
		while ( s >= 1.0L ) {
			u = uniform_sample();
			v = uniform_sample();
			s = u * u + v * v;
		}
		double long z(
			math::square_root(
				( -2.0L * math::natural_logarithm( s ) ) / s
			)
		);
		x = u * z;
		_cache = v * z;
		_cached = true;
	}
	return ( x * _sigma + _mu );
}

double long HNormal::uniform_sample( void ) {
	double long x( to_standard_uniform( (*_rng)() ) );
	return ( min( max( -1.0L, ( 2 * x ) - 1.0L ), 1.0L ) );
}

yaal::i64_t HNormal::do_next_discrete( void ) {
	M_ASSERT( !"Invalid use of continuous distribution."[0] );
	return ( 0LL );
}

double long HNormal::do_next_continuous( void ) {
	return ( operator()() );
}

}

namespace rng_helper {

distribution::HDiscrete make_random_number_generator( i64_t cap_ ) {
	M_PROLOG
	M_ASSERT( cap_ >= 1 );
	return ( distribution::HDiscrete( 0, cap_ - 1 ) );
	M_EPILOG
}

}

}

}

