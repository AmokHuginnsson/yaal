/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashcontainer.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/hhashcontainer.hxx
 * \brief Declaration of HHashContainer<>, a halper for HHash* and HMultiHash* classes.
 */

#ifndef YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED
#define YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED 1

#include <new>

#include "hcore/trait.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hpair.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

template<typename key_t>
inline int long hash( key_t const& key_ )
	{
	return ( static_cast<int long>( key_ ) );
	}

extern int long const* const _primes_;

class HHashContainer : private trait::HNonCopyable
	{
public:
	class HIterator;
private:
	class HAbstractAtom
		{
	private: /* for UT */
		HAbstractAtom* _next;
		HAbstractAtom( void ) : _next( NULL ) {}
		HAbstractAtom( HAbstractAtom const& );
		virtual ~HAbstractAtom( void ) {}
		HAbstractAtom& operator = ( HAbstractAtom const& );
		virtual HAbstractAtom* clone( void ) const = 0;
		friend class HHashContainer;
		friend class HIterator;
		};
	template<typename value_t>
	class HAtom : public HAbstractAtom
		{
	private: /* for UT */
		value_t _value;
		HAtom( value_t const& value_ ) : HAbstractAtom(), _value( value_ ) {}
		virtual HAbstractAtom* clone( void ) const
			{ return ( new HAtom( _value ) ); }
		friend class HHashContainer;
		friend class HIterator;
		};
	int long _prime;
	int _size;
	HChunk _buckets;
public:
	HHashContainer( void );
	int long get_size( void ) const;
	bool is_empty( void ) const;
	void clear( void );
	template<typename key_t, typename hasher_t>
	HIterator find( key_t const&, hasher_t const& ) const;
	template<typename value_t, typename hasher_t>
	HPair<HIterator, bool> insert( value_t const&, hasher_t const& );
	void erase( HIterator const& );
	HIterator begin( void ) const;
	HIterator end( void ) const;
	HIterator rbegin( void ) const;
	HIterator rend( void ) const;
	template<typename hasher_t>
	void resize( int long, hasher_t const& );
	void copy_from( HHashContainer const& );
	void swap( HHashContainer& );
	};

/*! \brief Iterator for HHashContainer data structure.
 */
class HHashContainer::HIterator
	{
	HHashContainer const* _owner;
	int long _index;
	HHashContainer::HAbstractAtom* _atom;
public:
	HIterator( void ) : _owner( NULL ), _index( 0 ), _atom( NULL ) {}
	HIterator( HIterator const& it_ )
		: _owner( it_._owner ), _index( it_._index ),
		_atom( it_._atom )
		{
		}
	HIterator& operator = ( HIterator const& it_ )
		{
		if ( &it_ != this )
			{
			_owner = it_._owner;
			_index = it_._index;
			_atom = it_._atom;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		if ( _atom )
			{
			_atom = _atom->_next;
			if ( ! _atom )
				_index ++;
			}
		if ( ! _atom )
			{
			HHashContainer::HAbstractAtom* const* buckets = _owner->_buckets.get<HHashContainer::HAbstractAtom*>();
			while ( ( _index < _owner->_prime ) && ! buckets[ _index ] )
				_index ++;
			if ( _index < _owner->_prime )
				_atom = buckets[ _index ];
			}
		if ( ! _atom )
			_index = _owner->_prime;
		return ( *this );
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( *this );
		operator ++ ();
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		return ( *this );
		}
	HIterator const operator -- ( int )
		{
		HIterator it( *this );
		operator -- ();
		return ( it );
		}
	template<typename tType>
	tType& get( void )
		{
		M_ASSERT( _atom );
		return ( static_cast<HHashContainer::HAtom<tType>*>( _atom )->_value );
		}
	template<typename tType>
	tType const& get( void ) const
		{
		M_ASSERT( _atom );
		return ( static_cast<HHashContainer::HAtom<tType> const*>( _atom )->_value );
		}
	bool operator == ( HIterator const& it ) const
		{
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

template<typename key_t, typename hasher_t>
HHashContainer::HIterator HHashContainer::find( key_t const& key_, hasher_t const& hasher_ ) const
	{
	M_PROLOG
	int long idx( 0 );
	HAtom<typename hasher_t::value_type>* atom( NULL );
	if ( _prime )
		{
		idx = yaal::abs( hasher_( key_ ) ) % _prime;
		atom = _buckets.get<HAtom<typename hasher_t::value_type>*>()[ idx ];
		while ( atom && ! hasher_( atom->_value, key_ ) )
			atom = static_cast<HAtom<typename hasher_t::value_type>*>( atom->_next );
		}
	return ( atom ? HIterator( this, idx, atom ) : end() );
	M_EPILOG
	}

template<typename tType, typename hasher_t>
HPair<HHashContainer::HIterator, bool> HHashContainer::insert( tType const& val_, hasher_t const& hasher_ )
	{
	M_PROLOG
	HIterator it( _prime ? find( val_, hasher_ ) : end() );
	bool inserted( false );
	if ( it == end() )
		{
		if ( ( _size + 1 ) > _prime )
			resize( ( _size + 1 ) * 2, hasher_ );
		HAtom<typename hasher_t::value_type>* atom = new ( std::nothrow ) HAtom<typename hasher_t::value_type>( val_ );
		if ( ! atom )
			M_THROW( "memory allocation error", errno );

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
void HHashContainer::resize( int long size_, hasher_t const& hasher_ )
	{
	M_PROLOG
	if ( size_ < 1 )
		M_THROW ( "bad new container size", size_ );
	if ( size_ > _size )
		{
		int n = 0;
		while ( size_ )
			{
			size_ >>= 1;
			n ++;
			}
		int long prime( _primes_[ n - 1 ] );
		HChunk buckets( chunk_size<HAtom<typename hasher_t::value_type>*>( prime ), HChunk::STRATEGY::GEOMETRIC );
		M_ASSERT( ( buckets.size() / static_cast<int long>( sizeof ( HAtom<typename hasher_t::value_type>* ) ) ) >= prime );
		HAtom<typename hasher_t::value_type>** oldBuckets = _buckets.get<HAtom<typename hasher_t::value_type>*>();
		HAtom<typename hasher_t::value_type>** newBuckets = buckets.get<HAtom<typename hasher_t::value_type>*>();
		for ( int long i( 0 ); i < _prime; ++ i )
			{
			HAtom<typename hasher_t::value_type>* a( oldBuckets[ i ] );
			while ( a )
				{
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

#endif /* not YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED */

