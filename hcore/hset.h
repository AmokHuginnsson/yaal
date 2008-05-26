/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hset.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HSET_H
#define __YAAL_HCORE_HSET_H

#line 31

#define D_VCSID_HSET_H "$Id$"

#include "hcore/hsbbstree.h"

namespace yaal
{

namespace hcore
{

template<typename tType>
struct set_helper
{

template<typename key_t>
inline static void update( key_t&, key_t const& )
	{	}

template<typename key_t>
inline static bool less( key_t const& left, key_t const& right )
	{	return ( left < right );	}

};

template<typename tType, typename ttType = set_helper<tType> >
class HSet
	{
public:
	class HIterator;
	typedef HIterator iterator;
private:
	HSBBSTree f_oEngine;
public:
	HSet( void ) : f_oEngine() {};
	size_t size( void ) const
		{ return ( f_oEngine.size() );	}
	bool empty( void ) const
		{ return ( f_oEngine.empty() );	}
	HIterator insert( tType const& e )
		{	return ( HIterator( f_oEngine.insert<tType, ttType>( e ) ) );	}
	void remove( tType const& e )
		{	f_oEngine.remove<tType, tType, ttType>( e );	}
	HIterator erase( HIterator const& it )
		{
		HIterator newIt( it );
		++ newIt;
		f_oEngine.remove( it.f_oEngine );
		return ( newIt );
		}
	HIterator find( tType const& e ) const
		{ return ( HIterator( f_oEngine.find<tType, tType, ttType>( e ) ) ); }
	HIterator begin( void ) const
		{ return ( HIterator( f_oEngine.begin() ) ); }
	HIterator end( void ) const
		{ return ( HIterator( f_oEngine.end() ) ); }
	HIterator rbegin( void ) const
		{ return ( HIterator( f_oEngine.rbegin() ) ); }
	HIterator rend( void ) const
		{ return ( HIterator( f_oEngine.rend() ) ); }
	void clear( void )
		{ f_oEngine.clear(); }
	static void swap( HSet<tType, ttType>& left, HSet<tType, ttType>& right )
		{
		if ( &left != &right )
			HSBBSTree::swap( left.f_oEngine, right.f_oEngine );
		}
	void copy_from( HSet<tType, ttType> const& source )
		{
		if ( &source != this )
			f_oEngine.copy_from<tType, ttType>( source.f_oEngine );
		}
	};

template<typename tType, typename ttType = set_helper<tType> >
class HSet<tType, ttType>::HIterator
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
	tType const& operator * ( void )
		{	return ( f_oEngine.operator*<tType>() );	}
	tType const* operator -> ( void )
		{ return ( &f_oEngine.operator*<tType>() );	}
	bool operator == ( HIterator const& it ) const
		{ return ( f_oEngine == it.f_oEngine ); }
	bool operator != ( HIterator const& it ) const
		{ return ( f_oEngine != it.f_oEngine ); }
private:
	friend class HSet<tType, ttType>;
	explicit HIterator( HSBBSTree::HIterator const& it ) : f_oEngine( it ) {};
	};

}

}

#endif /* not __YAAL_HCORE_HSET_H */

