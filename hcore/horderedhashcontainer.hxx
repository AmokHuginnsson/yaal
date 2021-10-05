/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hhashcontainer.hxx
 * \brief Declaration of HOrderedHashContainer<>, a helper for HHash* and HMultiHash* classes.
 */

#ifndef YAAL_HCORE_HORDEREDHASHCONTAINER_HXX_INCLUDED
#define YAAL_HCORE_HORDEREDHASHCONTAINER_HXX_INCLUDED 1

#include "hcore/memory.hxx"
#include "hcore/trait.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hpair.hxx"
#include "hcore/hash.hxx"
#include "hcore/allocator.hxx"
#include "hcore/bit.hxx"

namespace yaal {

namespace hcore {

template<typename value_t, typename hasher_t, typename equal_key_t, typename get_key_t>
class HOrderedHashContainer final {
public:
	typedef HOrderedHashContainer this_type;
	typedef value_t value_type;
	typedef hasher_t hasher_type;
	typedef equal_key_t equal_key_type;
	typedef get_key_t get_key_type;
	typedef typename get_key_type::key_type key_type;
	typedef int long size_type;
	static hash_value_t const EMPTY_SLOT = meta::max_unsigned<hash_value_t>::value;
	static hash_value_t const HASH_MASK = EMPTY_SLOT >> 1;
	static hash_value_t const HASH_PERTURBATION_SHIFT = 5;
	static size_type const START_PRIME = 7;
	static size_type const INVALID = -1;
	static size_type const ERASED  = -2;
private:
	class HAtom {
	private: /* for UT */
		hash_value_t _hash;
		value_type _value;
		HAtom( hash_value_t hash_, value_type const& value_ )
			: _hash( hash_ )
			, _value( value_ ) {
			return;
		}
		HAtom( hash_value_t hash_, value_type&& value_ ) noexcept
			: _hash( hash_ )
			, _value( yaal::move( value_ ) ) {
			return;
		}
		HAtom( HAtom const& );
		HAtom& operator = ( HAtom const& );
		friend class HOrderedHashContainer;
		friend class HIterator;
	};
	struct constructor_copy {
		value_type const& _value;
		constructor_copy( value_type const& val_ )
			: _value( val_ ) {
		}
		void construct( hash_value_t hash_, HAtom* atom ) {
			new ( atom ) HAtom( hash_, _value );
		}
	};
	struct constructor_move {
		value_type& _value;
		constructor_move( value_type& val_ )
			: _value( val_ ) {
		}
		void construct( hash_value_t hash_, HAtom* atom ) {
			new ( atom ) HAtom( hash_, yaal::move( _value ) );
		}
	};
	static void atom_copy( HAtom* dst_, HAtom* src_ ) {
		new ( dst_ ) HAtom( src_->_hash,  src_->_value );
	}
	static void atom_move( HAtom* dst_, HAtom* src_ ) {
		new ( dst_ ) HAtom( src_->_hash,  yaal::move( src_->_value ) );
		M_SAFE( src_->~HAtom() );
		src_->_hash = EMPTY_SLOT;
	}
	static void atom_safe_move( HAtom* dst_, HAtom* src_ ) {
		if ( dst_ == src_ ) {
			return;
		}
		new ( dst_ ) HAtom( src_->_hash,  yaal::move( src_->_value ) );
		M_SAFE( src_->~HAtom() );
		src_->_hash = EMPTY_SLOT;
	}
	typedef void ( *atom_func_t )( HAtom*, HAtom* );
	struct Projection {
		size_type mapIndex;
		size_type storeIndex;
		Projection( size_type position_, size_type lookup_ )
			: mapIndex( position_ )
			, storeIndex( lookup_ ) {
		}
		Projection( Projection const& ) = default;
		Projection& operator = ( Projection const& ) = default;
	};
public:
	typedef trait::no_type allocator_type;
	typedef trait::delayed_sizeof<HAtom> node_size;
	typedef HAtom node_type;
	/*! \brief Iterator for HOrderedHashContainer data structure.
	 */
	class HIterator {
		typedef HOrderedHashContainer<value_type, hasher_type, equal_key_type, get_key_type> owner_t;
		owner_t const* _owner;
		size_type _index;
	public:
		HIterator( void )
			: _owner( nullptr )
			, _index( 0 ) {
		}
		HIterator( HIterator const& it_ )
			: _owner( it_._owner )
			, _index( it_._index ) {
		}
		HIterator( HIterator&& ) noexcept = default;
		HIterator& operator = ( HIterator const& it_ ) {
			if ( &it_ != this ) {
				_owner = it_._owner;
				_index = it_._index;
			}
			return ( *this );
		}
		HIterator& operator = ( HIterator&& ) noexcept = default;
		HIterator& operator ++ ( void ) {
			M_ASSERT( _owner );
			typename owner_t::HAtom const* store = _owner->_store.template get<typename owner_t::HAtom>();
			size_type end( _owner->_size + _owner->_erased );
			if ( _index == end ) {
				_index = -1;
			}
			++ _index;
			while ( _index < end ) {
				if ( store[_index]._hash != EMPTY_SLOT ) {
					break;
				}
				++ _index;
			}
			return ( *this );
		}
		HIterator& operator -- ( void ) {
			M_ASSERT( _owner );
			typename owner_t::HAtom const* store = _owner->_store.template get<typename owner_t::HAtom>();
			size_type end( _owner->_size + _owner->_erased );
			if ( _index == 0 ) {
				_index = -1;
				return ( *this );
			}
			-- _index;
			while ( _index >= 0 ) {
				if ( store[_index]._hash != EMPTY_SLOT ) {
					break;
				}
				-- _index;
			}
			if ( _index < 0 ) {
				_index = end;
			}
			return ( *this );
		}
		value_type& get( void ) const {
			typename owner_t::HAtom const* store = _owner->_store.template get<typename owner_t::HAtom>();
			M_ASSERT( ( _index >= 0 ) && ( _index < ( _owner->_size + _owner->_erased ) ) && ( store[_index]._hash != EMPTY_SLOT ) );
			return ( const_cast<owner_t::HAtom*>( store )[_index]._value );
		}
		bool operator == ( HIterator const& it ) const {
			M_ASSERT( _owner == it._owner );
			return ( _index == it._index );
		}
		bool operator != ( HIterator const& it ) const {
			M_ASSERT( _owner == it._owner );
			return ( _index != it._index );
		}
		void const* node_id( void ) const {
			typename owner_t::HAtom const* store = _owner->_store.template get<typename owner_t::HAtom>();
			M_ASSERT( ( _index >= 0 ) && ( _index < ( _owner->_size + _owner->_erased ) ) && ( store[_index]._hash != EMPTY_SLOT ) );
			return ( reinterpret_cast<void*>( static_cast<int_native_t>( _index ) ) );
		}
	private:
		friend class HOrderedHashContainer<value_type, hasher_type, equal_key_type, get_key_type>;
		explicit HIterator( owner_t const* owner_, size_type index_ )
			: _owner( owner_ )
			, _index( index_ ) {
		}
	};
private:
	HChunk _store;
	HChunk _indexMap;
	size_type _prime;
	size_type _cover;
	size_type _size;
	size_type _erased;
	hasher_type _hasher;
	equal_key_type _equals;
public:
	HOrderedHashContainer( hasher_type const& hasher_, equal_key_type const& equals_, allocator_type const& )
		: _store()
		, _indexMap()
		, _prime( 0 )
		, _cover( 0 )
		, _size( 0 )
		, _erased( 0 )
		, _hasher( hasher_ )
		, _equals( equals_ ) {
		provide_storage( START_PRIME );
		grow_index_map( START_PRIME );
	}
	~HOrderedHashContainer( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		static allocator_type const allocator;
		return ( allocator );
	}
	size_type get_size( void ) const {
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
		HAtom* store = _store.get<HAtom>();
#	pragma GCC diagnostic push
#if __GCC_VERSION__ > 0
#	pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif /* #if __GCC_VERSION__ > 0 */
		for ( size_type i( _size + _erased - 1 ); i >= 0; -- i ) {
#	pragma GCC diagnostic pop
			HAtom* atom( store + i );
			if ( atom->_hash == EMPTY_SLOT ) {
				continue;
			}
			M_SAFE( atom->~HAtom() );
			atom->_hash = EMPTY_SLOT;
		}
		size_type* indexMap( _indexMap.get<size_type>() );
		fill( indexMap, indexMap + _indexMap.count_of<size_type>(), INVALID + 0 );
		_erased = 0;
		_size = 0;
		return;
		M_EPILOG
	}
	hasher_type const& hasher( void ) const {
		return ( _hasher );
	}
	equal_key_type const& equal_key( void ) const {
		return ( _equals );
	}
	HIterator find( key_type const& key_ ) const {
		M_PROLOG
		if ( _size == 0 ) {
			return end();
		}
		Projection projection( find_projection( key_, static_cast<hash_value_t>( _hasher( key_ ) ) & HASH_MASK ) );
		return projection.storeIndex != INVALID ? HIterator( this, projection.storeIndex ) : end();
		M_EPILOG
	}
	HPair<HIterator, bool> insert( value_type const& val_ ) {
		return ( insert_impl( constructor_copy( val_ ) ) );
	}
	HPair<HIterator, bool> insert( value_type&& val_ ) {
		return ( insert_impl( constructor_move( val_ ) ) );
	}
	HIterator begin( void ) const {
		M_PROLOG
		HIterator it( this, _size + _erased );
		return ( ++ it );
		M_EPILOG
	}
	HIterator end( void ) const {
		M_PROLOG
		return ( HIterator( this, _size + _erased ) );
		M_EPILOG
	}
	void resize( size_type size_ ) {
		M_PROLOG
		if ( size_ < 1 ) {
			M_THROW( "bad new container size", size_ );
		}
		if ( grow_index_map( size_ ) ) {
			rebuild_index();
		}
		return;
		M_EPILOG
	}
	void compact( void ) {
		M_PROLOG
		if ( ( _size / 2 ) > _erased ) {
			return;
		}
		HAtom* store = _store.get<HAtom>();
#ifndef NDEBUG
		size_type copiedCount =
#endif
		copy_store( store, store, _size + _erased, atom_safe_move );
		M_ASSERT( copiedCount == _size );
		_erased = 0;
		rebuild_index();
		return;
		M_EPILOG
	}
	void erase( HIterator const& it ) {
		M_PROLOG
		HAtom* store = _store.get<HAtom>();
		HAtom* atom( store + it._index );
		M_ASSERT( atom->_hash != EMPTY_SLOT );
		hash_value_t hash( atom->_hash );
		M_SAFE( atom->~HAtom() );
		atom->_hash = EMPTY_SLOT;
		size_type* indexMap( _indexMap.get<size_type>() );
		hash_value_t pertubator( hash );
		hash_value_t mapIndex( hash % static_cast<hash_value_t>( _prime ) );
		while ( true ) {
			size_type storeIndex( indexMap[mapIndex] );
			M_ASSERT( storeIndex != INVALID );
			if ( storeIndex == it._index ) {
				indexMap[mapIndex] = ERASED;
				break;
			}
			pertubator >>= HASH_PERTURBATION_SHIFT;
			mapIndex = ( 5 * mapIndex + 1 + pertubator ) % static_cast<hash_value_t>( _cover );
		}
		-- _size;
		++ _erased;
		return;
		M_EPILOG
	}
	void copy_from( HOrderedHashContainer const& src_ ) {
		M_PROLOG
		clear();
		if ( src_._size == 0 ) {
			return;
		}
		provide_storage( src_._size );
		HAtom const* srcStore = src_._store.template get<HAtom>();
		HAtom* store = _store.get<HAtom>();

		_size = copy_store( store, const_cast<HAtom*>( srcStore ), src_._size + src_._erased, atom_copy );

		grow_index_map( _size );
		rebuild_index();
		return;
		M_EPILOG
	}
	void swap( HOrderedHashContainer& hc_ ) {
		if ( &hc_ == this ) {
			return;
		}
		using yaal::swap;
		swap( _store, hc_._store );
		swap( _indexMap, hc_._indexMap );
		swap( _prime, hc_._prime );
		swap( _cover, hc_._cover );
		swap( _size, hc_._size );
		swap( _erased, hc_._erased );
		swap( _hasher, hc_._hasher );
		swap( _equals, hc_._equals );
		return;
	}
private:
	HOrderedHashContainer( HOrderedHashContainer const& ) = delete;
	HOrderedHashContainer& operator = ( HOrderedHashContainer const& ) = delete;
	template<typename constructor_t>
	HPair<HIterator, bool> insert_impl( constructor_t&& constructor_ ) {
		M_PROLOG
		key_type const& key( get_key_type::key( constructor_._value ) );
		hash_value_t hashValue( static_cast<hash_value_t>( _hasher( key ) ) & HASH_MASK );
		Projection projection( find_projection( key, hashValue ) );
		if ( projection.storeIndex != INVALID ) {
			return ( make_pair( HIterator( this, projection.storeIndex ), false ) );
		}
		size_type next( _size + _erased  );
		bool storageExpanded( provide_storage( next + 1 ) );
		if ( grow_index_map( next + 1 ) || storageExpanded ) {
			rebuild_index();
			next = _size + _erased;
			projection = find_projection( key, hashValue );
		}
		M_ASSERT( next < _store.count_of<HAtom>() );
		M_ASSERT( projection.storeIndex == INVALID );
		HAtom* store = _store.get<HAtom>();
		size_type* indexMap( _indexMap.get<size_type>() );
		HAtom* atom( store + next );
		try {
			constructor_.construct( hashValue, atom );
		} catch ( ... ) {
			throw;
		}
		indexMap[projection.mapIndex] = next;
		++ _size;
		return ( make_pair( HIterator( this, next ), true ) );
		M_EPILOG
	}
	bool provide_storage( size_type size_ ) {
		M_PROLOG
		size_type storeSize( _store.count_of<HAtom>() );
		if ( storeSize >= size_ ) {
			return ( false );
		}
		int msb( bit::most_significant( static_cast<u64_t>( size_ ) ) );
		size_type newStoreSize( static_cast<size_type>( 1ULL << msb ) );
		HChunk newStore;
		if ( newStoreSize < size_ ) {
			newStoreSize *= 2;
		}
		M_ASSERT( newStoreSize >= size_ );
		newStore.realloc<HAtom>( newStoreSize );
		newStoreSize = newStore.count_of<HAtom>();
		M_ASSERT( newStoreSize >= size_ );
		HAtom const* srcStore = _store.get<HAtom>();
		HAtom* store = newStore.get<HAtom>();
#ifndef NDEBUG
		size_type copiedCount =
#endif
		copy_store( store, const_cast<HAtom*>( srcStore ), _size + _erased, atom_move );
		M_ASSERT( copiedCount == _size );
		_erased = 0;
		_store.swap( newStore );
		for ( size_type i( _size ); i < newStoreSize; ++ i ) {
			store[i]._hash = EMPTY_SLOT;
		}
		M_ASSERT( _store.count_of<HAtom>() >= size_ );
		return ( true );
		M_EPILOG
	}
	static size_type copy_store( HAtom* dstStore_, HAtom* srcStore_, size_type srcEnd_, atom_func_t atomFunc_ ) {
		M_PROLOG
		size_type size( 0 );
		for ( size_type i( 0 ); i < srcEnd_; ++ i ) {
			HAtom& srcAtom( srcStore_[i] );
			if ( srcAtom._hash == EMPTY_SLOT ) {
				continue;
			}
			atomFunc_( dstStore_ + size, &srcAtom );
			++ size;
		}
		return ( size );
		M_EPILOG
	}
	bool grow_index_map( size_type val_ ) {
		M_PROLOG
		if ( val_ <= ( _prime * 2 / 3 ) ) {
			return ( false );
		}
		int msb( bit::most_significant( static_cast<u64_t>( val_ ) * 3ULL / 2ULL + 1ULL ) );
		_prime = _primes_[ msb ];
		_cover = static_cast<size_type>( 1ULL << static_cast<u64_t>( msb + 1 ) );
		M_ASSERT( ( _cover > _prime ) && ( _cover < ( _prime << 1 ) ) && ( bit::count( static_cast<u64_t>( _cover ) ) == 1 ) );
		size_type indexMapSize( _indexMap.count_of<size_type>() );
		_indexMap.realloc<size_type>( _cover );
		if ( _cover > indexMapSize ) {
			size_type* indexMap( _indexMap.get<size_type>() );
			fill( indexMap + indexMapSize, indexMap + _cover, INVALID + 0 );
		}
		return ( true );
		M_EPILOG
	}
	void rebuild_index( void ) {
		M_PROLOG
		HAtom const* store = _store.get<HAtom>();
		size_type* indexMap( _indexMap.get<size_type>() );
		fill( indexMap, indexMap + _cover, INVALID + 0 );
		size_type endPos( _size + _erased  );
		for ( size_type i( 0 ); i < endPos; ++ i ) {
			HAtom const& atom( store[i] );
			if ( atom._hash == EMPTY_SLOT ) {
				continue;
			}
			hash_value_t pertubator( atom._hash );
			hash_value_t mapIndex( atom._hash % static_cast<hash_value_t>( _prime ) );
			size_type storeIndex( indexMap[mapIndex] );
			while ( true ) {
				if ( storeIndex == INVALID ) {
					indexMap[mapIndex] = i;
					break;
				}
				pertubator >>= HASH_PERTURBATION_SHIFT;
				mapIndex = ( 5 * mapIndex + 1 + pertubator ) % static_cast<hash_value_t>( _cover );
				storeIndex = indexMap[mapIndex];
			}
		}
		return;
		M_EPILOG
	}
	Projection find_projection( key_type const& key_, hash_value_t hashValue_ ) const {
		M_PROLOG
		HAtom const* store = _store.get<HAtom>();
		size_type const* indexMap( _indexMap.get<size_type>() );
		hash_value_t pertubator( hashValue_ );
		hash_value_t mapIndex( hashValue_ % static_cast<hash_value_t>( _prime ) );
		size_type storeIndex( indexMap[mapIndex] );
		size_type erasedSlot( INVALID );
		while ( true ) {
			if ( storeIndex == INVALID ) {
				return Projection( erasedSlot != INVALID ? erasedSlot : mapIndex, storeIndex );
			} else if ( storeIndex == ERASED ) {
				erasedSlot = static_cast<size_type>( mapIndex );
			} else {
				HAtom const& atom( store[storeIndex] );
				if ( ( atom._hash == hashValue_ ) && _equals( get_key_type::key( atom._value ), key_ ) ) {
					return Projection( mapIndex, storeIndex );
				}
			}
			pertubator >>= HASH_PERTURBATION_SHIFT;
			mapIndex = ( 5 * mapIndex + 1 + pertubator ) % static_cast<hash_value_t>( _cover );
			storeIndex = indexMap[mapIndex];
		}
		M_EPILOG
	}
};

template<typename value_t, typename hasher_t, typename equal_key_t, typename get_key_t>
inline void swap(
	yaal::hcore::HOrderedHashContainer<value_t, hasher_t, equal_key_t, get_key_t>& a,
	yaal::hcore::HOrderedHashContainer<value_t, hasher_t, equal_key_t, get_key_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HORDEREDHASHCONTAINER_HXX_INCLUDED */

