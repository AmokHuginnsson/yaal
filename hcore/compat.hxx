/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	compat.h - this file is integral part of `yaal' project.

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

/* compatibility header for various platforms */

#ifndef YAAL_HCORE_COMPAT_HXX_INCLUDED
#define YAAL_HCORE_COMPAT_HXX_INCLUDED

#ifdef __YAAL_BUILD__
#include "config.hxx"
#endif /* __YAAL_BUILD__ */

#if ! defined( HAVE_PRETTY_FUNCTION )
#if ! defined( HAVE_FUNCTION )
#if ! defined( HAVE_FUNC )
#define __PRETTY_FUNCTION__ ""
#endif
#endif
#endif

#if ! defined( HAVE_BASENAME_ARG_CONST )
#define basename( x ) basename( const_cast<char*>( x ) )
#endif /* not HAVE_BASENAME_ARG_CONST */

#if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 )
char* strcasestr( char const*, char const* );
#endif /* not HAVE_STRCASESTR */

#if ! defined( HAVE_DECL_TEMP_FAILURE_RETRY ) || ( HAVE_DECL_TEMP_FAILURE_RETRY == 0 )
#define TEMP_FAILURE_RETRY(x) (x)
#endif /* not HAVE_DECL_TEMP_FAILURE_RETRY */

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 )
void* memrchr( void const*, int, int );
#endif /* not HAVE_MEMRCHR */

#if ! defined( HAVE_DECL_SUN_LEN ) || ( HAVE_DECL_SUN_LEN == 0 )
#define SUN_LEN(x) ( ::std::strlen( x->sun_path ) + sizeof ( x->sun_family ) )
#endif /* not HAVE_DECL_SUN_LEN */

#if ! defined( HAVE_GNU_GETHOSTBYNAME_R )
#include <netdb.h>
int gethostbyname_r( char const*, struct hostent*, char*, size_t, struct hostent**, int* );
#endif /* not HAVE_GNU_GETHOSTBYNAME_R */

#if ! defined( HAVE_GNU_GETHOSTBYADDR_R )
#include <netdb.h>
int gethostbyaddr_r( void const*, int, int, struct hostent*, char*, size_t, struct hostent**, int* );
#endif /* not HAVE_GNU_GETHOSTBYADDR_R */

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 )
double long powl( double long, double long );
#endif /* not HAVE_POWL */

#if ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 )
double long floorl( double long );
#endif /* not HAVE_DECL_FLOORL */

#if ! defined( HAVE_STRTOLD ) || ( HAVE_STRTOLD == 0 )
double long strtold( char const*, char** );
#endif /* not HAVE_STRTOLD */

#if ! defined( HAVE_ASPRINTF ) || ( HAVE_ASPRINTF == 0 )
int asprintf( char**, char const*, ... );
#endif /* not HAVE_ASPRINTF */

#if ! defined( __DYNAMIC_LINKER__ )
#define __DYNAMIC_LINKER__ ""
#endif /* not __DYNAMIC_LINKER__ */

#if ! defined( HAVE_DECLTYPE )
#define __decltype typeof
#endif /* not HAVE_DECLTYPE */

#endif /* not YAAL_HCORE_COMPAT_HXX_INCLUDED */

