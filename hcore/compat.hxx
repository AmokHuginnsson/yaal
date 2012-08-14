/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	compat.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/compat.hxx
 * \brief Compatibility header for various platforms.
 */

#ifndef YAAL_HCORE_COMPAT_HXX_INCLUDED
#define YAAL_HCORE_COMPAT_HXX_INCLUDED 1

#include "config.hxx"

#ifdef __YAAL_BUILD__

/*! \cond */
#if ! defined( HAVE_BASENAME_ARG_CONST )
#define basename( x ) basename( const_cast<char*>( x ) )
#endif /* #if ! defined( HAVE_BASENAME_ARG_CONST ) */

#if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 )
char* strcasestr( char const*, char const* );
#endif /* #if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 ) */

#if ! defined( HAVE_DECL_TEMP_FAILURE_RETRY ) || ( HAVE_DECL_TEMP_FAILURE_RETRY == 0 )
#define TEMP_FAILURE_RETRY(x) (x)
#endif /* #if ! defined( HAVE_DECL_TEMP_FAILURE_RETRY ) || ( HAVE_DECL_TEMP_FAILURE_RETRY == 0 ) */

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 )
void* memrchr( void const*, int, int long );
#endif /* #if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 ) */

#if ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 )
int long strnlen( char const*, int long );
#endif /* #if ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 ) */

#if ! defined( HAVE_DECL_SUN_LEN ) || ( HAVE_DECL_SUN_LEN == 0 )
#define SUN_LEN(x) ( ::std::strlen( x->sun_path ) + sizeof ( x->sun_family ) )
#endif /* #if ! defined( HAVE_DECL_SUN_LEN ) || ( HAVE_DECL_SUN_LEN == 0 ) */

#if ! defined( HAVE_GNU_GETHOSTBYNAME_R )
int gethostbyname_r( char const*, struct hostent*, char*, long unsigned, struct hostent**, int* );
#endif /* #if ! defined( HAVE_GNU_GETHOSTBYNAME_R ) */

#if ! defined( HAVE_GNU_GETHOSTBYADDR_R )
int gethostbyaddr_r( void const*, int, int, struct hostent*, char*, long unsigned, struct hostent**, int* );
#endif /* #if ! defined( HAVE_GNU_GETHOSTBYADDR_R ) */

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 )
double long powl( double long, double long );
#endif /* #if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 ) */

#if ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 )
double long floorl( double long );
#endif /* #if ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 ) */

#if ! defined( HAVE_DECL_SQRTL ) || ( HAVE_DECL_SQRTL == 0 )
double long sqrtl( double long );
#endif /* #if ! defined( HAVE_DECL_SQRTL ) || ( HAVE_DECL_SQRTL == 0 ) */

#if ! defined( HAVE_STRTOLD ) || ( HAVE_STRTOLD == 0 )
double long strtold( char const*, char** );
#endif /* #if ! defined( HAVE_STRTOLD ) || ( HAVE_STRTOLD == 0 ) */

#if ! defined( LOGIN_NAME_MAX )
static int const LOGIN_NAME_MAX = 16;
#endif /* #if ! defined( LOGIN_NAME_MAX ) */

#if ! defined( __DYNAMIC_LINKER__ )
#define __DYNAMIC_LINKER__ ""
#endif /* #if ! defined( __DYNAMIC_LINKER__ ) */

#endif /* __YAAL_BUILD__ */

#ifdef __GNUC__
#define CXX_EXTENSION_TLS __thread
#elif defined( __MSVCXX__ )
#define CXX_EXTENSION_TLS __declspec( thread )
#endif /* #elif defined( __MSVCXX__ ) #ifdef __GNUC__ */

#if ! defined( HAVE_PRETTY_FUNCTION )
#if ! defined( HAVE_FUNCTION )
#if ! defined( HAVE_FUNC )
#define __PRETTY_FUNCTION__ ""
#endif
#endif
#endif

#if ! defined( HAVE_DECLTYPE )
#define __decltype typeof
#endif /* #if ! defined( HAVE_DECLTYPE ) */

#if ( HAVE_DECL_ERR == 1 )
#include <cstdlib>
#undef ERR
#endif /* HAVE_DECL_ERR */

/*! \endcond */

#endif /* #ifndef YAAL_HCORE_COMPAT_HXX_INCLUDED */

