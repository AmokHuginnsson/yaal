/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/compat.hxx
 * \brief Compatibility header for various platforms.
 */

#ifndef YAAL_HCORE_COMPAT_HXX_INCLUDED
#define YAAL_HCORE_COMPAT_HXX_INCLUDED 1

#include "config.hxx"

#ifdef __YAAL_BUILD__

/*! \cond */
#if ! defined( HAVE_BASENAME_ARG_CONST )
#	undef basename
char const* basename_nc_impl( char const* );
inline char const* basename( char const* x ) {
	return ( basename_nc_impl( x ) );
}
#endif /* #if ! defined( HAVE_BASENAME_ARG_CONST ) */

#if ! defined( HAVE_DECL_TEMP_FAILURE_RETRY ) || ( HAVE_DECL_TEMP_FAILURE_RETRY == 0 )
#	define TEMP_FAILURE_RETRY(x) (x)
#endif /* #if ! defined( HAVE_DECL_TEMP_FAILURE_RETRY ) || ( HAVE_DECL_TEMP_FAILURE_RETRY == 0 ) */

#if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 )
void* memrchr( void const*, int, int long unsigned );
#endif /* #if ! defined( HAVE_MEMRCHR ) || ( HAVE_MEMRCHR == 0 ) */

#if ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 )
int long strnlen( char const*, int long );
#endif /* #if ! defined( HAVE_STRNLEN ) || ( HAVE_STRNLEN == 0 ) */

#if ! defined( HAVE_DECL_SUN_LEN ) || ( HAVE_DECL_SUN_LEN == 0 )
#	define SUN_LEN( su ) ( ( sizeof ( *(su) ) - sizeof ( (su)->sun_path ) + std::strlen( (su)->sun_path ) ) )
#endif /* #if ! defined( HAVE_DECL_SUN_LEN ) || ( HAVE_DECL_SUN_LEN == 0 ) */

#if ! defined( HAVE_GNU_GETHOSTBYNAME_R )
int gethostbyname_r( char const*, struct hostent*, char*, long unsigned, struct hostent**, int* );
#endif /* #if ! defined( HAVE_GNU_GETHOSTBYNAME_R ) */

#if ! defined( HAVE_GNU_GETHOSTBYADDR_R )
int gethostbyaddr_r( void const*, int, int, struct hostent*, char*, long unsigned, struct hostent**, int* );
#endif /* #if ! defined( HAVE_GNU_GETHOSTBYADDR_R ) */

#if ! defined( HAVE_CLOCK_GETTIME )
enum CLOCK_TYPE {
	CLOCK_REALTIME = 0,
	CLOCK_MONOTONIC = 1,
	CLOCK_THREAD_CPUTIME_ID = 2
};
typedef int clockid_t;
int clock_gettime( clockid_t, struct timespec* );
#endif /* #if ! defined( HAVE_CLOCK_GETTIME ) */

#if ! defined( HAVE_TIMER_CREATE )
#	include <ctime>
class HDarwinTimer;
typedef HDarwinTimer* timer_t;
struct itimerspec {
	struct timespec it_interval;  /* Timer interval */
	struct timespec it_value;     /* Initial expiration */
};
int timer_create( clockid_t, struct sigevent*, timer_t* );
int timer_settime( timer_t, int, struct itimerspec const *, struct itimerspec* );
int timer_delete( timer_t );
#endif /* #if ! defined( HAVE_TIMER_CREATE ) */

#if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 )
double long powl( double long, double long );
#endif /* #if ! defined( HAVE_POWL ) || ( HAVE_POWL == 0 ) */

#if ! defined( HAVE_DECL_EXPL ) || ( HAVE_DECL_EXPL == 0 )
double long expl( double long );
#endif /* #if ! defined( HAVE_DECL_EXPL ) || ( HAVE_DECL_EXPL == 0 ) */

#if ! defined( HAVE_DECL_LOGL ) || ( HAVE_DECL_LOGL == 0 )
double long logl( double long );
#endif /* #if ! defined( HAVE_DECL_LOGL ) || ( HAVE_DECL_LOGL == 0 ) */

#if ! defined( HAVE_DECL_SQRTL ) || ( HAVE_DECL_SQRTL == 0 )
double long sqrtl( double long );
#endif /* #if ! defined( HAVE_DECL_SQRTL ) || ( HAVE_DECL_SQRTL == 0 ) */

#if ! defined( HAVE_DECL_SINL ) || ( HAVE_DECL_SINL == 0 )
double long sinl( double long );
#endif /* #if ! defined( HAVE_DECL_SINL ) || ( HAVE_DECL_SINL == 0 ) */

#if ! defined( HAVE_DECL_COSL ) || ( HAVE_DECL_COSL == 0 )
double long cosl( double long );
#endif /* #if ! defined( HAVE_DECL_COSL ) || ( HAVE_DECL_COSL == 0 ) */

#if ! defined( HAVE_DECL_TANL ) || ( HAVE_DECL_TANL == 0 )
double long tanl( double long );
#endif /* #if ! defined( HAVE_DECL_TANL ) || ( HAVE_DECL_TANL == 0 ) */

#if ! defined( HAVE_DECL_ASINL ) || ( HAVE_DECL_ASINL == 0 )
double long asinl( double long );
#endif /* #if ! defined( HAVE_DECL_ASINL ) || ( HAVE_DECL_ASINL == 0 ) */

#if ! defined( HAVE_DECL_ACOSL ) || ( HAVE_DECL_ACOSL == 0 )
double long acosl( double long );
#endif /* #if ! defined( HAVE_DECL_ACOSL ) || ( HAVE_DECL_ACOSL == 0 ) */

#if ! defined( HAVE_DECL_ATANL ) || ( HAVE_DECL_ATANL == 0 )
double long atanl( double long );
#endif /* #if ! defined( HAVE_DECL_ATANL ) || ( HAVE_DECL_ATANL == 0 ) */

#if ! defined( HAVE_DECL_SINCOSL ) || ( HAVE_DECL_SINCOSL == 0 )
void sincosl( double long, double long*, double long* );
#endif /* #if ! defined( HAVE_DECL_SINCOSL ) || ( HAVE_DECL_SINCOSL == 0 ) */

#if ! defined( HAVE_DECL_FMODL ) || ( HAVE_DECL_FMODL == 0 )
double long fmodl( double long, double long );
#endif /* #if ! defined( HAVE_DECL_FMODL ) || ( HAVE_DECL_FMODL == 0 ) */

#if ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 )
double long floorl( double long );
#endif /* #if ! defined( HAVE_DECL_FLOORL ) || ( HAVE_DECL_FLOORL == 0 ) */

#if ! defined( HAVE_DECL_CEILL ) || ( HAVE_DECL_CEILL == 0 )
double long ceill( double long );
#endif /* #if ! defined( HAVE_DECL_CEILL ) || ( HAVE_DECL_CEILL == 0 ) */

#if ! defined( HAVE_DECL_ROUNDL ) || ( HAVE_DECL_ROUNDL == 0 )
double long roundl( double long );
#endif /* #if ! defined( HAVE_DECL_ROUNDL ) || ( HAVE_DECL_ROUNDL == 0 ) */

#if ! defined( HAVE_DECL_STRTOLD ) || ( HAVE_DECL_STRTOLD == 0 )
double long strtold( char const*, char** );
#endif /* #if ! defined( HAVE_DECL_STRTOLD ) || ( HAVE_DECL_STRTOLD == 0 ) */

#if ! defined( LOGIN_NAME_MAX )
static int const LOGIN_NAME_MAX = 16;
#endif /* #if ! defined( LOGIN_NAME_MAX ) */

#endif /* __YAAL_BUILD__ */

#ifdef __GNUC__
#	define CXX_EXTENSION_TLS __thread
#elif defined( __MSVCXX__ )
#	define CXX_EXTENSION_TLS __declspec( thread )
#endif /* #elif defined( __MSVCXX__ ) #ifdef __GNUC__ */

/*
 * gcc ver 4.5.2 becomes 4005002
 *
 * Expected use:
 *
 * #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002
 * ... some code ...
 * #endif
 *
 * For non-gcc compilers those conditions must always evaluate to false,
 * so we define them:
 * __GCC_VERSION_LOWER_OR_EQUAL__ to be very large,
 * __GCC_VERSION_GREATER_OR_EQUAL__ to be very small, i.e.:
 *
 * "very large" <= 4005002
 *
 * will evaluate to false for all practical use cases.
 */
#if defined ( __GNUC__ ) && ! defined ( __clang__ )
#	define __GCC_VERSION__ ( ( ( __GNUC__ * 1000 ) + __GNUC_MINOR__ ) * 1000 + __GNUC_PATCHLEVEL__ )
#	define __GCC_VERSION_LOWER_OR_EQUAL__ __GCC_VERSION__
#	define __GCC_VERSION_GREATER_OR_EQUAL__ __GCC_VERSION__
#else /* #if defined ( __GNUC__ ) && ! defined ( __clang__ ) */
#	define __GCC_VERSION__ -1
#	define __GCC_VERSION_LOWER_OR_EQUAL__ 999999999
#	define __GCC_VERSION_GREATER_OR_EQUAL__ 0
#endif /* #else #if defined ( __GNUC__ ) && ! defined ( __clang__ ) */

#if ! defined( HAVE_PRETTY_FUNCTION )
#	if ! defined( HAVE_FUNCTION )
#		if ! defined( HAVE_FUNC )
#			define __PRETTY_FUNCTION__ ""
#		endif
#	endif
#endif

#if ( HAVE_DECL_ERR == 1 )
#	include <cstdlib>
#	undef ERR
#endif /* HAVE_DECL_ERR */

/*! \endcond */

#endif /* #ifndef YAAL_HCORE_COMPAT_HXX_INCLUDED */

