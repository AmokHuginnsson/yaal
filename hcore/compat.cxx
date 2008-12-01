/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	compat.cxx - this file is integral part of `yaal' project.

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

#include "compat.h"

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 ) || ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 )
#include <cmath>
#endif /* not HAVE_POWL *//* not HAVE_DECL_FLOORL */

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 )
#include <cstring>
#endif /* not HAVE_MEMRCHR */

#if ! defined( HAVE_ASPRINTF ) || ( HAVE_ASPRINTF == 0 )
#include <cstdio>
#include <cstdarg>
#endif /* not HAVE_ASPRINTF */

#if ( defined( HAVE_GETHOSTBYNAME_R ) && ( ! defined( HAVE_GNU_GETHOSTBYNAME_R ) ) ) || ( defined( HAVE_GETHOSTBYADDR_R ) && ( ! defined( HAVE_GNU_GETHOSTBYADDR_R ) ) )
#include <cerrno>
#endif /* ( HAVE_GETHOSTBYNAME_R && not HAVE_GNU_GETHOSTBYNAME_R ) || ( HAVE_GETHOSTBYADDR_R && not HAVE_GNU_GETHOSTBYADDR_R ) */

#if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 )
#include "hstring.h"
#endif /* not HAVE_STRCASESTR */

#include "xalloc.h"

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 )
void* memrchr( void const* ptr, int what, int from )
	{
	char const* str = static_cast<char const*>( ptr );
	while ( -- from >= 0 )
		{
		if ( str[ from ] == what )
			break;
		}
	return ( from >= 0 ? const_cast<char*>( str + from ) : 0 );
	}
#endif /* not HAVE_MEMRCHR */

#if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 )
char* strcasestr( char const* haystack, char const* needle )
	{
	int long idx = ::yaal::hcore::string_helper::kmpcasesearch( ( haystack ), ::std::strlen( haystack ), ( needle ), ::std::strlen( needle ) );
	return ( idx >= 0 ? const_cast<char*>( haystack ) + idx : 0 );
	}
#endif /* not HAVE_STRCASESTR */

#if defined( HAVE_GETHOSTBYNAME_R )
#if ! defined( HAVE_GNU_GETHOSTBYNAME_R )
int gethostbyname_r( char const* a0, struct hostent* a1, char* a2, size_t a3, struct hostent** a4, int* a5 )
	{
	hostent* h = ::gethostbyname_r( a0, a1, a2, a3, a5 );
	if ( a4 )
		*a4 = h;
	return ( h ? 0 : errno );
	}
#endif /* not HAVE_GNU_GETHOSTBYNAME_R */
#endif /* HAVE_GETHOSTBYNAME_R */

#if defined( HAVE_GETHOSTBYADDR_R )
#if ! defined( HAVE_GNU_GETHOSTBYADDR_R )
int gethostbyaddr_r( void const* a0, int a1, int a2, struct hostent* a3, char* a4, size_t a5, struct hostent** a6, int* a7 )
	{
	hostent* h = ::gethostbyaddr_r( static_cast<char const*>( a0 ), a1, a2, a3, a4, a5, a7 );
	if ( a6 )
		*a6 = h;
	return ( h ? 0 : errno );
	}
#endif /* not HAVE_GNU_GETHOSTBYADDR_R */
#endif /* HAVE_GETHOSTBYADDR_R */

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 )
double long powl( double long a, double long b )
	{
	return ( ::std::pow( a, b ) );
	}
#endif /* not HAVE_POWL */

#if ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 )
double long floorl( double long x )
	{
	return ( ::std::floor( x ) );
	}
#endif /* not HAVE_DECL_FLOORL */

#if ! defined( HAVE_STRTOLD ) || ( HAVE_STRTOLD == 0 )
double long strtold( char const* str, char** tail )
	{
	return ( strtod( str, tail ) );
	}
#endif /* not HAVE_STRTOLD */

#if ! defined( HAVE_ASPRINTF ) || ( HAVE_ASPRINTF == 0 )
int asprintf( char** pbuf, char const* fmt, ... )
	{
	::std::va_list ap;
	va_start( ap, fmt );
	::std::va_list apTest;
	__va_copy( apTest, ap );
	int size = vsnprintf( NULL, 0, fmt, apTest ) + 1;
	if ( size > 0 )
		{
		*pbuf = yaal::hcore::xcalloc<char>( size );
		size = vsnprintf( *pbuf, size, fmt, ap );
		}
	va_end( apTest );
	va_end( ap );
	return ( size );
	}
#endif /* not HAVE_ASPRINTF */

