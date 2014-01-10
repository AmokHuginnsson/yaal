dnl Bail out if running as root.
dnl --------------------------------------------------------------------------
AC_DEFUN([PRIVILEGES_SANITY], [
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
AC_DEFUN([YAAL_CXX_STANDARD_CHECK], [
	CXX_STANDARD_NO=2003
	if test ["x$CXX_2011"] = ["xyes"] ; then
		YAAL_DETECT_FLAGS(CXX_STANDARD, [-std=c++11], [C++])
		if test ["x$CXX_STANDARD"] != ["x"] ; then
			AC_SUBST( [CXX_STANDARD], ["c++11"] )
			CXX_STANDARD_NO=2011
			CXXFLAGS=["$CXXFLAGS -std=c++11"]
		else
			AC_MSG_ERROR([Requested C++ Standard version is not available in this environment!])
		fi
	else
		AC_SUBST( [CXX_STANDARD], ["gnu++98"] )
	fi
	AC_DEFINE_UNQUOTED([CXX_STANDARD], $CXX_STANDARD_NO, [Version of C++ standard used.])
])

dnl Checks for which function macros exist
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_DETECT_FUNCTION_MACRO], [
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
AC_DEFUN([YAAL_DETECT_OPERATING_SYSTEM], [
	AC_MSG_CHECKING([host operating system])
	AC_CANONICAL_HOST
	HOST_OS_TYPE=""
	LIB_PREFIX=["lib"]
	LIB_EXT=['"so"']
	EXE_SUFFIX=[""]
	SYMBOL_PREFIX=['""']
	AC_CHECK_PROG([LINUX_DISTRO],[lsb_release],[yes])
	if test ["x${LINUX_DISTRO}"] = ["xyes"] ; then
		HOST_OS_TYPE=`lsb_release -i|awk '/Distributor/{print [$]3}'`
		AC_SUBST([SERIAL_DEVICE],['ttyS0'])
	fi
	if test ["x${HOST_OS_TYPE}"] = ["xDebian"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/debconf.conf] \) ; then
		AC_DEFINE([__HOST_OS_TYPE_LINUX__], [], [Your operating system is Linux.])
		AC_DEFINE([__HOST_OS_TYPE_DEBIAN__], [], [Your specific linux version is Debian.])
		HOST_OS_TYPE=[Debian]
		YAAL_LXXFLAGS=["-Wl,--entry=\"${PACKAGE_NAME}_\$(*)_main\""]
	elif test ["x${HOST_OS_TYPE}"] = ["xUbuntu"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/00-header] \) ; then
		AC_DEFINE([__HOST_OS_TYPE_LINUX__], [], [Your operating system is Linux.])
		AC_DEFINE([__HOST_OS_TYPE_UBUNTU__], [], [Your specific linux version is Ubuntu.])
		HOST_OS_TYPE=[Ubuntu]
		YAAL_LXXFLAGS=["-Wl,--entry=\"${PACKAGE_NAME}_\$(*)_main\""]
	elif test ["x${HOST_OS_TYPE}"] = ["xCentOS"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/yum.repos.d/CentOS-Base.repo] \) ; then
		AC_DEFINE([__HOST_OS_TYPE_LINUX__], [], [Your operating system is Linux.])
		AC_DEFINE([__HOST_OS_TYPE_CENTOS__], [], [Your specific linux version is CentOS.])
		HOST_OS_TYPE=[CentOS]
		YAAL_LXXFLAGS=["-Wl,--entry=\"${PACKAGE_NAME}_\$(*)_main\""]
	elif test ["x${HOST_OS_TYPE}"] = ["xPLD"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/poldek/poldek.conf] \) ; then
		AC_DEFINE([__HOST_OS_TYPE_LINUX__], [], [Your operating system is Linux.])
		AC_DEFINE([__HOST_OS_TYPE_PLD__], [], [Your specific linux version is PLD.])
		HOST_OS_TYPE=[PLD]
		YAAL_LXXFLAGS=["-Wl,--entry=\"${PACKAGE_NAME}_\$(*)_main\""]
	elif test ["x${HOST_OS_TYPE}"] = ["xSlackware"] -o \( ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/random-seed] \) ; then
		AC_DEFINE([__HOST_OS_TYPE_LINUX__], [], [Your operating system is Linux.])
		AC_DEFINE([__HOST_OS_TYPE_SLACKWARE__], [], [Your specific linux version is Slackware.])
		HOST_OS_TYPE=[Slackware]
		YAAL_LXXFLAGS=["-Wl,--entry=\"${PACKAGE_NAME}_\$(*)_main\""]
	elif test ["x${HOST_OS_TYPE}"] != ["x"] ; then
		HOST_OS_TYPE=[Linux]
		AC_DEFINE([__HOST_OS_TYPE_LINUX__], [], [Your operating system is Linux.])
		YAAL_LXXFLAGS=["-Wl,--entry=\"${PACKAGE_NAME}_\$(*)_main\""]
	elif test ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/rc] ; then
		AC_DEFINE([__HOST_OS_TYPE_FREEBSD__], [], [Your operating system is FreeBSD.])
		HOST_OS_TYPE=[FreeBSD]
	elif test ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/dfs/dfstab] ; then
		AC_DEFINE([__HOST_OS_TYPE_SOLARIS__], [], [Your operating system is Solaris.])
		HOST_OS_TYPE=[Solaris]
	elif test ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/tizen-release] ; then
		AC_DEFINE([__HOST_OS_TYPE_TIZEN__], [], [Your operating system is Tizen.])
		HOST_OS_TYPE=[Tizen]
	elif test ["x${HOST_OS_TYPE}"] = ["x"] -a -d [c:/windows] ; then
		AC_DEFINE([__HOST_OS_TYPE_CYGWIN__], [], [Your operating system is Cygwin.])
		YAAL_LXXFLAGS=["${YAAL_LXXFLAGS} -Wl,--export-all-symbols -Wl,--out-implib=lib\$(*).\$(LIB_ARCHIVE_SUFFIX)"]
		HOST_OS_TYPE=[Cygwin]
		LIB_PREFIX=["cyg"]
		LIB_EXT=['"dll"']
		TOOLS_LIBS=["${TOOLS_LIBS} -liconv"]
	fi
	AC_DEFINE_UNQUOTED([SYMBOL_PREFIX],${SYMBOL_PREFIX},[Symbol prefix used on this platform.])
	AC_DEFINE_UNQUOTED([LIB_PREFIX],"${LIB_PREFIX}",[Dynamic library file name prefix used on this platform.])
	AC_DEFINE_UNQUOTED([LIB_INFIX],"${LIB_INFIX}",[Target dependent library name infix.])
	AC_DEFINE_UNQUOTED([LIB_EXT],${LIB_EXT},[Dynamic library file name extension used on this platform.])
	AC_DEFINE_UNQUOTED([EXE_SUFFIX],${EXE_SUFFIX},[Suffix used for binary program executable on this platform.])

	if test ["x${HOST_OS_TYPE}"] = ["x"] ; then
		AC_MSG_ERROR([Cannot recognize host operating system type!])
	else
		AC_MSG_RESULT([Your system type is $HOST_OS_TYPE.])
	fi
])


dnl Detect amount of physical memory on this system.
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_DETECT_PHYSICAL_MEMORY], [
	AC_MSG_CHECKING([for amount for physical memory])
	if test ["x${HOST_OS_TYPE}"] = ["x"] ; then
		AC_MSG_ERROR([[You need to use YAAL_DETECT_OPERATING_SYSTEM first!]])
	fi
	PHYS_MEM="0"
	case "x${HOST_OS_TYPE}" in
		xDebian|xUbuntu|xCentOS|xPLD|xSlackware|xLinux|xCygwin)
			PHYS_MEM=`free -m | awk '/^Mem:/{print [$]2}'`
		;;
		xFreeBSD)
			PHYS_MEM=`/sbin/sysctl hw.physmem | awk '{print int( [$]2 / 1024 / 1024 )}'`
		;;
		xSolaris)
			PHYS_MEM=`prtconf | awk '/Memory/{print [$]3}'`
		;;
	esac
	AC_DEFINE_UNQUOTED([__PHYSICAL_MEMORY__], ${PHYS_MEM}, [Amount of physical memory on this system.])
	AC_MSG_RESULT([${PHYS_MEM}])
])


dnl Check available git features.
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_CHECK_GIT], [
	PACKAGE_VCS=""
	AC_CHECK_PROG(HAS_GIT,[git],["yes"],["no"])
	AC_MSG_CHECKING([git id sub-command])
	AC_SUBST(GITID,[true])
	if test ["$HAS_GIT"] = ["yes"] ; then
		if test -d "${srcdir}/.git" ; then
			THIS_ID=`git id ${0}.ac 2> /dev/null`
			if test ["x${THIS_ID}"] != ["x"] ; then
				AC_SUBST(GITID,["git id"])
				AC_MSG_RESULT([yes])
			else
				AC_SUBST(GITID,["git log --no-color -n 1 --pretty=format:\"%H %ad %an\""])
				AC_MSG_RESULT([no])
			fi
			PACKAGE_VCS=`git rev-parse HEAD`
		else
			AC_MSG_WARN([not a git hosted project])
		fi
	else
		AC_MSG_WARN([no git in the first place])
	fi
	AC_DEFINE_UNQUOTED([PACKAGE_VCS], "${PACKAGE_VCS}", [Version Control System current commit id.])
])


dnl YAAL_CHECK_COMPILER_VERSION
dnl Check compiler version.
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_CHECK_COMPILER_VERSION], [
	AC_MSG_CHECKING([compiler version])
	GCC_MAJOR=`echo | cpp -dM | grep __GNUC__ | awk '{print [$]3}'`
	GCC_MINOR=`echo | cpp -dM | grep __GNUC_MINOR__ | awk '{print [$]3}'`
	AC_MSG_RESULT([major $GCC_MAJOR, minor $GCC_MINOR. ])
])

dnl YAAL_DETECT_COMMON_FLAGS
dnl What special compiler flags we can set?
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_DETECT_COMMON_FLAGS], [
	YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-pthread], [C++])
	YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-m64], [C++])
	YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-fPIC], [C++])
	YAAL_DETECT_FLAGS(RDYNAMIC, [-rdynamic], [C++])
	YAAL_DETECT_FLAGS(EXTRA_CXXFLAGS, [-Wmissing-declarations], [C++])
	YAAL_DETECT_FLAGS(FATAL_WARNINGS, [-Wl,--fatal-warnings], [C++])
	EXTRA_LXXFLAGS="${EXTRA_LXXFLAGS} ${FATAL_WARNINGS}"
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-m64], [C++])
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,--demangle], [C++])
	YAAL_DETECT_FLAGS(EXTRA_LXXFLAGS, [-Wl,-C], [C++])
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

	_FLAGS=''
	CXXFLAGS_ORIG=$CXXFLAGS;
	CPPFLAGS_ORIG=$CPPFLAGS;
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
	CXXFLAGS=$CXXFLAGS_ORIG
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
	CXXFLAGS=$CXXFLAGS_ORIG;
	CPPFLAGS=$CXXFLAGS_ORIG;
])

AC_DEFINE_UNQUOTED([SYSCONFDIR], "$sysconfdir", [Path to global system configuration directory.])

