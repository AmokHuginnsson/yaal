/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	xalloc.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "xalloc.hxx"

namespace yaal
{

namespace hcore
{

void* xmalloc_internal( int long const size_ )
	{
	register void* newPtr = NULL;
	if ( size_ < 0 )
		{
		::perror( _( "xmalloc_internal: requested size lower than 0" ) );
		::abort();
		}
	newPtr = ::malloc( size_ );
	if ( newPtr == 0 )
		{
		::perror( _( "xmalloc_internal: malloc returned NULL" ) );
		::abort();
		}
	return ( newPtr );
	}

void* xcalloc_internal( int long size_ )
	{
	register void* newPtr = xmalloc_internal( size_ );
	::memset( newPtr, 0, size_ );
	return ( newPtr );
	}

void* xrealloc_internal( void* ptr_, int long size_ )
	{
	register void* newPtr = NULL;
	if ( size_ < 0 )
		{
		::perror( _( "xrealloc_internal: requested size lower than 0" ) );
		::abort();
		}
	newPtr = realloc( ptr_, size_ );
	if ( newPtr == 0 )
		{
		::perror( _( "xrealloc_internal: realloc returned NULL" ) );
		::abort();
		}
	return ( newPtr );
	}

void xfree_internal( void* ptr_ ) throw()
	{
	if ( ptr_ == NULL )
		{
		::perror( "xfree_internal: request to free NULL pointer" );
		::abort();
		}
	::free( ptr_ );
	return;
	}

char* xstrdup( char const* const str_ )
	{
	char* str = 0;
	if ( ! str_ )
		{
		::perror( "xstrdup: request to duplicate NULL pointer string" );
		::abort();
		}
	str = yaal::hcore::xcalloc<char>( static_cast<int long>( ::strlen( str_ ) ) + 1 );
	::strcpy( str, str_ );
	return ( str );
	}

}

}

