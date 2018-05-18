dnl Clear CFLAGS and CXXFLAGS.
dnl --------------------------------------------------------------------------
:${CXXFLAGS=""}
:${CFLAGS=""}
dnl --------------------------------------------------------------------------

dnl Bail out if running as root.
dnl --------------------------------------------------------------------------
AC_DEFUN_ONCE([PRIVILEGES_SANITY], [
	EID=`id -u`
	if test "x${EID}" = "x0" ; then
		AC_MSG_ERROR([running with super-user privileges - bailing out])
	fi
	UMASK=`umask | sed 's/0077/77/'`
	if test "x${UMASK}" != "x77" ; then
		AC_MSG_ERROR([running with too permissive umask: `${UMASK}', must be: `0077' - bailing out])
	fi
])


dnl YAAL_DETECT_FLAGS(RESULT, FLAGSET)
dnl Detect if the compiler supports a set of flags
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_DETECT_FLAGS], [
	ORIG=[$]$1
	$1=
	AC_LANG_PUSH(ifelse( "x$3", "xC++", $3, C ))
	define([FLAGS], ifelse( "x$3", "xC++", CXXFLAGS, CFLAGS ))
	define([COMPILER], ifelse( "x$3", "xC++", C++, C ))
	for flag in $2; do
		if test -z "[$]$1"; then
			$1_save_FLAGS="$FLAGS"
			FLAGS="-Werror $FLAGS $flag -pedantic-errors $4"
			AC_MSG_CHECKING([whether COMPILER compiler understands [$]flag])
			touch conftest.c
			UNRECOGNIZED=`${CC} -o conftest.o -c ${FLAGS} conftest.c 2>&1 | grep "unrecognized option"`
			/bin/rm -f conftest.c conftest.o
			if test "x${UNRECOGNIZED}" = "x" ; then
				AC_LINK_IFELSE([AC_LANG_SOURCE([[int main( int, char** ) { return ( 0 ); }]])], [$1_works=yes], [$1_works=no])
			else
				$1_works=no
			fi
			AC_MSG_RESULT([$]$1_works)
			FLAGS="[$]$1_save_FLAGS"
			if test "x[$]$1_works" = "xyes"; then
				$1="$ORIG $flag"
			else
				$1="$ORIG"
			fi
		fi
	done
	undefine([COMPILER],[FLAGS])
	AC_LANG_POP(ifelse( "x$3", "xC++", $3, C ))
])

dnl Handle user choice regarding C++ standard.
dnl --------------------------------------------------------------------------
AC_DEFUN_ONCE([YAAL_CXX_STANDARD_CHECK], [
	AC_REQUIRE([YAAL_CHECK_COMPILER_VERSION])
	YAAL_DETECT_FLAGS(CXX_STANDARD, [-std=c++11], [C++])
	if test ["x$CXX_STANDARD"] = ["x"] ; then
		AC_MSG_ERROR([Requested C++ Standard version is not available in this environment!])
	fi
])

dnl Checks for which function macros exist
dnl --------------------------------------------------------------------------
AC_DEFUN_ONCE([YAAL_DETECT_FUNCTION_MACRO], [
	AC_MSG_CHECKING(whether $CC implements __PRETTY_FUNCTION__)
	AC_CACHE_VAL(yaal_cv_have_func,
							 [AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstdio>
int main( int, char** ){ printf( "%s", __PRETTY_FUNCTION__ ); return ( 0 );}]])],
							 yaal_cv_have_func=yes, yaal_cv_have_func=no)])
	AC_MSG_RESULT($yaal_cv_have_func)
	if test "$yaal_cv_have_func" = yes; then
		AC_DEFINE(HAVE_PRETTY_FUNCTION,[],[Does the compiler understand __PRETTY_FUNCTION__])
	else
		AC_MSG_CHECKING(whether $CC implements __FUNCTION__)
		AC_CACHE_VAL(yaal_cv_have_pretty_function,
								 [AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstdio>
int main( int, char** ){ printf( "%s", __FUNCTION__ ); return ( 0 );}]])],
								 yaal_cv_have_pretty_function=yes, yaal_cv_have_pretty_function=no)])
		AC_MSG_RESULT($yaal_cv_have_pretty_function)
		if test "$yaal_cv_have_pretty_function" = yes; then
			AC_DEFINE(HAVE_FUNCTION,[],[Does compiler understand __FUNCTION__])
		else
			AC_MSG_CHECKING(whether $CC implements __func__)
			AC_CACHE_VAL(yaal_cv_have_function,
									 [AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstdio>
int main( int, char** ){ printf( "%s", __func__ ); return ( 0 );}]])],
									 yaal_cv_have_function=yes, yaal_cv_have_function=no)])
			AC_MSG_RESULT($yaal_cv_have_function)
			if test "$yaal_cv_have_function" = yes; then
				AC_DEFINE(HAVE_FUNC,[],[Does compiler understand __func__])
			fi
		fi
	fi
])


dnl We need to know what operating system yaal will be compiled on.
dnl --------------------------------------------------------------------------
AC_DEFUN_ONCE([YAAL_DETECT_OPERATING_SYSTEM], [
	AC_MSG_CHECKING([host operating system])
	AC_CANONICAL_HOST
	HOST_OS_TYPE=""
	UNAME_OS_NAME=["`uname -s | sed -e 's/[^a-zA-Z].*//g'`"]
	LIB_PREFIX=["lib"]
	LIB_EXT=["so"]
	EXE_SUFFIX=[""]
	HOME_ENV_VAR=['"HOME"']
	SYMBOL_PREFIX=['""']
	if test ["x${UNAME_OS_NAME}"] = ["xLinux"] ; then
		AC_SUBST([SERIAL_DEVICE],['ttyS0'])
		AC_CHECK_PROG([LINUX_DISTRO],[lsb_release],[yes])
		if test ["x${LINUX_DISTRO}"] = ["xyes"] ; then
			HOST_OS_TYPE=`lsb_release -i|awk '/Distributor/{print [$]3}'`
		fi
		AC_DEFINE([__HOST_OS_TYPE_LINUX__], [], [Your operating system is Linux.])
		YAAL_LXXFLAGS=["-Wl,--entry=\"${PACKAGE_NAME}_\$(*)_main\""]
		if test ["x${HOST_OS_TYPE}"] = ["xDebian"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/debconf.conf] \) ; then
			AC_DEFINE([__HOST_OS_TYPE_DEBIAN__], [], [Your specific Linux version is Debian.])
			HOST_OS_TYPE=[Debian]
		elif test ["x${HOST_OS_TYPE}"] = ["xUbuntu"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/00-header] \) ; then
			AC_DEFINE([__HOST_OS_TYPE_UBUNTU__], [], [Your specific Linux version is Ubuntu.])
			HOST_OS_TYPE=[Ubuntu]
		elif test ["x${HOST_OS_TYPE}"] = ["xCentOS"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/yum.repos.d/CentOS-Base.repo] \) ; then
			AC_DEFINE([__HOST_OS_TYPE_CENTOS__], [], [Your specific Linux version is CentOS.])
			HOST_OS_TYPE=[CentOS]
		elif test ["x${HOST_OS_TYPE}"] = ["xFedora"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/yum.repos.d/CentOS-Base.repo] \) ; then
			AC_DEFINE([__HOST_OS_TYPE_FEDORA__], [], [Your specific Linux version is Fedora.])
			HOST_OS_TYPE=[CentOS]
		elif test ["x${HOST_OS_TYPE}"] = ["xRaspbian"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/rpi-issue] \) ; then
			AC_DEFINE([__HOST_OS_TYPE_RASPBIAN__], [], [Your specific Linux version is Raspbian.])
			HOST_OS_TYPE=[Raspbian]
		elif test ["x${HOST_OS_TYPE}"] = ["xPLD"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/poldek/poldek.conf] \) ; then
			AC_DEFINE([__HOST_OS_TYPE_PLD__], [], [Your specific Linux version is PLD.])
			HOST_OS_TYPE=[PLD]
		elif test ["x${HOST_OS_TYPE}"] = ["xSlackware"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/random-seed] \) ; then
			AC_DEFINE([__HOST_OS_TYPE_SLACKWARE__], [], [Your specific Linux version is Slackware.])
			HOST_OS_TYPE=[Slackware]
		else
			HOST_OS_TYPE=[Linux]
		fi
	elif test ["x${UNAME_OS_NAME}"] = ["xFreeBSD"] ; then
		AC_DEFINE([__HOST_OS_TYPE_FREEBSD__], [], [Your operating system is FreeBSD.])
		HOST_OS_TYPE=[FreeBSD]
		EXTRA_INCLUDE_PATHS=["${EXTRA_INCLUDE_PATHS} -I/usr/local/include"]
		EXTRA_LIBRARY_PATHS=["${EXTRA_LIBRARY_PATHS} -L/usr/local/lib"]
	elif test ["x${UNAME_OS_NAME}"] = ["xSunOS"] ; then
		AC_DEFINE([__HOST_OS_TYPE_SOLARIS__], [], [Your operating system is Solaris.])
		HOST_OS_TYPE=[Solaris]
		EXTRA_LIBRARY_PATHS=["${EXTRA_LIBRARY_PATHS} -L/usr/local/lib -L/usr/gnu/lib/amd64 -L/usr/gnu/lib"]
	elif test ["x${UNAME_OS_NAME}"] = ["xDarwin"] ; then
		AC_DEFINE([__HOST_OS_TYPE_DARWIN__], [], [Your operating system is Darwin.])
		HOST_OS_TYPE=[Darwin]
		YAAL_LXXFLAGS=["${YAAL_LXXFLAGS} -Wl,-undefined,dynamic_lookup"]
		EXTRA_INCLUDE_PATHS="-isystem /opt/local/include ${EXTRA_INCLUDE_PATHS}"
		EXTRA_LIBRARY_PATHS=["${EXTRA_LIBRARY_PATHS} -L/opt/local/lib"]
		LIB_EXT=["dylib"]
	elif test ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/tizen-release] ; then
		AC_DEFINE([__HOST_OS_TYPE_TIZEN__], [], [Your operating system is Tizen.])
		HOST_OS_TYPE=[Tizen]
	elif test ["x${UNAME_OS_NAME}"] = ["xCYGWIN"] ; then
		AC_DEFINE([__HOST_OS_TYPE_CYGWIN__], [], [Your operating system is Cygwin.])
		EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -D_GNU_SOURCE -U__STRICT_ANSI__"]
		YAAL_LXXFLAGS=["${YAAL_LXXFLAGS} -Wl,--export-all-symbols -Wl,--enable-auto-import -Wl,--out-implib=lib\$(*)\$(LIB_INFIX).\$(LIB_ARCHIVE_SUFFIX)"]
		HOST_OS_TYPE=[Cygwin]
		LIB_PREFIX=["cyg"]
		LIB_EXT=["dll"]
		HCORE_LIBS=["${HCORE_LIBS} -liconv"]
	fi
	AC_DEFINE_UNQUOTED([SYMBOL_PREFIX],${SYMBOL_PREFIX},[Symbol prefix used on this platform.])
	AC_DEFINE_UNQUOTED([LIB_PREFIX],"${LIB_PREFIX}",[Dynamic library file name prefix used on this platform.])
	AC_DEFINE_UNQUOTED([LIB_INFIX],"${LIB_INFIX}",[Target dependent library name infix.])
	AC_DEFINE_UNQUOTED([LIB_EXT],"${LIB_EXT}",[Dynamic library file name extension used on this platform.])
	AC_DEFINE_UNQUOTED([EXE_SUFFIX],${EXE_SUFFIX},[Suffix used for binary program executable on this platform.])
	AC_DEFINE_UNQUOTED([HOME_ENV_VAR],${HOME_ENV_VAR},[Name of environment variable used to point to user home directory.])
	AC_SUBST([LIB_EXT],[${LIB_EXT}])
	AC_SUBST([UNAME_OS_NAME],[${UNAME_OS_NAME}])
	AC_SUBST([HOST_OS_TYPE],[${HOST_OS_TYPE}])

	if test ["x${HOST_OS_TYPE}"] = ["x"] ; then
		AC_MSG_ERROR([Cannot recognize host operating system type!])
	else
		AC_MSG_RESULT([Your system type is $HOST_OS_TYPE.])
	fi
])


dnl Check available git features.
dnl --------------------------------------------------------------------------
AC_DEFUN_ONCE([YAAL_CHECK_GIT], [
	AC_CHECK_PROG(HAS_GIT,[git],["yes"],["no"])
	AC_MSG_CHECKING([git id sub-command])
	AC_SUBST(GITID,[true])
	if test ["$HAS_GIT"] = ["yes"] ; then
		if test -e "${srcdir}/.git" ; then
			THIS_ID=`git id ${0}.ac 2> /dev/null`
			if test ["x${THIS_ID}"] != ["x"] ; then
				AC_SUBST(GITID,["git id"])
				AC_MSG_RESULT([yes])
			else
				AC_SUBST(GITID,["git log --no-color -n 1 --pretty=format:\"%H %ad %an\""])
				AC_MSG_RESULT([no])
			fi
		else
			AC_MSG_WARN([not a git hosted project])
		fi
	else
		AC_MSG_WARN([no git in the first place])
	fi
])

dnl YAAL_DETECT_COMPILER
dnl Detect installed compiler package.
dnl --------------------------------------------------------------------------
AC_DEFUN_ONCE([YAAL_DETECT_COMPILER], [
	AC_MSG_RESULT([--- Testing presence of C and C++ compiler ... ---      [[19%]]])
	AC_PROG_CC([${CC} gcc clang])
	AC_PROG_CXX([${CXX} g++ clang++ clang])
	AC_MSG_RESULT([--- Testing presence of C and C++ preprocesor ... ---   [[28%]]])
	AC_PROG_CPP([${CC} gcc clang])
	AC_PROG_CXXCPP([${CXX} g++ clang++ clang])
])

dnl YAAL_CHECK_COMPILER_VERSION
dnl Check compiler version.
dnl --------------------------------------------------------------------------
AC_DEFUN_ONCE([YAAL_CHECK_COMPILER_VERSION], [
	AC_REQUIRE([YAAL_DETECT_COMPILER])
	AC_MSG_CHECKING([compiler version])
	GCC_MAJOR=`echo | cpp -dM | grep __GNUC__ | awk '{print [$]3}'`
	GCC_MINOR=`echo | cpp -dM | grep __GNUC_MINOR__ | awk '{print [$]3}'`
	AC_MSG_RESULT([major $GCC_MAJOR, minor $GCC_MINOR. ])
])

dnl YAAL_DETECT_COMMON_FLAGS
dnl What special compiler flags we can set?
dnl --------------------------------------------------------------------------
AC_DEFUN_ONCE([YAAL_DETECT_COMMON_FLAGS], [
	if test ["x${HOST_OS_TYPE}"] != ["xDarwin"] ; then
		YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-pthread], [C++])
		YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-pthread], [C++])
	fi
	if test ["x${HOST_OS_TYPE}"] == ["xRaspbian"] ; then
		YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-mcpu=cortex-a53], [C++])
		YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-mfpu=neon-fp-armv8], [C++])
		YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-mfloat-abi=hard], [C++])
		YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-mlittle-endian], [C++])
		YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-munaligned-access], [C++])
		YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-funsafe-math-optimizations], [C++])
	fi
	YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-m64], [C++])
	AC_ARG_ENABLE([native],[AS_HELP_STRING([--enable-native],[Use all available CPU features.])],[YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-march=native], [C++])])
	YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-fPIC], [C++])
	YAAL_DETECT_FLAGS(EXTRA_COMPILER_OPTIMIZATION_FLAGS, [-fexpensive-optimizations], [C++])
	YAAL_DETECT_FLAGS(RDYNAMIC, [-rdynamic], [C++])
	YAAL_DETECT_FLAGS(YES_AS_NEEDED, [-Wl,--as-needed], [C++])
	YAAL_DETECT_FLAGS(NO_AS_NEEDED, [-Wl,--no-as-needed], [C++])
	YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-Wmissing-declarations], [C++])
	YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-Wlarger-than-65536], [C++])
	YAAL_DETECT_FLAGS(FATAL_WARNINGS, [-Wl,--fatal-warnings], [C++])
	EXTRA_LXXFLAGS="${EXTRA_LXXFLAGS} ${FATAL_WARNINGS}"
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-m64], [C++])
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--demangle], [C++])
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,-C], [C++])
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--export-dynamic], [C++], [${FATAL_WARNINGS}])
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,-export_dynamic], [C++], [${FATAL_WARNINGS}])
	YAAL_DETECT_FLAGS(LXXFLAGS_NO_UNDEFINED, [-Wl,--no-undefined], [C++])
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--enable-auto-import], [C++])
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--enable-runtime-pseudo-relocs], [C++])

	YAAL_DETECT_FLAGS(START_GROUP, [-Wl,--start-group], [C++])
	YAAL_DETECT_FLAGS(SONAME_FLAG, [-Wl,-soname,foo], [C++])
	if test ["x${HOST_OS_TYPE}"] = ["xSolaris"] ; then
		YAAL_DETECT_FLAGS(EXTRA_COMPILER_DEBUG_FLAGS, [-gstabs+], [C++])
	fi
	if test ["x${HOST_OS_TYPE}"] != ["xRaspbian"] ; then
		YAAL_DETECT_FLAGS(EXTRA_COMPILER_DEBUG_FLAGS, [-femit-class-debug-always], [C++])
		YAAL_DETECT_FLAGS(EXTRA_COMPILER_DEBUG_FLAGS, [-fno-eliminate-unused-debug-types], [C++])
	else
		YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--no-keep-memory], [C++])
	fi
	if test ["x${START_GROUP}"] != ["x"] ; then
		END_GROUP=["-Wl,--end-group"]
	fi
	if test ["x${SONAME_FLAG}"] != ["x"] ; then
		SONAME_FLAG=["-Wl,-soname,\$(SONAME_\$(*))"]
	fi

	_FLAGS=''
	CXXFLAGS_ORIG=["${CXXFLAGS}"]
	CPPFLAGS_ORIG=["${CPPFLAGS}"]
	CXXFLAGS=["-Wextra"]
	AC_MSG_CHECKING([does gcc support -Wextra])
	RESULT=["no"]
	W_EXTRA=[""]
	AC_COMPILE_IFELSE([AC_LANG_SOURCE([ ])],
								[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -Wextra"]]
								[RESULT=["yes"]]
								[W_EXTRA=["-Wextra"]],
								[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -W]"]
								[W_EXTRA=["-W"]])
	AC_MSG_RESULT([$RESULT])
	_FLAGS=["$W_EXTRA"]
	CXXFLAGS=["-Wshadow -Werror"]
	AC_MSG_CHECKING([can we use -Wshadow in <pthread.h>])
	RESULT=["no"]
	AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <pthread.h>]])],
								[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -Wshadow"]]
								[_FLAGS=["$_FLAGS -Wshadow"]]
								[RESULT=["yes"]],
								[AC_MSG_WARN([[Cannot use -Wshadow!]])])
	AC_MSG_RESULT([$RESULT])
	CXXFLAGS=["${CXXFLAGS_ORIG}"]
	CXXFLAGS=["-pedantic-errors -Wno-long-long -Werror"]
	AC_MSG_CHECKING([can we use -pedantic-errors in <cstdlib>])
	RESULT=["no"]
	AC_COMPILE_IFELSE([AC_LANG_SOURCE([[#include <cstdlib>]])],
								[EXTRA_CXXFLAGS=["${EXTRA_CXXFLAGS} -pedantic-errors"]] dnl -Wno-long-long is added in _aux/mk/5_flags.mk
								[_FLAGS=["$_FLAGS -pedantic-errors"]]
								[RESULT=["yes"]],
								[AC_MSG_WARN([[Cannot use -pedantic-errors!]])])
	AC_MSG_RESULT([$RESULT])
	CXXFLAGS=$CXXFLAGS_ORIG
	if test ["$_FLAGS"] = [""] ; then
		_FLAGS='(none)'
	fi
	AC_MSG_RESULT([Compiler extra flags are: $_FLAGS])
	CXXFLAGS=["${CXXFLAGS_ORIG}"]
	CPPFLAGS=["${CXXFLAGS_ORIG}"]
	AC_SUBST(LXXFLAGS_NO_UNDEFINED, [${LXXFLAGS_NO_UNDEFINED}])
])

dnl Work around an autoconf bug.
if test "$prefix" = "NONE"; then
	prefix="${ac_default_prefix}"
fi
if test "$exec_prefix" = "NONE"; then
	exec_prefix="${prefix}"
fi

SYSCONFDIR=`eval echo ${sysconfdir}`
AC_DEFINE_UNQUOTED([SYSCONFDIR], "${SYSCONFDIR}", [Path to global system configuration directory.])
LOCALSTATEDIR=`eval echo ${localstatedir}`
AC_DEFINE_UNQUOTED([LOCALSTATEDIR], "${LOCALSTATEDIR}", [Path to data files which the programs modify while they run.])
datadir_int=`eval echo ${datadir}`
DATADIR=`eval echo ${datadir_int}`
AC_DEFINE_UNQUOTED([DATADIR], "${DATADIR}", [Path to read only application speciffic data files.])
LIBEXECDIR=`eval echo ${libexecdir}`
AC_DEFINE_UNQUOTED([LIBEXECDIR], "${LIBEXECDIR}", [The directory for installing executable programs to be run by other programs rather than by users.])
LIBDIR=`eval echo ${libdir}`
AC_DEFINE_UNQUOTED([LIBDIR], "${LIBDIR}", [The directory for object files and libraries of object code.])

