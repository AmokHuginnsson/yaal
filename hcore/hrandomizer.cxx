/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrandomizer.cxx - this file is integral part of `yaal' project.

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

#include <sys/time.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hrandomizer.hxx"
#include "hexception.hxx"
#include "algorithm.hxx"

namespace yaal {

namespace hcore {

namespace {
	static int const MM = HRandomizer::STATE_SIZE / 2;
	static u64_t const MATRIX_A = 0xB5026F5AA96619E9ull;
	static u64_t const UM = 0xFFFFFFFF80000000ull; /* Most significant 33 bits */
	static u64_t const LM = 0x7FFFFFFFull; /* Least significant 31 bits */
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
		: _z( seed_ ^ 362436069 ), _w( seed_ ^ 521288629 ), _jsr( seed_ ^ 123456789 ), _jcong( seed_ ^ 380116160 )
		{}
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

HRandomizer::HRandomizer( u64_t seed_, u64_t cap_ )
	: _index( STATE_SIZE + 1 ), _range( cap_ ), _state() {
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

HRandomizer::HRandomizer( u64_t const* stateFirst_, u64_t const* stateLast_, u64_t cap_ )
	: _index( STATE_SIZE + 1 ), _range( cap_ ), _state() {
	M_PROLOG
	M_ASSERT( stateFirst_ && stateLast_ );
	M_ASSERT( stateFirst_ != stateLast_ );
	init( stateFirst_, static_cast<int>( stateLast_ - stateFirst_ ) );
	return;
	M_EPILOG
}

void HRandomizer::swap( HRandomizer& randomizer_ ) {
	M_PROLOG
	if ( &randomizer_ != this ) {
		using yaal::swap;
		swap( _index, randomizer_._index );
		swap( _range, randomizer_._range );
		swap_ranges( _state, _state + countof ( _state ), randomizer_._state );
	}
	return;
	M_EPILOG
}

void HRandomizer::init( u64_t seed_ ) {
	M_PROLOG
	_state[0] = seed_;
	for ( _index = 1; _index < STATE_SIZE; ++ _index ) 
		_state[_index] = ( 6364136223846793005ull * ( _state[_index-1] ^ ( _state[_index - 1] >> 62 ) ) + _index );
	return;
	M_EPILOG
}

void HRandomizer::init( u64_t const* state_, int stateSize_ ) {
	M_PROLOG
	init( 19650218ull );
	int i( 1 );
	int j( 0 );
	int k( ( STATE_SIZE > stateSize_ ? STATE_SIZE : stateSize_ ) );
	for ( ; k; -- k ) {
		_state[i] = ( _state[i] ^ ( ( _state[i - 1] ^ ( _state[i - 1] >> 62 ) ) * 3935559000370003845ull ) ) + state_[j] + j; /* non linear */
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
		_state[i] = ( _state[i] ^ ( (_state[i - 1] ^ ( _state[i - 1] >> 62 ) ) * 2862933555777941757ull ) ) - i; /* non linear */
		++ i;
		if ( i >= STATE_SIZE ) {
			_state[0] = _state[STATE_SIZE - 1];
			i = 1;
		}
	}
	_state[0] = 1ull << 63; /* MSB is 1; assuring non-zero initial array */ 
	return;
	M_EPILOG
}
	
u64_t HRandomizer::operator()( u64_t range_ ) {
	M_PROLOG
	return ( operator()() % range_ );
	M_EPILOG
}

u64_t HRandomizer::operator()( void ) {
	M_PROLOG
	u64_t x( 0 );
	u64_t const mag01[ 2 ] = { 0ull, MATRIX_A };
	if ( _index >= STATE_SIZE ) { /* generate STATE_SIZE words at one time */
		int i( 0 );
		for ( ; i < STATE_SIZE - MM; ++ i ) {
			x = ( _state[i] & UM ) | ( _state[ i + 1 ] & LM );
			_state[i] = _state[i + MM] ^ ( x >> 1 ) ^ mag01[static_cast<int>( x & 1ull )];
		}
		for ( ; i < STATE_SIZE - 1; ++ i ) {
			x = ( _state[i] & UM ) | ( _state[ i + 1 ] & LM );
			_state[i] = _state[i + ( MM - STATE_SIZE )] ^ ( x >> 1 ) ^ mag01[static_cast<int>( x & 1ull )];
		}
		x = ( _state[ STATE_SIZE - 1 ] & UM ) | ( _state[0] & LM );
		_state[ STATE_SIZE - 1 ] = _state[ MM - 1 ] ^ ( x >> 1 ) ^ mag01[static_cast<int>( x & 1ull )];
		_index = 0;
	}

	x = _state[ _index ++ ];
	x ^= ( x >> 29 ) & 0x5555555555555555ull;
	x ^= ( x << 17 ) & 0x71D67FFFEDA60000ull;
	x ^= ( x << 37 ) & 0xFFF7EEE000000000ull;
	x ^= ( x >> 43 );
	return ( x % _range );
	M_EPILOG
}

namespace randomizer_helper {

HRandomizer make_randomizer( u64_t cap_ ) {
	M_PROLOG
		struct timeval tv;
	M_ENSURE( gettimeofday( &tv, NULL ) == 0 );
	return ( HRandomizer( tv.tv_sec + tv.tv_usec, cap_ ) );
	M_EPILOG
}

}

}

}

