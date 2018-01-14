/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cmath>
#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "prime.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/math.hxx"

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
	int long unsigned memSize( ( upperLimit_ >> 3 ) + 1u );
	HChunk mem( static_cast<int long>( memSize ) );
	u8_t* sieve( mem.get<u8_t>() );
	::memset( sieve, 0xff, memSize );
	int long unsigned limit( static_cast<int long unsigned>( math::square_root( static_cast<double long>( upperLimit_ ) ) ) + 1 );
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

