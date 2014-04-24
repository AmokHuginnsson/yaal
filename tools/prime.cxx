/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	prime.cxx - this file is integral part of `yaal' project.

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

#include <cmath>
#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "prime.hxx"
#include "hcore/hchunk.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace {
u8_t _mask_[] = {
	1, 2, 4, 8, 16, 32, 64, 128
};

u8_t _negMask_[] = {
	255 - 1, 255 - 2, 255 - 4, 255 - 8, 255 - 16, 255 - 32, 255 - 64, 255 - 128
};
}

void find_primes( primes_t& primes_, int long unsigned lowerLimit_, int long unsigned upperLimit_ ) {
	M_PROLOG
	M_ENSURE( upperLimit_ > lowerLimit_ );
	primes_.clear();
	int long memSize( ( upperLimit_ >> 3 ) + 1 );
	HChunk mem( memSize );
	u8_t* sieve( mem.get<u8_t>() );
	::memset( sieve, 0xff, static_cast<size_t>( memSize ) );
	int long unsigned limit( static_cast<int long unsigned>( square_root( static_cast<double long>( upperLimit_ ) ) ) + 1 );
	int long unsigned step( 0 );
	for ( int long unsigned i( 2 ); i <= limit; ++ i ) {
		if ( sieve[ i >> 3 ] & _mask_[ i & 0x07 ] ) {
			for ( int long unsigned j( i ); j <= limit; ++ j ) {
				if ( sieve[ j >> 3 ] & _mask_[ j & 0x07 ] ) {
					step = j;
					break;
				}
			}
			int long unsigned lowerLimit( ( lowerLimit_ / step ) * step );
			if ( lowerLimit < limit ) {
				for ( int long unsigned j( step << 1 ); j <= upperLimit_; j += step ) {
					int long unsigned octet( j >> 3 );
					sieve[ octet ] = static_cast<u8_t>( sieve[ octet ] & _negMask_[ j & 0x07 ] );
				}
			} else {
				for ( int long unsigned j( step << 1 ); j <= limit; j += step ) {
					int long unsigned octet( j >> 3 );
					sieve[ octet ] = static_cast<u8_t>( sieve[ octet ] & _negMask_[ j & 0x07 ] );
				}
				for ( int long unsigned j( lowerLimit ); j <= upperLimit_; j += step ) {
					int long unsigned octet( j >> 3 );
					sieve[ octet ] = static_cast<u8_t>( sieve[ octet ] & _negMask_[ j & 0x07 ] );
				}
			}
		}
	}
	static int long unsigned const FIRST_PRIME( 2 );
	for ( int long unsigned i( max( lowerLimit_, FIRST_PRIME ) ); i <= upperLimit_; ++ i ) {
		if ( sieve[ i >> 3 ] & _mask_[ i & 0x07 ] ) {
			primes_.push_back( i );
		}
	}
	return;
	M_EPILOG
}

}

}

