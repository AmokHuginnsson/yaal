/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	xalloc.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_XALLOC_H
#define __HCORE_XALLOC_H

extern "C"
{

void * xmalloc_internal ( int long const );
void * xcalloc_internal ( int long const );
void * xrealloc_internal ( void *, int long const );
void xfree_internal ( void * * );
char * xstrdup ( char const * const );

}

namespace stdhapi
{

namespace hcore
{

template < typename tType >
inline tType * xmalloc ( int long const a_iCount )
	{
	return ( static_cast < tType * > ( xmalloc_internal ( a_iCount * sizeof ( tType ) ) ) );
	}

template < typename tType >
inline tType * xcalloc ( int long const a_iCount )
	{
	return ( static_cast < tType * > ( xcalloc_internal ( a_iCount * sizeof ( tType ) ) ) );
	}

template < typename tType >
inline tType * xrealloc ( void * a_pvPointer, int long const a_iCount )
	{
	return ( static_cast < tType * > ( xrealloc_internal ( a_pvPointer, a_iCount * sizeof ( tType ) ) ) );
	}

template < typename tType >
void xfree ( tType a_tPointer )
	{
	xfree_internal ( reinterpret_cast < void * * > ( & a_tPointer ) );
	return;
	}

}

}

#endif /* not __HCORE_XALLOC_H */

