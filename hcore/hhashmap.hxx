/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashmap.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HHASHMAP_HXX_INCLUDED
#define YAAL_HCORE_HHASHMAP_HXX_INCLUDED

#include <new>

#include "hcore/hhashcontainer.hxx"
#include "hcore/hpair.hxx"
#include "hcore/hexception.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

extern int long const* const _primes_;

template<typename key_t>
inline int long hash( key_t const& key_ )
	{
	return ( static_cast<int long>( key_ ) );
	}

/*! \brief Hash map container implementation.
 */
template<typename key_t, typename data_t>
class HHashMap
	{
	typedef HHashMap<key_t, data_t> self_t;
public:
	typedef key_t key_type;
	typedef data_t data_type;
	typedef HPair<key_t, data_t> value_type;
	typedef int long ( *hasher_t )( key_t const& );
	template<typename const_qual_t>
	class HIterator;
private:
	class HAtom
		{
	private:
		HAtom* _next;
		value_type _value;
		HAtom( value_type const& );
		HAtom( HAtom const& );
		virtual ~HAtom( void );
		HAtom& operator = ( HAtom const& );
		friend class HHashMap<key_t, data_t>;
		template<typename const_qual_t>
		friend class HIterator;
		};
	hasher_t _hasher;
	HHashContainer _engine;
public:
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	HHashMap( void );
	/*! \brief Lower bound of size of map's table */
	HHashMap( int long );
	HHashMap( int long, hasher_t );
	template<typename iterator_t>
	HHashMap( iterator_t, iterator_t );
	template<typename iterator_t>
	HHashMap( iterator_t, iterator_t, int long );
	HHashMap( HHashMap const& );
	virtual ~HHashMap( void );
	HHashMap& operator = ( HHashMap const& );
	data_t& operator [] ( key_t const& );
	const_iterator begin( void ) const;
	iterator begin( void );
	const_iterator end( void ) const;
	iterator end( void );
	const_iterator find( key_t const& ) const;
	iterator find( key_t const& );
	HPair<iterator, bool> insert( value_type const& );
	void resize( int long );
	template<typename iterator_t>
	void insert( iterator_t, iterator_t );
	void erase( iterator );

	/*! \brief  Remove given key from map.
	 *
	 * \param  key Key to be removed.
	 * \return  Number of erased elements.
	 */
	int long erase( key_t const& key );
	int long count( key_t const& ) const;
	void clear( void );
	int long get_size( void ) const;
	int long size( void ) const;
	bool is_empty( void ) const;
	bool empty( void ) const;
	void swap( HHashMap& );
private:
	bool find( key_t const&, int long&, HAtom*& ) const;
	};


template<typename key_type_t, typename data_type_t>
template<typename const_qual_t>
class HHashMap<key_t, data_t>::HIterator
	{
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HHashMap<key_type, data_type, helper_t> map_t;
	HHashContainer::HIterator _engine;
public:
	HIterator( void ) : _engine() {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ ) : _engine( it_._engine )
		{
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
		}
	HIterator& operator = ( HIterator const& it_ )
		{
		if ( &it_ != this )
			_engine = it_._engine;
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		++ _engine;
		return ( *this );
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( _engine );
		++ _engine;
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		-- _engine;
		return ( *this );
		}
	HIterator const operator -- ( int )
		{
		HIterator it( _engine );
		-- _engine;
		return ( it );
		}
	const_qual_t& operator* ( void )
		{ return ( _engine.operator*<typename map_t::value_type>() ); }
	const_qual_t& operator* ( void ) const
		{ return ( _engine.operator*<typename map_t::value_type>() ); }
	const_qual_t* operator-> ( void )
		{ return ( &_engine.operator*<typename map_t::value_type>() ); }
	const_qual_t* operator-> ( void ) const
		{ return ( &_engine.operator*<typename map_t::value_type>() ); }
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{ return ( _engine == it._engine ); }
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{ return ( _engine != it._engine ); }
private:
	friend class HMap<key_type, data_type, helper_t>;
	template<typename other_const_qual_t>
	friend class HIterator;
	explicit HIterator( HHashContainer::HIterator const& it ) : _engine( it ) {};
	};

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>::HAtom::HAtom( value_type const& value_ )
	: _next( NULL ), _value( value_ )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>::HAtom::~HAtom( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>::HHashMap( int long size_ )
	: _hasher(  &hash ), _engine()
	{
	M_PROLOG
	_engine.resize( size_ );
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>::HHashMap( int long size_, hasher_t hasher_ )
	: _hasher(  hasher_ ), _engine()
	{
	M_PROLOG
	_engine.resize( size_ );
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
template<typename iterator_t>
HHashMap<key_t, data_t>::HHashMap( iterator_t first, iterator_t last )
	: _hasher(  &hash ), _engine()
	{
	M_PROLOG
	for ( ; first != last; ++ first )
		insert( *first );
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
template<typename iterator_t>
HHashMap<key_t, data_t>::HHashMap( iterator_t first, iterator_t last, int long size_ )
	: _hasher(  &hash ), _engine()
	{
	M_PROLOG
	resize( size_ );
	for ( ; first != last; ++ first )
		insert( *first );
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>::HHashMap( HHashMap const& map_ )
	: _hasher( map_._hasher ), _engine()
	{
	M_PROLOG
	_engine.resize( _size * 2 );
	HAtom const* const* otherBuckets( map_._buckets.template get<HAtom const*>() );
	HAtom** buckets( _buckets.get<HAtom*>() );
	for ( int long i( 0 ); i < map_._prime; ++ i )
		{
		HAtom const* origAtom( otherBuckets[ i ] );
		while ( origAtom )
			{
			HAtom* atom( new ( std::nothrow ) HAtom( origAtom->_value ) );
			origAtom = origAtom->_next;
			int long newHash( _hasher( atom->_value.first ) % _prime );
			atom->_next = buckets[ newHash ];
			buckets[ newHash ] = atom;
			}
		}
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>::~HHashMap( void )
	{
	M_PROLOG
	clear();
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>& HHashMap<key_t, data_t>::operator = ( HHashMap const& map_ )
	{
	M_PROLOG
	int i( 0 );
	if ( &map_ != this )
		{
		HHashMap tmp( map_ );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

template<typename key_t, typename data_t>
void HHashMap<key_t, data_t>::resize( int long size_ )
	{
	M_PROLOG
	if ( size_ < 1 )
		M_THROW ( "bad map size", size_ );
	if ( size_ > _size )
		{
		int n = 0;
		while ( size_ )
			{
			size_ >>= 1;
			n ++;
			}
		int long prime( _primes_[ n - 1 ] );
		HChunk buckets( chunk_size<HAtom*>( prime ), HChunk::STRATEGY::GEOMETRIC );
		M_ASSERT( ( buckets.size() / static_cast<int long>( sizeof ( HAtom* ) ) ) >= prime );
		HAtom** oldBuckets( _buckets.get<HAtom*>() );
		HAtom** newBuckets( buckets.get<HAtom*>() );
		for ( int long i( 0 ); i < _prime; ++ i )
			{
			HAtom* a( oldBuckets[ i ] );
			while ( a )
				{
				HAtom* atom( a );
				a = a->_next;
				int long newHash( _hasher( atom->_value.first ) % prime );
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

template<typename key_t, typename data_t>
void HHashMap<key_t, data_t>::clear( void )
	{
	M_PROLOG
	_engine.clear();
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
int long HHashMap<key_t, data_t>::get_size( void ) const
	{
	M_PROLOG
	return ( _engine.get_size() );
	M_EPILOG
	}

template<typename key_t, typename data_t>
int long HHashMap<key_t, data_t>::size( void ) const
	{
	M_PROLOG
	return ( _engine.get_size() );
	M_EPILOG
	}

template<typename key_t, typename data_t>
bool HHashMap<key_t, data_t>::is_empty( void ) const
	{
	M_PROLOG
	return ( _engine.is_empty() );
	M_EPILOG
	}

template<typename key_t, typename data_t>
bool HHashMap<key_t, data_t>::empty( void ) const
	{
	M_PROLOG
	return ( _engine.is_empty() );
	M_EPILOG
	}

template<typename key_t, typename data_t>
data_t& HHashMap<key_t, data_t>::operator[]( key_t const& key_ )
	{
	M_PROLOG
	return ( insert( make_pair( key_, data_t() ) ).first->second );
	M_EPILOG
	}

template<typename key_t, typename data_t>
yaal::hcore::HPair<typename HHashMap<key_t, data_t>::iterator, bool> HHashMap<key_t, data_t>::insert( value_type const& val_ )
	{
	M_PROLOG
	iterator it( _prime ? find( val_.first ) : end() );
	bool inserted( false );
	if ( it == end() )
		{
		if ( ( _size + 1 ) > _prime )
			resize( ( _size + 1 ) * 2 );
		HAtom* atom = new ( std::nothrow ) HAtom( val_ );
		if ( ! atom )
			M_THROW( "memory allocation error", errno );
		int long newHash = abs( _hasher( val_.first ) ) % _prime;
		HAtom** buckets( _buckets.get<HAtom*>() );
		atom->_next = buckets[ newHash ];
		buckets[ newHash ] = atom;
		++ _size;
		it = iterator( this, newHash, atom );
		inserted = true;
		}
	return ( make_pair( it, inserted ) );
	M_EPILOG
	}

template<typename key_t, typename data_t>
template<typename iterator_t>
void HHashMap<key_t, data_t>::insert( iterator_t first, iterator_t last )
	{
	M_PROLOG
	for ( ; first != last; ++ first )
		insert( *first );
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
typename HHashMap<key_t, data_t>::iterator HHashMap<key_t, data_t>::begin( void )
	{
	M_PROLOG
	return ( _engine.begin() );
	M_EPILOG
	}

template<typename key_t, typename data_t>
typename HHashMap<key_t, data_t>::iterator HHashMap<key_t, data_t>::end( void )
	{
	M_PROLOG
	return ( _engine.end() );
	M_EPILOG
	}

template<typename key_t, typename data_t>
typename HHashMap<key_t, data_t>::const_iterator HHashMap<key_t, data_t>::begin( void ) const
	{
	M_PROLOG
	const_iterator it( this, 0, NULL );
	return ( ++ it );
	M_EPILOG
	}

template<typename key_t, typename data_t>
typename HHashMap<key_t, data_t>::const_iterator HHashMap<key_t, data_t>::end( void ) const
	{
	M_PROLOG
	return ( const_iterator( this, _prime, NULL ) );
	M_EPILOG
	}

template<typename key_t, typename data_t>
typename HHashMap<key_t, data_t>::iterator HHashMap<key_t, data_t>::find( key_t const& key_ )
	{
	M_PROLOG
	int long idx( 0 );
	HAtom* atom( NULL );
	return ( find( key_, idx, atom ) ? iterator( this, idx, atom ) : end() );
	M_EPILOG
	}

template<typename key_t, typename data_t>
typename HHashMap<key_t, data_t>::const_iterator HHashMap<key_t, data_t>::find( key_t const& key_ ) const
	{
	M_PROLOG
	int long idx( 0 );
	HAtom* atom( NULL );
	return ( find( key_, idx, atom ) ? const_iterator( this, idx, atom ) : end() );
	M_EPILOG
	}

template<typename key_t, typename data_t>
void HHashMap<key_t, data_t>::erase( iterator it )
	{
	M_PROLOG
	_engine.erase( it._engine );
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
int long HHashMap<key_t, data_t>::erase( key_t const& key_ )
	{
	M_PROLOG
	iterator it( find( key_ ) );
	bool erased( it != end() );
	if ( erased )
		erase( it );
	return ( erased ? 1 : 0 );
	M_EPILOG
	}

template<typename key_t, typename data_t>
int long HHashMap<key_t, data_t>::count( key_t const& key_ ) const
	{
	M_PROLOG
	return ( find( key_ ) != end() ? 1 : 0 );
	M_EPILOG
	}

}

}

#endif /* not YAAL_HCORE_HHASHMAP_HXX_INCLUDED */

