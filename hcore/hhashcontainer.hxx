/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashcontainer.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hhashcontainer.hxx
 * \brief Declaration of HHashContainer<>, a halper for HHash* and HMultiHash* classes.
 */

#ifndef YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED
#define YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED 1

#include "hcore/memory.hxx"
#include "hcore/trait.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hpair.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/allocator.hxx"

namespace yaal {

namespace hcore {

template<typename key_t>
struct hash {
	int long operator () ( key_t const& key_ ) const;
};

extern M_YAAL_HCORE_PUBLIC_API int long const* const _primes_;

template<typename value_t, typename hasher_t, typename get_key_t, typename allocator_t>
class HHashContainer : private trait::HNonCopyable {
public:
	typedef value_t value_type;
	typedef hasher_t hasher_type;
	typedef get_key_t get_key_type;
	typedef typename get_key_type::key_type key_type;
private:
	class HAtom {
	private: /* for UT */
		HAtom* _next;
		value_type _value;
		HAtom( value_type const& value_ )
			: _next( NULL ), _value( value_ )
			{}
		HAtom( HAtom const& );
		HAtom& operator = ( HAtom const& );
		friend class HHashContainer;
		friend class HIterator;
	};
public:
	typedef typename allocator_t::template rebind<HAtom>::other allocator_type;
	/*! \brief Iterator for HHashContainer data structure.
	 */
	class HIterator {
		typedef HHashContainer<value_type, hasher_type, get_key_type, allocator_t> owner_t;
		owner_t const* _owner;
		int long _index;
		typename owner_t::HAtom* _atom;
	public:
		HIterator( void ) : _owner( NULL ), _index( 0 ), _atom( NULL ) {}
		HIterator( HIterator const& it_ )
			: _owner( it_._owner ), _index( it_._index ),
			_atom( it_._atom ) {
		}
		HIterator& operator = ( HIterator const& it_ ) {
			if ( &it_ != this ) {
				_owner = it_._owner;
				_index = it_._index;
				_atom = it_._atom;
			}
			return ( *this );
		}
		HIterator& operator ++ ( void ) {
			M_ASSERT( _owner );
			int long oldIndex( _index );
			if ( _atom ) {
				_atom = _atom->_next;
				if ( ! _atom )
					++ _index;
			}
			if ( ! _atom ) {
				typename owner_t::HAtom* const* buckets = _owner->_buckets.template get<typename owner_t::HAtom*>();
				if ( oldIndex == _owner->_prime )
					_index = 0;
				while ( ( _index < _owner->_prime ) && ! buckets[ _index ] )
					++ _index;
				if ( _index < _owner->_prime )
					_atom = buckets[ _index ];
			}
			if ( ! _atom )
				_index = _owner->_prime;
			return ( *this );
		}
		HIterator& operator -- ( void ) {
			M_ASSERT( _owner );
			typename owner_t::HAtom* const* buckets = _owner->_buckets.template get<typename owner_t::HAtom*>();
			if ( _atom ) {
				typename owner_t::HAtom* atom( buckets[ _index ] );
				while ( ( atom != _atom ) && ( atom->_next != _atom ) )
					atom = atom->_next;
				if ( atom == _atom )
					_atom = NULL;
				else
					_atom = atom;
			}
			if ( ! _atom ) {
				if ( _index == 0 )
					_index = _owner->_prime - 1;
				while ( ( _index > 0 ) && ! buckets[ _index ] )
					-- _index;
				_atom = buckets[ _index ];
				while ( _atom && _atom->_next )
					_atom = _atom->_next;
			}
			return ( *this );
		}
		value_type& get( void ) const {
			M_ASSERT( _atom );
			return ( _atom->_value );
		}
		bool operator == ( HIterator const& it ) const {
			M_ASSERT( _owner == it._owner );
			return ( ( _index == it._index ) && ( _atom == it._atom ) );
		}
		bool operator != ( HIterator const& it ) const
			{ return ( ! operator == ( it ) ); }
	private:
		friend class HHashContainer<value_type, hasher_type, get_key_type, allocator_t>;
		explicit HIterator( owner_t const* owner_, int long index_, typename owner_t::HAtom* atom_ )
			: _owner( owner_ ), _index( index_ ), _atom( atom_ ) {};
	};
private:
	int long _prime;
	int _size;
	HChunk _buckets;
	hasher_type _hasher;
	allocator_type _allocator;
public:
	HHashContainer( hasher_type const& hasher_, allocator_type const& allocator_ )
		: _prime( 0 ), _size( 0 ), _buckets(), _hasher( hasher_ ), _allocator( allocator_ ) {
	}
	virtual ~HHashContainer( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
	}
	int long get_size( void ) const {
		M_PROLOG
		return ( _size );
		M_EPILOG
	}
	bool is_empty( void ) const {
		M_PROLOG
		return ( ! _size );
		M_EPILOG
	}
	void clear( void ) {
		M_PROLOG
		HAtom** buckets = _buckets.get<HAtom*>();
		for ( int long i = 0; i < _prime; i ++ ) {
			HAtom* atom( buckets[ i ] );
			while ( atom ) {
				HAtom* del( atom );
				atom = atom->_next;
				M_SAFE( delete del );
			}
			buckets[ i ] = NULL;
		}
		_size = 0;
		return;
		M_EPILOG
	}
	hasher_type const& hasher( void ) const {
		return ( _hasher );
	}
	HIterator find( key_type const& ) const;
	HPair<HIterator, bool> insert( value_type const& );
	HIterator begin( void ) const {
		M_PROLOG
		HIterator it( this, 0, NULL );
		return ( ++ it );
		M_EPILOG
	}
	HIterator end( void ) const {
		M_PROLOG
		return ( HIterator( this, _prime, NULL ) );
		M_EPILOG
	}
	void resize( int long );
	void erase( HIterator const& it ) {
		M_PROLOG
		HAtom** buckets = _buckets.get<HAtom*>();
		HAtom* atom( buckets[ it._index ] );
		HAtom* ancestor( NULL );
		M_ASSERT( atom );
		while ( atom != it._atom )
			ancestor = atom, atom = atom->_next;
		if ( atom ) {
			if ( ancestor )
				ancestor->_next = atom->_next;
			else
				buckets[ it._index ] = atom->_next;
			M_SAFE( delete atom );
			-- _size;
		}
		return;
		M_EPILOG
	}
	void copy_from( HHashContainer const& src_ ) {
		M_PROLOG
		HChunk newBuckets( src_._buckets.get_size(), HChunk::STRATEGY::EXACT );
		HAtom const* const* otherBuckets = src_._buckets.template get<HAtom const*>();
		HAtom** buckets = newBuckets.get<HAtom*>();
		for ( int long i( 0 ); i < src_._prime; ++ i ) {
			HAtom const* origAtom( otherBuckets[ i ] );
			while ( origAtom ) {
				HAtom* atom( new HAtom( origAtom->_value ) );
				origAtom = origAtom->_next;
				atom->_next = buckets[ i ];
				buckets[ i ] = atom;
			}
		}
		clear();
		_prime = src_._prime;
		_size = src_._size;
		_buckets.swap( newBuckets );
		return;
		M_EPILOG
	}
	void swap( HHashContainer& hc_ ) {
		if ( &hc_ != this ) {
			using yaal::swap;
			swap( _prime, hc_._prime );
			swap( _size, hc_._size );
			swap( _buckets, hc_._buckets );
		}
		return;
	}
};

template<typename value_t, typename hasher_t, typename get_key_t, typename allocator_t>
typename HHashContainer<value_t, hasher_t, get_key_t, allocator_t>::HIterator
HHashContainer<value_t, hasher_t, get_key_t, allocator_t>::find( key_type const& key_ ) const {
	M_PROLOG
	int long idx( 0 );
	HAtom* atom( NULL );
	if ( _prime ) {
		idx = yaal::abs( _hasher( key_ ) ) % _prime;
		atom = _buckets.get<HAtom*>()[ idx ];
		while ( atom && ! ( get_key_type::key( atom->_value ) == key_ ) )
			atom = atom->_next;
	}
	return ( atom ? HIterator( this, idx, atom ) : end() );
	M_EPILOG
}

template<typename value_t, typename hasher_t, typename get_key_t, typename allocator_t>
HPair<typename HHashContainer<value_t, hasher_t, get_key_t, allocator_t>::HIterator, bool>
HHashContainer<value_t, hasher_t, get_key_t, allocator_t>::insert( value_type const& val_ ) {
	M_PROLOG
	HIterator it( _prime ? find( get_key_type::key( val_ ) ) : end() );
	bool inserted( false );
	if ( it == end() ) {
		if ( ( _size + 1 ) > _prime )
			resize( ( _size + 1 ) * 2 );
		HAtom* atom = new ( memory::yaal ) HAtom( val_ );

		/* I cannot use index calculated in find() call above because here we use different prime.
		 */
		int long newHash = yaal::abs( _hasher( get_key_type::key( val_ ) ) ) % _prime;
		HAtom** buckets = _buckets.get<HAtom*>();
		atom->_next = buckets[ newHash ];
		buckets[ newHash ] = atom;
		++ _size;
		it = HIterator( this, newHash, atom );
		inserted = true;
	}
	return ( make_pair( it, inserted ) );
	M_EPILOG
}

template<typename value_t, typename hasher_t, typename get_key_t, typename allocator_t>
void HHashContainer<value_t, hasher_t, get_key_t, allocator_t>::resize( int long size_ ) {
	M_PROLOG
	if ( size_ < 1 )
		M_THROW( "bad new container size", size_ );
	if ( size_ > _size ) {
		int n = 0;
		while ( size_ ) {
			size_ >>= 1;
			n ++;
		}
		int long prime( _primes_[ n - 1 ] );
		HChunk buckets( chunk_size<HAtom*>( prime ), HChunk::STRATEGY::GEOMETRIC );
		M_ASSERT( ( buckets.size() / static_cast<int long>( sizeof ( HAtom* ) ) ) >= prime );
		HAtom** oldBuckets = _buckets.get<HAtom*>();
		HAtom** newBuckets = buckets.get<HAtom*>();
		for ( int long i( 0 ); i < _prime; ++ i ) {
			HAtom* a( oldBuckets[ i ] );
			while ( a ) {
				HAtom* atom( a );
				a = a->_next;
				int long newHash( yaal::abs( _hasher( get_key_type::key( atom->_value ) ) ) % prime );
				atom->_next = newBuckets[ newHash ];
				newBuckets[ newHash ] = atom;
			}
		}
		_buckets.swap( buckets );
		_prime = prime;
	}
	return;
	M_EPILOG
}

}

template<typename value_t, typename hasher_t, typename get_key_t, typename allocator_t>
inline void swap( yaal::hcore::HHashContainer<value_t, hasher_t, get_key_t, allocator_t>& a, yaal::hcore::HHashContainer<value_t, hasher_t, get_key_t, allocator_t>& b )
	{ a.swap( b );	}

}

#endif /* #ifndef YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED */

