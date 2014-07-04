/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hbitset.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hbitset.hxx
 * \brief Declaration and implementation of HBitset class template.
 */

#ifndef YAAL_HCORE_HBITSET_HXX_INCLUDED
#define YAAL_HCORE_HBITSET_HXX_INCLUDED 1

#include "hcore/static_assert.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/iterator.hxx"
#include "hcore/hstring.hxx"

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

}

/*! \brief Provision operations for set of bits.
 */
template<int const SIZE>
class HBitset {
	typedef u64_t word_t;
	static int const BITS_IN_WORD = static_cast<int>( sizeof ( word_t ) ) * 8;
	static word_t const BIT = 0x1LL << ( BITS_IN_WORD - 1 );
public:
	typedef HBitset<SIZE> this_type;
private:
	word_t _buf[SIZE / BITS_IN_WORD + ( SIZE % BITS_IN_WORD ? 1 : 0 )];
public:
	HBitset()
		: _buf() {
	}
	HBitset( word_t val_ )
		: _buf() {
			_buf[0] = bit::reverse( val_ );
			for ( int i( SIZE ); i < BITS_IN_WORD; ++ i ) {
				_buf[0] &= ~( BIT >> i );
			}
		}
	HBitset( HBitset const& bs_ )
		: _buf() {
		copy( bs_._buf, end( bs_._buf ), _buf );
	}
	HBitset& operator = ( HBitset const& bs_ ) {
		if ( &bs_ != this ) {
			HBitset tmp( bs_ );
			swap( tmp );
		}
		return ( *this );
	}
	void swap( HBitset& bs_ ) {
		if ( &bs_ != this ) {
			swap_ranges( begin( _buf ), end( _buf ), begin( bs_._buf ) );
		}
		return;
	}
	void set( int pos_, bool value_ = true ) {
		M_ASSERT( pos_ < SIZE );
		int const wordIdx( pos_ / BITS_IN_WORD );
		int const bitIdx( pos_ % BITS_IN_WORD );
		if ( value_ ) {
			_buf[wordIdx] |= ( BIT >> bitIdx );
		} else {
			_buf[wordIdx] &= ~( BIT >> bitIdx );
		}
		return;
	}
	void reset( int pos_ ) {
		M_ASSERT( pos_ < SIZE );
		int const wordIdx( pos_ / BITS_IN_WORD );
		int const bitIdx( pos_ % BITS_IN_WORD );
		_buf[wordIdx] &= ~( BIT >> bitIdx );
		return;
	}
	bool get( int pos_ ) const {
		M_ASSERT( pos_ < SIZE );
		int const wordIdx( pos_ / BITS_IN_WORD );
		int const bitIdx( pos_ % BITS_IN_WORD );
		return ( _buf[wordIdx] & ( BIT >> bitIdx ) ? true : false );
	}
	void flip( int pos_ ) {
		M_ASSERT( pos_ < SIZE );
		int const wordIdx( pos_ / BITS_IN_WORD );
		int const bitIdx( pos_ % BITS_IN_WORD );
		_buf[wordIdx] ^= ( BIT >> bitIdx );
		return;
	}
	void flip( void ) {
		for ( word_t* i( _buf ), * e( end( _buf ) ); i != e; ++ i ) {
			*i = ~*i;
		}
		return;
	}
	int count( void ) {
		int c( 0 );
		for ( word_t* i( _buf ), * e( end( _buf ) ); i != e; ++ i ) {
			c += bit::count( *i );
		}
		return ( c );
	}
	bool operator == ( HBitset const& bs_ ) const {
		return ( equal( _buf, end( _buf ), bs_._buf ) );
	}
	bool operator != ( HBitset const& bs_ ) const {
		return ( ! operator == ( bs_ ) );
	}
#define YAAL_DEFINE_OPER( OP ) \
	HBitset& operator OP ( HBitset const& bs_ ) { \
		word_t const* bs( bs_._buf ); \
		for ( word_t* i( _buf ), * e( end( _buf ) ); i != e; ++ i, ++ bs ) { \
			*i OP *bs; \
		} \
		return ( *this ); \
	}
YAAL_DEFINE_OPER( |= )
YAAL_DEFINE_OPER( &= )
YAAL_DEFINE_OPER( ^= )
#undef YAAL_DEFINE_OPER
#define YAAL_DEFINE_OPER( OP ) \
	HBitset<SIZE> operator OP ( HBitset const& bs_ ) const { \
		HBitset<SIZE> bs( *this ); \
		bs OP##= bs_; \
		return ( bs ); \
	}
YAAL_DEFINE_OPER( | )
YAAL_DEFINE_OPER( & )
YAAL_DEFINE_OPER( ^ )
#undef YAAL_DEFINE_OPER
	HBitset<SIZE> operator ~ ( void ) const {
		HBitset<SIZE> bs( *this );
		bs.flip();
		return ( bs );
	}
	word_t to_integer( void ) const {
		STATIC_ASSERT( SIZE <= BITS_IN_WORD );
		return ( bit::reverse( _buf[ 0 ] ) );
	}
	word_t const* raw( void ) const {
		return ( _buf );
	}
	HString to_string( char falseChar_ = '0', char trueChar_ = '1' ) const {
		M_PROLOG
		HString s( SIZE, falseChar_ );
		for ( int i( 0 ); i < SIZE; ++ i ) {
			if ( get( i ) ) {
				s.set_at( i, trueChar_ );
			}
		}
		return ( s );
		M_EPILOG
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HBITSET_HXX_INCLUDED */

