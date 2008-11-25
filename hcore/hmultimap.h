/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmultimap.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HMULTIMAP_H
#define __YAAL_HCORE_HMULTIMAP_H

#include "hcore/hpointer.h"
#include "hcore/hmap.h"
#include "hcore/hlist.h"

namespace yaal
{

namespace hcore
{

/*! \brief Binary tree based map with relation one-to-many on keys to values.
 *
 * HMultiMap<> is a template representing self balancing binary search tree
 * data structure that holds pairs of keys and lists of values.
 *
 * \tparam key_t - type of key held in map.
 * \tparam value_t - type of value held in map.
 * \tparam helper_t - HSBBSTree plugable code.
 */
template<typename key_t, typename value_t, typename helper_t = map_helper<key_t const, HPointer<HList<value_t> > > >
class HMultiMap
	{
	typedef HList<value_t> value_list_t;
	typedef HPointer<value_list_t> value_list_ptr_t;
	typedef HMap<key_t, value_list_ptr_t, helper_t> multimap_engine_t;
	typedef HPair<key_t const&, value_t&> map_elem_t;
public:
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<map_elem_t> iterator;
	typedef HIterator<map_elem_t const> const_iterator;
private:
	multimap_engine_t f_oEngine;
public:
	HMultiMap( void ) : f_oEngine() {};
	size_t size( void ) const
		{
		M_PROLOG
		int l_iSize = 0;
		for ( typename multimap_engine_t::const_iterator it = f_oEngine.begin(); it != f_oEngine.end(); ++ it )
			l_iSize += it->second->size();
		return ( l_iSize );
		M_EPILOG
		}
	bool empty( void ) const
		{ return ( f_oEngine.empty() );	}
	iterator push_back( key_t const& key, value_t const& value )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( key );
		major->second->push_back( value );
		typename value_list_t::iterator minor = major->second->rbegin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator push_front( key_t const& key, value_t const& value )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = ensure_key( key );
		major->second->push_front( value );
		typename value_list_t::iterator minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	void remove_set( key_t const& key )
		{
		M_PROLOG
		iterator it = find( key );
		if ( it != end() )
			f_oEngine.erase( it.f_oMajor );
		return;
		M_EPILOG
		}
	iterator erase( iterator& it )
		{
		M_PROLOG
		iterator newIt( it );
		++ newIt;
		value_list_ptr_t list = it.f_oMajor->second;
		list->erase( it.f_oMinor );
		if ( ! list->size() )
			f_oEngine.erase( it.f_oMajor );
		return ( newIt );
		M_EPILOG
		}
	const_iterator find( key_t const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::cons_iterator major = f_oEngine.find( key );
		typename value_list_t::const_iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator find( key_t const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.find( key );
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator upper_bound( key_t const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.find( key );
		if ( major != f_oEngine.end() )
			++ major;
		typename value_list_t::const_iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->rbegin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator upper_bound( key_t const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.find( key );
		if ( major != f_oEngine.end() )
			++ major;
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->rbegin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator begin( void ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.begin();
		typename value_list_t::const_iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator begin( void )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.begin();
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator end( void ) const
		{ return ( const_iterator( this, f_oEngine.end(), typename value_list_t::const_iterator() ) ); }
	iterator end( void )
		{ return ( iterator( this, f_oEngine.end(), typename value_list_t::iterator() ) ); }
	const_iterator rbegin( void ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.rbegin();
		typename value_list_t::const_iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->rbegin();
		return ( const_iterator( this, major, minor ) );
		M_EPILOG
		}
	iterator rbegin( void )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.rbegin();
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->rbegin();
		return ( iterator( this, major, minor ) );
		M_EPILOG
		}
	const_iterator rend( void ) const
		{ return ( const_iterator( this, f_oEngine.rend(), typename value_list_t::const_iterator() ) ); }
	iterator rend( void )
		{ return ( iterator( this, f_oEngine.rend(), typename value_list_t::iterator() ) ); }
	void clear( void )
		{ f_oEngine.clear(); }
	int long count( key_t const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::const_iterator major = f_oEngine.find( key );
		int long cnt = 0;
		if ( major != f_oEngine.end() )
			cnt = major->second->size();
		return ( cnt );
		M_EPILOG
		}

private:
	typename multimap_engine_t::iterator ensure_key( key_t const& key )
		{
		M_PROLOG
		typename multimap_engine_t::iterator major = f_oEngine.find( key );
		if ( major == f_oEngine.end() )
			{
			value_list_ptr_t list = value_list_ptr_t( new value_list_t() );
			major = f_oEngine.insert( key, list );
			}
		return ( major );
		M_EPILOG
		}
	};

/*! \brief Forward iterator for HMultiMap<>.
 */
template<typename key_t, typename value_t, typename helper_t>
template<typename const_qual_t>
class HMultiMap<key_t, value_t, helper_t>::HIterator
	{
	typedef HMultiMap<key_t, value_t, helper_t> multi_map_t;
	typedef typename multi_map_t::multimap_engine_t::iterator key_iterator_t;
	typedef typename value_list_t::iterator value_iterator_t;
	multi_map_t const* f_poOwner;
	key_iterator_t f_oMajor;
	value_iterator_t f_oMinor;
public:
	HIterator( void ) : f_poOwner( NULL ), f_oMajor(), f_oMinor() {}
	HIterator( HIterator const& a_oIt ) : f_poOwner( a_oIt.f_poOwner ), f_oMajor( a_oIt.f_oMajor ), f_oMinor( a_oIt.f_oMinor ) {}
	HIterator& operator = ( HIterator const& a_oIt )
		{
		if ( &a_oIt != this )
			{
			f_poOwner = a_oIt.f_poOwner;
			f_oMajor = a_oIt.f_oMajor;
			f_oMinor = a_oIt.f_oMinor;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		M_PROLOG
		++ f_oMinor;
		if ( ! f_oMinor.is_valid() )
			{
			++ f_oMajor;
			if ( f_oMajor != f_poOwner->f_oEngine.end() )
				f_oMinor = f_oMajor->second->begin();
			}
		return ( *this );
		M_EPILOG
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( f_poOwner, f_oMajor, f_oMinor );
		operator++();
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		M_PROLOG
		-- f_oMinor;
		if ( ! f_oMinor.is_valid() )
			{
			-- f_oMajor;
			if ( f_oMajor != f_poOwner->f_oEngine.rend() )
				f_oMinor = f_oMajor->second->rbegin();
			}
		return ( *this );
		M_EPILOG
		}
	HIterator const operator -- ( int )
		{
		HIterator it( f_poOwner, f_oMajor, f_oMinor );
		-- f_oEngine;
		return ( it );
		}
	map_elem_t operator* ( void )
		{	return ( map_elem_t( f_oMajor->first, *f_oMinor ) );	}
	bool operator == ( HIterator const& it ) const
		{ return ( ( f_oMinor == it.f_oMinor ) && ( f_oMajor == it.f_oMajor ) ); }
	bool operator != ( HIterator const& it ) const
		{ return ( ( f_oMinor != it.f_oMinor ) || ( f_oMajor != it.f_oMajor ) ); }
private:
	friend class HMultiMap<key_t, value_t, helper_t>;
	explicit HIterator( multi_map_t const* const a_poOwner,
			key_iterator_t const& major,
			value_iterator_t const& minor ) : f_poOwner( a_poOwner ), f_oMajor( major ), f_oMinor( minor ) {};
	};

}

}

#endif /* not __YAAL_HCORE_HMULTIMAP_H */

