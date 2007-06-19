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

#line 31 "hhashmap.h"

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

extern int long unsigned const * const g_pulPrimes;

template < typename tType >
inline int long unsigned hash ( tType const & a_rtKey )
	{
	return ( static_cast < int long unsigned > ( a_rtKey ) );
	}

template < typename tType, typename ttType >
class HHashMap
	{
	class HAtom
		{
	protected:
		/*{*/
		tType		f_tKey;
		ttType	f_tValue;
		HAtom * f_poNext;
		/*}*/
	public:
		/*{*/
		HAtom ( void );
		virtual ~HAtom ( void );
		/*}*/
	private:
		/*{*/
		HAtom ( HAtom const & );
		HAtom & operator = ( HAtom const & );
		/*}*/
		friend class HHashMap < tType, ttType >;
		};
	int unsigned f_uiPrime;
	int unsigned f_uiIndex;
	int f_iQuantity;
	HAtom * f_poAtomPtr;
	HAtom ** f_ppoAtomArray;
public:
	HHashMap ( size_t ); /* Lower bound of size of map's table */
	HHashMap ( HHashMap const & );
	virtual ~HHashMap ( void );
	HHashMap & operator = ( HHashMap const & );
	ttType & operator [ ] ( tType const & );
	void rewind ( void );
	bool iterate ( tType &, ttType & );
	bool has_key ( tType const & ) const;
	bool get ( tType const &, ttType & ) const;
	bool remove ( tType const & );
	void flush ( void );
	int quantity ( void ) const;
	};

template < typename tType, typename ttType >
HHashMap<tType, ttType>::HAtom::HAtom ( void ) : f_tKey(), f_tValue(),
																						 f_poNext ( NULL )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template < typename tType, typename ttType >
HHashMap<tType, ttType>::HAtom::~HAtom ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template < typename tType, typename ttType >
HHashMap<tType, ttType>::HHashMap ( size_t a_uiSize ) : f_uiPrime ( 0 ), f_uiIndex ( 0 ),
	f_iQuantity ( 0 ), f_poAtomPtr ( NULL ), f_ppoAtomArray ( NULL )
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
	f_uiPrime = g_pulPrimes [ l_uiCtr - 1 ];
	f_ppoAtomArray = xcalloc < HAtom * > ( f_uiPrime );
	return;
	M_EPILOG
	}

template < typename tType, typename ttType >
HHashMap<tType, ttType>::HHashMap ( HHashMap const & a_roMap ) :  f_uiPrime ( 0 ), f_uiIndex ( 0 ),
	f_iQuantity ( 0 ), f_poAtomPtr ( NULL ), f_ppoAtomArray ( NULL )
	{
	M_PROLOG
	operator = ( a_roMap );
	return;
	M_EPILOG
	}

template < typename tType, typename ttType >
HHashMap<tType, ttType>::~HHashMap ( void )
	{
	M_PROLOG
	flush();
	if ( f_ppoAtomArray )
		xfree ( f_ppoAtomArray );
	f_ppoAtomArray = NULL;
	return;
	M_EPILOG
	}

template < typename tType, typename ttType >
HHashMap<tType, ttType> & HHashMap<tType, ttType>::operator = ( HHashMap const & a_roMap )
	{
	M_PROLOG
	int unsigned l_iCtr = 0;
	HAtom * l_poAtom = NULL;
	HAtom ** l_ppoAtom = NULL;
	if ( & a_roMap != this )
		{
		flush();
		if ( f_ppoAtomArray )
			xfree ( f_ppoAtomArray );
		f_ppoAtomArray = NULL;
		f_uiPrime = a_roMap.f_uiPrime;
		f_uiIndex = a_roMap.f_uiIndex;
		f_iQuantity = a_roMap.f_iQuantity;
		f_ppoAtomArray = xcalloc < HAtom * > ( f_uiPrime );
		for ( l_iCtr = 0; l_iCtr < f_uiPrime; ++ l_iCtr )
			{
			l_poAtom =  a_roMap.f_ppoAtomArray [ l_iCtr ];
			l_ppoAtom = & f_ppoAtomArray [ l_iCtr ];
			while ( l_poAtom )
				{
				( * l_ppoAtom ) = new ( std::nothrow ) HAtom ();
				( * l_ppoAtom )->f_tKey = l_poAtom->f_tKey;
				( * l_ppoAtom )->f_tValue = l_poAtom->f_tValue;
				if ( l_poAtom == a_roMap.f_poAtomPtr )
					f_poAtomPtr = ( * l_ppoAtom );
				l_ppoAtom = & ( * l_ppoAtom )->f_poNext;
				l_poAtom = l_poAtom->f_poNext;
				}
			}
		}
	return ( * this );
	M_EPILOG
	}

template < typename tType, typename ttType >
void HHashMap<tType, ttType>::flush ( void )
	{
	M_PROLOG
	int unsigned l_uiCtr = 0;
	HAtom * l_poAtom = NULL;
	for ( l_uiCtr = 0; l_uiCtr < f_uiPrime; l_uiCtr ++ )
		while ( f_ppoAtomArray [ l_uiCtr ] )
			{
			l_poAtom = f_ppoAtomArray [ l_uiCtr ]->f_poNext;
			delete f_ppoAtomArray [ l_uiCtr ];
			f_ppoAtomArray [ l_uiCtr ] = l_poAtom;
			}
	f_iQuantity = 0;
	rewind();
	return;
	M_EPILOG
	}

template < typename tType, typename ttType >
int HHashMap < tType, ttType >::quantity ( void ) const
	{
	M_PROLOG
	return ( f_iQuantity );
	M_EPILOG
	}

template < typename tType, typename ttType >
ttType & HHashMap<tType, ttType>::operator [ ] ( tType const & a_rtKey )
	{
	M_PROLOG
	int l_iHash = - 1;
	HAtom * l_poAtom = NULL;
	l_iHash = hash ( a_rtKey ) % f_uiPrime;
	l_poAtom = f_ppoAtomArray [ l_iHash ];
	while ( l_poAtom && ( l_poAtom->f_tKey != a_rtKey ) )
		l_poAtom = l_poAtom->f_poNext;
	if ( ! l_poAtom )
		{
		l_poAtom = new ( std::nothrow ) HAtom();
		if ( ! l_poAtom )
			M_THROW ( "memory allocation error", errno );
		f_iQuantity ++;
		l_poAtom->f_poNext = f_ppoAtomArray [ l_iHash ];
		f_ppoAtomArray [ l_iHash ] = l_poAtom;
		l_poAtom->f_tKey = a_rtKey;
		}
	return ( l_poAtom->f_tValue );
	M_EPILOG
	}

template < typename tType, typename ttType >
void HHashMap<tType, ttType>::rewind ( void )
	{
	M_PROLOG
	f_uiIndex = 0;
	f_poAtomPtr = NULL;
	return;
	M_EPILOG
	}

template < typename tType, typename ttType >
bool HHashMap<tType, ttType>::iterate ( tType & a_rtKey, ttType & a_rtValue )
	{
	M_PROLOG
	if ( f_poAtomPtr )
		{
		f_poAtomPtr = f_poAtomPtr->f_poNext;
		if ( ! f_poAtomPtr )
			f_uiIndex ++;
		}
	if ( ! f_poAtomPtr )
		{
		while ( ( f_uiIndex < f_uiPrime ) && ! f_ppoAtomArray [ f_uiIndex ] )
			f_uiIndex ++;
		if ( f_uiIndex < f_uiPrime )
			f_poAtomPtr = f_ppoAtomArray [ f_uiIndex ];
		}
	if ( f_poAtomPtr )
		{
		a_rtKey = f_poAtomPtr->f_tKey;
		a_rtValue = f_poAtomPtr->f_tValue;
		}
	return ( f_poAtomPtr ? true : false );
	M_EPILOG
	}

template < typename tType, typename ttType >
bool HHashMap<tType, ttType>::has_key ( tType const & a_rtKey ) const
	{
	M_PROLOG
	int l_iHash = - 1;
	HAtom * l_poAtom = NULL;
	l_iHash = hash ( a_rtKey ) % f_uiPrime;
	l_poAtom = f_ppoAtomArray [ l_iHash ];
	while ( l_poAtom && ( l_poAtom->f_tKey != a_rtKey ) )
		l_poAtom = l_poAtom->f_poNext;
	return ( l_poAtom ? true : false );
	M_EPILOG
	}

template < typename tType, typename ttType >
bool HHashMap<tType, ttType>::get ( tType const & a_rtKey, ttType & a_rtValue ) const
	{
	M_PROLOG
	int l_iHash = - 1;
	HAtom * l_poAtom = NULL;
	l_iHash = hash ( a_rtKey ) % f_uiPrime;
	l_poAtom = f_ppoAtomArray [ l_iHash ];
	while ( l_poAtom && ( l_poAtom->f_tKey != a_rtKey ) )
		l_poAtom = l_poAtom->f_poNext;
	if ( l_poAtom )
		a_rtValue = l_poAtom->f_tValue;
	return ( l_poAtom ? true : false );
	M_EPILOG
	}

template < typename tType, typename ttType >
bool HHashMap<tType, ttType>::remove ( tType const & a_rtKey )
	{
	M_PROLOG
	int l_iHash = - 1;
	HAtom * l_poAtom = NULL, * l_poAncestor = NULL;
	l_iHash = hash ( a_rtKey ) % f_uiPrime;
	l_poAtom = f_ppoAtomArray [ l_iHash ];
	while ( l_poAtom && ( l_poAtom->f_tKey != a_rtKey ) )
		{
		l_poAncestor = l_poAtom;
		l_poAtom = l_poAtom->f_poNext;
		}
	if ( l_poAtom )
		{
		if ( l_poAncestor )
			l_poAncestor->f_poNext = l_poAtom->f_poNext;
		else
			f_ppoAtomArray [ l_iHash ] = NULL;
		delete l_poAtom;
		rewind();
		f_iQuantity --;
		}
	return ( l_poAtom ? true : false );
	M_EPILOG
	}

/* Helpers and premaps */

int long unsigned hash ( HString const & );

}

}

#endif /* not __YAAL_HCORE_HHASHMAP_H */

