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
#cmakedefine __HOST_OS_TYPE_FEDORA__
#cmakedefine __HOST_OS_TYPE_PLD__
#cmakedefine __HOST_OS_TYPE_SLACKWARE__
#cmakedefine __HOST_OS_TYPE_FREEBSD__
#cmakedefine __HOST_OS_TYPE_SOLARIS__
#cmakedefine __HOST_OS_TYPE_CYGWIN__
#cmakedefine __HOST_OS_TYPE_WINDOWS__
#cmakedefine YAAL_AUTO_SANITY
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
#define SIZEOF_INO_T @SIZEOF_INO_T@
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
#cmakedefine HAVE_GETLINE
#cmakedefine HAVE_BASENAME_IN_CSTRING
#cmakedefine HAVE_BASENAME_ARG_CONST
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
#cmakedefine01 HAVE_MEMRCHR
#define LIB_PREFIX "@LIB_PREFIX@"
#define LIB_INFIX "@LIB_INFIX@"
#define LIB_EXT "@LIB_EXT@"
#define SYMBOL_PREFIX "@SYMBOL_PREFIX@"
#define EXE_SUFFIX "@EXE_SUFFIX@"
#define SYSCONFDIR "@CMAKE_INSTALL_FULL_SYSCONFDIR@"
#define LOCALSTATEDIR "@CMAKE_INSTALL_FULL_LOCALSTATEDIR@"
#define DATADIR "@CMAKE_INSTALL_FULL_DATADIR@"
#define LIBDIR "@CMAKE_INSTALL_FULL_LIBDIR@"
#define HOME_ENV_VAR "@HOME_ENV_VAR@"
#cmakedefine01 HAVE_OPENSSL_SSL_H
#cmakedefine01 HAVE_LIBXML2_LIBXML_XMLVERSION_H
#cmakedefine HAVE_EXECINFO_H
#cmakedefine HAVE_TERMIO_H
#cmakedefine HAVE_TTY_H
#cmakedefine HAVE_LIBGEN_H
#cmakedefine HAVE_NETDB_H
#cmakedefine HAVE_ARPA_INET_H
#cmakedefine HAVE_NETINET_IN_H
#cmakedefine HAVE_SYS_UN_H
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
#ifdef _CSTRING_
#include <errno.h>
#include "hcore/macro.hxx"
#define strerror msvcxx::windows_strerror
namespace msvcxx { M_YAAL_HCORE_PUBLIC_API char const* windows_strerror( int ); }
#endif /* _CSTRING_ */
#include "_aux/msvcxx/cleanup.hxx"
#endif /* __YAAL_BUILD__ */
#endif /* __MSVCXX__ */
#endif /* not YAAL_CONFIG_HXX_INCLUDED */
