#ifndef YAAL_CONFIG_HXX_INCLUDED
#define YAAL_CONFIG_HXX_INCLUDED 1
#define PACKAGE_NAME "yaal"
#define PACKAGE_VERSION "0.0.0"
#cmakedefine01 HAVE_STRCASESTR
#cmakedefine01 HAVE_ASPRINTF
#cmakedefine01 HAVE_STRTOLD
#cmakedefine01 HAVE_POWL
#cmakedefine01 HAVE_DECL_TEMP_FAILURE_RETRY
#cmakedefine01 HAVE_DECL_FLOORL
#cmakedefine01 HAVE_GETHOSTBYNAME_R
#cmakedefine01 HAVE_DECL_GETADDRINFO
#cmakedefine01 HAVE_GNU_GETHOSTBYNAME_R
#cmakedefine01 HAVE_DECL_SUN_LEN
#cmakedefine01 HAVE_MEMRCHR
#define LIB_PREFIX "@LIB_PREFIX@"
#define LIB_INFIX "@LIB_INFIX@"
#define LIB_EXT "@LIB_EXT@"
#define SYMBOL_PREFIX @SYMBOL_PREFIX@
#define NCURSES_ATTR_GET_SECOND_ARG_TYPE @NCURSES_ATTR_GET_SECOND_ARG_TYPE@
#cmakedefine01 HAVE_SQLITE3_H
#cmakedefine01 HAVE_POSTGRESQL_LIBPQ_FE_H
#cmakedefine01 HAVE_LIBPQ_FE_H
#cmakedefine01 HAVE_MYSQL_MYSQL_H
#cmakedefine01 HAVE_OCI_H
#cmakedefine01 HAVE_OPENSSL_SSL_H
#cmakedefine01 HAVE_LIBXML2_LIBXML_XMLVERSION_H
#cmakedefine01 HAVE_EXECINFO_H
#cmakedefine HAVE_SYS_CONSIO_H
#cmakedefine01 HAVE_GPM_H
#cmakedefine01 HAVE_NCURSES_CURSES_H
#cmakedefine01 HAVE_CURSES_H
#ifndef __GNUC__
#define __attribute__( x ) /**/
#endif /* not __GNUC__ */
#ifdef __MSVCXX__
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1
#pragma warning( disable : 4068 )
#pragma warning( disable : 4180 )
#pragma warning( disable : 4250 )
#pragma warning( disable : 4351 )
#pragma warning( disable : 4355 )
#pragma warning( disable : 4646 )
#pragma warning( disable : 4996 )
#undef HAVE_DECL_TEMP_FAILURE_RETRY
#undef TEMP_FAILURE_RETRY
#define snprintf _snprintf
#define strcasecmp stricmp
#define __va_copy( x, y ) ( ( x ) = ( y ) )
#ifdef PTHREAD_H
#define pthread_self() pthread_self().p
#endif /* PTHREAD_H */
#ifdef _CTIME_
#include <pthread.h>
#include <bits/types.h>
extern int clock_gettime( __clockid_t, struct timespec* );
static int const CLOCK_REALTIME = 0;
#endif /* _CTIME */
#ifdef _CSTRING_
#include <pthread.h>
/* Those functions are available from libgw32c library, but declarations
 * are placed in glibc/string.h which horribly conflicts with Visual C++. */
extern void* memrchr( void const*, int, size_t );
extern char *basename ( char const* );
extern char* strptime( char const*, char const*, struct tm* );
#endif /* _CSTRING_ */
#include "cleanup.hxx"
#endif /* __MSVCXX__ */
#endif /* not YAAL_CONFIG_HXX_INCLUDED */
