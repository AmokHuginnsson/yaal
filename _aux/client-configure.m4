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

AC_DEFUN_ONCE([YAAL_CONFIGURE_CLIENT_INIT], [
dnl Initialization data.

define([CLIENT_CONFIGURE_PROJECT_NAME_UC],m4_toupper([$1]))

dnl Unique file in project: projectnamerc is sample configuration file.
AC_CONFIG_SRCDIR([[$1rc]])

dnl yaal client is not Free Software.
AC_COPYRIGHT([[$1 (c) 2018 All rights reserved.]])

AC_REVISION([$2])

YAAL_CHECK_GIT

dnl We select default language.
AC_LANG([C++])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

YAAL_DETECT_COMPILER

dnl Cheching for machine type.

YAAL_DETECT_OPERATING_SYSTEM

dnl Looking for main libc headers
AC_HEADER_MAJOR

AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_MKDIR_P
AC_CHECK_PROG([CTAGS],[exctags],[exctags],[ctags])
AC_CHECK_PROG([GTAGS],[gtags],[gtags],[true])

YAAL_CHECK_COMPILER_VERSION

YAAL_CXX_STANDARD_CHECK

YAAL_DETECT_COMMON_FLAGS

dnl Now we can look for all needed libraries.

CXXFLAGS=["-std=c++11 ${EXTRA_CXXFLAGS} ${EXTRA_INCLUDE_PATHS}"]
LDFLAGS=["${EXTRA_LXXFLAGS} ${LDFLAGS} ${EXTRA_LIBRARY_PATHS}"]
CPPFLAGS=["$CXXFLAGS"]
AC_CHECK_LIB([yaal_hcore${LIB_INFIX}],[yaal_hcore_main],
							[],[AC_MSG_ERROR([Can not continue without yaal hcore library.])])
AC_CHECK_LIB([yaal_tools${LIB_INFIX}],[yaal_tools_main],
							[],[AC_MSG_ERROR([Can not continue without yaal tools library.])])
AC_CHECK_LIB([yaal_dbwrapper${LIB_INFIX}],[yaal_dbwrapper_main],
							[],[AC_MSG_ERROR([Can not continue without yaal dbwrapper library.])])
AC_CHECK_LIB([yaal_hconsole${LIB_INFIX}],[yaal_hconsole_main],
							[],[AC_MSG_ERROR([Can not continue without yaal hconsole library.])],[-lyaal_tools${LIB_INFIX}])

REV_LIBS=$(echo "${LIBS}" | awk '{ for (i=NF; i>1; i--) printf("%s ",[$]i); print [$]1; }')

AC_CHECK_LIB([yaal_hdata${LIB_INFIX}],[yaal_hdata_main],
							[],[AC_MSG_ERROR([Can not continue without yaal hdata library.])])

LIBS=["${REV_LIBS} ${LIBS}"]

AC_SEARCH_LIBS([libintl_gettext],[intl],,[AC_SEARCH_LIBS([gettext],[intl],,[AC_MSG_ERROR([Cannot continue without localization library.])])])
AC_CHECK_LIB([ncurses],[initscr],
							[],[AC_MSG_ERROR([Can not continue without ncurses library.])])


AC_CHECK_HEADERS([yaal/yaal.hxx],,
			[AC_MSG_ERROR([[Can not continue without yaal devel!]])])

dnl We need to check if out ncurses devel is not brain-damaged.
])

AC_DEFUN_ONCE([GEN_CONFIG_HXX_IN], [
AH_TOP([/* $Id$ */

#ifndef $1_CONFIG_HXX_INCLUDED
#define $1_CONFIG_HXX_INCLUDED

/* Define core behavior here. */])
AH_BOTTOM(
#endif /* not $1_CONFIG_HXX_INCLUDED */
)
])

AC_DEFUN_ONCE([YAAL_CONFIGURE_CLIENT_FINALIZE], [
AC_SUBST([EXTRA_INCLUDE_PATHS], [${EXTRA_INCLUDE_PATHS}])
AC_SUBST([EXTRA_LIBRARY_PATHS], [${EXTRA_LIBRARY_PATHS}])
AC_SUBST([EXTRA_CXXFLAGS],[${EXTRA_CXXFLAGS}])
AC_SUBST([EXTRA_LXXFLAGS],[${EXTRA_LXXFLAGS}])
AC_SUBST([EXTRA_COMPILER_DEBUG_FLAGS],[${EXTRA_COMPILER_DEBUG_FLAGS}])
AC_SUBST([EXTRA_COMPILER_COVERAGE_FLAGS],[${EXTRA_COMPILER_COVERAGE_FLAGS}])
AC_SUBST([EXTRA_COMPILER_OPTIMIZATION_FLAGS],[${EXTRA_COMPILER_OPTIMIZATION_FLAGS}])
AC_SUBST([YAAL_LXXFLAGS],[${YAAL_LXXFLAGS}])
AC_SUBST([START_GROUP],[${START_GROUP}])
AC_SUBST([END_GROUP],[${END_GROUP}])
AC_SUBST([SONAME_FLAG],[${SONAME_FLAG}])
AC_SUBST([LIB_PREFIX],[${LIB_PREFIX}])
AC_SUBST([RDYNAMIC],[${RDYNAMIC}])
AC_SUBST([AS_NEEDED],[${YES_AS_NEEDED}])
AC_SUBST([NO_AS_NEEDED],[${NO_AS_NEEDED}])
AC_SUBST([DO_TARGET],[${DO_TARGET}])

GEN_CONFIG_HXX_IN(CLIENT_CONFIGURE_PROJECT_NAME_UC)

dnl We have to set up proper compiler/linker flags.

AC_CONFIG_HEADERS([config.hxx])
AC_CONFIG_FILES([Makefile.mk] [configure.mk:_aux/configure.mk.in])
AC_CHECK_FILE(
	[${srcdir}/LICENSE.md],
	AC_CONFIG_FILES(
		[license.hxx:LICENSE.md],
		[${srcdir}/_aux/mklicense license.hxx ${srcdir}/LICENSE.md]
	)
)
AC_OUTPUT
AC_MSG_RESULT([[Now you can type \`make'.]])
])

