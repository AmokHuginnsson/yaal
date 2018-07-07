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
static u64_t const BIT_MASK64[] = {
	0x5555555555555555,
	0x3333333333333333,
	0x0f0f0f0f0f0f0f0f,
	0x00ff00ff00ff00ff,
	0x0000ffff0000ffff,
	0x00000000ffffffff
};
static u32_t const BIT_MASK32[] = {
	0x55555555,
	0x33333333,
	0x0f0f0f0f,
	0x00ff00ff,
	0x0000ffff
};
static u16_t const BIT_MASK16[] = {
	0x5555,
	0x3333,
	0x0f0f,
	0x00ff
};
static u8_t const BIT_MASK8[] = {
	0x55,
	0x33,
	0x0f
};

static const int MULTIPLY_DE_BRUIJN_BIT_POSITION[64] = {
	0, 1, 48, 2, 57, 49, 28, 3, 61, 58, 50, 42, 38, 29, 17, 4,
	62, 55, 59, 36, 53, 51, 43, 22, 45, 39, 33, 30, 24, 18, 12, 5,
	63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21, 44, 32, 23, 11,
	46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9, 13, 8, 7, 6
};

inline int count( u64_t v_ ) {
	for ( int i( 0 ); i < yaal::size( BIT_MASK64 ); ++ i ) {
		v_ = ( v_ & BIT_MASK64[i] ) + ( ( v_ >> SHIFT[i] ) & BIT_MASK64[i] );
	}
	return ( static_cast<int>( v_ ) );
}

inline u64_t reverse( u64_t v_ ) {
	for ( int i( 0 ); i < yaal::size( BIT_MASK64 ); ++ i ) {
		v_ = ( ( v_ >> SHIFT[i] ) & BIT_MASK64[i] ) | ( ( v_ & BIT_MASK64[i] ) << SHIFT[i] );
	}
	return ( v_ );
}

inline int count( u32_t v_ ) {
	for ( int i( 0 ); i < yaal::size( BIT_MASK32 ); ++ i ) {
		v_ = ( v_ & BIT_MASK32[i] ) + ( ( v_ >> SHIFT[i] ) & BIT_MASK32[i] );
	}
	return ( static_cast<int>( v_ ) );
}

inline u32_t reverse( u32_t v_ ) {
	for ( int i( 0 ); i < yaal::size( BIT_MASK32 ); ++ i ) {
		v_ = ( ( v_ >> SHIFT[i] ) & BIT_MASK32[i] ) | ( ( v_ & BIT_MASK32[i] ) << SHIFT[i] );
	}
	return ( v_ );
}

inline int count( u16_t v_ ) {
	for ( int i( 0 ); i < yaal::size( BIT_MASK16 ); ++ i ) {
		v_ = static_cast<u16_t>( ( v_ & BIT_MASK16[i] ) + ( ( v_ >> SHIFT[i] ) & BIT_MASK16[i] ) );
	}
	return ( static_cast<int>( v_ ) );
}

inline u16_t reverse( u16_t v_ ) {
	for ( int i( 0 ); i < yaal::size( BIT_MASK16 ); ++ i ) {
		v_ = static_cast<u16_t>( ( ( v_ >> SHIFT[i] ) & BIT_MASK16[i] ) | ( ( v_ & BIT_MASK16[i] ) << SHIFT[i] ) );
	}
	return ( v_ );
}

inline int count( u8_t v_ ) {
	for ( int i( 0 ); i < yaal::size( BIT_MASK8 ); ++ i ) {
		v_ = static_cast<u8_t>( ( v_ & BIT_MASK8[i] ) + ( ( v_ >> SHIFT[i] ) & BIT_MASK8[i] ) );
	}
	return ( static_cast<int>( v_ ) );
}

inline u8_t reverse( u8_t v_ ) {
	for ( int i( 0 ); i < yaal::size( BIT_MASK8 ); ++ i ) {
		v_ = static_cast<u8_t>( ( ( v_ >> SHIFT[i] ) & BIT_MASK8[i] ) | ( ( v_ & BIT_MASK8[i] ) << SHIFT[i] ) );
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

