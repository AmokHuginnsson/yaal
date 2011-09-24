dnl $Id$
dnl configure.ac is integral part of yaal project.

dnl Initialization data.

pushdef([AC_VERSION],esyscmd(awk '/^VERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.mk.in))
pushdef([AC_SUBVERSION],esyscmd(awk '/^SUBVERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.mk.in))
pushdef([AC_EXTRAVERSION],esyscmd(awk '/^EXTRAVERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.mk.in))
pushdef([YAAL_PACKAGE_VERSION],[AC_VERSION.AC_SUBVERSION.AC_EXTRAVERSION-esyscmd(date +%Y%m%d|tr -d '\n')])
AC_INIT([[yaal]],[YAAL_PACKAGE_VERSION],[[amok@codestation.org]])
m4_include([_aux/aclib.m4])
PRIVILEGES_SANITY

AC_MSG_RESULT([--- Welcome to yaal configurator ---                     [[0%]]])

AC_ARG_WITH([console-mouse],[AC_HELP_STRING([--with-console-mouse],[Compile with console mouse support, default is `yes' if apriopriate library can be found.])],[CONSOLE_MOUSE=$withval],[CONSOLE_MOUSE="auto"])
dnl ` <- this one fixes syntax highlighting in vim.
AC_ARG_WITH([mysql],[AC_HELP_STRING([--with-mysql],[Compile with MySQL driver, default is `yes' if MySQL can be found.])],[MYSQL=$withval],[MYSQL="auto"])
dnl `
AC_ARG_WITH([postgres],[AC_HELP_STRING([--with-postgres],[Compile with PostgreSQL driver, default is `yes' if PostgreSQL can be found.])],[POSTGRES=$withval],[POSTGRES="auto"])
dnl `
AC_ARG_WITH([sqlite3],[AC_HELP_STRING([--with-sqlite3],[Compile with SQLite3 driver, default is `yes' if SQLite3 can be found.])],[SQLITE3=$withval],[SQLITE3="auto"])
dnl `
AC_ARG_WITH([oracle],[AC_HELP_STRING([--with-oracle],[Compile with Oralce driver, default is `yes' if Oracle can be found.])],[ORACLE=$withval],[ORACLE="auto"])
dnl `
DO_TARGET="RELEASE"
AC_ARG_ENABLE([debug],[AC_HELP_STRING([--enable-debug],[Compile with debugging symbols.])],[DO_TARGET="DEBUG" LIB_INFIX="-d"])
AC_ARG_ENABLE([release],[AC_HELP_STRING([--enable-release],[Compile with full optimizations on (this is the default).])],[DO_TARGET="RELEASE" LIB_INFIX=""])
AC_ARG_ENABLE([reldeb],[AC_HELP_STRING([--enable-reldeb],[Compile with full optimizations but include debugging information.])],[DO_TARGET="RELDEB" LIB_INFIX="-rd"])
AC_ARG_ENABLE([relassert],[AC_HELP_STRING([--enable-relassert],[Compile with full optimizations but keep assertion checks.])],[DO_TARGET="RELASSERT" LIB_INFIX="-ra"])
AC_ARG_ENABLE([profiling],[AC_HELP_STRING([--enable-profiling],[Provide support for profiling tools.])],[DO_TARGET="PROFILING" LIB_INFIX="-p"])
AC_ARG_ENABLE([coverage],[AC_HELP_STRING([--enable-coverage],[Get ready to gather coverage data.])],[DO_TARGET="COVERAGE" LIB_INFIX="-c"])

AC_MSG_RESULT([--- Initialization ... ---                               [[6%]]])

dnl Unique file in project: yaalrc is sample configuration file.
AC_MSG_RESULT([--- Finding base directory ... ---                      [[12%]]])
AC_CONFIG_SRCDIR([[yaalrc.in]])
AC_CONFIG_AUX_DIR([_aux])

dnl yaal is not Free Software.
AC_COPYRIGHT([[yaal (c) 2011 All rights reserved.]])

YAAL_CHECK_GIT
dnl yaal is still in alpha stage.
AC_REVISION(${THIS_ID})

dnl We select default language.
AC_LANG([C++])
CXXFLAGS=["-L/usr/local/lib -I/usr/local/include"]

dnl Cheching for machine type.
AC_MSG_RESULT([--- Checking for machine type ... ---                   [[14%]]])
AC_MSG_CHECKING([host dynamic linker])
READELF=`whereis -b readelf | awk '{print $2}'`
DYNAMIC_LINKER=`test "x${READELF}" != "x" && ${READELF} -l /usr/bin/readelf | awk 'BEGIN{FS=":"};/program interp/{gsub("[[]] ]","",$2);print $2}'`
if test ["$DYNAMIC_LINKER"] = [""] ; then
	AC_MSG_WARN([not found])
else
	AC_MSG_RESULT([$DYNAMIC_LINKER])
	AC_DEFINE_UNQUOTED([__DYNAMIC_LINKER__],["$DYNAMIC_LINKER"],[Path to your dynamic linker.])
fi

dnl Testing presence of c compiler
if test ["$CXX"] = ["colorgcc"] -o ["$CXX"] = ["colorg++"] ; then
	CXX=["g++"]
fi
if test ["$CC"] = ["colorgcc"] ; then
	CC=["gcc"]
fi

AC_MSG_RESULT([--- Testing presence of C and C++ compiler ... ---      [[19%]]])
AC_PROG_CC
AC_PROG_CXX
AC_MSG_RESULT([--- Testing presence of C and C++ preprocesor ... ---   [[28%]]])
AC_PROG_CPP
AC_PROG_CXXCPP
AC_MSG_RESULT([--- Looking standrad development software ... ---       [[33%]]])
dnl AC_PROG_MKDIR_P dnl Centos 5.4 has obsolete version of autoconf package.
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_CHECK_PROG([CTAGS],[exctags],[exctags],[ctags])

dnl Looking for main libc headers
AC_MSG_RESULT([--- Looking for main libc headers ... ---               [[41%]]])
AC_HEADER_MAJOR

YAAL_DETECT_OPERATING_SYSTEM
YAAL_DETECT_PHYSICAL_MEMORY

dnl What special compiler flags we can set?
AC_MSG_RESULT([--- Guessing specific compiler/linker features ... ---  [[53%]]])
_FLAGS=''
CXXFLAGS_ORIG=$CXXFLAGS;
CPPFLAGS_ORIG=$CPPFLAGS;

YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-pthread], [C++])
YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-m64], [C++])
YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-fPIC], [C++])
YAAL_DETECT_FLAGS(RDYNAMIC, [-rdynamic], [C++])
YAAL_DETECT_FLAGS(FATAL_WARNINGS, [-Wl,--fatal-warnings], [C++])
EXTRA_LXXFLAGS="${EXTRA_LXXFLAGS} ${FATAL_WARNINGS}"
YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-m64], [C++])
YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--export-dynamic], [C++], [${FATAL_WARNINGS}])
YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--no-undefined], [C++])
YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--enable-auto-import], [C++])
YAAL_DETECT_FLAGS(START_GROUP, [-Wl,--start-group], [C++])
YAAL_DETECT_FLAGS(SONAME_FLAG, [-Wl,-soname,foo], [C++])
if test ["x${START_GROUP}"] != ["x"] ; then
	END_GROUP="-Wl,--end-group"
fi
if test ["x${SONAME_FLAG}"] != ["x"] ; then
	SONAME_FLAG="-Wl,-soname,\$(SONAME_\$(*))"
fi
CXXFLAGS=["-Wextra"]
AC_MSG_CHECKING([does gcc support -Wextra])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([ ])],
							[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -Wextra"]]
							[RESULT=["yes"]],
							[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -W]"])
AC_MSG_RESULT([$RESULT])
CXXFLAGS=["-Wshadow -Werror"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <pthread.h>]])],
							[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -Wshadow"]
							_FLAGS=["-Wshadow"]],
							[AC_MSG_WARN([[Cannot use -Wshadow!]])])
CXXFLAGS=$CXXFLAGS_ORIG
CXXFLAGS=["-pedantic-errors"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstdlib>]])],
							[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -pedantic-errors"]
							_FLAGS=["$_FLAGS -pedantic-errors"]],
							[AC_MSG_WARN([[Cannot use -pedantic-errors!]])])
CXXFLAGS=$CXXFLAGS_ORIG
if test ["$_FLAGS"] = [""] ; then
	_FLAGS='none. ---                      [[76%]]'
fi
AC_MSG_RESULT([--- Guessed fetures are: $_FLAGS])

YAAL_DETECT_FUNCTION_MACRO

dnl Check for POD sizes.
dnl We shall not pollute namespace with unnecessary macros.
dnl AC_CHECK_SIZEOF([char])
dnl AC_CHECK_SIZEOF([int short])
dnl AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([int long])
AC_CHECK_SIZEOF([int long long])

dnl Now we can look for all needed libraries.
AC_MSG_RESULT([--- Looking for needed libraries ... ---                [[67%]]])
CXXFLAGS=$CXXFLAGS_ORIG;
CPPFLAGS=$CXXFLAGS_ORIG;
AC_SEARCH_LIBS([pthread_create],[thr pthread],,[AC_MSG_ERROR([Cannot build without threading library.])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_CHECK_LIB([ssl],[SSL_load_error_strings],,[AC_MSG_ERROR([Cannot continue without OpenSSL library.])]) LIBS=""
AC_SEARCH_LIBS([clock_gettime],[rt],,[AC_MSG_ERROR([Cannot continue without rt library.])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([timer_create],[rt],,[AC_MSG_ERROR([Cannot continue without rt library.])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([gethostbyname_r],[nsl]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([getaddrinfo],[socket getaddrinfo],,[AC_MSG_ERROR([Cannot continue without getaddrinfo feature.])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([getnameinfo],[socket getaddrinfo],,[AC_MSG_ERROR([Cannot continue without getnameinfo feature.])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([libintl_gettext],[intl],,[AC_SEARCH_LIBS([gettext],[intl],,[AC_MSG_ERROR([Cannot continue without localization library.])])]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_SEARCH_LIBS([libiconv],[iconv]) HCORE_LIBS="${HCORE_LIBS} ${LIBS}" LIBS=""
AC_CHECK_LIB([xml2],[xmlParseFile],,[AC_MSG_ERROR([Cannot continue without XML library.])]) LIBS=""
AC_CHECK_LIB([z],[deflateEnd],,[AC_MSG_ERROR([Cannot continue without zlib library.])]) LIBS=""
AC_CHECK_LIB([xslt],[xsltParseStylesheetFile],,[AC_MSG_ERROR([Cannot continue without XSLT library.])]) LIBS=""
AC_CHECK_LIB([exslt],[exsltRegisterAll],,[AC_MSG_ERROR([Cannot continue without EXSLT library.])]) LIBS=""
AC_SEARCH_LIBS([dlopen],[dl],,[AC_MSG_ERROR([Cannot continue without dynamic dynamic linker library.])]) TOOLS_LIBS="${TOOLS_LIBS} ${LIBS}" LIBS=""
AC_CHECK_LIB([ncurses],[initscr],,[AC_MSG_ERROR([Cannot continue without ncurses library.])]) LIBS=""
AC_CHECK_LIB([gpm],[Gpm_Open]) HCONSOLE_LIBS="${HCONSOLE_LIBS} ${LIBS}" LIBS=""

if test ["$POSTGRES"] != ["no"] ; then
	AC_CHECK_LIB([pq],[PQsetdbLogin],
								[],[if test ["$POSTGRES"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without PostgreSQL.])
										fi;POSTGRES=["no"]])
	LIBS=""
fi
if test ["$SQLITE3"] != ["no"] ; then
	AC_CHECK_LIB([sqlite3],[sqlite3_open],
								[],[if test ["$SQLITE3"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without SQLite3.])
										fi;SQLITE3=["no"]])
	LIBS=""
fi
if test ["$ORACLE"] != ["no"] ; then
	CXXFLAGS=["$CXXFLAGS -L$ORACLE_HOME/lib"]
	AC_CHECK_LIB([clntsh],[OCIEnvCreate],
								[EXTRA_LIB_PATHS="$EXTRA_LIB_PATHS -L$ORACLE_HOME/lib"],
								[if test ["$ORACLE"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without Oracle.])
										fi;ORACLE=["no"]])
	CXXFLAGS=$CXXFLAGS_ORIG
	LIBS=""
fi
if test ["$MYSQL"] != ["no"] ; then
	AC_CHECK_LIB([mysqlclient],[mysql_init],[],[
			CXXFLAGS=["$CXXFLAGS -L/usr/local/lib/mysql"]
			unset ac_cv_lib_mysqlclient_mysql_init
			AC_CHECK_LIB([mysqlclient],[mysql_init],[
				EXTRA_LIB_PATHS="-L/usr/local/lib/mysql"],
				[if test ["$MYSQL"] = ["yes"] ; then
					AC_MSG_ERROR([Cannot continue without MySQL.])
				fi;MYSQL=["no"]])])
	CXXFLAGS=$CXXFLAGS_ORIG
	LIBS=""
fi

AC_MSG_RESULT([--- All needed libraries found. ---                     [[70%]]])

AC_MSG_RESULT([--- Additional checks ... ---                           [[71%]]])
AC_SUBST([SERIAL_DEVICE],['null'])
AC_CHECK_FILE([/dev/cuaa0],[AC_SUBST([SERIAL_DEVICE],['cuaa0'])],
	[AC_CHECK_FILE([/dev/ttyS0],[AC_SUBST([SERIAL_DEVICE],['ttyS0'])])])

CXXFLAGS=$CXXFLAGS_ORIG

AC_MSG_CHECKING([whether C++ compiler supports __decltype])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[int main( int, char const* const* const ) { int a = 0; typedef __decltype( a ) a_t; a_t b; return ( 0 ); }]])],
	[AC_DEFINE([HAVE_DECLTYPE],[],[Is __decltype available?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether strftime() returns required buffer size])
RESULT=["no"]
AC_RUN_IFELSE([AC_LANG_SOURCE([[#include <ctime>
int main( int, char const* const* const )
	{
	time_t t( time( NULL ) );
	struct tm broken;
	localtime_r( &t, &broken );
	int size( strftime( NULL, 1024, "%F %T", &broken ) );
	return ( ! size );
	}]])], [AC_DEFINE([HAVE_SMART_STRFTIME],[],[Can your strftime calculate output length?])] [RESULT=["yes"]])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether contents of input buffer for iconv() is const])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <iconv.h>
int main( int, char const* const* const )
	{
	iconv_t conv( 0 );
	size_t size( 0 );
	char const* in( NULL );
	char* out( NULL );
	iconv( conv, &in, &size, &out, &size );
	return ( 0 );
	}]])], [AC_DEFINE([HAVE_ICONV_INPUT_CONST],[],[Is your iconv() input args const?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether arg for basename is const])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstring>
int main( int, char const* const* const )
	{
	char const* const p( NULL );
	basename( p );
	return ( 0 );
	}]])], [AC_DEFINE([HAVE_BASENAME_ARG_CONST],[],[Is your argument to basename a pointer to const?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether gethostbyname_r is GNU variant])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <netdb.h>
int main( int, char const* const* const )
	{
	int i( 0 );
	char* p( 0 );
	struct hostent h, * ph( 0 );
	::gethostbyname_r( "", &h, p, 0, &ph, &i );
	return ( 0 );
	}]])], [AC_DEFINE([HAVE_GNU_GETHOSTBYNAME_R],[],[Is gethostbyname_r a GNU flavour?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether gethostbyaddr_r is GNU variant])
RESULT=["no"]
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include<netdb.h>
int main( int, char const* const* const )
	{
	int i( 0 );
	char* p( 0 );
	struct hostent h, * ph( 0 );
	::gethostbyaddr_r( 0, 0, 0, &h, p, 0, &ph, &i );
	return ( 0 );
	}]])], [AC_DEFINE([HAVE_GNU_GETHOSTBYADDR_R],[],[Is gethostbyaddr_r a GNU flavour?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

DEFAULT_DRIVER=["null"]

AC_MSG_RESULT([--- Looking for needed headers ... ---                  [[77%]]])
AC_CHECK_HEADERS([openssl/ssl.h],[],[AC_MSG_ERROR([Cannot continue without SSL development files.])])
_CONSOLE_MOUSE=[""];
AC_CHECK_HEADERS([sys/consio.h],_CONSOLE_MOUSE=["moused"])
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_HEADERS([gpm.h],_CONSOLE_MOUSE=["gpm"])
if test ["$_CONSOLE_MOUSE"] = [""] -a ["$CONSOLE_MOUSE"] = ["yes"] ; then
	AC_MSG_ERROR([Cannot continue without console mouse support.])
fi
CXXFLAGS=["$CXXFLAGS -I/usr/include/libxml2"]
CPPFLAGS=["$CPPFLAGS -I/usr/include/libxml2"]
AC_CHECK_HEADERS([libxml2/libxml/xmlversion.h],
	EXTRA_INCLUDE_PATHS=["$EXTRA_INCLUDE_PATHS -I/usr/include/libxml2"],
	[
		CXXFLAGS=$CXXFLAGS_ORIG
		CPPFLAGS=$CPPFLAGS_ORIG
		unset ac_cv_header_libxml2_libxml_xmlversion_h
		CXXFLAGS=["$CXXFLAGS -I/usr/local/include/libxml2"]
		CPPFLAGS=["$CPPFLAGS -I/usr/local/include/libxml2"]
		AC_CHECK_HEADERS([libxml2/libxml/xmlversion.h],
			EXTRA_INCLUDE_PATHS=["$EXTRA_INCLUDE_PATHS -I/usr/local/include/libxml2"],
			[AC_MSG_ERROR([Cannot continue without XML development files.])])
	])
CXXFLAGS=$CXXFLAGS_ORIG
CPPFLAGS=$CPPFLAGS_ORIG
AC_CHECK_HEADERS([ncurses/curses.h],_NCURSES_HEADER=['ncurses/curses.h'],
	[AC_CHECK_HEADERS([curses.h],_NCURSES_HEADER=['curses.h'],
			[AC_MSG_ERROR([Cannot continue without ncurses devel!])])])
if test ["$ORACLE"] != ["no"] ; then
	CXXFLAGS=["$CXXFLAGS -I$ORACLE_HOME/rdbms/public"]
	CPPFLAGS=["$CXXFLAGS"]
	AC_CHECK_HEADERS([oci.h],
		[AC_SUBST([DO_ORACLE_DRIVER],["oracle"])
		 AC_SUBST([ORACLE_SID],["$ORACLE_SID"])
			EXTRA_INCLUDE_PATHS=["$EXTRA_INCLUDE_PATHS -I\$(ORACLE_HOME)/rdbms/public"]
			DEFAULT_DRIVER=["Oracle"]],
				[if test ["$ORACLE"] = ["yes"] ; then
					AC_MSG_ERROR([[Cannot continue without Oracle devel(client)!]])
				fi;ORACLE=["no"]])
	CXXFLAGS=$CXXFLAGS_ORIG
	CPPFLAGS=$CPPFLAGS_ORIG
fi
if test ["$POSTGRES"] != ["no"] ; then
	AC_CHECK_HEADERS([postgresql/libpq-fe.h],
		[AC_SUBST([DO_POSTGRESQL_DRIVER],["postgresql"])
			DEFAULT_DRIVER=["PostgreSQL"]],
		[AC_CHECK_HEADERS([libpq-fe.h],
			[AC_SUBST([DO_POSTGRESQL_DRIVER],["postgresql"])
			DEFAULT_DRIVER=["PostgreSQL"]],
				[if test ["$POSTGRES"] = ["yes"] ; then
					AC_MSG_ERROR([[Cannot continue without PostgreSQL devel(client)!]])
				fi;POSTGRES=["no"]])])
fi
if test ["$MYSQL"] != ["no"] ; then
	AC_CHECK_HEADERS([mysql/mysql.h],
		[AC_SUBST([DO_MYSQL_DRIVER],["mysql"])
			DEFAULT_DRIVER=["MySQL"]],
				[if test ["$MYSQL"] = ["yes"] ; then
					AC_MSG_ERROR([[Cannot continue without MySQL devel(client)!]])
				fi;MYSQL=["no"]])
fi
if test ["$SQLITE3"] != ["no"] ; then
	AC_CHECK_HEADERS([sqlite3.h],
		[AC_SUBST([DO_SQLITE3_DRIVER],["sqlite3"])
			DEFAULT_DRIVER=["SQLite3"]],
				[if test ["$SQLITE3"] = ["yes"] ; then
					AC_MSG_ERROR([[Cannot continue without SQLite3 devel(client)!]])
				fi;SQLITE3=["no"]])
fi

AC_MSG_RESULT([--- Done looking for needed header, all found. ---      [[82%]]])

AC_MSG_RESULT([--- Looking for functions and data members ... ---      [[83%]]])
LIBS="${HCORE_LIBS}"
AC_CHECK_FUNCS([getline memrchr strnlen getaddrinfo getnameinfo gethostbyname_r gethostbyaddr_r strcasestr strtold pthread_setname_np pthread_set_name_np prctl])
AC_CHECK_DECLS([VSWTC],[],[],[#include <termios.h>])
AC_CHECK_DECLS([deflateInit],[],[],[#include <zlib.h>])
AC_MSG_CHECKING([whether basename is declared in cstring])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstring>
	int main( int, char** ){ char* p( NULL ); basename( p ); return ( 0 ); }]])],
	[AC_DEFINE([HAVE_BASENAME_IN_CSTRING],[1],[Does your basename() is included in cstring header?]) AC_MSG_RESULT([yes])],
	[AC_MSG_RESULT([no])])
AC_CHECK_DECLS([RLIMIT_AS, RLIMIT_NPROC],[],[],[#include <sys/resource.h>])
AC_CHECK_DECLS([B76800, B28800, B14400, B7200],[],[],[#include <termios.h>])
AC_CHECK_DECLS([TEMP_FAILURE_RETRY],[],[],[#include <unistd.h>])
AC_CHECK_DECLS([floorl],[],[],[#include <cmath>])
AC_CHECK_DECLS([has_mouse],[],[],[#include <${_NCURSES_HEADER}>])
AC_CHECK_DECLS([SIGIOT],[],[],[#include <csignal>])
AC_CHECK_DECLS([SUN_LEN],[],[],[#include <cstdlib>
								#include <sys/un.h>])
AC_CHECK_DECLS([ERR],[],[],[#include <cstdlib>])
AC_MSG_CHECKING([whether powl is available])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cmath>
									 int main( int, char** ) { ::powl( 0., 0. ); return ( 0 ); }]])],
	[AC_DEFINE([HAVE_POWL],[1],[We have ::powl().]) AC_MSG_RESULT([yes])], AC_MSG_RESULT([no]))

AC_MSG_RESULT([--- Done looking for functions and data members. ---    [[92%]]])

dnl We need to check if out ncurses devel is not brain-damaged.
AC_MSG_RESULT([--- Libraries/environment sanity check ... ---          [[93%]]])
AC_MSG_CHECKING([if your ncurses-devel is seriously brain-damaged])
CXXFLAGS=$CXXFLAGS_ORIG
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
									 int main( int, char** ){ attr_t* a( NULL ); int short* p( NULL ); attr_get( a, p, NULL ); return( 0 ); }]])],
	[AC_DEFINE_UNQUOTED([NCURSES_ATTR_GET_SECOND_ARG_TYPE],[int short],[Type of second argument of attr_get() of ncurses library..]) AC_MSG_RESULT([[int short]])],
	[AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <$_NCURSES_HEADER>
	 int main( int, char** ){ attr_t* a( NULL ); int* p( NULL ); attr_get( a, p, NULL ); return( 0 ); }]])],
	[AC_DEFINE_UNQUOTED([NCURSES_ATTR_GET_SECOND_ARG_TYPE],[int],[Type of second argument of attr_get() of ncurses library..]) AC_MSG_RESULT([[int]])],
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

AC_SUBST([EXTRA_LIB_PATHS], [${EXTRA_LIB_PATHS}])
AC_SUBST([EXTRA_INCLUDE_PATHS], [${EXTRA_INCLUDE_PATHS}])
AC_SUBST([DEFAULT_DRIVER], [${DEFAULT_DRIVER}])
AC_SUBST([MYSQL_SOCKET],[${MYSQL_SOCKET}])
AC_SUBST([EXTRA_CXXFLAGS],[${EXTRA_CXXFLAGS}])
AC_SUBST([EXTRA_LXXFLAGS],[${EXTRA_LXXFLAGS}])
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
AH_TOP([/* $Id$ */

#ifndef YAAL_CONFIG_HXX_INCLUDED
#define YAAL_CONFIG_HXX_INCLUDED

/* Define core behavior here. */])
AH_BOTTOM([
#endif /* not YAAL_CONFIG_HXX_INCLUDED */
])

dnl We have to set up proper compiler/linker flags.
AC_CONFIG_HEADERS(config.hxx)
AC_CONFIG_FILES([Makefile.mk yaalrc])
AC_OUTPUT
AC_MSG_RESULT([--- All ok, good luck! ---                     	       [[100%]]])
AC_MSG_RESULT([[Now you can type \`make'.]])

