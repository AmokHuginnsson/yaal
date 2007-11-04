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

#define D_VCSID_HMAP_H "$Id$"

#include "hcore/hsbbstree.h"

namespace yaal
{

namespace hcore
{

template<typename tType, typename ttType>
class HPair
	{
	tType f_tKey;
	ttType f_tValue;
public:
	tType& first;
	ttType& second;
	HPair( void ) : f_tKey(), f_tValue(), first( f_tKey ), second( f_tValue ) {}
	HPair( tType key, ttType value ) : f_tKey( key ), f_tValue( value ), first( f_tKey ), second( f_tValue ) {}
	HPair( HPair const& pair ) : f_tKey( pair.f_tKey ), f_tValue( pair.f_tValue ), first( f_tKey ), second( f_tValue ) {}
	HPair& operator = ( HPair const& pair )
		{
		if ( &pair != this )
			{
			f_tKey = pair.f_tKey;
			f_tValue = pair.f_tValue;
			}
		return ( *this );
		}
	bool operator == ( HPair const& pair ) const
		{	return ( ( f_tKey == pair.f_tKey ) && ( f_tValue == pair.f_tValue ) );	}
	bool operator != ( HPair const& pair ) const
		{	return ( ! operator == ( pair ) );	}
	bool operator < ( HPair const& pair ) const
		{
		return ( ( f_tKey < pair.f_tKey )
				|| ( ! ( pair.f_tKey < f_tKey ) && ( f_tValue < pair.f_tValue ) ) );
		}
	};

template<typename tType, typename ttType>
struct map_helper
{

inline static void update( HPair<tType, ttType>& left, HPair<tType, ttType> const& right )
	{
	left.second = right.second;
	}

inline static bool less( HPair<tType, ttType> const& left, HPair<tType, ttType> const& right )
	{	return ( left.first < right.first );	}

inline static bool less( tType const& left, HPair<tType, ttType> const& right )
	{	return ( left < right.first );	}

inline static bool less( HPair<tType, ttType> const& left, tType const& right )
	{	return ( left.first < right );	}

};

template<typename tType, typename ttType, typename tttType = map_helper<tType const, ttType> >
class HMap
	{
	typedef HPair<tType const, ttType> map_elem_t;
public:
	class HIterator
		{
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
		map_elem_t const& operator * ( void )
			{	return ( f_oEngine.operator*<map_elem_t>() );	}
		map_elem_t const* const operator -> ( void )
			{ return ( &f_oEngine.operator*<map_elem_t>() );	}
		bool operator == ( HIterator const& it ) const
			{ return ( f_oEngine == it.f_oEngine ); }
		bool operator != ( HIterator const& it ) const
			{ return ( f_oEngine != it.f_oEngine ); }
	private:
		friend class HMap<tType, ttType>;
		explicit HIterator( HSBBSTree::HIterator const& it ) : f_oEngine( it ) {};
		};
private:
	HSBBSTree f_oEngine;
public:
	HMap( void ) : f_oEngine() {};
	size_t size( void ) const
		{ return ( f_oEngine.size() );	}
	bool empty( void ) const
		{ return ( f_oEngine.empty() );	}
	HIterator insert( map_elem_t const& e )
		{	return ( HIterator( f_oEngine.insert<map_elem_t, tttType>( e ) ) );	}
	void remove( tType const& e )
		{
		HIterator it = find( e );
		if ( it != end() )
			f_oEngine.remove( it.f_oEngine );
		return;
		}
	HIterator find( tType const& e ) const
		{ return ( HIterator( f_oEngine.find<map_elem_t, tType, tttType>( e ) ) ); }
	HIterator begin( void ) const
		{ return ( HIterator( f_oEngine.begin() ) ); }
	HIterator end( void ) const
		{ return ( HIterator( f_oEngine.end() ) ); }
	HIterator rbegin( void ) const
		{ return ( HIterator( f_oEngine.rbegin() ) ); }
	HIterator rend( void ) const
		{ return ( HIterator( f_oEngine.rend() ) ); }
	ttType& operator[] ( tType const& key )
		{
		HIterator it = find( key );
		if ( it == end() )
			it = insert( map_elem_t( key, ttType() ) );
		return ( it->second );
		}
	};

}

}

#endif /* not __YAAL_HCORE_HMAP_H */

