/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hbitset.hxx
 * \brief Declaration and implementation of HBitset class template.
 */

#ifndef YAAL_HCORE_HBITSET_HXX_INCLUDED
#define YAAL_HCORE_HBITSET_HXX_INCLUDED 1

#include "hcore/algorithm.hxx"
#include "hcore/iterator.hxx"
#include "hcore/hstring.hxx"
#include "hcore/bit.hxx"

namespace yaal {

namespace hcore {

/*! \brief Provision operations for set of bits.
 */
template<int const SIZE>
class HBitset final {
	typedef u64_t word_t;
	static int const BITS_IN_WORD = static_cast<int>( sizeof ( word_t ) ) * 8;
	static word_t const BIT = 0x1ULL << ( BITS_IN_WORD - 1 );
	static int const WORD_COUNT = SIZE / BITS_IN_WORD + ( SIZE % BITS_IN_WORD ? 1 : 0 );
public:
	typedef HBitset<trait::to_unsigned<int, SIZE>::value> this_type;
	class HBitReference;
private:
	word_t _buf[trait::to_unsigned<int, WORD_COUNT>::value];
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
	int size( void ) const {
		return ( SIZE );
	}
	bool all( void ) const {
		bool ok( true );
		int const words( SIZE / BITS_IN_WORD );
		for ( int i( 0 ); i < words; ++ i ) {
			if ( ~_buf[i] ) {
				ok = false;
				break;
			}
		}
		int const bits( SIZE % BITS_IN_WORD );
		if ( ok && bits ) {
			for ( int i( 0 ); i < bits; ++ i ) {
				if ( !get( i ) ) {
					ok = false;
					break;
				}
			}
		}
		return ( ok );
	}
	bool none( void ) const {
		bool ok( true );
		for ( int i( 0 ); i < WORD_COUNT; ++ i ) {
			if ( _buf[i] ) {
				ok = false;
				break;
			}
		}
		return ( ok );
	}
	bool any( void ) const {
		return ( !none() );
	}
	bool operator[] ( int pos_ ) const {
		M_ASSERT( pos_ < SIZE );
		return ( get( pos_ ) );
	}
	HBitReference operator[] ( int pos_ ) {
		return ( HBitReference( this, pos_ ) );
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
		static_assert( SIZE <= BITS_IN_WORD, "integer overflow" );
		return ( bit::reverse( _buf[ 0 ] ) );
	}
	word_t const* raw( void ) const {
		return ( _buf );
	}
	HString to_string( code_point_t falseChar_ = '0'_ycp, code_point_t trueChar_ = '1'_ycp ) const {
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

/*! \brief Writable HBitset bit reference interface.
 */
template<int const SIZE>
class HBitset<SIZE>::HBitReference {
	HBitset* _owner;
	int _pos;
public:
	HBitReference( HBitReference const& br_ )
		: _owner( br_._owner ), _pos( br_._pos ) {
	}
	HBitReference& operator = ( HBitReference const& br_ ) {
		_owner->set( _pos, static_cast<bool>( br_ ) );
		return ( *this );
	}
	HBitReference& operator = ( bool value_ ) {
		_owner->set( _pos, value_ );
		return ( *this );
	}
	bool operator == ( bool value_ ) const {
		return ( _owner->get( _pos ) == value_ );
	}
	bool operator != ( bool value_ ) const {
		return ( _owner->get( _pos ) != value_ );
	}
	operator bool ( void ) const {
		return ( _owner->get( _pos ) );
	}
	void swap( typename HBitset::HBitReference& br_ ) {
		M_PROLOG
		bool bit( _owner->get( _pos ) );
		_owner->set( _pos, static_cast<bool>( br_ ) );
		br_._owner->set( br_._pos, bit );
		return;
		M_EPILOG
	}
private:
	friend class HBitset;
	HBitReference( HBitset* owner_, int pos_ )
		: _owner( owner_ ), _pos( pos_ ) {
	}
};

/*! \brief Swap values two HBitset bits.
 *
 * To programmers:
 * Arguments types are intentionally not-references but lvalues
 * because they must work when returned by HBitset::operator[],
 * and operator[] returns lvalue.
 * You cannot bind lvalue to reference to one could not use swap() in following
 * context:
 * swap( bs1[k], bs2[j] );
 *
 * Because interface does not match standard swap(T&,T&)
 * this swap() implementation must be in the same namespace as its operands
 * to be found by Koenig lookup.
 *
 * \param a - first of bits to swap.
 * \param b - second of bits to swap.
 */
template<int const SIZE>
inline void swap( typename yaal::hcore::HBitset<SIZE>::HBitReference a, typename yaal::hcore::HBitset<SIZE>::HBitReference b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HBITSET_HXX_INCLUDED */

