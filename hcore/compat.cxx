/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	compat.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib>

#include "compat.hxx"

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 ) || ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 )
#include <cmath>
#endif /* not HAVE_POWL *//* not HAVE_DECL_FLOORL */

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 ) || ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 )
#include <cstring>
#endif /* #if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 ) || ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 ) */

#if ! defined( HAVE_ASPRINTF ) || ( HAVE_ASPRINTF == 0 )
#include <cstdio>
#include <cstdarg>
#endif /* not HAVE_ASPRINTF */

#if ( defined( HAVE_GETHOSTBYNAME_R ) && ( ! defined( HAVE_GNU_GETHOSTBYNAME_R ) ) ) || ( defined( HAVE_GETHOSTBYADDR_R ) && ( ! defined( HAVE_GNU_GETHOSTBYADDR_R ) ) )
#include <cerrno>
#endif /* ( HAVE_GETHOSTBYNAME_R && not HAVE_GNU_GETHOSTBYNAME_R ) || ( HAVE_GETHOSTBYADDR_R && not HAVE_GNU_GETHOSTBYADDR_R ) */

#if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 )
#include "hstring.hxx"
#endif /* not HAVE_STRCASESTR */

#include "base.hxx"

M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 )
void* memrchr( void const* ptr, int what, int long from ) {
	char const* str = static_cast<char const*>( ptr );
	while ( -- from >= 0 ) {
		if ( str[ from ] == what )
			break;
	}
	return ( from >= 0 ? const_cast<char*>( str + from ) : 0 );
}
#endif /* not HAVE_MEMRCHR */

#if ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 )
int long strnlen( char const* str_, int long maxLen_ ) {
	char const* end( static_cast<char const*>( ::memchr( str_, 0, maxLen_ ) ) );
	return ( end ? end - str_ : maxLen_ );
}
#endif /* #if ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 ) */

#if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 )
#include <cstring>
char* strcasestr( char const* haystack, char const* needle ) {
	int long idx = ::yaal::hcore::string_helper::kmpcasesearch( ( haystack ), static_cast<int long>( ::std::strlen( haystack ) ), ( needle ), static_cast<int long>( ::std::strlen( needle ) ) );
	return ( idx >= 0 ? const_cast<char*>( haystack ) + idx : 0 );
}
#endif /* not HAVE_STRCASESTR */

#if defined( HAVE_GETHOSTBYNAME_R )
#if ! defined( HAVE_GNU_GETHOSTBYNAME_R )
#include <cstdlib>
#include <netdb.h>
int gethostbyname_r( char const* a0, struct hostent* a1, char* a2, long unsigned a3, struct hostent** a4, int* a5 ) {
	hostent* h = ::gethostbyname_r( a0, a1, a2, static_cast<int>( a3 ), a5 );
	if ( a4 )
		*a4 = h;
	return ( h ? 0 : errno );
}
#endif /* not HAVE_GNU_GETHOSTBYNAME_R */
#endif /* HAVE_GETHOSTBYNAME_R */

#if defined( HAVE_GETHOSTBYADDR_R )
#if ! defined( HAVE_GNU_GETHOSTBYADDR_R )
#include <cstdlib>
#include <netdb.h>
int gethostbyaddr_r( void const* a0, int a1, int a2, struct hostent* a3, char* a4, long unsigned a5, struct hostent** a6, int* a7 ) {
	hostent* h = ::gethostbyaddr_r( static_cast<char const*>( a0 ), a1, a2, a3, a4, static_cast<int>( a5 ), a7 );
	if ( a6 )
		*a6 = h;
	return ( h ? 0 : errno );
}
#endif /* not HAVE_GNU_GETHOSTBYADDR_R */
#endif /* HAVE_GETHOSTBYADDR_R */

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 )
double long powl( double long a, double long b ) {
	return ( ::std::pow( a, b ) );
}
#endif /* not HAVE_POWL */

#if ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 )
double long floorl( double long x ) {
	return ( ::std::floor( x ) );
}
#endif /* not HAVE_DECL_FLOORL */

#if ! defined( HAVE_DECL_SQRTL ) || ( HAVE_DECL_SQRTL == 0 )
double long sqrtl( double long x ) {
	return ( ::std::sqrt( x ) );
}
#endif /* not HAVE_DECL_SQRTL */

#if ! defined( HAVE_STRTOLD ) || ( HAVE_STRTOLD == 0 )
double long strtold( char const* str, char** tail ) {
	return ( strtod( str, tail ) );
}
#endif /* not HAVE_STRTOLD */

