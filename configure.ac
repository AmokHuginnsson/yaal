dnl configure.ac is integral part of stdhapi project.

dnl Initialization data.
AC_INIT([[stdhapi]],[[0.0.0]],[[amok@frihet.ath.cx]])

AC_MSG_RESULT([--- Welcome to stdhapi configurator ---                  [[0%]]])
AC_MSG_RESULT([--- Initialization ... ---                               [[6%]]])

dnl Unique file in project: stdhapirc is sample configuration file.
AC_MSG_RESULT([--- Finding base directory ... ---                      [[12%]]])
AC_CONFIG_SRCDIR([[stdhapirc]])

dnl stdhapi is not Free Software.
AC_COPYRIGHT([[stdhapi (c) 2003 All rights reserved.]])

dnl stdhapi is still in alpha stage.
AC_REVISION([[$ Revision: 0.0.0 $]])

dnl We select default language.
AC_LANG([C++])

dnl We will generate config.h with basic #defines.
AC_CONFIG_HEADER([config.h])

dnl Cheching for machine type.
AC_MSG_RESULT([--- Checking for machine type ... ---                   [[18%]]])
AC_CANONICAL_HOST()

dnl Testing presence of c compiler
AC_MSG_RESULT([--- Testing presence of c compiler ... ---              [[27%]]])
AC_PROG_CC
dnl and for c++ compiler
AC_MSG_RESULT([--- Testing presence of c++ compiler ... ---            [[35%]]])
AC_PROG_CXX
dnl and for c preprocesor
AC_MSG_RESULT([--- Looking for c/c++ preprocessor and egrep ... ---    [[43%]]])
AC_PROG_CPP
AC_PROG_CXXCPP
AC_PROG_EGREP

dnl Looking for main libc headers
AC_MSG_RESULT([--- Looking for main libc headers ... ---               [[51%]]])
AC_HEADER_MAJOR

dnl We need to know what operating system stdhapi will be compiled on.
AC_MSG_RESULT([--- Guessing host operating system ... ---              [[69%]]])
AC_CHECK_HEADER(
	[[ncurses/ncurses.h]],
	[AC_DEFINE([[__PLD_HOST__]]) _HOST=['PLD. ---    ']],
	[AC_CHECK_HEADER(
		[[bsd/curses.h]],
		[AC_DEFINE([[__FREEBSD_HOST__]]) _HOST=['FreeBSD. ---']],
		[AC_CHECK_HEADER(
			[[ncurses.h]],
			[AC_DEFINE([[__DEBIAN_HOST__]]) _HOST=['Debian. --- ']],
			[AC_MSG_ERROR([Can not recognize operating system!])]
		)]
	)]
)
AC_MSG_RESULT([--- Guessed host operating system is $_HOST       [[74%]]])

dnl What special compiler flags we can set?
AC_MSG_RESULT([--- Guessing specific compiler/linker features ... ---  [[75%]]])
_FLAG____________='unfortunetely none'
dnl CXXFLAGS=["$CXXFLAGS -fPIC -shared -Wl,--no-undefined -Wl,--fatal-warnings --Wl,--export-dynamic -Wl,-soname,conftest"]
dnl CXXFLAGS=["$CXXFLAGS -fPIC -shared -Wl,--no-undefined -Wl,--fatal-warnings --Wl,--export-dynamic -Wl,-soname,conftest"]
CXXFLAGS=["$CXXFLAGS -fPIC -shared -Wl,--no-undefined -Wl,--fatal-warnings -Wl,-soname,conftest"]
AC_LINK_IFELSE([[extern int errno;void foo(void){errno=0;}]],
							[AC_SUBST([SPECIAL_LINKER_FLAG],
							[[-Wl,--no-undefined]])] _FLAG____________=[[-Wl,--no-undefined]],
							[AC_MSG_WARN([[Can not use -Wl,--no-undefined!]])])
AC_MSG_RESULT([--- Guessed fetures are $_FLAG____________. ---         [[83%]]])

dnl Now we can look for all needed libraries.
AC_MSG_RESULT([--- Looking for needed libraries ... ---                [[84%]]])
AC_CHECK_LIB([mysqlclient],[mysql_init],[],[Can not continue without MySQL.])
AC_CHECK_LIB([pq],[PQsetdbLogin],
							[],[AC_MSG_ERROR([Can not continue without PostGreSQL.])])
AC_MSG_RESULT([--- All needed libraries found. ---                     [[92%]]])

dnl We have to set up proper compiler/linker flags.
AC_OUTPUT([Makefile])

AC_MSG_RESULT([--- All ok, good luck! ---                              [[99%]]])
AC_MSG_RESULT([[Now you can type \`make'.                               [100%]]])

