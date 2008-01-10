/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	xalloc.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <libintl.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "xalloc.h"

namespace yaal
{

namespace hcore
{

void* xmalloc_internal( int long const a_lSize )
	{
	register void* l_pvNewPtr = NULL;
	if ( a_lSize < 0 )
		{
		::perror( _( "xmalloc_internal: requested size lower than 0" ) );
		::abort();
		}
	l_pvNewPtr = ::malloc( a_lSize );
	if ( l_pvNewPtr == 0 )
		{
		::perror( _( "xmalloc_internal: malloc returned NULL" ) );
		::abort();
		}
	return ( l_pvNewPtr );
	}

void* xcalloc_internal( int long a_lSize )
	{
	register void* l_pvNewPtr = xmalloc_internal( a_lSize );
	::memset( l_pvNewPtr, 0, a_lSize );
	return ( l_pvNewPtr );
	}

void* xrealloc_internal( void* a_pvPtr, int long a_lSize )
	{
	register void* l_pvNewPtr = NULL;
	if ( a_lSize < 0 )
		{
		::perror( _( "xrealloc_internal: requested size lower than 0" ) );
		::abort();
		}
	l_pvNewPtr = realloc ( a_pvPtr, a_lSize );
	if ( l_pvNewPtr == 0 )
		{
		::perror( _( "xrealloc_internal: realloc returned NULL" ) );
		::abort();
		}
	return ( l_pvNewPtr );
	}

void xfree_internal( void* a_pvPtr ) throw()
	{
	if ( a_pvPtr == NULL )
		{
		::perror( "xfree_internal: request to free NULL pointer" );
		::abort();
		}
	::free( a_pvPtr );
	return;
	}

char* xstrdup( char const* const a_pcStr )
	{
	char* l_pcNew = 0;
	if ( ! a_pcStr )
		{
		::perror( "xstrdup: request to duplicate NULL pointer string" );
		::abort();
		}
	l_pcNew = yaal::hcore::xcalloc<char>( ::strlen( a_pcStr ) + 1 );
	::strcpy( l_pcNew, a_pcStr );
	return ( l_pcNew );
	}

}

}

