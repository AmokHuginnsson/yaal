dnl configure.ac is integral part of stdhapi project.

dnl Initialization data.
AC_INIT([[stdhapi]],[[0.0.0]],[[amok@frihet.ath.cx]])

AC_MSG_RESULT([--- Welcome to stdhapi configurator ---])

dnl Cheching for machine type.
AC_CANONICAL_HOST()

dnl Unique file in project: stdhapirc is sample configuration file.
AC_CONFIG_SRCDIR([[stdhapirc]])

dnl stdhapi is not Free Software.
AC_COPYRIGHT([[stdhapi (c) 2003 All rights reserved.]])

dnl stdhapi is still in alpha stage.
AC_REVISION([[$ Revision: 0.0.0 $]])

dnl We will generate config.h with basic #defines.
AC_CONFIG_HEADER([config.h])

dnl Testing for presence of c compiler
AC_PROG_CC
dnl and for c++ compiler
AC_PROG_CXX
dnl and for c preprocesor
AC_PROG_CPP

dnl Looking for main libc headers
AC_HEADER_MAJOR

dnl We need to know what operating system stdhapi will be compiled on.
AC_MSG_RESULT([--- Guessing host type ... ---])
AC_CHECK_HEADER([[ncurses/ncurses.h]],[AC_DEFINE([[__PLD_HOST__]] _HOST='PLD')],
	[AC_CHECK_HEADER([[bsd/curses.h]],
		[AC_DEFINE([[__FREEBSD_HOST__]] _HOST=[FreeBSD])],
		[AC_DEFINE([[__DEBIAN_HOST__]])] _HOST=[Debian])])
AC_MSG_RESULT([--- Guessed host type is $_HOST. ---])

dnl We have to set up proper compiler/linker flags.
AC_OUTPUT([Makefile])

AC_MSG_RESULT([--- All ok, good luck! ---])
AC_MSG_RESULT([[Now you can type \`make'.]])

