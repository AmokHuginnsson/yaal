dnl YAAL_DETECT_FLAGS(RESULT, FLAGSET)
dnl Detect if the compiler supports a set of flags

AC_DEFUN([YAAL_DETECT_FLAGS],
[
	$1=
	AC_LANG_PUSH(ifelse( "x$3", "xC++", $3, C ))
	define(FLAGS, ifelse( "x$3", "xC++", CXXFLAGS, CFLAGS ))
	define(COMPILER, ifelse( "x$3", "xC++", C++, C ))
	for flag in $2; do
		if test -z "[$]$1"; then
			$1_save_FLAGS="$FLAGS"
			FLAGS="$FLAGS $flag"
			AC_MSG_CHECKING([whether COMPILER compiler understands [$]flag])
			AC_COMPILE_IFELSE([ ], [$1_works=yes], [$1_works=no])
			AC_MSG_RESULT([$]$1_works)
			FLAGS="[$]$1_save_FLAGS"
			if test "x[$]$1_works" = "xyes"; then
				$1="$flag"
			fi
		fi
	done
	AC_LANG_POP(ifelse( "x$3", "xC++", $3, C ))
])

