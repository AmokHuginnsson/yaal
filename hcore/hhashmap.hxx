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

#include "hcore/hhashcontainer.hxx"
#include "hcore/hexception.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Hash map container implementation.
 */
template<typename key_t, typename data_t, typename hash_function_t = int long(*)( key_t const& )>
class HHashMap
	{
public:
	typedef key_t key_type;
	typedef data_t data_type;
	typedef HPair<key_t const, data_t> value_type;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	struct hasher
		{
		typedef typename HHashMap<key_t, data_t, hash_function_t>::value_type value_type;
		hash_function_t _hasher;
		hasher( hash_function_t hashFunction_ ) : _hasher( hashFunction_ ) {}
		int long operator()( value_type const& val_ ) const
			{ return ( _hasher( val_.first ) ); }
		int long operator()( key_t const& key_ ) const
			{ return ( _hasher( key_ ) ); }
		bool operator()( value_type const& a_, value_type const& b_ ) const
			{ return ( a_.first == b_.first ); }
		bool operator()( value_type const& a_, key_t const& b_ ) const
			{ return ( a_.first == b_ ); }
		};
	typedef HHashMap<key_t, data_t, hash_function_t> this_type;
	hasher _hasher;
	HHashContainer _engine;
public:
	HHashMap( void )
		: _hasher( &hash ), _engine()
		{}
	/*! \brief Lower bound of size of map's table */
	HHashMap( int long size_ )
		: _hasher(  &hash ), _engine()
		{
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
		}
	HHashMap( int long size_, hash_function_t hasher_ )
		: _hasher(  hasher_ ), _engine()
		{
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
		}
	template<typename iterator_t>
	HHashMap( iterator_t first, iterator_t last )
		: _hasher(  &hash ), _engine()
		{
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
		}
	template<typename iterator_t>
	HHashMap( iterator_t first, iterator_t last, int long size_ )
		: _hasher(  &hash ), _engine()
		{
		M_PROLOG
		resize( size_, _hasher );
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
		}
	HHashMap( HHashMap const& map_ )
		: _hasher( map_._hasher ), _engine()
		{
		M_PROLOG
		_engine.copy_from( map_._engine );
		return;
		M_EPILOG
		}
	virtual ~HHashMap( void )
		{
		M_PROLOG
		clear();
		return;
		M_EPILOG
		}
	HHashMap& operator = ( HHashMap const& map_ )
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
	data_t& operator [] ( key_t const& key_ )
		{ M_PROLOG return ( insert( make_pair( key_, data_t() ) ).first->second ); M_EPILOG }
	const_iterator begin( void ) const
		{ M_PROLOG return ( const_iterator( _engine.begin() ) ); M_EPILOG }
	iterator begin( void )
		{ M_PROLOG return ( iterator( _engine.begin() ) ); M_EPILOG }
	const_iterator end( void ) const
		{ M_PROLOG return ( const_iterator( _engine.end() ) ); M_EPILOG }
	iterator end( void )
		{ M_PROLOG return ( iterator( _engine.end() ) ); M_EPILOG }
	const_iterator find( key_t const& key_ ) const
		{ M_PROLOG return ( const_iterator( _engine.find( key_, _hasher ) ) ); M_EPILOG }
	iterator find( key_t const& key_ )
		{ M_PROLOG return ( iterator( _engine.find( key_, _hasher ) ) ); M_EPILOG }
	HPair<iterator, bool> insert( value_type const& val_ )
		{
		M_PROLOG
		HPair<HHashContainer::HIterator, bool> it( _engine.insert( val_, _hasher ) );
		return ( make_pair( iterator( it.first ), it.second ) );
		M_EPILOG
		}
	void resize( int long size_ )
		{
		M_PROLOG
		_engine.resize( size_, _hasher );
		return;
		M_EPILOG
		}
	template<typename iterator_t>
	void insert( iterator_t first, iterator_t last )
		{
		M_PROLOG
		for ( ; first != last; ++ first )
			insert( *first );
		return;
		M_EPILOG
		}
	void erase( iterator it )
		{
		M_PROLOG
		_engine.erase( it._engine );
		return;
		M_EPILOG
		}
	/*! \brief Remove given key from map.
	 *
	 * \param key_ - key to be removed.
	 * \return Number of erased elements.
	 */
	int long erase( key_t const& key_ )
		{
		M_PROLOG
		iterator it( find( key_ ) );
		bool erased( it != end() );
		if ( erased )
			erase( it );
		return ( erased ? 1 : 0 );
		M_EPILOG
		}
	void erase( iterator first_, iterator const& last_ )
		{
		M_PROLOG
		while ( first_ != last_ )
			first_ = erase( first_ );
		return;
		M_EPILOG
		}
	int long count( key_t const& key_ ) const
		{ M_PROLOG return ( find( key_ ) != end() ? 1 : 0 ); M_EPILOG }
	void clear( void )
		{ M_PROLOG _engine.clear(); return; M_EPILOG }
	int long get_size( void ) const
		{ M_PROLOG return ( _engine.get_size() ); M_EPILOG }
	int long size( void ) const
		{ M_PROLOG return ( _engine.get_size() ); M_EPILOG }
	bool is_empty( void ) const
		{ M_PROLOG return ( _engine.is_empty() ); M_EPILOG }
	bool empty( void ) const
		{ M_PROLOG return ( _engine.is_empty() ); M_EPILOG }
	void swap( HHashMap& map_ )
		{
		if ( &map_ != this )
			{
			using yaal::swap;
			swap( _engine, map_._engine );
			swap( _hasher, map_._hasher );
			}
		return;
		}
	bool operator == ( HHashMap const& map_ ) const
		{ M_PROLOG return ( ( &map_ == this ) || safe_equal( begin(), end(), map_.begin(), map_.end() ) ); M_EPILOG }
	bool operator < ( HHashMap const& map_ ) const
		{ M_PROLOG return ( lexicographical_compare( begin(), end(), map_.begin(), map_.end() ) ); M_EPILOG }
private:
	};


template<typename key_type_t, typename data_type_t, typename hash_function_t>
template<typename const_qual_t>
class HHashMap<key_type_t, data_type_t, hash_function_t>::HIterator : public iterator_interface<const_qual_t>
	{
	typedef key_type_t key_type;
	typedef data_type_t data_type;
	typedef HHashMap<key_type, data_type, hash_function_t> map_t;
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
		{ return ( _engine.get<typename map_t::value_type>() ); }
	const_qual_t& operator* ( void ) const
		{ return ( _engine.get<typename map_t::value_type>() ); }
	const_qual_t* operator-> ( void )
		{ return ( &_engine.get<typename map_t::value_type>() ); }
	const_qual_t* operator-> ( void ) const
		{ return ( &_engine.get<typename map_t::value_type>() ); }
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{ return ( _engine == it._engine ); }
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{ return ( _engine != it._engine ); }
private:
	friend class HHashMap<key_type, data_type, hash_function_t>;
	template<typename other_const_qual_t>
	friend class HIterator;
	explicit HIterator( HHashContainer::HIterator const& it ) : _engine( it ) {};
	};

}

template<typename key_type, typename data_type, typename hash_function_t>
inline void swap( yaal::hcore::HHashMap<key_type, data_type, hash_function_t>& a, yaal::hcore::HHashMap<key_type, data_type, hash_function_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HHASHMAP_HXX_INCLUDED */

