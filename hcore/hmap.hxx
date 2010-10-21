/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmap.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HMAP_HXX_INCLUDED
#define YAAL_HCORE_HMAP_HXX_INCLUDED 1

#include "hcore/hsbbstree.hxx"
#include "hcore/hpair.hxx"
#include "hcore/iterator.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief HSBBSTree util, a helper for HMap<> instatiations.
 */
template<typename key_type, typename data_type>
struct map_helper
{

inline static bool less( HPair<key_type, data_type> const& left, HPair<key_type, data_type> const& right )
	{	return ( left.first < right.first );	}

inline static bool less( key_type const& left, HPair<key_type, data_type> const& right )
	{	return ( left < right.first );	}

inline static bool less( HPair<key_type, data_type> const& left, key_type const& right )
	{	return ( left.first < right );	}

};

/*! \brief Binary tree based map.
 *
 * HMap<> is a template representing self balancing binary search tree
 * data structure that holds pairs of keys and values.
 *
 * \tparam key_type_t - type of key held in map.
 * \tparam value_type_t - type of value held in map.
 * \tparam helper_t - HSBBSTree plugable code.
 */
template<typename key_type_t, typename value_type_t, typename helper_t = map_helper<key_type_t const, value_type_t> >
class HMap
	{
public:
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HPair<key_type const, data_type> value_type;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<value_type> iterator;
	typedef HIterator<value_type const> const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	HSBBSTree _engine;
public:
	HMap( void ) : _engine() {}
	template<typename iterator_t>
	HMap( iterator_t first, iterator_t last ) : _engine()
		{
		M_PROLOG
		insert( first, last );
		return;
		M_EPILOG
		}
	HMap( HMap const& source ) : _engine()
		{
		M_PROLOG
		_engine.copy_from( source._engine );
		return;
		M_EPILOG
		}
	HMap& operator = ( HMap const& map_ )
		{
		M_PROLOG
		if ( &map_ != this )
			{
			HMap tmp( map_ );
			swap( tmp );
			}
		return ( *this );
		M_EPILOG
		}
	int long size( void ) const
		{ return ( get_size() ); }
	int long get_size( void ) const
		{ return ( _engine.get_size() );	}
	bool empty( void ) const
		{ return ( is_empty() );	}
	bool is_empty( void ) const
		{ return ( _engine.is_empty() );	}
	HPair<iterator, bool> insert( value_type const& e )
		{
		HPair<HSBBSTree::HIterator, bool> p = _engine.insert<value_type, helper_t>( e );
		return ( make_pair( iterator( p.first ), p.second ) );
		}
	template<typename iter_t>
	void insert( iter_t i, iter_t endIt )
		{
		M_PROLOG
		for ( ; i != endIt; ++ i )
			insert( *i );
		return;
		M_EPILOG
		}
	int long erase( key_type const& key )
		{
		M_PROLOG
		iterator it = find( key );
		int long erased( 0 );
		if ( it != end() )
			{
			erased = 1;
			_engine.remove( it._engine );
			}
		return ( erased );
		M_EPILOG
		}
	iterator erase( iterator const& it )
		{
		M_PROLOG
		iterator newIt( it );
		++ newIt;
		_engine.remove( it._engine );
		return ( newIt );
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
	iterator find( key_type const& e )
		{ return ( iterator( _engine.find<value_type, key_type, helper_t>( e ) ) ); }
	const_iterator find( key_type const& e ) const
		{ return ( const_iterator( _engine.find<value_type, key_type, helper_t>( e ) ) ); }
	iterator begin( void )
		{ return ( iterator( _engine.begin() ) ); }
	const_iterator begin( void ) const
		{ return ( const_iterator( _engine.begin() ) ); }
	iterator end( void )
		{ return ( iterator( _engine.end() ) ); }
	const_iterator end( void ) const
		{ return ( const_iterator( _engine.end() ) ); }
	reverse_iterator rbegin( void )
		{ return ( end() ); }
	const_reverse_iterator rbegin( void ) const
		{ return ( end() ); }
	reverse_iterator rend( void )
		{ return ( begin() ); }
	const_reverse_iterator rend( void ) const
		{ return ( begin() ); }
	data_type& operator[] ( key_type const& key )
		{
		M_PROLOG
		return ( insert( value_type( key, data_type() ) ).first->second );
		M_EPILOG
		}
	int long count( key_type const& key_ ) const
		{ M_PROLOG return ( find( key_ ) != end() ? 1 : 0 ); M_EPILOG }
	void clear( void )
		{ _engine.clear(); }
	void swap( HMap<key_type, data_type, helper_t>& other )
		{
		if ( &other != this )
			{
			using yaal::swap;
			_engine.swap( other._engine );
			}
		}
	bool operator == ( HMap const& map_ ) const
		{ M_PROLOG return ( ( &map_ == this ) || safe_equal( begin(), end(), map_.begin(), map_.end() ) ); M_EPILOG }
	bool operator < ( HMap const& map_ ) const
		{ M_PROLOG return ( lexicographical_compare( begin(), end(), map_.begin(), map_.end() ) ); M_EPILOG }
	};

/*! \brief Iterator for HMap<> data structure.
 */
template<typename key_type_t, typename value_type_t, typename helper_t>
template<typename const_qual_t>
class HMap<key_type_t, value_type_t, helper_t>::HIterator : public iterator_interface<const_qual_t>
	{
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HMap<key_type, data_type, helper_t> map_t;
	HSBBSTree::HIterator _engine;
public:
	HIterator( void )
		: iterator_interface<const_qual_t>(), _engine() {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& it_ )
		: iterator_interface<const_qual_t>(), _engine( it_._engine )
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
	friend class HMap<key_type, data_type, helper_t>;
	template<typename other_const_qual_t>
	friend class HIterator;
	explicit HIterator( HSBBSTree::HIterator const& it )
		: iterator_interface<const_qual_t>(), _engine( it ) {};
	};

}

template<typename key_type, typename data_type, typename helper_t>
inline void swap( yaal::hcore::HMap<key_type, data_type, helper_t>& a, yaal::hcore::HMap<key_type, data_type, helper_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HMAP_HXX_INCLUDED */

