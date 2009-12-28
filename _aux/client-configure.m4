dnl configure.ac is integral part of yaal client project.

AC_CONFIG_AUX_DIR([../yaal/_aux])

m4_include([_aux/aclib.m4])

if test ["$CXX"] = ["colorgcc"] -o ["$CXX"] = ["colorg++"] ; then
	CXX=["g++"]
fi
if test ["$CC"] = ["colorgcc"] ; then
	CC=["gcc"]
fi

AC_DEFUN_ONCE([YAAL_CONFIGURE_CLIENT_INIT],
[
dnl Initialization data.

define([CLIENT_CONFIGURE_PROJECT_NAME_UC],m4_toupper([$1]))

dnl Unique file in project: projectnamerc is sample configuration file.
AC_CONFIG_SRCDIR([[$1rc]])

dnl yaal client is not Free Software.
AC_COPYRIGHT([[$1 (c) 2009 All rights reserved.]])

AC_REVISION([$2])

dnl We select default language.
AC_LANG([C++])
CXXFLAGS=["-L~/lib -L/usr/local/lib -I~/usr/include -I/usr/local/include"]

dnl Set up how configure will handle header checks.
AC_CONFIG_HEADERS([config.hxx])

dnl Cheching for machine type.

YAAL_DETECT_OPERATING_SYSTEM
YAAL_DETECT_PHYSICAL_MEMORY
YAAL_CHECK_GIT

dnl Testing presence of c compiler

dnl and for c preprocesor
AC_PROG_CC
AC_PROG_CPP

dnl Looking for main libc headers
AC_HEADER_MAJOR

dnl and for c++ preprocesor
AC_PROG_CXX
AC_PROG_CXXCPP

AC_CHECK_PROG([CTAGS],[exctags],[exctags],[ctags])

dnl Check compiler version.
AC_MSG_CHECKING([compiler version])
GCC_MAJOR=`echo | cpp -dM | grep __GNUC__ | awk '{print [$]3}'`
GCC_MINOR=`echo | cpp -dM | grep __GNUC_MINOR__ | awk '{print [$]3}'`
AC_MSG_RESULT([major $GCC_MAJOR, minor $GCC_MINOR. ])

dnl What special compiler flags we can set?

YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-fPIC], [C++])
YAAL_DETECT_FLAGS(RDYNAMIC, [-rdynamic], [C++])
YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--fatal-warnings], [C++])
YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--export-dynamic], [C++])
YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--no-undefined], [C++])
YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--enable-auto-import], [C++])
YAAL_DETECT_FLAGS(START_GROUP, [-Wl,--start-group], [C++])
if test ["x${START_GROUP}"] != ["x"] ; then
	END_GROUP="-Wl,--end-group"
fi

CXXFLAGS_ORIG=$CXXFLAGS;
CXXFLAGS=["-Wextra"]
AC_MSG_CHECKING([does gcc support -Wextra])
RESULT=["no"]
AC_COMPILE_IFELSE([ ],
							[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -Wextra"]]
							[RESULT=["yes"]],
							[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -W]"])
AC_MSG_RESULT([$RESULT])
CXXFLAGS=["-Wshadow -Werror"]
AC_MSG_CHECKING([can we use -Wshadow in <pthread.h>])
RESULT=["no"]
AC_COMPILE_IFELSE([#include <pthread.h>],
							[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -Wshadow"]]
							[RESULT=["yes"]],
							[AC_MSG_WARN([[Cannot use -Wshadow!]])])
AC_MSG_RESULT([$RESULT])
CXXFLAGS=["-pedantic-errors -Werror"]
AC_MSG_CHECKING([can we use -pedantic-errors in <cstdlib>])
RESULT=["no"]
AC_COMPILE_IFELSE([#include <cstdlib>],
							[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -pedantic-errors"]]
							[RESULT=["yes"]],
							[AC_MSG_WARN([[Cannot use -pedantic-errors!]])])
AC_MSG_RESULT([$RESULT])
CXXFLAGS=${CXXFLAGS_ORIG}

dnl Now we can look for all needed libraries.
CPPFLAGS=$CXXFLAGS;
AC_SEARCH_LIBS([libintl_gettext],[intl],,[AC_SEARCH_LIBS([gettext],[intl],,[AC_MSG_ERROR([Cannot continue without localization library.])])])
AC_CHECK_LIB([ncurses],[initscr],
							[],[AC_MSG_ERROR([Can not continue without ncurses.])])
AC_CHECK_LIB([hcore],[yaal_hcore_main],
							[],[AC_MSG_ERROR([Can not continue without hcore.])])
AC_CHECK_LIB([hconsole],[yaal_hconsole_main],
							[],[AC_MSG_ERROR([Can not continue without hconsole.])])
AC_CHECK_LIB([tools],[yaal_tools_main],
							[],[AC_MSG_ERROR([Can not continue without tools.])])
AC_CHECK_LIB([dbwrapper],[yaal_dbwrapper_main],
							[],[AC_MSG_ERROR([Can not continue without dbwrapper.])])
AC_CHECK_LIB([hdata],[yaal_hdata_main],
							[],[AC_MSG_ERROR([Can not continue without hdata.])])

AC_CHECK_HEADERS([yaal/yaal.hxx],,
			[AC_MSG_ERROR([[Can not continue without yaal devel!]])])

dnl We need to check if out ncurses devel is not brain-damaged.
])

AC_DEFUN([GEN_CONFIG_HXX_IN],
[
AH_TOP([/* $Id$ */

#ifndef $1_CONFIG_HXX_INCLUDED
#define $1_CONFIG_HXX_INCLUDED

/* Define core behavior here. */])
AH_BOTTOM(
#endif /* not $1_CONFIG_HXX_INCLUDED */
)
])

AC_DEFUN([YAAL_CONFIGURE_CLIENT_FINALIZE],
[
AC_SUBST([EXTRA_CXXFLAGS],[${EXTRA_CXXFLAGS}])
AC_SUBST([EXTRA_LXXFLAGS],[${EXTRA_LXXFLAGS}])
AC_SUBST([START_GROUP],[${START_GROUP}])
AC_SUBST([END_GROUP],[${END_GROUP}])
AC_SUBST([RDYNAMIC],[${RDYNAMIC}])

GEN_CONFIG_HXX_IN(CLIENT_CONFIGURE_PROJECT_NAME_UC)

dnl We have to set up proper compiler/linker flags.
AC_OUTPUT([Makefile.mk])
AC_MSG_RESULT([[Now you can type \`make'.]])
])

