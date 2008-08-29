dnl $Id$
dnl configure.ac is integral part of yaal project.

dnl Initialization data.

pushdef([AC_VERSION],esyscmd(awk '/^VERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.in))
pushdef([AC_SUBVERSION],esyscmd(awk '/^SUBVERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.in))
pushdef([AC_EXTRAVERSION],esyscmd(awk '/^EXTRAVERSION[[[:space:]]]+=[[[:space:]]]+/{printf $3}' Makefile.in))
AC_INIT([[yaal]],[AC_VERSION.AC_SUBVERSION.AC_EXTRAVERSION],[[amok@codestation.org]])

AC_MSG_RESULT([--- Welcome to yaal configurator ---                     [[0%]]])

AC_ARG_WITH([console-mouse],[AC_HELP_STRING([--with-console-mouse],[Compile with console mouse support, default is `yes' if apriopriate library can be found.])],[CONSOLE_MOUSE=$withval],[CONSOLE_MOUSE="auto"])
dnl ` <- this one fixes syntax highlighting in vim.
AC_ARG_WITH([mysql],[AC_HELP_STRING([--with-mysql],[Compile with MySQL driver, default is `yes' if MySQL can be found.])],[MYSQL=$withval],[MYSQL="auto"])
dnl `
AC_ARG_WITH([postgres],[AC_HELP_STRING([--with-postgres],[Compile with PostgreSQL driver, default is `yes' if PostgreSQL can be found.])],[POSTGRES=$withval],[POSTGRES="auto"])
dnl `
AC_ARG_WITH([sqlite],[AC_HELP_STRING([--with-sqlite],[Compile with SQLite driver, default is `yes' if SQLite can be found.])],[SQLITE=$withval],[SQLITE="auto"])
dnl `
AC_ARG_WITH([sqlite3],[AC_HELP_STRING([--with-sqlite3],[Compile with SQLite3 driver, default is `yes' if SQLite3 can be found.])],[SQLITE3=$withval],[SQLITE3="auto"])
dnl `
AC_ARG_WITH([oracle],[AC_HELP_STRING([--with-oracle],[Compile with Oralce driver, default is `yes' if Oracle can be found.])],[ORACLE=$withval],[ORACLE="auto"])
dnl `

AC_MSG_RESULT([--- Initialization ... ---                               [[6%]]])

dnl Unique file in project: yaalrc is sample configuration file.
AC_MSG_RESULT([--- Finding base directory ... ---                      [[12%]]])
AC_CONFIG_SRCDIR([[yaalrc.in]])
AC_CONFIG_AUX_DIR([_aux])

dnl yaal is not Free Software.
AC_COPYRIGHT([[yaal (c) 2004 All rights reserved.]])

AC_CHECK_PROG(HAS_GIT,[git-log],["true"],["false"])
AC_MSG_CHECKING([git id sub-command])
AC_SUBST(GITID,[true])
if test ["$HAS_GIT"] = ["true"] ; then
	THIS_ID=`git id configure.ac 2> /dev/null`
	if test ["x${THIS_ID}"] != ["x"] ; then
		AC_SUBST(GITID,["git id"])
		AC_MSG_RESULT([ok])
	else
		AC_MSG_WARN([not present])
	fi
else
	AC_MSG_WARN([no git in the first place])
fi

dnl yaal is still in alpha stage.
AC_REVISION(${THIS_ID})

dnl We select default language.
AC_LANG([C++])
CXXFLAGS=["-L/usr/local/lib -I/usr/local/include"]

dnl Set up how configure will handle header checks.
AC_CONFIG_HEADERS([config.h])

dnl Cheching for machine type.
AC_MSG_RESULT([--- Checking for machine type ... ---                   [[14%]]])
AC_CANONICAL_HOST()
AC_MSG_CHECKING([host dynamic linker])
DYNAMIC_LINKER=`readelf -l /usr/bin/readelf | awk 'BEGIN{FS=":"};/program interp/{gsub("[[]] ]","",$2);print $2}'`
if test ["$DYNAMIC_LINKER"] = [""] ; then
	AC_MSG_ERROR([not found])
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
AC_MSG_RESULT([--- Testing presence of c compiler ... ---              [[19%]]])
AC_PROG_CC
dnl and for c++ compiler
AC_MSG_RESULT([--- Testing presence of c++ compiler ... ---            [[25%]]])
AC_PROG_CXX
dnl and for c preprocesor
AC_MSG_RESULT([--- Looking standrad development software ... ---       [[33%]]])
AC_PROG_CPP
AC_PROG_CXXCPP
AC_CHECK_PROG([CTAGS],[exctags],[exctags],[ctags])

dnl Looking for main libc headers
AC_MSG_RESULT([--- Looking for main libc headers ... ---               [[41%]]])
AC_HEADER_MAJOR

dnl We need to know what operating system yaal will be compiled on.
AC_MSG_RESULT([--- Guessing host operating system ... ---              [[58%]]])
AC_CHECK_FILE(
	[[/etc/rc]],
	[AC_DEFINE([__HOST_OS_TYPE_FREEBSD__],[],[Your operating system is FreeBSD.]) _HOST________=['FreeBSD. ---  ']],
	[AC_CHECK_FILE(
		[[/etc/debconf.conf]],
		[AC_DEFINE([__HOST_OS_TYPE_DEBIAN__],[],[Your operating system is Debian.]) _HOST________=['Debian. ---   ']],
		[AC_CHECK_FILE(
			[[/etc/poldek/poldek.conf]],
			[AC_DEFINE([__HOST_OS_TYPE_PLD__],[],[Your operating system is PLD.]) _HOST________=['PLD. ---      ']],
			[AC_CHECK_FILE(
				[[/etc/random-seed]],
				[AC_DEFINE([__HOST_OS_TYPE_SLACKWARE__],[],[Your operating system is Slackware.]) _HOST________=['Slackware. ---']],
				[AC_MSG_ERROR([Cannot recognize host operating system type!])]
			)]
		)]
	) AC_SUBST([SERIAL_DEVICE],['ttyS0'])]
)

AC_MSG_RESULT([--- Guessed host operating system is $_HOST________     [[62%]]])

dnl What special compiler flags we can set?
AC_MSG_RESULT([--- Guessing specific compiler/linker features ... ---  [[63%]]])
_FLAGS=''
CXXFLAGS_ORIG=$CXXFLAGS;
CPPFLAGS_ORIG=$CPPFLAGS;

CXXFLAGS=["-Wextra"]
AC_MSG_CHECKING([does gcc support -Wextra])
RESULT=["no"]
AC_TRY_COMPILE([],[],[AC_SUBST([EXTRA_WARNINGS],[-Wextra])] [RESULT=["yes"]],
							[AC_SUBST([EXTRA_WARNINGS],[-W])])
AC_MSG_RESULT([$RESULT])
CXXFLAGS=["-Wshadow -Werror"]
AC_TRY_COMPILE([#include <pthread.h>],[],
							[AC_SUBST([WARN_SHADOW],[-Wshadow])
							_FLAGS=["-Wshadow"]],
							[AC_MSG_WARN([[Cannot use -Wshadow!]])])
CXXFLAGS=$CXXFLAGS_ORIG
CXXFLAGS=["-pedantic-errors"]
AC_TRY_COMPILE([#include <stdlib.h>],[],
							[AC_SUBST([PEDANTIC_ERRORS],	[-pedantic-errors])
							_FLAGS=["$_FLAGS -pedantic-errors"]],
							[AC_MSG_WARN([[Cannot use -pedantic-errors!]])])
CXXFLAGS=$CXXFLAGS_ORIG
CXXFLAGS=["$CXXFLAGS -fPIC -shared -Wl,--no-undefined -Wl,--fatal-warnings -Wl,-soname,conftest"]
AC_LINK_IFELSE([[extern int my_undefined_symbol;void foo(void){my_undefined_symbol=0;}]],
							[AC_SUBST([SPECIAL_LINKER_FLAGS],
								[[-Wl,--no-undefined]]) _FLAGS=["$_FLAGS -Wl,--no-undefined."]],
							[AC_MSG_WARN([[Cannot use -Wl,--no-undefined!]])])
if test ["$_FLAGS"] = [""] ; then
	_FLAGS='unfortunetely none. ---         [[68%]]'
fi
AC_MSG_RESULT([--- Guessed fetures are $_FLAGS])

AC_MSG_RESULT([--- Additional checks ... ---                           [[67%]]])
AC_SUBST([SERIAL_DEVICE],['null'])
AC_CHECK_FILE([/dev/cuaa0],[AC_SUBST([SERIAL_DEVICE],['cuaa0'])],
	[AC_CHECK_FILE([/dev/ttyS0],[AC_SUBST([SERIAL_DEVICE],['ttyS0'])])])

dnl Test if dynamic loader is in libc or in separate library dl.
AC_MSG_CHECKING([whether dl library is inside libc])
RESULT=["no"]
CXXFLAGS=$CXXFLAGS_ORIG
CXXFLAGS=["-ldl"]
AC_LINK_IFELSE([[#include <dlfcn.h>
int main()
	{
	dlopen("",0);
	return(0);
	}
]],[AC_SUBST([DLOPEN_IN],[-ldl])],[RESULT=["yes"]])
AC_MSG_RESULT([$RESULT])

dnl Test if gettext is in libc or in separate library intl.
AC_MSG_CHECKING([whether intl(gettext) library is inside libc])
RESULT=["no"]
CXXFLAGS=$CXXFLAGS_ORIG
CXXFLAGS=["-lintl"]
AC_LINK_IFELSE([[#include <libintl.h>
int main()
	{
	gettext("");
	return(0);
	}
]],[AC_SUBST([GETTEXT_LIB],[-lintl])],[RESULT=["yes"]])
AC_MSG_RESULT([$RESULT])

CXXFLAGS=$CXXFLAGS_ORIG
AC_MSG_CHECKING([whether strftime() returns required buffer size])
RESULT=["no"]
AC_TRY_RUN([#include <time.h>
#define NULL	0
int main ( int, char const * const * const )
	{
	int size = 0;
	time_t t = time ( NULL );
	struct tm broken;
	localtime_r ( & t, & broken );
	size = strftime ( NULL, 1024, "%F %T", & broken );
	return ( ! size );
	}], [AC_DEFINE([HAVE_SMART_STRFTIME],[],[Can your strftime calculate output length?])] [RESULT=["yes"]])
AC_MSG_RESULT([$RESULT])

AC_MSG_CHECKING([whether contents of input buffer for iconv() is const])
RESULT=["no"]
AC_COMPILE_IFELSE([#include<iconv.h>
#define NULL	0
int main ( int, char const * const * const )
	{
	iconv_t conv = 0;
	size_t size = 0;
	char const * in = NULL;
	char * out = NULL;
	iconv ( conv, & in, & size, & out, & size );
	return ( 0 );
	}], [AC_DEFINE([HAVE_ICONV_INPUT_CONST],[],[Is your iconv() input args const?])] RESULT=["yes"])
AC_MSG_RESULT([$RESULT])

DEFAULT_DRIVER=["null"]

dnl Now we can look for all needed libraries.
AC_MSG_RESULT([--- Looking for needed libraries ... ---                [[70%]]])
CXXFLAGS=$CXXFLAGS_ORIG;
CPPFLAGS=$CXXFLAGS_ORIG;
AC_CHECK_LIB([pthread],[pthread_create],
						 [],[AC_MSG_ERROR([Cannot build without threading library.])])
LIBS=""
AC_CHECK_LIB([rt],[clock_gettime],
						 [],[AC_MSG_ERROR([Cannot continue without rt library.])])
LIBS=""
AC_CHECK_LIB([ssl],[SSL_load_error_strings],
						 [],[AC_MSG_ERROR([Cannot continue without OpenSSL library.])])
LIBS=""
AC_CHECK_LIB([xml2],[xmlParseFile],
						 [],[AC_MSG_ERROR([Cannot continue without XML library.])])
LIBS=""
AC_CHECK_LIB([xslt],[xsltParseStylesheetFile],
						 [],[AC_MSG_ERROR([Cannot continue without XSLT library.])])
LIBS=""
AC_CHECK_LIB([ncurses],[initscr],
						 [],[AC_MSG_ERROR([Cannot continue without ncurses.])])
LIBS=""
AC_CHECK_LIB([gpm],[Gpm_Open],[AC_SUBST([MOUSE_LIB],[-lgpm])])
LIBS=""
if test ["$POSTGRES"] != ["no"] ; then
	AC_CHECK_LIB([pq],[PQsetdbLogin],
								[],[if test ["$POSTGRES"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without PostgreSQL.])
										fi;POSTGRES=["no"]])
	LIBS=""
fi
if test ["$SQLITE"] != ["no"] ; then
	AC_CHECK_LIB([sqlite],[sqlite_open],
								[],[if test ["$SQLITE"] = ["yes"] ; then
											AC_MSG_ERROR([Cannot continue without SQLite.])
										fi;SQLITE=["no"]])
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

AC_MSG_RESULT([--- All needed libraries found. ---                     [[76%]]])

AC_MSG_RESULT([--- Looking for needed headers ... ---                  [[77%]]])
AC_CHECK_HEADERS([openssl/ssl.h],[],[AC_MSG_ERROR([Cannot continue without SSL development files.])])
_CONSOLE_MOUSE=[""];
AC_CHECK_HEADERS([sys/consio.h],_CONSOLE_MOUSE=["moused"])
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
if test ["$SQLITE"] != ["no"] ; then
	AC_CHECK_HEADERS([sqlite.h],
		[AC_SUBST([DO_SQLITE_DRIVER],["sqlite"])
			DEFAULT_DRIVER=["SQLite"]],
				[if test ["$SQLITE"] = ["yes"] ; then
					AC_MSG_ERROR([[Cannot continue without SQLite devel(client)!]])
				fi;SQLITE=["no"]])
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

AC_CHECK_FUNCS([getline gethostbyname_r gethostbyaddr_r],[],[])
AC_CHECK_DECLS([VSWTC],[],[],[#include <termios.h>])
AC_CHECK_DECL([basename],[],[AC_DEFINE([HAVE_BASE_NAME_IN_LIBGEN],[],[Does your basename() is included in libgen library?])],[#include <string.h>])
AC_CHECK_DECLS([B76800, B28800, B14400, B7200],[],[],[#include <termios.h>])

AC_MSG_RESULT([--- Done looking for functions and data members. ---    [[92%]]])

dnl We need to check if out ncurses devel is not brain-damaged.
AC_MSG_RESULT([--- Libraries/environment sanity check ... ---          [[93%]]])
AC_MSG_CHECKING([if your ncurses-devel is seriously brain-damaged])
CXXFLAGS=$CXXFLAGS_ORIG
CXXFLAGS=["$EXTRA_WARNINGS -Wall -Werror -Wcast-align -Wconversion -Wwrite-strings -pedantic-errors -Wcast-qual"]
AC_TRY_COMPILE([#include <$_NCURSES_HEADER>],
[printw("x");],[AC_MSG_RESULT([[no :)]])],
	[AC_MSG_RESULT([[yes :(]])
		AC_MSG_ERROR([Cannot continue with seriously brain-damaged ncurses.])])
AC_MSG_CHECKING([if your ncurses is damaged about char-subscripts in ncurses (acs_map)])
AC_TRY_COMPILE([#include <$_NCURSES_HEADER>],
	[addch(ACS_DARROW);],
	[AC_DEFINE([HAVE_ASCII_GRAPHICS],[],[Does your ncurses installation support ASCII graphic chars?]) AC_MSG_RESULT([[no :)]])],
	[AC_MSG_RESULT([[yes :(]])
		AC_MSG_WARN([You will not have pretty ascii graphics.])])

FILES=["/etc/my.cnf /etc/mysql/my.cnf /usr/local/etc/my.cnf"]
FILES=["$FILES /usr/local/etc/mysql/my.cnf /etc/mysqld.conf"]
FILES=["$FILES /etc/mysql/mysqld.conf /usr/local/etc/mysqld.conf"]
FILES=["$FILES /usr/local/etc/mysql/mysqld.conf"]
MYSQL_SOCKET=[`grep -s "^socket" $FILES|awk '{sub(".*=[ \\t]+","");print;exit}'`]

AC_MSG_RESULT([--- Creating appropriate links.  ---                    [[97%]]])
AC_CONFIG_COMMANDS_PRE([cd doc/html;
												ln -sf yaal_toc.html index.html;cd ../..])

if test ["$ORACLE"] = ["no"] -a ["$POSTGRES"] = ["no"] -a ["$MYSQL"] = ["no"] -a ["$SQLITE"] = ["no"] -a ["SQLITE3"] = ["no"] ; then
	AC_MSG_WARN([yaal without any data base driver will be
                    unusable for most professional purpouses.])
fi

AC_SUBST([EXTRA_LIB_PATHS], ["$EXTRA_LIB_PATHS"])
AC_SUBST([EXTRA_INCLUDE_PATHS], ["$EXTRA_INCLUDE_PATHS"])
AC_SUBST([DEFAULT_DRIVER], ["$DEFAULT_DRIVER"])
AC_SUBST([MYSQL_SOCKET],["$MYSQL_SOCKET"])
AH_TOP([/* $Id$ */

#ifndef __YAAL_CONFIG_H
#define __YAAL_CONFIG_H

/* Define core behavior here. */])
AH_BOTTOM([
#endif /* not __YAAL_CONFIG_H */
])

dnl We have to set up proper compiler/linker flags.
AC_OUTPUT([Makefile yaalrc])
AC_MSG_RESULT([--- All ok, good luck! ---                     	       [[100%]]])
AC_MSG_RESULT([[Now you can type \`make'.]])

