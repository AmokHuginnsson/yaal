/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/bit.hxx
 * \brief Declaration and implementation of bit twiddling functions.
 */

#ifndef YAAL_HCORE_BIT_HXX_INCLUDED
#define YAAL_HCORE_BIT_HXX_INCLUDED 1

#include "hcore/base.hxx"

namespace yaal {

namespace hcore {

namespace bit {

static int const SHIFT[] = { 1, 2, 4, 8, 16, 32 };
static u64_t const BIT_MASK[] = {
	0x5555555555555555,
	0x3333333333333333,
	0x0f0f0f0f0f0f0f0f,
	0x00ff00ff00ff00ff,
	0x0000ffff0000ffff,
	0x00000000ffffffff
};

static const int MULTIPLY_DE_BRUIJN_BIT_POSITION[64] = {
	0, 1, 48, 2, 57, 49, 28, 3, 61, 58, 50, 42, 38, 29, 17, 4,
	62, 55, 59, 36, 53, 51, 43, 22, 45, 39, 33, 30, 24, 18, 12, 5,
	63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21, 44, 32, 23, 11,
	46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9, 13, 8, 7, 6
};

inline int count( u64_t v_ ) {
	for ( int i( 0 ); i < countof ( SHIFT ); ++ i ) {
		v_ = ( v_ & BIT_MASK[i] ) + ( ( v_ >> SHIFT[i] ) & BIT_MASK[i] );
	}
	return ( static_cast<int>( v_ ) );
}

inline u64_t reverse( u64_t v_ ) {
	for ( int i( 0 ); i < countof ( SHIFT ); ++ i ) {
		v_ = ( ( v_ >> SHIFT[i] ) & BIT_MASK[i] ) | ( ( v_ & BIT_MASK[i] ) << SHIFT[i] );
	}
	return ( v_ );
}

inline int least_significant( u64_t v_ ) {
	return ( v_ ? MULTIPLY_DE_BRUIJN_BIT_POSITION[ ( ( v_ & -v_) * 0x03F79D71B4CB0A89ull ) >> 58 ] : -1 );
}

}

}

}

#endif /* #ifndef YAAL_HCORE_BIT_HXX_INCLUDED */

