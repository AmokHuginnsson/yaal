/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hhashcontainer.hxx
 * \brief Declaration of HHashContainer<>, a helper for HHash* and HMultiHash* classes.
 */

#ifndef YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED
#define YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED 1

#include "hcore/memory.hxx"
#include "hcore/trait.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hpair.hxx"
#include "hcore/allocator.hxx"

namespace yaal {

namespace hcore {

template<typename key_t>
struct hash {
	typedef hash_value_t hash_value_type;
	hash_value_type operator () ( key_t const& ) const;
};

template<typename key_t>
struct hash<key_t*> {
	typedef hash_value_t hash_value_type;
	hash_value_type operator () ( key_t* const& key_ ) const {
		return ( reinterpret_cast<hash_value_type>( key_ ) );
	}
};

template<typename first_t, typename second_t>
struct hash<yaal::hcore::HPair<first_t, second_t>> {
	typedef hash_value_t hash_value_type;
	hash_value_type operator () ( yaal::hcore::HPair<first_t, second_t> const& key_ ) const {
		return ( hash<first_t>()( key_.first ) * 3 + hash<second_t>()( key_.second ) );
	}
};

template<typename key_t, typename TAG>
struct hash<yaal::hcore::HTaggedPOD<key_t, TAG>> {
	typedef hash_value_t hash_value_type;
	hash_value_type operator () ( HTaggedPOD<key_t, TAG> const& key_ ) const {
		return ( hash<key_t>()( key_.get() ) );
	}
};

extern M_YAAL_HCORE_PUBLIC_API int long const* const _primes_;

template<typename value_t, typename hasher_t, typename equal_key_t, typename get_key_t, typename allocator_t>
class HHashContainer final {
public:
	typedef HHashContainer this_type;
	typedef value_t value_type;
	typedef hasher_t hasher_type;
	typedef equal_key_t equal_key_type;
	typedef get_key_t get_key_type;
	typedef typename get_key_type::key_type key_type;
	typedef int long size_type;
private:
#ifndef __sun__
#pragma pack( push, 1 )
#else /* #ifndef __sun__ */
#pragma pack( 1 )
#endif /* #else #ifndef __sun__ */
	class HAtom {
	private: /* for UT */
		HAtom* _next;
		value_type _value;
		HAtom( value_type const& value_ )
			: _next( nullptr )
			, _value( value_ ) {
			return;
		}
		HAtom( value_type&& value_ ) noexcept
			: _next( nullptr )
			, _value( yaal::move( value_ ) ) {
			return;
		}
		HAtom( HAtom const& );
		HAtom& operator = ( HAtom const& );
		friend class HHashContainer;
		friend class HIterator;
	};
#ifndef __sun__
#pragma pack( pop )
#else /* #ifndef __sun__ */
#pragma pack()
#endif /* #else #ifndef __sun__ */
	struct constructor_copy {
		value_type const& _value;
		constructor_copy( value_type const& val_ )
			: _value( val_ ) {
		}
		void construct( HAtom* atom ) {
			new ( atom ) HAtom( _value );
		}
	};
	struct constructor_move {
		value_type& _value;
		constructor_move( value_type& val_ )
			: _value( val_ ) {
		}
		void construct( HAtom* atom ) {
			new ( atom ) HAtom( yaal::move( _value ) );
		}
	};
public:
	typedef typename allocator_t::template rebind<HAtom>::other allocator_type;
	typedef trait::delayed_sizeof<HAtom> node_size;
	typedef HAtom node_type;
	/*! \brief Iterator for HHashContainer data structure.
	 */
	class HIterator {
		typedef HHashContainer<value_type, hasher_type, equal_key_type, get_key_type, allocator_t> owner_t;
		owner_t const* _owner;
		size_type _index;
		typename owner_t::HAtom* _atom;
	public:
		HIterator( void )
			: _owner( nullptr )
			, _index( 0 )
			, _atom( nullptr ) {
		}
		HIterator( HIterator const& it_ )
			: _owner( it_._owner )
			, _index( it_._index )
			, _atom( it_._atom ) {
		}
		HIterator( HIterator&& ) noexcept = default;
		HIterator& operator = ( HIterator const& it_ ) {
			if ( &it_ != this ) {
				_owner = it_._owner;
				_index = it_._index;
				_atom = it_._atom;
			}
			return ( *this );
		}
		HIterator& operator = ( HIterator&& ) noexcept = default;
		HIterator& operator ++ ( void ) {
			M_ASSERT( _owner );
			size_type oldIndex( _index );
			if ( _atom ) {
				_atom = _atom->_next;
				if ( ! _atom ) {
					++ _index;
				}
			}
			if ( ! _atom ) {
				typename owner_t::HAtom* const* buckets = _owner->_buckets.template get<typename owner_t::HAtom*>();
				if ( oldIndex == _owner->_prime ) {
					_index = 0;
				}
				while ( ( _index < _owner->_prime ) && ! buckets[ _index ] ) {
					++ _index;
				}
				if ( _index < _owner->_prime ) {
					_atom = buckets[ _index ];
				}
			}
			if ( ! _atom ) {
				_index = _owner->_prime;
			}
			return ( *this );
		}
		HIterator& operator -- ( void ) {
			M_ASSERT( _owner );
			typename owner_t::HAtom* const* buckets = _owner->_buckets.template get<typename owner_t::HAtom*>();
			if ( _atom ) {
				typename owner_t::HAtom* atom( buckets[ _index ] );
				while ( ( atom != _atom ) && ( atom->_next != _atom ) ) {
					atom = atom->_next;
				}
				if ( atom == _atom ) {
					_atom = nullptr;
				} else {
					_atom = atom;
				}
			}
			if ( ! _atom ) {
				if ( _index > 0 ) {
					-- _index;
					while ( ( _index > 0 ) && ! buckets[ _index ] ) {
						-- _index;
					}
					_atom = buckets[ _index ];
					while ( _atom && _atom->_next ) {
						_atom = _atom->_next;
					}
					if ( ! _atom ) {
						_index = _owner->_prime;
					}
				} else {
					_index = _owner->_prime;
				}
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
		bool operator != ( HIterator const& it ) const {
			M_ASSERT( _owner == it._owner );
			return ( ( _atom != it._atom ) || ( _index != it._index ) );
		}
		void const* node_id( void ) const {
			return ( _atom );
		}
	private:
		friend class HHashContainer<value_type, hasher_type, equal_key_type, get_key_type, allocator_t>;
		explicit HIterator( owner_t const* owner_, size_type index_, typename owner_t::HAtom* atom_ )
			: _owner( owner_ )
			, _index( index_ )
			, _atom( atom_ ) {
		}
	};
private:
	size_type _prime;
	size_type _size;
	HChunk _buckets;
	hasher_type _hasher;
	equal_key_type _equals;
	allocator_type _allocator;
public:
	HHashContainer( hasher_type const& hasher_, equal_key_type const& equals_, allocator_type const& allocator_ )
		: _prime( 0 )
		, _size( 0 )
		, _buckets()
		, _hasher( hasher_ )
		, _equals( equals_ )
		, _allocator( allocator_ ) {
	}
	~HHashContainer( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	allocator_type const& get_allocator( void ) const {
		return ( _allocator );
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
		HAtom** buckets = _buckets.get<HAtom*>();
#	pragma GCC diagnostic push
#if __GCC_VERSION__ > 0
#	pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif /* #if __GCC_VERSION__ > 0 */
		for ( size_type i = 0; i < _prime; i ++ ) {
#	pragma GCC diagnostic pop
			HAtom* atom( buckets[ i ] );
			while ( atom ) {
				HAtom* del( atom );
				atom = atom->_next;
				M_SAFE( del->~HAtom() );
				_allocator.deallocate( del, 1 );
			}
			buckets[ i ] = nullptr;
		}
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
		size_type idx( 0 );
		HAtom* atom( nullptr );
		if ( _prime ) {
			idx = static_cast<size_type>(
				static_cast<hash_value_t>( _hasher( key_ ) ) % static_cast<hash_value_t>( _prime )
			);
			atom = _buckets.get<HAtom*>()[ idx ];
			while ( atom && ! _equals( get_key_type::key( atom->_value ), key_ ) ) {
				atom = atom->_next;
			}
		}
		return ( atom ? HIterator( this, idx, atom ) : end() );
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
		HIterator it( this, _prime, nullptr );
		return ( ++ it );
		M_EPILOG
	}
	HIterator end( void ) const {
		M_PROLOG
		return ( HIterator( this, _prime, nullptr ) );
		M_EPILOG
	}
	void resize( size_type size_ ) {
		M_PROLOG
		if ( size_ < 1 )
			M_THROW( "bad new container size", size_ );
		if ( size_ > _size ) {
			int n = 0;
			while ( size_ ) {
				size_ >>= 1;
				n ++;
			}
			size_type prime( _primes_[ n - 1 ] );
			HChunk buckets( chunk_size<HAtom*>( prime ), HChunk::STRATEGY::GEOMETRIC );
			M_ASSERT( ( buckets.size() / static_cast<size_type>( sizeof ( HAtom* ) ) ) >= prime );
			HAtom** oldBuckets = _buckets.get<HAtom*>();
			HAtom** newBuckets = buckets.get<HAtom*>();
			for ( size_type i( 0 ); i < _prime; ++ i ) {
				HAtom* a( oldBuckets[ i ] );
				while ( a ) {
					HAtom* atom( a );
					a = a->_next;
					size_type newHash(
						static_cast<size_type>(
							static_cast<hash_value_t>( _hasher( get_key_type::key( atom->_value ) ) ) % static_cast<hash_value_t>( prime )
						)
					);
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
	void erase( HIterator const& it ) {
		M_PROLOG
		HAtom** buckets = _buckets.get<HAtom*>();
		HAtom* atom( buckets[ it._index ] );
		HAtom* ancestor( nullptr );
		M_ASSERT( atom );
		while ( atom != it._atom ) {
			ancestor = atom, atom = atom->_next;
		}
		if ( atom ) {
			if ( ancestor ) {
				ancestor->_next = atom->_next;
			} else {
				buckets[ it._index ] = atom->_next;
			}
			M_SAFE( atom->~HAtom() );
			_allocator.deallocate( atom, 1 );
			-- _size;
		}
		return;
		M_EPILOG
	}
	void copy_from( HHashContainer const& src_ ) {
		M_PROLOG
		if ( src_._size > 0 ) {
			HChunk newBuckets( src_._buckets.get_size(), HChunk::STRATEGY::EXACT );
			HAtom const* const* otherBuckets = src_._buckets.template get<HAtom const*>();
			HAtom** buckets = newBuckets.get<HAtom*>();
			for ( size_type i( 0 ); i < src_._prime; ++ i ) {
				HAtom const* origAtom( otherBuckets[ i ] );
				while ( origAtom ) {
					HAtom* atom( _allocator.allocate( 1 ) );
					new ( atom ) HAtom( origAtom->_value );
					origAtom = origAtom->_next;
					atom->_next = buckets[ i ];
					buckets[ i ] = atom;
				}
			}
			clear();
			_prime = src_._prime;
			_size = src_._size;
			_buckets.swap( newBuckets );
		} else {
			clear();
		}
		return;
		M_EPILOG
	}
	void swap( HHashContainer& hc_ ) {
		if ( &hc_ != this ) {
			using yaal::swap;
			swap( _prime, hc_._prime );
			swap( _size, hc_._size );
			swap( _buckets, hc_._buckets );
			swap( _hasher, hc_._hasher );
			swap( _equals, hc_._equals );
			swap( _allocator, hc_._allocator );
		}
		return;
	}
private:
	HHashContainer( HHashContainer const& ) = delete;
	HHashContainer& operator = ( HHashContainer const& ) = delete;
	template<typename constructor_t>
	HPair<HIterator, bool> insert_impl( constructor_t&& constructor_ ) {
		M_PROLOG
		HIterator it( _prime ? find( get_key_type::key( constructor_._value ) ) : end() );
		bool inserted( false );
		if ( ! ( it != end() ) ) {
			if ( ( _size + 1 ) > _prime ) {
				resize( ( _size + 1 ) * 2 );
			}

			size_type newHash(
				static_cast<size_type>(
					static_cast<hash_value_t>( _hasher( get_key_type::key( constructor_._value ) ) ) % static_cast<hash_value_t>( _prime )
				)
			);

			HAtom* atom( _allocator.allocate( 1 ) );
			try {
				constructor_.construct( atom );
			} catch ( ... ) {
				_allocator.deallocate( atom, 1 );
				throw;
			}

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
};

template<typename value_t, typename hasher_t, typename equal_key_t, typename get_key_t, typename allocator_t>
inline void swap(
	yaal::hcore::HHashContainer<value_t, hasher_t, equal_key_t, get_key_t, allocator_t>& a,
	yaal::hcore::HHashContainer<value_t, hasher_t, equal_key_t, get_key_t, allocator_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED */

