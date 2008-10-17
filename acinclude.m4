dnl YAAL_DETECT_FLAGS(RESULT, FLAGSET)
dnl Detect if the compiler supports a set of flags

AC_DEFUN([YAAL_DETECT_FLAGS],
[
	$1=
	if test "x$3" = "xC++" ; then
		AC_LANG_PUSH($3)
		define(FLAGS,CXXFLAGS)
		define(COMPILER,C++ compiler)
	else
		true
		define(FLAGS,CFLAGS)
		define(COMPILER,C compiler)
	fi
	for flag in $2; do
		if test -z "[$]$1"; then
			$1_save_FLAGS="$FLAGS"
			FLAGS="$FLAGS $flag"
			AC_MSG_CHECKING([whether COMPILER understands [$]flag])
			AC_COMPILE_IFELSE([ ], [$1_works=yes], [$1_works=no])
			AC_MSG_RESULT([$]$1_works)
			FLAGS="[$]$1_save_FLAGS"
			if test "x[$]$1_works" = "xyes"; then
				$1="$flag"
			fi
		fi
	done
	if test "x$3" = "xC++" ; then
		AC_LANG_POP($3)
	fi
])

