#ifndef YAAL_CONFIG_HXX_INCLUDED
#define YAAL_CONFIG_HXX_INCLUDED 1
#define PACKAGE_NAME "yaal"
#define PACKAGE_VERSION @PACKAGE_VERSION@
#define PACKAGE_VCS "@PACKAGE_VCS@"
#define TARGET_CPU "@CMAKE_SYSTEM_PROCESSOR@"
#define TARGET_CPU_BITS @TARGET_CPU_BITS@
#cmakedefine __HOST_OS_TYPE_LINUX__
#cmakedefine __HOST_OS_TYPE_DEBIAN__
#cmakedefine __HOST_OS_TYPE_UBUNTU__
#cmakedefine __HOST_OS_TYPE_CENTOS__
#cmakedefine __HOST_OS_TYPE_PLD__
#cmakedefine __HOST_OS_TYPE_SLACKWARE__
#cmakedefine __HOST_OS_TYPE_FREEBSD__
#cmakedefine __HOST_OS_TYPE_SOLARIS__
#cmakedefine __HOST_OS_TYPE_CYGWIN__
#cmakedefine __HOST_OS_TYPE_WINDOWS__
#define ALIGNOF_INT_SHORT @SIZEOF_INT_SHORT@
#define ALIGNOF_INT @SIZEOF_INT@
#define ALIGNOF_INT_LONG @SIZEOF_INT_LONG@
#define ALIGNOF_INT_LONG_LONG @SIZEOF_INT_LONG_LONG@
#define ALIGNOF_FLOAT @SIZEOF_FLOAT@
#define ALIGNOF_DOUBLE @SIZEOF_DOUBLE@
#define ALIGNOF_DOUBLE_LONG @SIZEOF_DOUBLE_LONG@
#define SIZEOF_INT_SHORT @SIZEOF_INT_SHORT@
#define SIZEOF_INT @SIZEOF_INT@
#define SIZEOF_INT_LONG @SIZEOF_INT_LONG@
#define SIZEOF_INT_LONG_LONG @SIZEOF_INT_LONG_LONG@
#define SIZEOF_FLOAT @SIZEOF_FLOAT@
#define SIZEOF_DOUBLE @SIZEOF_DOUBLE@
#define SIZEOF_DOUBLE_LONG @SIZEOF_DOUBLE_LONG@
#define SIZEOF_VOIDP @SIZEOF_VOIDP@
#define SIZEOF_TIME_T @SIZEOF_TIME_T@
#cmakedefine HAVE_DECLTYPE
#cmakedefine HAVE_PRETTY_FUNCTION
#cmakedefine HAVE_FUNCTION
#cmakedefine HAVE_FUNC
#cmakedefine HAVE_SIGHANDLER_T
#cmakedefine HAVE___SIGHANDLER_T
#cmakedefine HAVE_SIG_PF
#cmakedefine01 HAVE_STRCASESTR
#cmakedefine01 HAVE_ASPRINTF
#cmakedefine01 HAVE_ASCII_GRAPHICS
#cmakedefine01 HAVE_DECL_STRTOLD
#cmakedefine HAVE_PTHREAD_SETNAME_NP
#cmakedefine HAVE_PTHREAD_SET_NAME_NP
#cmakedefine HAVE_PRCTL
#cmakedefine HAVE_CLOCK_GETTIME
#cmakedefine HAVE_TIMER_CREATE
#cmakedefine01 HAVE_POWL
#cmakedefine01 HAVE_DECL_EXPL
#cmakedefine01 HAVE_DECL_LOGL
#cmakedefine01 HAVE_DECL_SQRTL
#cmakedefine01 HAVE_DECL_SINL
#cmakedefine01 HAVE_DECL_COSL
#cmakedefine01 HAVE_DECL_TANL
#cmakedefine01 HAVE_DECL_ASINL
#cmakedefine01 HAVE_DECL_ACOSL
#cmakedefine01 HAVE_DECL_ATANL
#cmakedefine01 HAVE_DECL_SINCOSL
#cmakedefine01 HAVE_DECL_FMODL
#cmakedefine01 HAVE_DECL_TEMP_FAILURE_RETRY
#cmakedefine01 HAVE_DECL_FLOORL
#cmakedefine01 HAVE_DECL_CEILL
#cmakedefine01 HAVE_DECL_ROUNDL
#cmakedefine01 HAVE_GETHOSTBYNAME_R
#cmakedefine01 HAVE_GETADDRINFO
#cmakedefine01 HAVE_GETNAMEINFO
#cmakedefine01 HAVE_GNU_GETHOSTBYNAME_R
#cmakedefine01 HAVE_GNU_GETHOSTBYADDR_R
#cmakedefine01 HAVE_BASENAME_IN_CSTRING
#cmakedefine01 HAVE_BASENAME_ARG_CONST
#cmakedefine01 HAVE_DECL_SUN_LEN
#cmakedefine01 HAVE_DECL_RLIMIT_AS
#cmakedefine01 HAVE_DECL_RLIMIT_NPROC
#cmakedefine01 HAVE_DECL_B76800
#cmakedefine01 HAVE_DECL_B28800
#cmakedefine01 HAVE_DECL_B14400
#cmakedefine01 HAVE_DECL_B7200
#cmakedefine01 HAVE_DECL_VSWTC
#cmakedefine01 HAVE_DECL_SIGIOT
#cmakedefine01 HAVE_DECL_ERR
#cmakedefine01 HAVE_GETLINE
#cmakedefine01 HAVE_MEMRCHR
#cmakedefine01 HAVE_GETLINE
#define LIB_PREFIX "@LIB_PREFIX@"
#define LIB_INFIX "@LIB_INFIX@"
#define LIB_EXT "@LIB_EXT@"
#define SYMBOL_PREFIX "@SYMBOL_PREFIX@"
#define EXE_SUFFIX "@EXE_SUFFIX@"
#define SYSCONFDIR "@CMAKE_INSTALL_FULL_SYSCONFDIR@"
#define LOCALSTATEDIR "@CMAKE_INSTALL_FULL_LOCALSTATEDIR@"
#cmakedefine01 HAVE_OPENSSL_SSL_H
#cmakedefine01 HAVE_LIBXML2_LIBXML_XMLVERSION_H
#cmakedefine01 HAVE_EXECINFO_H
#cmakedefine01 HAVE_PCREPOSIX_H
#cmakedefine HAVE_TERMIO_H
#cmakedefine HAVE_TTY_H
#cmakedefine HAVE_SQLITE3_H
#cmakedefine HAVE_POSTGRESQL_LIBPQ_FE_H
#cmakedefine HAVE_LIBPQ_FE_H
#cmakedefine HAVE_MYSQL_MYSQL_H
#cmakedefine HAVE_IBASE_H
#cmakedefine HAVE_OCI_H
#cmakedefine HAVE_SYS_IOCTL_H
#cmakedefine HAVE_SYS_CONSIO_H
#cmakedefine HAVE_GPM_H
#cmakedefine01 HAVE_NCURSES_CURSES_H
#cmakedefine01 HAVE_CURSES_H
#cmakedefine01 HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME
#define NCURSES_ATTR_GET_SECOND_ARG_TYPE @NCURSES_ATTR_GET_SECOND_ARG_TYPE@
#ifndef __GNUC__
#define __attribute__( x ) /**/
#endif /* not __GNUC__ */
#ifdef __MSVCXX__
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1
#define NOMINMAX 1
#undef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#define HAVE_PRETTY_FUNCTION 1
#define strcasecmp stricmp
#ifdef __YAAL_BUILD__
#undef HAVE_DECL_TEMP_FAILURE_RETRY
#define HAVE_ICONV_INPUT_CONST 1
#undef TEMP_FAILURE_RETRY
#undef HAVE_GETHOSTBYNAME_R
#define HAVE_GETHOSTBYNAME_R 1
#define __va_copy( x, y ) ( ( x ) = ( y ) )
extern "C" int setenv( char const*, char  const*, int );
extern "C" int unsetenv( char const* );
#ifdef PTHREAD_H
#define pthread_self() pthread_self().p
#endif /* PTHREAD_H */
#if 0 // def _CTIME_
#include <pthread.h>
extern "C" int getntptimeofday( struct timespec*, struct timezone* );
typedef __clockid_t clockid_t;
inline int clock_gettime( __clockid_t, struct timespec* tp_ )
	{ return ( getntptimeofday( tp_, NULL ) ); }
static clockid_t const CLOCK_REALTIME = 0;
#endif /* #ifdef _CTIME_ */
#ifdef _CSTRING_
#include <errno.h>
#include "hcore/macro.hxx"
#undef OVERFLOW
extern "C" void* memrchr( void const*, int, size_t );
extern "C" char* basename( char const* );
extern "C" char* strptime( char const*, char const*, struct tm* );
extern "C" char* strsignal( int );
#define strerror msvcxx::windows_strerror
namespace msvcxx { M_YAAL_HCORE_PUBLIC_API char const* windows_strerror( int ); }
#endif /* _CSTRING_ */
#ifdef _CSTDIO_
extern "C" int long getline( char**, size_t*, FILE* );
#endif /* _CSTDIO_ */
#include "_aux/msvcxx/cleanup.hxx"
#endif /* __YAAL_BUILD__ */
#endif /* __MSVCXX__ */
#endif /* not YAAL_CONFIG_HXX_INCLUDED */
