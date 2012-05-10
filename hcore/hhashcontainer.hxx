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

namespace yaal {

namespace hcore {

template<typename key_t>
inline int long hash( key_t const& key_ ) {
	return ( static_cast<int long>( key_ ) );
}

extern M_YAAL_HCORE_PUBLIC_API int long const* const _primes_;

class HHashContainer : private trait::HNonCopyable {
	class HAbstractAtom {
	private: /* for UT */
		HAbstractAtom* _next;
		HAbstractAtom( void ) : _next( NULL ) {}
		HAbstractAtom( HAbstractAtom const& );
		HAbstractAtom& operator = ( HAbstractAtom const& );
		virtual HAbstractAtom* clone( void ) const = 0;
		friend class HHashContainer;
		friend class HIterator;
	public:
		virtual ~HAbstractAtom( void ) {}
	};
public:
	/*! \brief Iterator for HHashContainer data structure.
	 */
	class HIterator {
		HHashContainer const* _owner;
		int long _index;
		HHashContainer::HAbstractAtom* _atom;
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
				HHashContainer::HAbstractAtom* const* buckets = _owner->_buckets.get<HHashContainer::HAbstractAtom*>();
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
			HHashContainer::HAbstractAtom* const* buckets = _owner->_buckets.get<HHashContainer::HAbstractAtom*>();
			if ( _atom ) {
				HHashContainer::HAbstractAtom* atom( buckets[ _index ] );
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
		template<typename tType>
		tType& get( void ) const {
			M_ASSERT( _atom );
			return ( static_cast<HHashContainer::HAtom<tType>*>( _atom )->_value );
		}
		bool operator == ( HIterator const& it ) const {
			M_ASSERT( _owner == it._owner );
			return ( ( _index == it._index ) && ( _atom == it._atom ) );
		}
		bool operator != ( HIterator const& it ) const
			{ return ( ! operator == ( it ) ); }
	private:
		friend class HHashContainer;
		explicit HIterator( HHashContainer const* owner_, int long index_, HHashContainer::HAbstractAtom* atom_ )
			: _owner( owner_ ), _index( index_ ), _atom( atom_ ) {};
	};
private:
	template<typename value_t>
	class HAtom : public HAbstractAtom {
	private: /* for UT */
		value_t _value;
		HAtom( value_t const& value_ ) : HAbstractAtom(), _value( value_ ) {}
		virtual HAbstractAtom* clone( void ) const
			{ return ( new ( memory::yaal ) HAtom( _value ) ); }
		friend class HHashContainer;
		friend class HIterator;
	};
	int long _prime;
	int _size;
	HChunk _buckets;
public:
	HHashContainer( void )
		: _prime( 0 ), _size( 0 ), _buckets() {
	}
	virtual ~HHashContainer( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
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
		HAbstractAtom** buckets = _buckets.get<HAbstractAtom*>();
		for ( int long i = 0; i < _prime; i ++ ) {
			HAbstractAtom* atom( buckets[ i ] );
			while ( atom ) {
				HAbstractAtom* del( atom );
				atom = atom->_next;
				M_SAFE( delete del );
			}
			buckets[ i ] = NULL;
		}
		_size = 0;
		return;
		M_EPILOG
	}
	template<typename key_t, typename hasher_t>
	HIterator find( key_t const&, hasher_t const& ) const;
	template<typename value_t, typename hasher_t>
	HPair<HIterator, bool> insert( value_t const&, hasher_t const& );
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
	template<typename hasher_t>
	void resize( int long, hasher_t const& );
	void erase( HIterator const& it ) {
		M_PROLOG
		HAbstractAtom** buckets = _buckets.get<HAbstractAtom*>();
		HAbstractAtom* atom( buckets[ it._index ] );
		HAbstractAtom* ancestor( NULL );
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
		HAbstractAtom const* const* otherBuckets = src_._buckets.get<HAbstractAtom const*>();
		HAbstractAtom** buckets = newBuckets.get<HAbstractAtom*>();
		for ( int long i( 0 ); i < src_._prime; ++ i ) {
			HAbstractAtom const* origAtom( otherBuckets[ i ] );
			while ( origAtom ) {
				HAbstractAtom* atom( origAtom->clone() );
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

template<typename key_t, typename hasher_t>
HHashContainer::HIterator HHashContainer::find( key_t const& key_, hasher_t const& hasher_ ) const {
	M_PROLOG
	int long idx( 0 );
	HAtom<typename hasher_t::value_type>* atom( NULL );
	if ( _prime ) {
		idx = yaal::abs( hasher_( key_ ) ) % _prime;
		atom = _buckets.get<HAtom<typename hasher_t::value_type>*>()[ idx ];
		while ( atom && ! hasher_( atom->_value, key_ ) )
			atom = static_cast<HAtom<typename hasher_t::value_type>*>( atom->_next );
	}
	return ( atom ? HIterator( this, idx, atom ) : end() );
	M_EPILOG
}

template<typename tType, typename hasher_t>
HPair<HHashContainer::HIterator, bool> HHashContainer::insert( tType const& val_, hasher_t const& hasher_ ) {
	M_PROLOG
	HIterator it( _prime ? find( val_, hasher_ ) : end() );
	bool inserted( false );
	if ( it == end() ) {
		if ( ( _size + 1 ) > _prime )
			resize( ( _size + 1 ) * 2, hasher_ );
		HAtom<typename hasher_t::value_type>* atom = new ( memory::yaal ) HAtom<typename hasher_t::value_type>( val_ );

		/* I cannot use index calculated in find() call above because here we use different prime.
		 */
		int long newHash = yaal::abs( hasher_( val_ ) ) % _prime;
		HAtom<typename hasher_t::value_type>** buckets = _buckets.get<HAtom<typename hasher_t::value_type>*>();
		atom->_next = buckets[ newHash ];
		buckets[ newHash ] = atom;
		++ _size;
		it = HIterator( this, newHash, atom );
		inserted = true;
	}
	return ( make_pair( it, inserted ) );
	M_EPILOG
}

template<typename hasher_t>
void HHashContainer::resize( int long size_, hasher_t const& hasher_ ) {
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
		HChunk buckets( chunk_size<HAtom<typename hasher_t::value_type>*>( prime ), HChunk::STRATEGY::GEOMETRIC );
		M_ASSERT( ( buckets.size() / static_cast<int long>( sizeof ( HAtom<typename hasher_t::value_type>* ) ) ) >= prime );
		HAtom<typename hasher_t::value_type>** oldBuckets = _buckets.get<HAtom<typename hasher_t::value_type>*>();
		HAtom<typename hasher_t::value_type>** newBuckets = buckets.get<HAtom<typename hasher_t::value_type>*>();
		for ( int long i( 0 ); i < _prime; ++ i ) {
			HAtom<typename hasher_t::value_type>* a( oldBuckets[ i ] );
			while ( a ) {
				HAtom<typename hasher_t::value_type>* atom( a );
				a = static_cast<HAtom<typename hasher_t::value_type>*>( a->_next );
				int long newHash( yaal::abs( hasher_( atom->_value ) ) % prime );
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

inline void swap( yaal::hcore::HHashContainer& a, yaal::hcore::HHashContainer& b )
	{ a.swap( b );	}

}

#endif /* #ifndef YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED */

