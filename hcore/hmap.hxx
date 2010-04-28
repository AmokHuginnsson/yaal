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
#define YAAL_HCORE_HMAP_HXX_INCLUDED

#include "hcore/hsbbstree.hxx"
#include "hcore/hpair.hxx"

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
private:
	HSBBSTree f_oEngine;
public:
	HMap( void ) : f_oEngine() {};
	int long size( void ) const
		{ return ( get_size() ); }
	int long get_size( void ) const
		{ return ( f_oEngine.get_size() );	}
	bool empty( void ) const
		{ return ( is_empty() );	}
	bool is_empty( void ) const
		{ return ( f_oEngine.is_empty() );	}
	HPair<iterator, bool> insert( value_type const& e )
		{
		HPair<HSBBSTree::HIterator, bool> p = f_oEngine.insert<value_type, helper_t>( e );
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
			f_oEngine.remove( it.f_oEngine );
			}
		return ( erased );
		M_EPILOG
		}
	iterator erase( iterator const& it )
		{
		M_PROLOG
		iterator newIt( it );
		++ newIt;
		f_oEngine.remove( it.f_oEngine );
		return ( newIt );
		M_EPILOG
		}
	iterator find( key_type const& e )
		{ return ( iterator( f_oEngine.find<value_type, key_type, helper_t>( e ) ) ); }
	const_iterator find( key_type const& e ) const
		{ return ( const_iterator( f_oEngine.find<value_type, key_type, helper_t>( e ) ) ); }
	iterator begin( void )
		{ return ( iterator( f_oEngine.begin() ) ); }
	const_iterator begin( void ) const
		{ return ( const_iterator( f_oEngine.begin() ) ); }
	iterator end( void )
		{ return ( iterator( f_oEngine.end() ) ); }
	const_iterator end( void ) const
		{ return ( const_iterator( f_oEngine.end() ) ); }
	iterator rbegin( void )
		{ return ( iterator( f_oEngine.rbegin() ) ); }
	const_iterator rbegin( void ) const
		{ return ( const_iterator( f_oEngine.rbegin() ) ); }
	iterator rend( void )
		{ return ( iterator( f_oEngine.rend() ) ); }
	const_iterator rend( void ) const
		{ return ( const_iterator( f_oEngine.rend() ) ); }
	data_type& operator[] ( key_type const& key )
		{
		M_PROLOG
		return ( insert( value_type( key, data_type() ) ).first->second );
		M_EPILOG
		}
	void clear( void )
		{ f_oEngine.clear(); }
	void swap( HMap<key_type, data_type, helper_t>& other )
		{
		if ( &other != this )
			f_oEngine.swap( other.f_oEngine );
		}
	void copy_from( HMap<key_type, data_type, helper_t> const& source )
		{
		M_PROLOG
		if ( &source != this )
			f_oEngine.copy_from<value_type, helper_t>( source.f_oEngine );
		M_EPILOG
		}
	};

/*! \brief Iterator for HMap<> data structure.
 */
template<typename key_type_t, typename value_type_t, typename helper_t>
template<typename const_qual_t>
class HMap<key_type_t, value_type_t, helper_t>::HIterator
	{
	typedef key_type_t key_type;
	typedef value_type_t data_type;
	typedef HMap<key_type, data_type, helper_t> map_t;
	HSBBSTree::HIterator f_oEngine;
public:
	HIterator( void ) : f_oEngine() {}
	template<typename other_const_qual_t>
	HIterator( HIterator<other_const_qual_t> const& a_oIt ) : f_oEngine( a_oIt.f_oEngine )
		{
		STATIC_ASSERT(( trait::same_type<const_qual_t, other_const_qual_t>::value || trait::same_type<const_qual_t, other_const_qual_t const>::value ));
		}
	HIterator& operator = ( HIterator const& a_oIt )
		{
		if ( &a_oIt != this )
			f_oEngine = a_oIt.f_oEngine;
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		++ f_oEngine;
		return ( *this );
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( f_oEngine );
		++ f_oEngine;
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		-- f_oEngine;
		return ( *this );
		}
	HIterator const operator -- ( int )
		{
		HIterator it( f_oEngine );
		-- f_oEngine;
		return ( it );
		}
	const_qual_t& operator* ( void )
		{ return ( f_oEngine.operator*<typename map_t::value_type>() ); }
	const_qual_t& operator* ( void ) const
		{ return ( f_oEngine.operator*<typename map_t::value_type>() ); }
	const_qual_t* operator-> ( void )
		{ return ( &f_oEngine.operator*<typename map_t::value_type>() ); }
	const_qual_t* operator-> ( void ) const
		{ return ( &f_oEngine.operator*<typename map_t::value_type>() ); }
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{ return ( f_oEngine == it.f_oEngine ); }
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{ return ( f_oEngine != it.f_oEngine ); }
private:
	friend class HMap<key_type, data_type, helper_t>;
	template<typename other_const_qual_t>
	friend class HIterator;
	explicit HIterator( HSBBSTree::HIterator const& it ) : f_oEngine( it ) {};
	};

}

template<typename key_type, typename data_type, typename helper_t>
inline void swap( yaal::hcore::HMap<key_type, data_type, helper_t>& a, yaal::hcore::HMap<key_type, data_type, helper_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HMAP_HXX_INCLUDED */

