dnl $CVSHeader$
dnl configure.ac is integral part of stdhapi project.

dnl Initialization data.
AC_INIT([[stdhapi]],[[0.0.0]],[[amok@frihet.ath.cx]])

AC_MSG_RESULT([--- Welcome to stdhapi configurator ---                  [[0%]]])

AC_ARG_WITH([console-mouse],[AC_HELP_STRING([--with-console-mouse],[Compile with console mouse support, dafault is `yes' if apriopriate library can be found.])],[CONSOLE_MOUSE=$withval],[CONSOLE_MOUSE="auto"])
dnl ` <- this one fixes syntax highlighting in vim.
AC_ARG_WITH([mysql],[AC_HELP_STRING([--with-mysql],[Compile with MySQL driver, dafault is `yes' if MySQL can be found.])],[MYSQL=$withval],[MYSQL="auto"])
dnl `
AC_ARG_WITH([postgres],[AC_HELP_STRING([--with-postgres],[Compile with PostgreSQL driver, dafault is `yes' if PostgreSQL can be found.])],[POSTGRES=$withval],[POSTGRES="auto"])
dnl `
AC_ARG_WITH([sqlite],[AC_HELP_STRING([--with-sqlite],[Compile with SQLite driver, dafault is `yes' if SQLite can be found.])],[SQLITE=$withval],[SQLITE="auto"])
dnl `

AC_MSG_RESULT([--- Initialization ... ---                               [[6%]]])

dnl Unique file in project: stdhapirc is sample configuration file.
AC_MSG_RESULT([--- Finding base directory ... ---                      [[12%]]])
AC_CONFIG_SRCDIR([[stdhapirc.in]])
AC_CONFIG_AUX_DIR([aux])

dnl stdhapi is not Free Software.
AC_COPYRIGHT([[stdhapi (c) 2004 All rights reserved.]])

dnl stdhapi is still in alpha stage.
AC_REVISION([[$ Revision: 0.0.0 $]])

dnl We select default language.
AC_LANG([C++])
CXXFLAGS=["-L/usr/local/lib -I/usr/local/include"]

dnl Set up how configure will handle header checks.
AC_CONFIG_HEADERS([config.h])

dnl We will generate config.h with basic #defines.
dnl AC_CONFIG_HEADER([config.h])

dnl Cheching for machine type.
AC_MSG_RESULT([--- Checking for machine type ... ---                   [[14%]]])
AC_CANONICAL_HOST()

dnl Testing presence of c compiler
AC_MSG_RESULT([--- Testing presence of c compiler ... ---              [[19%]]])
AC_PROG_CC
dnl and for c++ compiler
AC_MSG_RESULT([--- Testing presence of c++ compiler ... ---            [[25%]]])
AC_PROG_CXX
dnl and for c preprocesor
AC_MSG_RESULT([--- Looking for c/c++ preprocessor and egrep ... ---    [[33%]]])
AC_PROG_CPP
AC_PROG_CXXCPP

dnl Looking for main libc headers
AC_MSG_RESULT([--- Looking for main libc headers ... ---               [[41%]]])
AC_HEADER_MAJOR

dnl We need to know what operating system stdhapi will be compiled on.
AC_MSG_RESULT([--- Guessing host operating system ... ---              [[58%]]])
AC_CHECK_FILE(
	[[/etc/rc]],
	[AC_DEFINE([[__HOST_OS_TYPE_FREEBSD__]]) _HOST________=['FreeBSD. ---  ']],
	[AC_CHECK_FILE(
		[[/etc/debconf.conf]],
		[AC_DEFINE([[__HOST_OS_TYPE_DEBIAN__]]) _HOST________=['Debian. ---   ']],
		[AC_CHECK_FILE(
			[[/etc/poldek.conf]],
			[AC_DEFINE([[__HOST_OS_TYPE_PLD__]]) _HOST________=['PLD. ---      ']],
			[AC_CHECK_FILE(
				[[/etc/random-seed]],
				[AC_DEFINE([[__HOST_OS_TYPE_SLACKWARE__]]) _HOST________=['Slackware. ---']],
				[AC_MSG_ERROR([Can not recognize host operating system type!])]
			)]
		)]
	) AC_SUBST([SERIAL_DEVICE],['ttyS0'])]
)
AC_MSG_RESULT([--- Guessed host operating system is $_HOST________     [[62%]]])

dnl What special compiler flags we can set?
AC_MSG_RESULT([--- Guessing specific compiler/linker features ... ---  [[63%]]])
_FLAGS=''
CXXFLAGS_ORIG=$CXXFLAGS;
CXXFLAGS=["-Wextra"]
AC_MSG_CHECKING([does gcc support -Wextra])
RESULT=["no"]
AC_TRY_COMPILE([],[],[AC_SUBST([EXTRA_WARNINGS],[-Wextra])] [RESULT=["yes"]],
							[AC_SUBST([EXTRA_WARNINGS],[-W])])
AC_MSG_RESULT([$RESULT])
CXXFLAGS=["-Wshadow -Werror"]
AC_TRY_COMPILE([#include <pthread.h>],[],
							[AC_SUBST([SPECIAL_COMPILER_FLAGS],[-Wshadow])
							_FLAGS=["-Wshadow"]],
							[AC_MSG_WARN([[Can not use -Wshadow!]])])
CXXFLAGS=["-pedantic-errors"]
AC_TRY_COMPILE([#include <stdlib.h>],[],
							[AC_SUBST([SPECIAL_HEAVY_COMPILER_FLAGS],	[-pedantic-errors])
							_FLAGS=["$_FLAGS -pedantic-errors"]],
							[AC_MSG_WARN([[Can not use -pedantic-errors!]])])
CXXFLAGS=["$CXXFLAGS -fPIC -shared -Wl,--no-undefined -Wl,--fatal-warnings -Wl,-soname,conftest"]
AC_LINK_IFELSE([[extern int errno;void foo(void){errno=0;}]],
							[AC_SUBST([SPECIAL_LINKER_FLAGS],
								[[-Wl,--no-undefined]]) _FLAGS=["$_FLAGS -Wl,--no-undefined."]],
							[AC_MSG_WARN([[Can not use -Wl,--no-undefined!]])])
if test ["$_FLAGS"] = [""] ; then
	_FLAGS='unfortunetely none. ---         [[68%]]'
fi
AC_MSG_RESULT([--- Guessed fetures are $_FLAGS])

AC_MSG_RESULT([--- Additional checks ... ---                           [[67%]]])
AC_SUBST([SERIAL_DEVICE],['null'])
AC_CHECK_FILE([[/dev/cuaa0]],[AC_SUBST([SERIAL_DEVICE],['cuaa0'])],
	[AC_CHECK_FILE([/dev/ttyS0],[AC_SUBST([SERIAL_DEVICE],['ttyS0'])])])

dnl Test if dynamic loader is in libc or in separate library dl.
AC_MSG_CHECKING([whether dl library is inside libc])
RESULT=["no"]
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
int main ( void )
	{
	int size = 0;
	time_t t = time ( NULL );
	struct tm broken;
	localtime_r ( & t, & broken );
	size = strftime ( NULL, 1024, "%F %T", & broken );
	return ( ! size );
	}], [AC_DEFINE([HAVE_SMART_STRFTIME])] [RESULT=["yes"]])
AC_MSG_RESULT([$RESULT])

DEFAULT_DRIVER=["null"]

dnl Now we can look for all needed libraries.
AC_MSG_RESULT([--- Looking for needed libraries ... ---                [[70%]]])
CXXFLAGS=$CXXFLAGS_ORIG;
CPPFLAGS=$CXXFLAGS_ORIG;
AC_CHECK_LIB([ncurses],[initscr],
							[],[AC_MSG_ERROR([Can not continue without ncurses.])])
if test ["$POSTGRES"] != ["no"] ; then
	AC_CHECK_LIB([pq],[PQsetdbLogin],
								[],[if test ["$POSTGRES"] = ["yes"] ; then
											AC_MSG_ERROR([Can not continue without PostgreSQL.])
										fi;POSTGRES=["no"]])
fi
if test ["$SQLITE"] != ["no"] ; then
	AC_CHECK_LIB([sqlite],[sqlite_open],
								[],[if test ["$SQLITE"] = ["yes"] ; then
											AC_MSG_ERROR([Can not continue without SQLite.])
										fi;SQLITE=["no"]])
fi
if test ["$MYSQL"] != ["no"] ; then
	AC_CHECK_LIB([mysqlclient],[mysql_init],[],[
			CXXFLAGS=["$CXXFLAGS -L/usr/local/lib/mysql"]
			unset ac_cv_lib_mysqlclient_mysql_init
			AC_CHECK_LIB([mysqlclient],[mysql_init],[
				AC_SUBST([EXTRA_LIB_PATHS], [[-L/usr/local/lib/mysql]])],
				[if test ["$MYSQL"] = ["yes"] ; then
					AC_MSG_ERROR([Can not continue without MySQL.])
				fi;MYSQL=["no"]])])
fi

CXXFLAGS=$CXXFLAGS_ORIG;
CPPFLAGS=$CXXFLAGS_ORIG;

AC_MSG_RESULT([--- All needed libraries found. ---                     [[76%]]])

AC_MSG_RESULT([--- Looking for needed headers ... ---                  [[77%]]])
_CONSOLE_MOUSE=[""];
AC_CHECK_HEADERS([[sys/consio.h]],_CONSOLE_MOUSE=["moused"])
AC_CHECK_HEADERS([[gpm.h]],_CONSOLE_MOUSE=["gpm"])
if test ["$_CONSOLE_MOUSE"] = [""] -a ["$CONSOLE_MOUSE"] = ["yes"] ; then
	AC_MSG_ERROR([Can not continue without console mouse support.])
fi
AC_CHECK_HEADERS([[ncurses/ncurses.h]],_NCURSES_HEADER=['ncurses/ncurses.h'],
	[AC_CHECK_HEADERS([[ncurses.h]],_NCURSES_HEADER=['ncurses.h'],
			[AC_MSG_ERROR([Can not continue without ncurses devel!])])])
if test ["$POSTGRES"] != ["no"] ; then
	AC_CHECK_HEADERS([[postgresql/libpq-fe.h]],
		[AC_SUBST([TARGET_POSTGRESQL_DRIVER],["\$(POSTGRESQL_DRIVER)"])
			DEFAULT_DRIVER=["PostgreSQL"]],
		[AC_CHECK_HEADERS([[libpq-fe.h]],
			[AC_SUBST([TARGET_POSTGRESQL_DRIVER],["\$(POSTGRESQL_DRIVER)"])
			DEFAULT_DRIVER=["PostgreSQL"]],
				[if test ["$POSTGRES"] = ["yes"] ; then
					AC_MSG_ERROR([[Can not continue without PostgreSQL devel(client)!]])
				fi;POSTGRES=["no"]])])
fi
if test ["$MYSQL"] != ["no"] ; then
	AC_CHECK_HEADERS([[mysql/mysql.h]],
		[AC_SUBST([TARGET_MYSQL_DRIVER],["\$(MYSQL_DRIVER)"])
			DEFAULT_DRIVER=["MySQL"]],
				[if test ["$MYSQL"] = ["yes"] ; then
					AC_MSG_ERROR([[Can not continue without MySQL devel(client)!]])
				fi;MYSQL=["no"]])
fi
if test ["$SQLITE"] != ["no"] ; then
	AC_CHECK_HEADERS([[sqlite.h]],
		[AC_SUBST([TARGET_SQLITE_DRIVER],["\$(SQLITE_DRIVER)"])
			DEFAULT_DRIVER=["SQLite"]],
				[if test ["$SQLITE"] = ["yes"] ; then
					AC_MSG_ERROR([[Can not continue without SQLite devel(client)!]])
				fi;SQLITE=["no"]])
fi
AC_SUBST([DEFAULT_DRIVER],["$DEFAULT_DRIVER"])

AC_MSG_RESULT([--- Done looking for needed header, all found. ---      [[82%]]])

AC_MSG_RESULT([--- Looking for functions and data members ... ---      [[83%]]])

AC_CHECK_FUNCS([getline],[],[])
AC_CHECK_DECLS([VSWTC],[],[],[#include <termios.h>])
AC_CHECK_DECL([basename],[],[AC_DEFINE([HAVE_BASE_NAME_IN_LIBGEN])],[#include <string.h>])
AC_CHECK_DECLS([B76800],[],[],[#include <termios.h>])
AC_CHECK_DECLS([B28800],[],[],[#include <termios.h>])
AC_CHECK_DECLS([B14400],[],[],[#include <termios.h>])
AC_CHECK_DECLS([B7200],[],[],[#include <termios.h>])

AC_MSG_RESULT([--- Done looking for functions and data members. ---    [[92%]]])

dnl We need to check if out ncurses devel is not brain-damaged.
AC_MSG_RESULT([--- Libraries/environment sanity check ... ---          [[93%]]])
AC_MSG_CHECKING([if your ncurses-devel is seriously brain-damaged])
CXXFLAGS=["$EXTRA_WARNINGS -Wall -Werror -Wcast-align -Wconversion -Wwrite-strings -pedantic-errors -Wcast-qual"]
AC_TRY_COMPILE([#include <$_NCURSES_HEADER>],
[printw("x");],[AC_MSG_RESULT([[no :)]])],
	[AC_MSG_RESULT([[yes :(]])
		AC_MSG_ERROR([Can not continue with seriously brain-damaged ncurses.])])
AC_MSG_CHECKING([if your ncurses is damaged about char-subscripts in ncurses (acs_map)])
AC_TRY_COMPILE([#include <$_NCURSES_HEADER>],
[addch(ACS_DARROW);],[AC_DEFINE([HAVE_ASCII_GRAPHICS]) AC_MSG_RESULT([[no :)]])],
	[AC_MSG_RESULT([[yes :(]])
		AC_MSG_WARN([You will not have pretty ascii graphics.])])

FILES=["/etc/my.cnf /etc/mysql/my.cnf /usr/local/etc/my.cnf"]
FILES=["$FILES /usr/local/etc/mysql/my.cnf /etc/mysqld.conf"]
FILES=["$FILES /etc/mysql/mysqld.conf /usr/local/etc/mysqld.conf"]
FILES=["$FILES /usr/local/etc/mysql/mysqld.conf"]
MYSQL_SOCKET=[`grep -s "^socket" $FILES|awk '{sub(".*=[ \\t]+","");print;exit}'`]
AC_SUBST([MYSQL_SOCKET],[$MYSQL_SOCKET])

AC_MSG_RESULT([--- Creating appropriate links.  ---                    [[97%]]])
AC_CONFIG_COMMANDS_PRE([cd doc/html;
												ln -sf stdhapi_toc.html index.html;cd ../..])

if test ["$POSTGRES"] = ["no"] -a ["$MYSQL"] = ["no"] -a ["$SQLITE"] = ["no"] ; then
	AC_MSG_WARN([stdhapi without any data base driver will be
                    unusable for most professional purpouses.])
fi

dnl We have to set up proper compiler/linker flags.
AC_OUTPUT([Makefile stdhapirc])
AC_MSG_RESULT([--- All ok, good luck! ---                     	       [[100%]]])
AC_MSG_RESULT([[Now you can type \`make'.]])

