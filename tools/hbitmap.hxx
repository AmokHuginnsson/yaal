/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/hbitmap.hxx
 * \brief Declaration of HBitmap related classes.
 *
 * HBitmap, HBitmap::HIterator, HBitmap::HBit.
 */

#ifndef YAAL_TOOLS_HBITMAP_HXX_INCLUDED
#define YAAL_TOOLS_HBITMAP_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/iterator.hxx"

namespace yaal {

namespace tools {

/*! \brief Abstract for bit pool bit operations.
 */
class HBitmap final {
	typedef HBitmap this_type;
public:
	class HBit;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<bool> const_iterator;
	typedef HIterator<HBit> iterator;
	typedef yaal::hcore::HReverseIterator<iterator> reverse_iterator;
	typedef yaal::hcore::HReverseIterator<const_iterator> const_reverse_iterator;
private:
	int long _allocatedBytes;
	int long _size;
	void* _data;
public:
	HBitmap( void );
	~HBitmap( void );
	HBitmap( int long );
	HBitmap( HBitmap const& );
	HBitmap& operator = ( HBitmap const& );
	bool operator == ( HBitmap const& ) const;
	bool operator != ( HBitmap const& ) const;
	bool operator[] ( int long ) const;
	HBit operator[] ( int long );
	HBitmap& operator |= ( HBitmap const& );
	HBitmap& operator &= ( HBitmap const& );
	HBitmap& operator ^= ( HBitmap const& );
	HBitmap& operator <<= ( int long );
	HBitmap& operator >>= ( int long );
	HBitmap operator | ( HBitmap const& ) const;
	HBitmap operator & ( HBitmap const& ) const;
	HBitmap operator ^ ( HBitmap const& ) const;
	HBitmap operator << ( int long );
	HBitmap operator >> ( int long );
	HBitmap& operator += ( HBitmap const& );
	void clear( void );
	void fill( bool );
	void fill( int long, int long, bool );
	void push_back( bool );
	int long get_size( void ) const;
	int long size( void ) const;
	void const* raw( void ) const;
	void copy( void const*, int long );
	void use( void*, int long );
	void reserve( int long );
	const_iterator begin( void ) const;
	const_iterator find( int long ) const;
	const_iterator end( void ) const;
	const_reverse_iterator rbegin( void ) const;
	const_reverse_iterator rend( void ) const;
	iterator begin( void );
	iterator find( int long );
	iterator end( void );
/* cppcheck-suppress functionConst */
	reverse_iterator rbegin( void );
/* cppcheck-suppress functionConst */
	reverse_iterator rend( void );
	bool get( int long ) const;
	void set( int long, bool );
	void rotate_left( int long, int long, int long );
	void rotate_right( int long, int long, int long );
private:
	void ensure_pool( int long );
	int long octets_for_bits( int long ) const;
	void* block( void );
	void const* block( void ) const;
};

typedef yaal::hcore::HExceptionT<HBitmap> HBitmapException;

/*! \brief HBitmap class helpers, utility functions.
 */
namespace bitmap_type_helper {

/*! \brief Get owner constness based on child type.
 *
 * \tparam input - child type.
 * \retval owner_t - const qualified owner type.
 */
template<typename input>
struct owner_const_qual_from_type {
};

/*! \cond */
template<>
struct owner_const_qual_from_type<bool> {
	typedef HBitmap const* owner_t;
};
template<>
struct owner_const_qual_from_type<HBitmap::HBit> {
	typedef HBitmap* owner_t;
};
/*! \endcond */

}

/*! \brief Bit iterator, allows access to single bits in sequence.
 */
template<typename const_qual_t>
class HBitmap::HIterator : public yaal::hcore::iterator_interface<const_qual_t, yaal::hcore::iterator_category::random_access> {
public:
	typedef yaal::hcore::iterator_interface<const_qual_t, yaal::hcore::iterator_category::random_access> base_type;
	typedef const_qual_t value_type;
	typedef const_qual_t reference;
private:
	typedef typename bitmap_type_helper::owner_const_qual_from_type<const_qual_t>::owner_t owner_t;
	owner_t _owner;
	int long _index;
public:
	HIterator( void ) : base_type(), _owner( nullptr ), _index( 0 ) {}
	HIterator( HIterator const& it ) : base_type(), _owner( it._owner ), _index( it._index ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it ) : base_type(), _owner( it._owner ), _index( it._index ) {
		static_assert(
			trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, bool>::value,
			"creating non-const iterator instance discards qualifiers"
		);
	}
	HIterator& operator ++ ( void ) {
		++ _index;
		return ( *this );
	}
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		++ _index;
		return ( it );
	}
	HIterator& operator -- ( void ) {
		-- _index;
		return ( *this );
	}
	HIterator operator -- ( int ) {
		HIterator it( *this );
		-- _index;
		return ( it );
	}
	HIterator& operator += ( int long offset_ ) {
		_index += offset_;
		return ( *this );
	}
	HIterator operator + ( int long offset_ ) {
		HIterator it( *this );
		it += offset_;
		return ( it );
	}
	HIterator& operator -= ( int long offset_ ) {
		_index -= offset_;
		return ( *this );
	}
	HIterator operator - ( int long offset_ ) {
		HIterator it( *this );
		it -= offset_;
		return ( it );
	}
	HIterator& operator = ( HIterator const& it ) {
		if ( &it != this ) {
			_owner = it._owner;
			_index = it._index;
		}
		return ( *this );
	}
	bool operator == ( HIterator const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index == it._index );
	}
	bool operator != ( HIterator const& it ) const {
		M_ASSERT( _owner == it._owner );
		return ( _index != it._index );
	}
	const_qual_t operator* ( void ) const;
private:
	friend class yaal::tools::HBitmap;
	HIterator( owner_t owner_, int long idx )
		: base_type(), _owner( owner_ ), _index( idx ) {}
};

/*! \brief Writable HBitmap bit reference interface.
 */
class HBitmap::HBit {
	HBitmap* _owner;
	int long _index;
public:
	HBit( HBit const& );
	HBit& operator = ( HBit const& );
	HBit& operator = ( bool );
	bool operator == ( HBit const& ) const;
	bool operator == ( bool ) const;
	bool operator != ( HBit const& ) const;
	bool operator != ( bool ) const;
	explicit operator bool ( void ) const;
	void swap( HBit& );
private:
	friend class HBitmap::HIterator<HBitmap::HBit>;
	friend class HBitmap;
	HBit( HBitmap*, int long );
};

/*! \brief Swap values two HBitmap bits.
 *
 * To programmers:
 * Arguments types are intentionally not-references but lvalues
 * because they must work when returned by HBitmap::HIterator::operator *,
 * and *it returns lvalue.
 * You cannot bind lvalue to reference to one could not use swap() in following
 * context:
 * swap( *it1, *it2 );
 *
 * Because interface does not match standard swap(T&,T&)
 * this swap() implementation must be in the same namespace as its operands
 * to be found by Koenig lookup.
 *
 * \param a - first of bits to swap.
 * \param b - second of bits to swap.
 */
inline void swap( yaal::tools::HBitmap::HBit a, yaal::tools::HBitmap::HBit b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_TOOLS_HBITMAP_HXX_INCLUDED */

