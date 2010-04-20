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

#include "hcore/hchunk.hxx"
#include "hcore/hpair.hxx"
#include "hcore/hexception.hxx"

namespace yaal
{

namespace hcore
{

extern int long const* const g_pulPrimes;

template<typename key_t>
inline int long hash( key_t const& a_rtKey )
	{
	return ( static_cast<int long unsigned>( a_rtKey ) );
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
	template<typename const_qual_t>
	class HIterator;
private:
	class HAtom
		{
	private:
		HAtom* _next;
		value_type _value;
		HAtom( key_t const&, data_t const& );
		virtual ~HAtom( void );
	private:
		HAtom( HAtom const& );
		HAtom& operator = ( HAtom const& );
		friend class HHashMap<key_t, data_t>;
		template<typename const_qual_t>
		friend class HIterator;
		};
	int long _prime;
	int _size;
	HChunk _buckets;
public:
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	HHashMap( size_t ); /* Lower bound of size of map's table */
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
	int size( void ) const;
	void swap( HHashMap& );
private:
	bool find( key_t const&, int long&, HAtom*& ) const;
	};

/*! \brief Iterator for HHashMap<> data structure.
 */
template<typename key_t, typename data_t>
template<typename const_qual_t>
class HHashMap<key_t, data_t>::HIterator
	{
	typedef typename trait::copy_const<const_qual_t, HHashMap<key_t, data_t> >::type owner_t;
	owner_t* _owner;
	int long _index;
	typedef typename trait::copy_const<const_qual_t, HAtom>::type atom_t;
	atom_t* _atom;
public:
	typedef key_t key_type;
	typedef data_t data_type;
	HIterator( void ) : _owner( NULL ), _index( 0 ), _atom( NULL ) {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ )
		: _owner( it_._owner ), _index( it_._index ),
		_atom( it_._atom )
		{
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
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
			typename trait::copy_const<const_qual_t, atom_t*>::type* buckets( _owner->_buckets.template get<atom_t*>() );
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
	const_qual_t& operator* ( void )
		{
		M_ASSERT( _atom );
		return ( _atom->_value );
		}
	const_qual_t& operator* ( void ) const
		{
		M_ASSERT( _atom );
		return ( *_atom );
		}
	const_qual_t* operator-> ( void )
		{ return ( &_atom->_value ); }
	const_qual_t* operator-> ( void ) const
		{ return ( &_atom->_value ); }
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{
		M_ASSERT( _owner == it._owner );
		return ( ( _index == it._index ) && ( _atom == it._atom ) );
		}
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{ return ( ! operator == ( it ) ); }
private:
	friend class HHashMap<key_t, data_t>;
	explicit HIterator( owner_t* owner_, int long index_, HAtom* atom_ )
		: _owner( owner_ ), _index( index_ ), _atom( atom_ ) {};
	};

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>::HAtom::HAtom( key_t const& key_, data_t const& data_ )
	: _next( NULL ), _value( key_, data_ )
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
HHashMap<key_t, data_t>::HHashMap( size_t a_uiSize )
	: _prime( 0 ), _size( 0 ), _buckets()
	{
	M_PROLOG
	int i = 0;
	if ( a_uiSize < 1 )
		M_THROW ( "bad map size", a_uiSize );
	while ( a_uiSize )
		{
		a_uiSize >>= 1;
		i ++;
		}
	_prime = g_pulPrimes[ i - 1 ];
	_buckets.realloc( chunk_size<HAtom*>( _prime ) );
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
HHashMap<key_t, data_t>::HHashMap( HHashMap const& map_ )
	: _prime( 0 ), _size( 0 ), _buckets()
	{
	M_PROLOG
	_prime = map_._prime;
	_size = map_._size;
	_buckets.realloc( chunk_size<HAtom*>( _prime ) );
	HAtom* const* otherBuckets( map_._buckets.template get<HAtom const*>() );
	HAtom** buckets( _buckets.get<HAtom*>() );
	for ( int long l_iCtr = 0; l_iCtr < _prime; ++ l_iCtr )
		{
		HAtom const* origAtom( otherBuckets[ l_iCtr ] );
		HAtom** atom( &buckets[ l_iCtr ] );
		while ( origAtom )
			{
			( *atom ) = new ( std::nothrow ) HAtom( origAtom->key );
			( *atom )->value = origAtom->value;
			atom = &( *atom )->_next;
			origAtom = origAtom->_next;
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
	int l_iCtr( 0 );
	if ( &map_ != this )
		{
		HHashMap tmp( map_ );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

template<typename key_t, typename data_t>
void HHashMap<key_t, data_t>::clear( void )
	{
	M_PROLOG
	HAtom** buckets( _buckets.get<HAtom*>() );
	for ( int long i = 0; i < _prime; i ++ )
		{
		HAtom* atom( buckets[ i ] );
		while ( atom )
			{
			HAtom* del( atom );
			atom = atom->_next;
			delete del;
			}
		buckets[ i ] = NULL;
		}
	_size = 0;
	return;
	M_EPILOG
	}

template<typename key_t, typename data_t>
int HHashMap<key_t, data_t>::size( void ) const
	{
	M_PROLOG
	return ( _size );
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
	iterator it = find( val_.first );
	bool inserted( false );
	if ( it == end() )
		{
		HAtom* atom = new ( std::nothrow ) HAtom( val_.first, val_.second );
		if ( ! atom )
			M_THROW( "memory allocation error", errno );
		int long l_ulHash = hash( val_.first ) % _prime;
		HAtom** buckets( _buckets.get<HAtom*>() );
		atom->_next = buckets[ l_ulHash ];
		buckets[ l_ulHash ] = atom;
		_size ++;
		it = iterator( this, l_ulHash, atom );
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
	iterator it( this, 0, NULL );
	return ( ++ it );
	M_EPILOG
	}

template<typename key_t, typename data_t>
typename HHashMap<key_t, data_t>::iterator HHashMap<key_t, data_t>::end( void )
	{
	M_PROLOG
	return ( iterator( this, _prime, NULL ) );
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
bool HHashMap<key_t, data_t>::find( key_t const& a_rtKey, int long& a_rulIndex, HAtom*& a_rpoAtom ) const
	{
	M_PROLOG
	a_rulIndex = hash( a_rtKey ) % _prime;
	a_rpoAtom = _buckets.get<HAtom*>()[ a_rulIndex ];
	while ( a_rpoAtom && ( a_rpoAtom->_value.first != a_rtKey ) )
		a_rpoAtom = a_rpoAtom->_next;
	return ( a_rpoAtom ? true : false );
	M_EPILOG
	}

template<typename key_t, typename data_t>
void HHashMap<key_t, data_t>::erase( iterator it )
	{
	M_PROLOG
	HAtom* atom( _buckets.get<HAtom*>()[ it._index ] );
	HAtom* ancestor( NULL );
	M_ASSERT( atom );
	while ( atom != it._atom )
		ancestor = atom, atom = atom->_next;
	if ( atom )
		{
		if ( ancestor )
			ancestor->_next = atom->_next;
		else
			_buckets.get<HAtom*>()[ it._index ] = NULL;
		delete atom;
		-- _size;
		}
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

