dnl $Id$
dnl configure.ac is integral part of yaal project.

dnl Initialization data.

pushdef([AC_VERSION],esyscmd(awk '/^VERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.mk.in))
pushdef([AC_SUBVERSION],esyscmd(awk '/^SUBVERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.mk.in))
pushdef([AC_EXTRAVERSION],esyscmd(awk '/^EXTRAVERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.mk.in))
pushdef([YAAL_PACKAGE_VERSION],[AC_VERSION.AC_SUBVERSION.AC_EXTRAVERSION-esyscmd(date +%Y%m%d|tr -d '\n')])
AC_INIT([yaal],[YAAL_PACKAGE_VERSION],[[amok@codestation.org]],[],[https://codestation.org/repo/yaal.git])
m4_include([_aux/aclib.m4])
PRIVILEGES_SANITY

AC_MSG_RESULT([--- Welcome to yaal configurator ---                     [[0%]]])

AC_ARG_WITH([console-mouse],[AS_HELP_STRING([--with-console-mouse],[Compile with console mouse support, default is `yes' if appropriate library can be found.])],[CONSOLE_MOUSE=$withval],[CONSOLE_MOUSE="auto"])
dnl ` <- this one fixes syntax highlighting in vim.
AC_ARG_WITH([sqlite3],[AS_HELP_STRING([--with-sqlite3],[Compile with SQLite3 driver, default is `yes' if SQLite3 can be found.])],[SQLITE3=$withval],[SQLITE3="auto"])
dnl `
AC_ARG_WITH([postgres],[AS_HELP_STRING([--with-postgres],[Compile with PostgreSQL driver, default is `yes' if PostgreSQL can be found.])],[POSTGRES=$withval],[POSTGRES="auto"])
dnl `
AC_ARG_WITH([mysql],[AS_HELP_STRING([--with-mysql],[Compile with MySQL driver, default is `yes' if MySQL can be found.])],[MYSQL=$withval],[MYSQL="auto"])
dnl `
AC_ARG_WITH([firebird],[AS_HELP_STRING([--with-firebird],[Compile with Firebird driver, default is `yes' if Firebird can be found.])],[FIREBIRD=$withval],[FIREBIRD="auto"])
dnl `
AC_ARG_WITH([oracle],[AS_HELP_STRING([--with-oracle],[Compile with Oracle driver, default is `yes' if Oracle can be found.])],[ORACLE=$withval],[ORACLE="auto"])
dnl `
DO_TARGET="RELEASE"
AC_ARG_ENABLE([debug],[AS_HELP_STRING([--enable-debug],[Compile with debugging symbols.])],[DO_TARGET="DEBUG" LIB_INFIX="-d"])
AC_ARG_ENABLE([release],[AS_HELP_STRING([--enable-release],[Compile with full optimizations on (this is the default).])],[DO_TARGET="RELEASE" LIB_INFIX=""])
AC_ARG_ENABLE([reldeb],[AS_HELP_STRING([--enable-reldeb],[Compile with full optimizations but include debugging information.])],[DO_TARGET="RELDEB" LIB_INFIX="-rd"])
AC_ARG_ENABLE([relassert],[AS_HELP_STRING([--enable-relassert],[Compile with full optimizations but keep assertion checks.])],[DO_TARGET="RELASSERT" LIB_INFIX="-ra"])
AC_ARG_ENABLE([profiling],[AS_HELP_STRING([--enable-profiling],[Provide support for profiling tools.])],[DO_TARGET="PROFILING" LIB_INFIX="-p"])
AC_ARG_ENABLE([coverage],[AS_HELP_STRING([--enable-coverage],[Get ready to gather coverage data.])],[DO_TARGET="COVERAGE" LIB_INFIX="-c"])
AC_ARG_ENABLE([auto-sanity],[AS_HELP_STRING([--enable-auto-sanity],[Try to automatically sanitize user environment.])],[AC_DEFINE_UNQUOTED(YAAL_AUTO_SANITY,1,[Enable automatic environment sanitization.])])

AC_MSG_RESULT([--- Initialization ... ---                               [[6%]]])

dnl Unique file in project: yaalrc is sample configuration file.
AC_MSG_RESULT([--- Finding base directory ... ---                      [[12%]]])
AC_CONFIG_SRCDIR([[yaalrc.in]])
AC_CONFIG_AUX_DIR([_aux])

dnl yaal is not Free Software.
AC_COPYRIGHT([[yaal (c) 2018 All rights reserved.]])

dnl yaal is still in alpha stage.
AC_REVISION(${THIS_ID})

YAAL_CHECK_GIT

dnl We select default language.
AC_LANG([C++])

dnl Checking for machine type.
AC_MSG_RESULT([--- Checking for machine type ... ---                   [[14%]]])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

YAAL_DETECT_COMPILER
YAAL_DETECT_OPERATING_SYSTEM

AC_DEFINE_UNQUOTED(TARGET_CPU,"${target_cpu}",[Target host system CPU.])
if test ["x${target_cpu}"] = ["xx86_64"] -o ["x${target_cpu}"] = ["xamd64"] -o ["x${target_cpu}"] = ["xaarch64"] ; then
	AC_DEFINE_UNQUOTED(TARGET_CPU_BITS,64,[Target host system CPU bits.])
elif test ["${target_os}"] != ["`echo ${target_os} | sed -e 's/solaris//'`"] ; then
	AC_DEFINE_UNQUOTED(TARGET_CPU_BITS,64,[Target host system CPU bits.])
else
	AC_DEFINE_UNQUOTED(TARGET_CPU_BITS,32,[Target host system CPU bits.])
fi

AC_CHECK_PROG([READELF], [readelf], [readelf])
AC_MSG_CHECKING([host dynamic linker])
if test ["x${HOST_OS_TYPE}"] = ["xLinux"] ; then
	DYNAMIC_LINKER=`test "x${READELF}" != "x" && ${READELF} -l /usr/bin/readelf | awk 'BEGIN{FS=":"};/program interp/{gsub("[[]] ]","",$2);print $2}'`
fi
if test ["x${DYNAMIC_LINKER}"] = ["x"] ; then
	AC_MSG_WARN([not found])
else
	AC_MSG_RESULT([$DYNAMIC_LINKER])
	AC_DEFINE_UNQUOTED([__DYNAMIC_LINKER__],["$DYNAMIC_LINKER"],[Path to your dynamic linker.])
fi

AC_MSG_RESULT([--- Looking standard development software ... ---       [[33%]]])

m4_ifndef([PKG_PROG_PKG_CONFIG], [m4_fatal([Cannot self-test for pkg-config, macro PKG_PROG_PKG_CONFIG is not available.])])
PKG_PROG_PKG_CONFIG()
if test ["x${PKG_CONFIG}"] = ["x"] ; then
	AC_MSG_ERROR([Cannot continue without pkg-config.])
fi

AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_MKDIR_P
AC_CHECK_PROG([CTAGS], [exctags], [exctags], [ctags])
AC_CHECK_PROG([GTAGS], [gtags], [gtags], [true])
AC_CHECK_PROG([GDB_ADD_INDEX], [gdb-add-index], [gdb-add-index], [true])

dnl Looking for main libc headers
AC_MSG_RESULT([--- Looking for main libc headers ... ---               [[41%]]])
AC_HEADER_MAJOR

dnl What special compiler flags we can set?
AC_MSG_RESULT([--- Guessing specific compiler/linker features ... ---  [[53%]]])

YAAL_CHECK_COMPILER_VERSION

YAAL_CXX_STANDARD_CHECK

YAAL_DETECT_COMMON_FLAGS

YAAL_DETECT_FUNCTION_MACRO

CXXFLAGS=["-std=c++11 ${EXTRA_CXXFLAGS} ${EXTRA_INCLUDE_PATHS}"]

dnl Check for POD sizes.
dnl We shall not pollute namespace with unnecessary macros.
dnl AC_CHECK_SIZEOF([char])
AC_CHECK_SIZEOF([int short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([int long])
AC_CHECK_SIZEOF([int long long])
AC_CHECK_SIZEOF([float])
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([double long])
AC_CHECK_SIZEOF([void*])
AC_CHECK_SIZEOF([time_t])
dnl Check for POD alignment.
AC_CHECK_ALIGNOF([int short])
AC_CHECK_ALIGNOF([int])
AC_CHECK_ALIGNOF([int long])
AC_CHECK_ALIGNOF([int long long])
AC_CHECK_ALIGNOF([float])
AC_CHECK_ALIGNOF([double])
AC_CHECK_ALIGNOF([double long])
AC_CHECK_ALIGNOF([void*])
AC_CHECK_ALIGNOF([time_t])

dnl Now we can look for all needed libraries.
AC_MSG_RESULT([--- Looking for needed libraries ... ---                [[67%]]])
AC_SEARCH_LIBS([pthread_create],[thr pthread],,[AC_MSG_ERROR([Cannot build without threading library.])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
PKG_CHECK_MODULES(
	SSL,
	[libssl],
	[true],
	[AC_CHECK_LIB([ssl],[SSL_CTX_new],,[AC_MSG_ERROR([Cannot continue without OpenSSL library.])]) LIBS=""]
)
PKG_CHECK_MODULES(PCRE, [libpcre], [true], [AC_MSG_ERROR([Cannot continue without PCRE library.])])
AC_SEARCH_LIBS([clock_gettime],[rt]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([timer_create],[rt]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([gethostbyname_r],[nsl]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([getaddrinfo],[socket getaddrinfo],,[AC_MSG_ERROR([Cannot continue without getaddrinfo feature.])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([getnameinfo],[socket getaddrinfo],,[AC_MSG_ERROR([Cannot continue without getnameinfo feature.])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([intl_gettext],[intl],,[AC_SEARCH_LIBS([gettext],[intl],,[AC_MSG_ERROR([Cannot continue without localization library.])])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([iconv],[iconv]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([libiconv],[iconv]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_CHECK_LIB([xml2],[xmlParseFile],,[AC_MSG_ERROR([Cannot continue without XML library.])]) LIBS=""
AC_CHECK_LIB([z],[deflateEnd],,[AC_MSG_ERROR([Cannot continue without zlib library.])]) LIBS=""
PKG_CHECK_MODULES(XSLT, [libxslt],,[AC_MSG_ERROR([Cannot continue without XSLT library.])])
PKG_CHECK_MODULES(EXSLT, [libexslt],,[AC_MSG_ERROR([Cannot continue without EXSLT library.])])
AC_SEARCH_LIBS([dlopen],[dl],,[AC_MSG_ERROR([Cannot continue without dynamic dynamic linker library.])]) TOOLS_LIBS="${TOOLS_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([initscr],[ncursesw ncurses],,[AC_MSG_ERROR([Cannot continue without ncurses library.])]) HCONSOLE_LIBS="${HCONSOLE_LIBS} ${LIBS}" LIBS=""
AC_CHECK_LIB([tinfo],[curs_set]) HCONSOLE_LIBS="${HCONSOLE_LIBS} ${LIBS}" LIBS=""
AC_CHECK_LIB([gpm],[Gpm_Open]) HCONSOLE_LIBS="${HCONSOLE_LIBS} ${LIBS}" LIBS=""

if test ["${SQLITE3}"] != ["no"] ; then
	AC_CHECK_LIB([sqlite3],[sqlite3_open],
								[],[if test ["${SQLITE3}"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without SQLite3.])
										fi;SQLITE3=["no"]])
	LIBS=""
fi
if test ["${POSTGRES}"] != ["no"] ; then
	AC_CHECK_LIB([pq],[PQsetdbLogin],
								[],[if test ["${POSTGRES}"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without PostgreSQL.])
										fi;POSTGRES=["no"]])
	LIBS=""
fi
MYSQL_REENTRANT=[""]
CXXFLAGS_ORIG="${CXXFLAGS}"
CPPFLAGS_ORIG="${CPPFLAGS}"
if test ["${MYSQL}"] != ["no"] ; then
	LIBS=""
	AC_CHECK_LIB([mysqlclient],[mysql_init])
	if test ["x${LIBS}"] = "x" ; then
		AC_CHECK_LIB([mariadbclient],[mysql_init])
	fi
	if test ["x${LIBS}"] = "x" ; then
		CXXFLAGS=["${CXXFLAGS_ORIG} -L/usr/local/lib/mysql"]
		unset ac_cv_lib_mysqlclient_mysql_init
		AC_CHECK_LIB([mysqlclient],[mysql_init])
		if test ["x${LIBS}"] = "x" ; then
			unset ac_cv_lib_mariadbclient_mysql_init
			AC_CHECK_LIB([mariadbclient],[mysql_init])
		fi
		if test ["x${LIBS}"] != "x" ; then
			EXTRA_LIBRARY_PATHS="${EXTRA_LIBRARY_PATHS} -L/usr/local/lib/mysql"
		fi
	fi
	if test ["x${LIBS}"] = "x" ; then
		CXXFLAGS=["${CXXFLAGS_ORIG} -L/usr/lib64/mysql/"]
		unset ac_cv_lib_mysqlclient_mysql_init
		AC_CHECK_LIB([mysqlclient],[mysql_init])
		if test ["x${LIBS}"] = "x" ; then
			unset ac_cv_lib_mariadbclient_mysql_init
			AC_CHECK_LIB([mariadbclient],[mysql_init])
		fi
		if test ["x${LIBS}"] != "x" ; then
			EXTRA_LIBRARY_PATHS="${EXTRA_LIBRARY_PATHS} -L/usr/lib64/mysql/"
		fi
	fi
	if test ["x${LIBS}"] = "x" ; then
		if test ["${MYSQL}"] = ["yes"] ; then
			AC_MSG_ERROR([Cannot continue without MySQL.])
		else
			MySQL=["no"]
		fi
	fi
	CXXFLAGS=["${CXXFLAGS_ORIG}"]
	MYSQLCLIENT_LIBRARY=["${LIBS}"]
	LIBS=""
fi
if test ["${FIREBIRD}"] != ["no"] ; then
	AC_CHECK_LIB([fbclient],[isc_attach_database],
								[],[if test ["${FIREBIRD}"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without Firebird.])
										fi;FIREBIRD=["no"]])
	LIBS=""
fi
if test ["${ORACLE}"] != ["no"] ; then
	CXXFLAGS=["${CXXFLAGS} -L${ORACLE_HOME}/lib"]
	AC_CHECK_LIB([clntsh],[OCIEnvCreate],
								[],[if test ["${ORACLE}"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without Oracle.])
										fi;ORACLE=["no"]])
	CXXFLAGS=["${CXXFLAGS_ORIG}"]
	LIBS=""
	if test ["x${ac_cv_lib_clntsh_OCIEnvCreate}"] = ["xyes"] ; then
		EXTRA_LIBRARY_PATHS="${EXTRA_LIBRARY_PATHS} -L${ORACLE_HOME}/lib"
	fi
fi

AC_MSG_RESULT([--- All needed libraries found. ---                     [[70%]]])

AC_MSG_RESULT([--- Additional checks ... ---                           [[71%]]])
AC_SUBST([SERIAL_DEVICE],['null'])

if test ["x${build_os}"] = ["x${host_os}"] ; then
	AC_CHECK_FILE([/dev/cuaa0],[AC_SUBST([SERIAL_DEVICE],['cuaa0'])],
		[AC_CHECK_FILE([/dev/ttyS0],[AC_SUBST([SERIAL_DEVICE],['ttyS0'])])])
fi

CXXFLAGS=["${CXXFLAGS_ORIG}"]

CXXFLAGS=["${CXXFLAGS_ORIG}"]
AC_MSG_RESULT([$RESULT])
AC_MSG_CHECKING([whether `.interp' user section is allowed])
dnl `
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[void f( void ) { static char const dynamicLinkerPath[] __attribute__(( __section__(".interp") )) = "/dummy"; }]])],
	[AC_DEFINE([HAVE_INTERP_ALLOWED],[],[Is `.interp' user section allowed?])] RESULT=["yes"])
dnl `
AC_MSG_RESULT([$RESULT])

if test ["x${build_os}"] = ["x${host_os}"] ; then
	AC_MSG_CHECKING([whether strftime() returns required buffer size])
	RESULT=["no"]
	AC_RUN_IFELSE([AC_LANG_SOURCE([[#include <ctime>
	#include <unistd.h>
	#include <sys/wait.h>
	int main( int, char const** ) {
		time_t t( time( NULL ) );
		struct tm broken;
		localtime_r( &t, &broken );
		pid_t pid( fork() );
		if ( ! pid ) {
			return ( strftime( NULL, 1024, "%Y-%m-%d %T", &broken ) );
		}
		int status( 0 );
		waitpid( pid, &status, 0 );
		return ( WIFEXITED( status ) ? WEXITSTATUS( status ) - 19 : 1 );
	}]])], [AC_DEFINE([HAVE_SMART_STRFTIME],[],[Can your strftime calculate output length?])] [RESULT=["yes"]])
	AC_MSG_RESULT([$RESULT])
fi

AC_MSG_CHECKING([whether contents of input buffer for iconv() is const])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <iconv.h>
int main( int, char const** ) {
	iconv_t conv( 0 );
	size_t size( 0 );
	char const* in( NULL );
	char* out( NULL );
	iconv( conv, &in, &size, &out, &size );
	return ( 0 );
}]])], [AC_DEFINE([HAVE_ICONV_INPUT_CONST],[],[Is your iconv() input args const?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether basename is declared in cstring])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstring>
	int main( int, char** ){ char* p( NULL ); basename( p ); return ( 0 ); }]])],
	[AC_DEFINE([HAVE_BASENAME_IN_CSTRING],[1],[Does your basename() is included in cstring header?]) AC_MSG_RESULT([yes])],
	[AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether arg for basename is const])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstring>
#ifndef HAVE_BASENAME_IN_CSTRING
# include <libgen.h>
#endif /* not HAVE_BASENAME_IN_CSTRING */
int main( int, char const** ) {
	char const* p( nullptr );
	basename( p );
	return ( 0 );
}]])], [AC_DEFINE([HAVE_BASENAME_ARG_CONST],[],[Is your argument to basename a pointer to const?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether gethostbyname_r is GNU variant])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <netdb.h>
int main( int, char const** ) {
	int i( 0 );
	char* p( 0 );
	struct hostent h, * ph( 0 );
	::gethostbyname_r( "", &h, p, 0, &ph, &i );
	return ( 0 );
}]])], [AC_DEFINE([HAVE_GNU_GETHOSTBYNAME_R],[],[Is gethostbyname_r a GNU flavor?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether gethostbyaddr_r is GNU variant])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include<netdb.h>
int main( int, char const** ) {
	int i( 0 );
	char* p( 0 );
	struct hostent h, * ph( 0 );
	::gethostbyaddr_r( 0, 0, 0, &h, p, 0, &ph, &i );
	return ( 0 );
}]])], [AC_DEFINE([HAVE_GNU_GETHOSTBYADDR_R],[],[Is gethostbyaddr_r a GNU flavor?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

DEFAULT_DRIVER=["null"]

AC_MSG_RESULT([--- Looking for needed headers ... ---                  [[77%]]])

CXXFLAGS=["${CXXFLAGS_ORIG} ${SSL_CFLAGS}"]
CPPFLAGS=["${CPPFLAGS_ORIG} ${SSL_CFLAGS}"]
AC_CHECK_HEADERS([openssl/ssl.h],[],[AC_MSG_ERROR([Cannot continue without SSL development files.])])
CXXFLAGS=["${CXXFLAGS_ORIG}"]
CPPFLAGS=["${CPPFLAGS_ORIG}"]
_CONSOLE_MOUSE=[""];
CXXFLAGS=["${CXXFLAGS_ORIG} ${PCRE_CFLAGS}"]
AC_CHECK_HEADERS([pcre.h], [], [AC_MSG_ERROR([Cannot continue without PCRE library.])])
dnl '
CXXFLAGS=["${CXXFLAGS_ORIG}"]
AC_CHECK_HEADERS([sys/consio.h],_CONSOLE_MOUSE=["moused"])
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_HEADERS([tty.h])
AC_CHECK_HEADERS([netdb.h])
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([sys/un.h])
AC_CHECK_HEADERS([libgen.h])
AC_CHECK_HEADERS([utime.h])
AC_CHECK_HEADERS([sys/utime.h])
AC_CHECK_HEADERS([termio.h])
AC_CHECK_HEADERS([sys/ioctl.h])
AC_CHECK_HEADERS([gpm.h],_CONSOLE_MOUSE=["gpm"])
AC_CHECK_HEADERS([valgrind/memcheck.h])
if test ["$_CONSOLE_MOUSE"] = [""] -a ["$CONSOLE_MOUSE"] = ["yes"] ; then
	AC_MSG_ERROR([Cannot continue without console mouse support.])
fi
PKG_CHECK_MODULES(XML2,[libxml-2.0],,[AC_MSG_ERROR([Cannot continue without XML development files.])])
AC_CHECK_HEADERS([ncurses/curses.h],_NCURSES_HEADER=['ncurses/curses.h'],
	[AC_CHECK_HEADERS([curses.h],_NCURSES_HEADER=['curses.h'],
			[AC_MSG_ERROR([Cannot continue without ncurses devel!])])])

dnl Reversed order of database API check is due to setting default db driver properly.

if test ["${ORACLE}"] != ["no"] ; then
	CXXFLAGS=["${CXXFLAGS} -I${ORACLE_HOME}/rdbms/public"]
	CPPFLAGS=["${CXXFLAGS}"]
	AC_CHECK_HEADERS([oci.h],
		[AC_SUBST([DO_ORACLE_DRIVER],["oracle"])
		 AC_SUBST([ORACLE_SID],["${ORACLE_SID}"])
				EXTRA_INCLUDE_PATHS=["${EXTRA_INCLUDE_PATHS} -I\$(ORACLE_HOME)/rdbms/public"]
			DEFAULT_DRIVER=["Oracle"]],
				[if test ["${ORACLE}"] = ["yes"] ; then
					AC_MSG_ERROR([[Cannot continue without Oracle devel(client)!]])
				fi;ORACLE=["no"]])
	CXXFLAGS=["${CXXFLAGS_ORIG}"]
	CPPFLAGS=["${CPPFLAGS_ORIG}"]
fi
if test ["${FIREBIRD}"] != ["no"] ; then
	AC_CHECK_HEADERS(
		[ibase.h],
	  [AC_SUBST(
			[DO_FIREBIRD_DRIVER],
			["firebird"]
		) DEFAULT_DRIVER=["Firebird"]],
		[AC_CHECK_HEADERS(
			[firebird/ibase.h],
			[AC_SUBST(
				[DO_FIREBIRD_DRIVER],
				["firebird"]
			) DEFAULT_DRIVER=["Firebird"]],
			[if test ["${FIREBIRD}"] = ["yes"] ; then
				AC_MSG_ERROR([[Cannot continue without Firebird devel(client)!]])
			fi; FIREBIRD=["no"]]
		)]
	)
fi
if test ["${MYSQL}"] != ["no"] ; then
	AC_CHECK_HEADERS([mysql/mysql.h],
		[AC_SUBST([DO_MYSQL_DRIVER],["mysql"])
			DEFAULT_DRIVER=["MySQL"]],
				[if test ["${MYSQL}"] = ["yes"] ; then
					AC_MSG_ERROR([[Cannot continue without MySQL devel(client)!]])
				fi;MYSQL=["no"]])
fi
if test ["${POSTGRES}"] != ["no"] ; then
	AC_CHECK_HEADERS([postgresql/libpq-fe.h],
		[AC_SUBST(
			[DO_POSTGRESQL_DRIVER],
			["postgresql"]
		) DEFAULT_DRIVER=["PostgreSQL"]],
		[AC_CHECK_HEADERS(
			[libpq-fe.h],
			[AC_SUBST(
				[DO_POSTGRESQL_DRIVER],
				["postgresql"]
			)	DEFAULT_DRIVER=["PostgreSQL"]],
			[if test ["${POSTGRES}"] = ["yes"] ; then
				AC_MSG_ERROR([[Cannot continue without PostgreSQL devel(client)!]])
			fi;POSTGRES=["no"]]
		)]
	)
fi
if test ["${SQLITE3}"] != ["no"] ; then
	AC_CHECK_HEADERS([sqlite3.h],
		[AC_SUBST([DO_SQLITE3_DRIVER],["sqlite3"])
			DEFAULT_DRIVER=["SQLite3"]],
				[if test ["${SQLITE3}"] = ["yes"] ; then
					AC_MSG_ERROR([[Cannot continue without SQLite3 devel(client)!]])
				fi;SQLITE3=["no"]])
fi

AC_MSG_RESULT([--- Done looking for needed header, all found. ---      [[82%]]])

AC_MSG_RESULT([--- Looking for functions and data members ... ---      [[83%]]])
LIBS="${HCORE_LIBS}"
AC_LANG([C])
CFLAGS_ORIG=["${CFLAGS}"]
CFLAGS=["-D_GNU_SOURCE=1"]
CPPFLAGS_ORIG=["${CPPFLAGS}"]
CPPFLAGS=`echo ${CPPFLAGS} | sed -e 's/-std=c++11//g'`
AC_CHECK_TYPES([sighandler_t],[],[AC_CHECK_TYPES([__sighandler_t],[],[],[#include <signal.h>])],[#include <signal.h>])
CPPFLAGS=["${CPPFLAGS_ORIG}"]
CFLAGS=${CFLAGS_ORIG}
AC_LANG([C++])
if test ["${ac_cv_type_sighandler_t}"] = ["no"] -a ["${ac_cv_type___sighandler_t}"] = ["no"] -a ["x${HOST_OS_TYPE}"] != ["xDarwin"] ; then
	AC_CHECK_TYPES([SIG_PF],[],[AC_MSG_ERROR([Cannot continue without signal handler type definition.])],[#include <signal.h>])
fi
AC_CHECK_FUNCS([clock_gettime timer_create getline memrchr strnlen getaddrinfo getnameinfo gethostbyname_r gethostbyaddr_r pthread_setname_np pthread_set_name_np prctl])
AC_CHECK_DECLS([strtold, ERR],[],[],[#include <cstdlib>])
AC_CHECK_DECLS([deflateInit],[],[],[#include <zlib.h>])
AC_CHECK_DECLS([RLIMIT_AS, RLIMIT_NPROC],[],[],[#include <sys/resource.h>])
AC_CHECK_DECLS([VSWTC, B76800, B28800, B14400, B7200],[],[],[#include <termios.h>])
AC_CHECK_DECLS([TEMP_FAILURE_RETRY],[],[],[#include <unistd.h>])
AC_CHECK_DECLS([expl, logl, sqrtl, sinl, cosl, tanl, asinl, acosl, atanl, sincosl, fmodl, floorl, ceill, roundl],[],[],[#include <cmath>])
AC_CHECK_DECLS([has_mouse],[],[],[#include <${_NCURSES_HEADER}>])
AC_CHECK_DECLS([SIGIOT],[],[],[#include <csignal>])
AC_CHECK_DECLS([SUN_LEN],[],[],[#include <cstdlib>
								#include <sys/un.h>])
if test ["$MYSQL"] != ["no"] ; then
	AC_CHECK_DECLS([MYSQL_AUTODETECT_CHARSET_NAME],[],[],[#include <mysql/mysql.h>])
fi
AC_MSG_CHECKING([whether powl is available])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cmath>
									 int main( int, char** ) { static_cast<void>( ::powl( 0., 0. ) ); return ( 0 ); }]])],
	[AC_DEFINE([HAVE_POWL],[1],[We have ::powl().]) AC_MSG_RESULT([yes])], AC_MSG_RESULT([no]))

AC_MSG_RESULT([--- Done looking for functions and data members. ---    [[92%]]])

dnl We need to check if out ncurses devel is not brain-damaged.
AC_MSG_RESULT([--- Libraries/environment sanity check ... ---          [[93%]]])
AC_MSG_CHECKING([if your ncurses-devel is seriously brain-damaged])
CXXFLAGS=["${CXXFLAGS_ORIG}"]
CXXFLAGS=["${EXTRA_CXXFLAGS} -Wall -Werror -Wcast-align -Wconversion -Wwrite-strings -pedantic-errors -Wcast-qual"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <$_NCURSES_HEADER>
									 int main( int, char** ) { printw("x"); return ( 0 ); }]])],
	[AC_MSG_RESULT([[no :)]])],
	[AC_MSG_RESULT([[yes :(]])
		AC_MSG_ERROR([Cannot continue with seriously brain-damaged ncurses.])])
AC_MSG_CHECKING([if your ncurses is damaged about char-subscripts in ncurses (acs_map)])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <$_NCURSES_HEADER>
									 int main( int, char** ) { addch(ACS_DARROW); return ( 0 ); }]])],
	[AC_DEFINE([HAVE_ASCII_GRAPHICS],[],[Does your ncurses installation support ASCII graphic chars?]) AC_MSG_RESULT([[no :)]])],
	[AC_MSG_RESULT([[yes :(]])
		AC_MSG_WARN([You will not have pretty ascii graphics.])])
AC_MSG_CHECKING([type of attr_get second argument])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <$_NCURSES_HEADER>
									 int main( int, char** ){ attr_t* a( NULL ); int short* p( NULL ); return ( attr_get( a, p, NULL ) ); }]])],
	[AC_DEFINE_UNQUOTED([NCURSES_ATTR_GET_SECOND_ARG_TYPE],[int short],[Type of second argument of attr_get() of ncurses library.]) AC_MSG_RESULT([[int short]])],
	[AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <$_NCURSES_HEADER>
	 int main( int, char** ){ attr_t* a( NULL ); int* p( NULL ); return ( attr_get( a, p, NULL ) ); }]])],
	[AC_DEFINE_UNQUOTED([NCURSES_ATTR_GET_SECOND_ARG_TYPE],[int],[Type of second argument of attr_get() of ncurses library.]) AC_MSG_RESULT([[int]])],
	[AC_MSG_ERROR([unknown type for second argument of attr_get in ncurses library!])])])

FILES=["/etc/my.cnf /etc/mysql/my.cnf /usr/local/etc/my.cnf"]
FILES=["$FILES /usr/local/etc/mysql/my.cnf /etc/mysqld.conf"]
FILES=["$FILES /etc/mysql/mysqld.conf /usr/local/etc/mysqld.conf"]
FILES=["$FILES /usr/local/etc/mysql/mysqld.conf"]
MYSQL_SOCKET=[`grep -s "^socket" $FILES|awk '{sub(".*=[ \\t]+","");print;exit}'`]

AC_MSG_RESULT([--- Creating appropriate links.  ---                    [[97%]]])

if test ["$ORACLE"] = ["no"] -a ["$POSTGRES"] = ["no"] -a ["$MYSQL"] = ["no"] -a ["$SQLITE"] = ["no"] -a ["SQLITE3"] = ["no"] ; then
	AC_MSG_WARN([yaal without any data base driver will be unusable for most professional purpouses.])
fi

AC_SUBST([EXTRA_INCLUDE_PATHS], [${EXTRA_INCLUDE_PATHS}])
AC_SUBST([EXTRA_LIBRARY_PATHS], [${EXTRA_LIBRARY_PATHS}])
AC_SUBST([EXTRA_CXXFLAGS],[${EXTRA_CXXFLAGS}])
AC_SUBST([EXTRA_LXXFLAGS],[${EXTRA_LXXFLAGS}])
AC_SUBST([EXTRA_COMPILER_DEBUG_FLAGS],[${EXTRA_COMPILER_DEBUG_FLAGS}])
AC_SUBST([EXTRA_COMPILER_COVERAGE_FLAGS],[${EXTRA_COMPILER_COVERAGE_FLAGS}])
AC_SUBST([EXTRA_COMPILER_OPTIMIZATION_FLAGS],[${EXTRA_COMPILER_OPTIMIZATION_FLAGS}])
AC_SUBST([EXTRA_LINKER_OPTIMIZATION_FLAGS],[${EXTRA_LINKER_OPTIMIZATION_FLAGS}])
AC_SUBST([YAAL_LXXFLAGS],[${YAAL_LXXFLAGS}])
AC_SUBST([START_GROUP],[${START_GROUP}])
AC_SUBST([END_GROUP],[${END_GROUP}])
AC_SUBST([SONAME_FLAG],[${SONAME_FLAG}])
AC_SUBST([LIB_PREFIX],[${LIB_PREFIX}])
AC_SUBST([RDYNAMIC],[${RDYNAMIC}])
AC_SUBST([HCORE_LIBS],[${HCORE_LIBS}])
AC_SUBST([TOOLS_LIBS],[${TOOLS_LIBS}])
AC_SUBST([HCONSOLE_LIBS],[${HCONSOLE_LIBS}])
AC_SUBST([DO_TARGET],[${DO_TARGET}])
AC_SUBST([VERSION],[YAAL_PACKAGE_VERSION])
AC_SUBST([DEFAULT_DRIVER], [${DEFAULT_DRIVER}])
AC_SUBST([MYSQLCLIENT_LIBRARY],[${MYSQLCLIENT_LIBRARY}])
AC_SUBST([MYSQL_SOCKET],[${MYSQL_SOCKET}])
AH_TOP([/* $Id$ */

#ifndef YAAL_CONFIG_HXX_INCLUDED
#define YAAL_CONFIG_HXX_INCLUDED

/* Define core behavior here. */])
AH_BOTTOM([
#endif /* not YAAL_CONFIG_HXX_INCLUDED */
])

dnl We have to set up proper compiler/linker flags.
AC_CONFIG_HEADERS(config.hxx)
AC_CONFIG_FILES([Makefile.mk yaalrc yaal.pc])
if test -f ["${srcdir}/LICENSE.md"] ; then
	AC_CONFIG_FILES(
		[license.hxx:LICENSE.md],
		[${srcdir}/_aux/mklicense license.hxx ${srcdir}/LICENSE.md]
	)
fi
AC_OUTPUT
AC_MSG_RESULT([--- All ok, good luck! ---                              [[100%]]])
AC_MSG_RESULT([[Now you can type `make'.]])

