/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	xalloc.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_XALLOC_HXX_INCLUDED
#define YAAL_HCORE_XALLOC_HXX_INCLUDED

namespace yaal
{

/*! \brief Core functionality.
 * 
 * This namespace contains core library functionality that is
 * independent from UI or external software.
 */
namespace hcore
{

void* xmalloc_internal( int long const );
void* xcalloc_internal( int long const );
void* xrealloc_internal( void*, int long const );
void xfree_internal( void* ) throw();
char* xstrdup ( char const* const );

template<typename tType>
inline tType* xmalloc( int long const count_ )
	{
	return ( static_cast<tType*>( xmalloc_internal( count_ * sizeof ( tType ) ) ) );
	}

template<typename tType>
inline tType* xcalloc( int long const count_ )
	{
	return ( static_cast<tType*>( xcalloc_internal( count_ * sizeof ( tType ) ) ) );
	}

template<typename tType>
inline tType* xrealloc( void* pointer_, int long const count_ )
	{
	return ( static_cast<tType*>( xrealloc_internal ( pointer_, count_ * sizeof ( tType ) ) ) );
	}

template<typename tType>
inline void xfree( tType& pointer_ ) throw()
	{
	xfree_internal( pointer_ );
	pointer_ = NULL;
	return;
	}

}

}

#endif /* not YAAL_HCORE_XALLOC_HXX_INCLUDED */

