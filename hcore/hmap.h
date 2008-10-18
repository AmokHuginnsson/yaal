/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmap.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCORE_HMAP_H
#define __YAAL_HCORE_HMAP_H

#line 31

#define D_VCSID_YAAL_HCORE_HMAP_H "$Id$"

#include "hcore/hsbbstree.h"
#include "hcore/hpair.h"

namespace yaal
{

namespace hcore
{

template<typename key_t, typename value_t>
struct map_helper
{

inline static void update( HPair<key_t, value_t>& left, HPair<key_t, value_t> const& right )
	{
	left.second = right.second;
	}

inline static bool less( HPair<key_t, value_t> const& left, HPair<key_t, value_t> const& right )
	{	return ( left.first < right.first );	}

inline static bool less( key_t const& left, HPair<key_t, value_t> const& right )
	{	return ( left < right.first );	}

inline static bool less( HPair<key_t, value_t> const& left, key_t const& right )
	{	return ( left.first < right );	}

};

template<typename key_t, typename value_t, typename helper_t = map_helper<key_t const, value_t> >
class HMap
	{
public:
	typedef HPair<key_t const, value_t> map_elem_t;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<map_elem_t> iterator;
	typedef HIterator<map_elem_t const> const_iterator;
private:
	HSBBSTree f_oEngine;
public:
	HMap( void ) : f_oEngine() {};
	int long size( void ) const
		{ return ( f_oEngine.size() );	}
	bool empty( void ) const
		{ return ( f_oEngine.empty() );	}
	bool is_empty( void ) const
		{ return ( f_oEngine.empty() );	}
	iterator insert( map_elem_t const& e )
		{	return ( iterator( f_oEngine.insert<map_elem_t, helper_t>( e ) ) );	}
	iterator insert( key_t const& key, value_t const& value )
		{	return ( iterator( f_oEngine.insert<map_elem_t, helper_t>( map_elem_t( key, value ) ) ) );	}
	void remove( key_t const& key )
		{
		M_PROLOG
		iterator it = find( key );
		if ( it != end() )
			f_oEngine.remove( it.f_oEngine );
		return;
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
	iterator find( key_t const& e )
		{ return ( iterator( f_oEngine.find<map_elem_t, key_t, helper_t>( e ) ) ); }
	const_iterator find( key_t const& e ) const
		{ return ( const_iterator( f_oEngine.find<map_elem_t, key_t, helper_t>( e ) ) ); }
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
	value_t& operator[] ( key_t const& key )
		{
		M_PROLOG
		iterator it = find( key );
		if ( it == end() )
			it = insert( map_elem_t( key, value_t() ) );
		return ( it->second );
		M_EPILOG
		}
	void clear( void )
		{ f_oEngine.clear(); }
	void swap( HMap<key_t, value_t, helper_t>& other )
		{
		if ( &other != this )
			f_oEngine.swap( other.f_oEngine );
		}
	void copy_from( HMap<key_t, value_t, helper_t> const& source )
		{
		M_PROLOG
		if ( &source != this )
			f_oEngine.copy_from<map_elem_t, helper_t>( source.f_oEngine );
		M_EPILOG
		}
	};

template<typename key_t, typename value_t, typename helper_t>
template<typename const_qual_t>
class HMap<key_t, value_t, helper_t>::HIterator
	{
	typedef HMap<key_t, value_t, helper_t> map_t;
	HSBBSTree::HIterator f_oEngine;
public:
	HIterator( void ) : f_oEngine() {}
	HIterator( HIterator const& a_oIt ) : f_oEngine( a_oIt.f_oEngine ) {}
	HIterator& operator= ( HIterator const& a_oIt )
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
		{ return ( f_oEngine.operator*<typename map_t::map_elem_t>() ); }
	const_qual_t& operator* ( void ) const
		{ return ( f_oEngine.operator*<typename map_t::map_elem_t>() ); }
	const_qual_t* operator-> ( void )
		{ return ( &f_oEngine.operator*<typename map_t::map_elem_t>() ); }
	const_qual_t* operator-> ( void ) const
		{ return ( &f_oEngine.operator*<typename map_t::map_elem_t>() ); }
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{ return ( f_oEngine == it.f_oEngine ); }
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{ return ( f_oEngine != it.f_oEngine ); }
private:
	friend class HMap<key_t, value_t, helper_t>;
	explicit HIterator( HSBBSTree::HIterator const& it ) : f_oEngine( it ) {};
	};

}

template<typename key_t, typename value_t, typename helper_t>
inline void swap( yaal::hcore::HMap<key_t, value_t, helper_t>& a, yaal::hcore::HMap<key_t, value_t, helper_t>& b )
	{ a.swap( b ); }

}

#endif /* not __YAAL_HCORE_HMAP_H */

