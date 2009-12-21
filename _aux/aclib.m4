dnl YAAL_DETECT_FLAGS(RESULT, FLAGSET)
dnl Detect if the compiler supports a set of flags
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_DETECT_FLAGS],
[
	ORIG=[$]$1
	$1=
	AC_LANG_PUSH(ifelse( "x$3", "xC++", $3, C ))
	define([FLAGS], ifelse( "x$3", "xC++", CXXFLAGS, CFLAGS ))
	define([COMPILER], ifelse( "x$3", "xC++", C++, C ))
	for flag in $2; do
		if test -z "[$]$1"; then
			$1_save_FLAGS="$FLAGS"
			FLAGS="$FLAGS $flag -Werror"
			AC_MSG_CHECKING([whether COMPILER compiler understands [$]flag])
			AC_LINK_IFELSE([int main( int, char** ) { return ( 0 ); }], [$1_works=yes], [$1_works=no])
			AC_MSG_RESULT([$]$1_works)
			FLAGS="[$]$1_save_FLAGS"
			if test "x[$]$1_works" = "xyes"; then
				$1="$ORIG $flag"
			fi
		fi
	done
	undefine([COMPILER],[FLAGS])
	AC_LANG_POP(ifelse( "x$3", "xC++", $3, C ))
])


dnl Checks for which function macros exist
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_DETECT_FUNCTION_MACRO],
[
	AC_MSG_CHECKING(whether $CC implements __PRETTY_FUNCTION__)
	AC_CACHE_VAL(yaal_cv_have_func,
							 [AC_COMPILE_IFELSE([#include <stdio.h>
int main( int, char** ){ printf("%s", __PRETTY_FUNCTION__); return ( 0 );}],
							 yaal_cv_have_func=yes, yaal_cv_have_func=no)])
	AC_MSG_RESULT($yaal_cv_have_func)
	if test "$yaal_cv_have_func" = yes; then
		AC_DEFINE(HAVE_PRETTY_FUNCTION,[],[Does the compiler understand __PRETTY_FUNCTION__])
	else
		AC_MSG_CHECKING(whether $CC implements __FUNCTION__)
		AC_CACHE_VAL(yaal_cv_have_pretty_function,
								 [AC_COMPILE_IFELSE([#include <stdio.h>
int main( int, char** ){ printf("%s", __FUNCTION__); return ( 0 );}],
								 yaal_cv_have_pretty_function=yes, yaal_cv_have_pretty_function=no)])
		AC_MSG_RESULT($yaal_cv_have_pretty_function)
		if test "$yaal_cv_have_pretty_function" = yes; then
			AC_DEFINE(HAVE_FUNCTION,[],[Does compiler understand __FUNCTION__])
		else
			AC_MSG_CHECKING(whether $CC implements __func__)
			AC_CACHE_VAL(yaal_cv_have_function,
									 [AC_COMPILE_IFELSE([#include <stdio.h>
int main( int, char** ){ printf("%s", __func__); return ( 0 );}],
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
AC_DEFUN([YAAL_DETECT_OPERATING_SYSTEM],
[
	AC_MSG_CHECKING([host operating system])
	HOST_OS_TYPE=""
	LIB_PREFIX=["lib"]
	LIB_EXT=['"so"']
	SYMBOL_PREFIX=['""']
	if test ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/debconf.conf] ; then
		AC_DEFINE([__HOST_OS_TYPE_DEBIAN__], [], [Your operating system is Debian.])
		HOST_OS_TYPE=[Debian]
		SPECIAL_LINKER_FLAGS=["-Wl,--entry=\"yaal_\$(NAME)_main\""]
	fi
	if test ["x${HOST_OS_TYPE}"] = ["x"] ; then
		if test -f [/etc/rc] ; then
			AC_DEFINE([__HOST_OS_TYPE_FREEBSD__], [], [Your operating system is FreeBSD.])
			HOST_OS_TYPE=[FreeBSD]
		else
			AC_SUBST([SERIAL_DEVICE],['ttyS0'])
		fi
	fi
	if test ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/poldek/poldek.conf] ; then
		AC_DEFINE([__HOST_OS_TYPE_PLD__], [], [Your operating system is PLD.])
		HOST_OS_TYPE=[PLD]
		SPECIAL_LINKER_FLAGS=["-Wl,--entry=\"yaal_\$(NAME)_main\""]
	fi
	if test ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/random-seed] ; then
		AC_DEFINE([__HOST_OS_TYPE_SLACKWARE__], [], [Your operating system is Slackware.])
		HOST_OS_TYPE=[Slackware]
		SPECIAL_LINKER_FLAGS=["-Wl,--entry=\"yaal_\$(NAME)_main\""]
	fi
	if test ["x${HOST_OS_TYPE}"] = ["x"] -a -f [/etc/dfs/dfstab] ; then
		AC_DEFINE([__HOST_OS_TYPE_SOLARIS__], [], [Your operating system is Solaris.])
		HOST_OS_TYPE=[Solaris]
	fi
	if test ["x${HOST_OS_TYPE}"] = ["x"] -a -d [c:/windows] ; then
		AC_DEFINE([__HOST_OS_TYPE_WINDOWS__], [], [Your operating system is MS Windows.])
		AC_SUBST([SYMBOL_PREFIX],[_])
		SYMBOL_PREFIX=['"_"']
		SPECIAL_LINKER_FLAGS=["-shared -Wl,--export-all-symbols -Wl,--enable-auto-import -Wl,--out-implib=lib\$(NAME).\$(LIB_ARCHIVE_SUFFIX)"]
		TARGET_LINKER_FLAGS=["-Wl\$(COMA)--enable-auto-import"]
		HOST_OS_TYPE=[Windows]
		LIB_PREFIX=["cyg"]
		LIB_EXT=['"dll"']
		TOOLS_LIBS=["${TOOLS_LIBS} -liconv"]
	fi
	AC_DEFINE_UNQUOTED([SYMBOL_PREFIX],${SYMBOL_PREFIX},[Symbol prefix used on this platform.])
	AC_DEFINE_UNQUOTED([LIB_PREFIX],"${LIB_PREFIX}",[Dynamic library file name prefix used on this platform.])
	AC_DEFINE_UNQUOTED([LIB_EXT],${LIB_EXT},[Dynamic library file name extension used on this platform.])

	if test ["x${HOST_OS_TYPE}"] = ["x"] ; then
		AC_MSG_ERROR([Cannot recognize host operating system type!])
	else
		AC_MSG_RESULT([$HOST_OS_TYPE])
	fi
])


dnl Detect amount of physical memory on this system.
dnl --------------------------------------------------------------------------
AC_DEFUN([YAAL_DETECT_PHYSICAL_MEMORY],
[
	AC_MSG_CHECKING([for amount for physical memory])
	if test ["x${HOST_OS_TYPE}"] = ["x"] ; then
		AC_MSG_ERROR([[You need to use YAAL_DETECT_OPERATING_SYSTEM first!]])
	fi
	PHYS_MEM="0"
	case "x${HOST_OS_TYPE}" in
		xDebian|xPLD|xSlackware)
			PHYS_MEM=`free -m | awk '/^Mem:/{print [$]2}'`
		;;
		xFreeBSD)
		;;
		xSolaris)
		;;
		xWindows)
		;;
	esac
	AC_DEFINE_UNQUOTED([__PHYSICAL_MEMORY__], ${PHYS_MEM}, [Amount of physical memory on this system.])
	AC_MSG_RESULT([${PHYS_MEM}])
])

