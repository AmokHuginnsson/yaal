/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashmap.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HHASHMAP_H
#define __YAAL_HCORE_HHASHMAP_H

#line 31

#define D_VCSID_HHASHMAP_H "$Id$"

#include <cstddef>
#include <cstdlib>
#include <new>

#include "hcore/xalloc.h"
#include "hcore/hexception.h"
#include "hcore/hstring.h"

namespace yaal
{

namespace hcore
{

extern int long unsigned const* const g_pulPrimes;

template<typename tType>
inline int long unsigned hash( tType const& a_rtKey )
	{
	return ( static_cast<int long unsigned>( a_rtKey ) );
	}

/*! \brief Hash map container implementation.
 */
template<typename tType, typename ttType>
class HHashMap
	{
	typedef HHashMap<tType, ttType> self_t;
public:
	template<typename const_qual_t>
	class HIterator;
private:
	class HAtom
		{
	private:
		/*{*/
		HAtom* f_poNext;
		/*}*/
	public:
		/*{*/
		tType const	key;
		ttType	value;
		HAtom( tType const& );
		virtual ~HAtom( void );
		/*}*/
	private:
		/*{*/
		HAtom( HAtom const& );
		HAtom& operator = ( HAtom const& );
		/*}*/
		friend class HHashMap<tType, ttType>;
		template<typename const_qual_t>
		friend class HIterator;
		};
	int long unsigned f_ulPrime;
	int f_ulSize;
	HAtom** f_ppoAtomArray;
public:
	typedef HIterator<HAtom> iterator;
	typedef HIterator<HAtom const> const_iterator;
	HHashMap( size_t ); /* Lower bound of size of map's table */
	HHashMap( HHashMap const& );
	virtual ~HHashMap( void );
	HHashMap& operator = ( HHashMap const& );
	ttType& operator [] ( tType const& );
	const_iterator begin( void ) const;
	iterator begin( void );
	const_iterator end( void ) const;
	iterator end( void );
	const_iterator find( tType const& ) const;
	iterator find( tType const& );
	iterator insert( tType const&, ttType const& );
	void erase( iterator );
	bool has_key( tType const& ) const;
	bool get( tType const&, ttType& ) const;

	/*! \brief  Remove given key from map.
	 *
	 * \param  key Key to be removed.
	 * \return  True in case of failure.
	 */
	bool remove( tType const& key );
	void clear( void );
	int size( void ) const;
private:
	bool find( tType const&, int long unsigned&, HAtom*& ) const;
	};

/*! \brief Iterator for HHashMap<> data structure.
 */
template<typename tType, typename ttType>
template<typename const_qual_t>
class HHashMap<tType, ttType>::HIterator
	{
	typedef HHashMap<tType, ttType> owner_t;
	owner_t const* f_poOwner;
	int long unsigned f_ulIndex;
	HAtom* f_poAtomPtr;
public:
	HIterator( void ) : f_poOwner( NULL ), f_ulIndex( 0 ), f_poAtomPtr( NULL ) {}
	HIterator( HIterator const& a_oIt )
		: f_poOwner( a_oIt.f_poOwner ), f_ulIndex( a_oIt.f_ulIndex ),
		f_poAtomPtr( a_oIt.f_poAtomPtr ) {}
	HIterator& operator = ( HIterator const& a_oIt )
		{
		if ( &a_oIt != this )
			{
			f_poOwner = a_oIt.f_poOwner;
			f_ulIndex = a_oIt.f_ulIndex;
			f_poAtomPtr = a_oIt.f_poAtomPtr;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		if ( f_poAtomPtr )
			{
			f_poAtomPtr = f_poAtomPtr->f_poNext;
			if ( ! f_poAtomPtr )
				f_ulIndex ++;
			}
		if ( ! f_poAtomPtr )
			{
			while ( ( f_ulIndex < f_poOwner->f_ulPrime ) && ! f_poOwner->f_ppoAtomArray[ f_ulIndex ] )
				f_ulIndex ++;
			if ( f_ulIndex < f_poOwner->f_ulPrime )
				f_poAtomPtr = f_poOwner->f_ppoAtomArray[ f_ulIndex ];
			}
		if ( ! f_poAtomPtr )
			f_ulIndex = f_poOwner->f_ulPrime;
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
		M_ASSERT( f_poAtomPtr );
		return ( *f_poAtomPtr );
		}
	const_qual_t& operator* ( void ) const
		{
		M_ASSERT( f_poAtomPtr );
		return ( *f_poAtomPtr );
		}
	const_qual_t* operator-> ( void )
		{ return ( f_poAtomPtr ); }
	const_qual_t* operator-> ( void ) const
		{ return ( f_poAtomPtr ); }
	template<typename other_const_qual_t>
	bool operator == ( HIterator<other_const_qual_t> const& it ) const
		{
		M_ASSERT( f_poOwner == it.f_poOwner );
		return ( ( f_ulIndex == it.f_ulIndex ) && ( f_poAtomPtr == it.f_poAtomPtr ) );
		}
	template<typename other_const_qual_t>
	bool operator != ( HIterator<other_const_qual_t> const& it ) const
		{ return ( ! operator == ( it ) ); }
private:
	friend class HHashMap<tType, ttType>;
	explicit HIterator( owner_t const* a_poOwner, int unsigned long a_ulIndex, HAtom* a_poAtomPtr )
		: f_poOwner( a_poOwner ), f_ulIndex( a_ulIndex ), f_poAtomPtr( a_poAtomPtr ) {};
	};

template<typename tType, typename ttType>
HHashMap<tType, ttType>::HAtom::HAtom( tType const& a_tKey )
	: f_poNext( NULL ), key( a_tKey ), value()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename tType, typename ttType>
HHashMap<tType, ttType>::HAtom::~HAtom( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template<typename tType, typename ttType>
HHashMap<tType, ttType>::HHashMap ( size_t a_uiSize ) : f_ulPrime( 0 ),
	f_ulSize( 0 ), f_ppoAtomArray( NULL )
	{
	M_PROLOG
	int unsigned l_uiCtr = 0;
	if ( a_uiSize < 1 )
		M_THROW ( "bad map size", a_uiSize );
	while ( a_uiSize )
		{
		a_uiSize >>= 1;
		l_uiCtr ++;
		}
	f_ulPrime = g_pulPrimes[ l_uiCtr - 1 ];
	f_ppoAtomArray = xcalloc<HAtom*>( f_ulPrime );
	return;
	M_EPILOG
	}

template<typename tType, typename ttType>
HHashMap<tType, ttType>::HHashMap( HHashMap const& a_roMap ) : f_ulPrime( 0 ),
	f_ulSize( 0 ), f_ppoAtomArray( NULL )
	{
	M_PROLOG
	operator = ( a_roMap );
	return;
	M_EPILOG
	}

template<typename tType, typename ttType>
HHashMap<tType, ttType>::~HHashMap ( void )
	{
	M_PROLOG
	clear();
	if ( f_ppoAtomArray )
		xfree ( f_ppoAtomArray );
	f_ppoAtomArray = NULL;
	return;
	M_EPILOG
	}

template<typename tType, typename ttType>
HHashMap<tType, ttType> & HHashMap<tType, ttType>::operator = ( HHashMap const & a_roMap )
	{
	M_PROLOG
	int unsigned l_iCtr = 0;
	if ( & a_roMap != this )
		{
		clear();
		if ( f_ppoAtomArray )
			xfree ( f_ppoAtomArray );
		f_ppoAtomArray = NULL;
		f_ulPrime = a_roMap.f_ulPrime;
		f_ulSize = a_roMap.f_ulSize;
		f_ppoAtomArray = xcalloc<HAtom*>( f_ulPrime );
		for ( l_iCtr = 0; l_iCtr < f_ulPrime; ++ l_iCtr )
			{
			HAtom* l_poOrigAtom = a_roMap.f_ppoAtomArray [ l_iCtr ];
			HAtom** l_ppoAtom = &f_ppoAtomArray[ l_iCtr ];
			while ( l_poOrigAtom )
				{
				( *l_ppoAtom ) = new ( std::nothrow ) HAtom ( l_poOrigAtom->key );
				( *l_ppoAtom )->value = l_poOrigAtom->value;
				l_ppoAtom = &( *l_ppoAtom )->f_poNext;
				l_poOrigAtom = l_poOrigAtom->f_poNext;
				}
			}
		}
	return ( * this );
	M_EPILOG
	}

template<typename tType, typename ttType>
void HHashMap<tType, ttType>::clear( void )
	{
	M_PROLOG
	int unsigned l_uiCtr = 0;
	HAtom * l_poAtom = NULL;
	for ( l_uiCtr = 0; l_uiCtr < f_ulPrime; l_uiCtr ++ )
		while ( f_ppoAtomArray [ l_uiCtr ] )
			{
			l_poAtom = f_ppoAtomArray [ l_uiCtr ]->f_poNext;
			delete f_ppoAtomArray [ l_uiCtr ];
			f_ppoAtomArray [ l_uiCtr ] = l_poAtom;
			}
	f_ulSize = 0;
	return;
	M_EPILOG
	}

template<typename tType, typename ttType>
int HHashMap<tType, ttType>::size( void ) const
	{
	M_PROLOG
	return ( f_ulSize );
	M_EPILOG
	}

template<typename tType, typename ttType>
ttType& HHashMap<tType, ttType>::operator [] ( tType const& a_tKey )
	{
	M_PROLOG
	iterator it = insert( a_tKey, ttType() );
	return ( it->value );
	M_EPILOG
	}

template<typename tType, typename ttType>
typename HHashMap<tType, ttType>::iterator HHashMap<tType, ttType>::insert( tType const& a_tKey, ttType const& a_tValue )
	{
	M_PROLOG
	iterator it = find( a_tKey );
	if ( it == end() )
		{
		HAtom* l_poAtom = new ( std::nothrow ) HAtom( a_tKey );
		if ( ! l_poAtom )
			M_THROW ( "memory allocation error", errno );
		int long unsigned l_ulHash = hash( a_tKey ) % f_ulPrime;
		l_poAtom->f_poNext = f_ppoAtomArray[ l_ulHash ];
		f_ppoAtomArray[ l_ulHash ] = l_poAtom;
		f_ulSize ++;
		it = iterator( this, l_ulHash, l_poAtom );
		it->value = a_tValue;
		}
	return ( it );
	M_EPILOG
	}

template<typename tType, typename ttType>
typename HHashMap<tType, ttType>::iterator HHashMap<tType, ttType>::begin( void )
	{
	M_PROLOG
	iterator it( this, 0, NULL );
	return ( ++ it );
	M_EPILOG
	}

template<typename tType, typename ttType>
typename HHashMap<tType, ttType>::iterator HHashMap<tType, ttType>::end( void )
	{
	M_PROLOG
	return ( iterator( this, f_ulPrime, NULL ) );
	M_EPILOG
	}

template<typename tType, typename ttType>
typename HHashMap<tType, ttType>::const_iterator HHashMap<tType, ttType>::begin( void ) const
	{
	M_PROLOG
	const_iterator it( this, 0, NULL );
	return ( ++ it );
	M_EPILOG
	}

template<typename tType, typename ttType>
typename HHashMap<tType, ttType>::const_iterator HHashMap<tType, ttType>::end( void ) const
	{
	M_PROLOG
	return ( const_iterator( this, f_ulPrime, NULL ) );
	M_EPILOG
	}

template<typename tType, typename ttType>
bool HHashMap<tType, ttType>::has_key( tType const& a_rtKey ) const
	{
	M_PROLOG
	return ( find( a_rtKey ) != end() );
	M_EPILOG
	}

template<typename tType, typename ttType>
typename HHashMap<tType, ttType>::iterator HHashMap<tType, ttType>::find( tType const& a_tKey )
	{
	M_PROLOG
	int long unsigned idx = 0;
	HAtom* l_poAtom = NULL;
	return ( find( a_tKey, idx, l_poAtom ) ? iterator( this, idx, l_poAtom ) : end() );
	M_EPILOG
	}

template<typename tType, typename ttType>
typename HHashMap<tType, ttType>::const_iterator HHashMap<tType, ttType>::find( tType const& a_tKey ) const
	{
	M_PROLOG
	int long unsigned idx = 0;
	HAtom* l_poAtom = NULL;
	return ( find( a_tKey, idx, l_poAtom ) ? const_iterator( this, idx, l_poAtom ) : end() );
	M_EPILOG
	}

template<typename tType, typename ttType>
bool HHashMap<tType, ttType>::find( tType const& a_rtKey, int long unsigned& a_rulIndex, HAtom*& a_rpoAtom ) const
	{
	M_PROLOG
	a_rulIndex = hash( a_rtKey ) % f_ulPrime;
	a_rpoAtom = f_ppoAtomArray[ a_rulIndex ];
	while ( a_rpoAtom && ( a_rpoAtom->key != a_rtKey ) )
		a_rpoAtom = a_rpoAtom->f_poNext;
	return ( a_rpoAtom ? true : false );
	M_EPILOG
	}

template<typename tType, typename ttType>
bool HHashMap<tType, ttType>::get( tType const& a_rtKey, ttType& a_rtValue ) const
	{
	M_PROLOG
	const_iterator it = find( a_rtKey );
	bool found = false;
	if ( it != end() )
		{
		a_rtValue = it->value;
		found = true;
		}
	return ( ! found );
	M_EPILOG
	}

template<typename tType, typename ttType>
void HHashMap<tType, ttType>::erase( iterator it )
	{
	M_PROLOG
	HAtom* l_poAtom = f_ppoAtomArray[ it.f_ulIndex ];
	HAtom* l_poAncestor = NULL;
	M_ASSERT( l_poAtom );
	while ( l_poAtom != it.f_poAtomPtr )
		l_poAncestor = l_poAtom, l_poAtom = l_poAtom->f_poNext;
	if ( l_poAtom )
		{
		if ( l_poAncestor )
			l_poAncestor->f_poNext = l_poAtom->f_poNext;
		else
			f_ppoAtomArray[ it.f_ulIndex ] = NULL;
		delete l_poAtom;
		f_ulSize --;
		}
	return;
	M_EPILOG
	}

template<typename tType, typename ttType>
bool HHashMap<tType, ttType>::remove( tType const& a_tKey )
	{
	M_PROLOG
	iterator it = find( a_tKey );
	bool erased = it != end();
	if ( erased )
		erase( it );
	return ( ! erased );
	M_EPILOG
	}

/* Helpers and premaps */

int long unsigned hash( HString const& );

}

}

#endif /* not __YAAL_HCORE_HHASHMAP_H */

