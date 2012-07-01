dnl configure.ac is integral part of yaal client project.

m4_include([_aux/aclib.m4])
PRIVILEGES_SANITY

DO_TARGET="RELEASE"
AC_ARG_ENABLE([debug],[AC_HELP_STRING([--enable-debug],[Compile with debugging symbols.])],[DO_TARGET="DEBUG" LIB_INFIX="-d"])
AC_ARG_ENABLE([release],[AC_HELP_STRING([--enable-release],[Compile with full optimizations on (this is the default).])],[DO_TARGET="RELEASE" LIB_INFIX=""])
AC_ARG_ENABLE([reldeb],[AC_HELP_STRING([--enable-reldeb],[Compile with full optimizations but include debugging information.])],[DO_TARGET="RELDEB" LIB_INFIX="-rd"])
AC_ARG_ENABLE([relassert],[AC_HELP_STRING([--enable-relassert],[Compile with full optimizations but keep assertion checks.])],[DO_TARGET="RELASSERT" LIB_INFIX="-ra"])
AC_ARG_ENABLE([profiling],[AC_HELP_STRING([--enable-profiling],[Provide support for profiling tools.])],[DO_TARGET="PROFILING" LIB_INFIX="-p"])
AC_ARG_ENABLE([coverage],[AC_HELP_STRING([--enable-coverage],[Get ready to gather coverage data.])],[DO_TARGET="COVERAGE" LIB_INFIX="-c"])

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

dnl Cheching for machine type.

YAAL_DETECT_OPERATING_SYSTEM
YAAL_DETECT_PHYSICAL_MEMORY
YAAL_CHECK_GIT

dnl Looking for main libc headers
AC_HEADER_MAJOR

dnl Testing presence of C and C++ compiler
AC_PROG_CC
AC_PROG_CXX

dnl and for C and C++ preprocesor
AC_PROG_CPP
AC_PROG_CXXCPP

dnl AC_PROG_MKDIR_P dnl Centos 5.4 has obsolete version of autoconf package.
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_MKDIR_P
AC_CHECK_PROG([CTAGS],[exctags],[exctags],[ctags])

YAAL_CHECK_COMPILER_VERSION

YAAL_DETECT_COMMON_FLAGS

dnl Now we can look for all needed libraries.

CXXFLAGS="${CXXFLAGS} ${EXTRA_CXXFLAGS}"
LDFLAGS="${LDFLAGS} ${EXTRA_LXXFLAGS}"
CPPFLAGS=$CXXFLAGS;
AC_SEARCH_LIBS([libintl_gettext],[intl],,[AC_SEARCH_LIBS([gettext],[intl],,[AC_MSG_ERROR([Cannot continue without localization library.])])])
AC_CHECK_LIB([ncurses],[initscr],
							[],[AC_MSG_ERROR([Can not continue without ncurses library.])])
AC_CHECK_LIB([hcore${LIB_INFIX}],[yaal_hcore_main],
							[],[AC_MSG_ERROR([Can not continue without yaal hcore library.])])
AC_CHECK_LIB([hconsole${LIB_INFIX}],[yaal_hconsole_main],
							[],[AC_MSG_ERROR([Can not continue without yaal hconsole library.])])
AC_CHECK_LIB([tools${LIB_INFIX}],[yaal_tools_main],
							[],[AC_MSG_ERROR([Can not continue without yaal tools library.])])
AC_CHECK_LIB([dbwrapper${LIB_INFIX}],[yaal_dbwrapper_main],
							[],[AC_MSG_ERROR([Can not continue without yaal dbwrapper library.])])
AC_CHECK_LIB([hdata${LIB_INFIX}],[yaal_hdata_main],
							[],[AC_MSG_ERROR([Can not continue without yaal hdata library.])])

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
AC_SUBST([YAAL_LXXFLAGS],[${YAAL_LXXFLAGS}])
AC_SUBST([START_GROUP],[${START_GROUP}])
AC_SUBST([END_GROUP],[${END_GROUP}])
AC_SUBST([RDYNAMIC],[${RDYNAMIC}])
AC_SUBST([DO_TARGET],[${DO_TARGET}])

GEN_CONFIG_HXX_IN(CLIENT_CONFIGURE_PROJECT_NAME_UC)

dnl We have to set up proper compiler/linker flags.

AC_CONFIG_HEADERS([config.hxx])
AC_CONFIG_FILES([Makefile.mk])
AC_OUTPUT
AC_MSG_RESULT([[Now you can type \`make'.]])
])

