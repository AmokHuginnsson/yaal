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

template<typename tType, typename ttType, typename tttType = map_helper<tType const, HPointer<HList<ttType>, HPointerScalar, HPointerRelaxed> > >
class HMultiMap
	{
	typedef HList<ttType> value_list_t;
	typedef HPointer<value_list_t, HPointerScalar, HPointerRelaxed> value_list_ptr_t;
	typedef HMap<tType, value_list_ptr_t, tttType> multimap_engine_t;
	typedef HPair<tType const&, ttType&> map_elem_t;
public:
	class HIterator
		{
		HMultiMap<tType, ttType, tttType> const* f_poOwner;
		typename multimap_engine_t::HIterator f_oMajor;
		typename value_list_t::iterator f_oMinor;
	public:
		HIterator( void ) : f_poOwner( NULL ), f_oMajor(), f_oMinor() {}
		HIterator( HIterator const& a_oIt ) : f_poOwner( a_oIt.f_poOwner ), f_oMajor( a_oIt.f_oMajor ), f_oMinor( a_oIt.f_oMinor ) {}
		HIterator& operator= ( HIterator const& a_oIt )
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
		friend class HMultiMap<tType, ttType, tttType>;
		explicit HIterator( HMultiMap<tType, ttType, tttType> const* const a_poOwner,
				typename multimap_engine_t::HIterator const& major,
				typename value_list_t::iterator const& minor ) : f_poOwner( a_poOwner ), f_oMajor( major ), f_oMinor( minor ) {};
		};
private:
	multimap_engine_t f_oEngine;
public:
	HMultiMap( void ) : f_oEngine() {};
	size_t size( void ) const
		{
		M_PROLOG
		int l_iSize = 0;
		for ( typename multimap_engine_t::HIterator it = f_oEngine.begin(); it != f_oEngine.end(); ++ it )
			l_iSize += it->second->size();
		return ( l_iSize );
		M_EPILOG
		}
	bool empty( void ) const
		{ return ( f_oEngine.empty() );	}
	HIterator push_back( tType const& key, ttType const& value )
		{
		M_PROLOG
		typename multimap_engine_t::HIterator major = ensuere_key( key );
		major->second->push_back( value );
		typename value_list_t::iterator minor = major->second->rbegin();
		return ( HIterator( this, major, minor ) );
		M_EPILOG
		}
	HIterator push_front( tType const& key, ttType const& value )
		{
		M_PROLOG
		typename multimap_engine_t::HIterator major = ensuere_key( key );
		major->second->push_front( value );
		typename value_list_t::iterator minor = major->second->begin();
		return ( HIterator( this, major, minor ) );
		M_EPILOG
		}
	void remove_set( tType const& key )
		{
		M_PROLOG
		HIterator it = find( key );
		if ( it != end() )
			f_oEngine.erase( it.f_oMajor );
		return;
		M_EPILOG
		}
	void erase( HIterator& it )
		{
		M_PROLOG
		value_list_ptr_t list = it.f_oMajor->second;
		list->erase( it.f_oMinor );
		if ( ! list->size() )
			f_oEngine.erase( it.f_oMajor );
		return;
		M_EPILOG
		}
	HIterator find( tType const& key ) const
		{
		M_PROLOG
		typename multimap_engine_t::HIterator major = f_oEngine.find( key );
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( HIterator( this, major, minor ) );
		M_EPILOG
		}
	HIterator begin( void ) const
		{
		M_PROLOG
		typename multimap_engine_t::HIterator major = f_oEngine.begin();
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->begin();
		return ( HIterator( this, major, minor ) );
		M_EPILOG
		}
	HIterator end( void ) const
		{ return ( HIterator( this, f_oEngine.end(), typename value_list_t::iterator() ) ); }
	HIterator rbegin( void ) const
		{
		M_PROLOG
		typename multimap_engine_t::HIterator major = f_oEngine.rbegin();
		typename value_list_t::iterator minor;
		if ( major != f_oEngine.end() )
			minor = major->second->rbegin();
		return ( HIterator( this, major, minor ) );
		M_EPILOG
		}
	HIterator rend( void ) const
		{ return ( HIterator( this, f_oEngine.rend(), typename value_list_t::iterator() ) ); }

private:
	typename multimap_engine_t::HIterator ensuere_key( tType const& key )
		{
		M_PROLOG
		typename multimap_engine_t::HIterator major = f_oEngine.find( key );
		if ( major == f_oEngine.end() )
			{
			value_list_ptr_t list = value_list_ptr_t( new value_list_t() );
			major = f_oEngine.insert( key, list );
			}
		return ( major );
		M_EPILOG
		}
	};


}

}

#endif /* not __YAAL_HCORE_HMULTIMAP_H */

