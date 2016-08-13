setenv LIBRARY_PATH {YAAL_PREFIX}/lib
setenv C_INCLUDE_PATH {YAAL_PREFIX}/include
setenv CPLUS_INCLUDE_PATH {YAAL_PREFIX}/include
setenv OBJC_INCLUDE_PATH {YAAL_PREFIX}/include
setenv LD_LIBRARY_PATH {YAAL_PREFIX}/lib
set path=( "{YAAL_PREFIX}/bin" ${path} )

set PHYS_MEM=0

switch ( "x${OSTYPE}" )
	case xlinux:
		set PHYS_MEM=`free -m | awk '/^Mem:/{print $2}'`
	breaksw
	case xFreeBSD:
		set PHYS_MEM=`/sbin/sysctl -n hw.physmem | awk '{print int( $1 / 1024 / 1024 )}'`
	breaksw
	case xsolaris:
		set PHYS_MEM=`prtconf | awk '/Memory/{print $3}'`
	breaksw
	case xdarwin:
		set PHYS_MEM=`/usr/sbin/sysctl -n hw.memsize | awk '{print int( $1 / 1024 / 1024 )}'`
	breaksw
endsw

limit coredumpsize unlimited
limit stacksize 8192k
set HAS_MAXPROC=`limit | awk '/maxproc/{print $1}'`
if ( "x${HAS_MAXPROC}" != "x" ) limit maxproc 400
unset HAS_MAXPROC
if ( ${PHYS_MEM} != 0 ) then
	if ( "x`limit | awk '/^memoryuse/{print $1}'`" != "x" ) limit memoryuse ${PHYS_MEM}m
	set MEMORYUSE='memoryuse'
	if ( "x`limit | awk '/^vmemoryuse/{print $1}'`" != "x" ) then
		limit vmemoryuse ${PHYS_MEM}m
		set MEMORYUSE='vmemoryuse'
	endif
	limit datasize ${PHYS_MEM}m
	if ( ( ${PHYS_MEM} > 4095 ) && ( "x`limit ${MEMORYUSE} | awk '{print \$2}'`" == "xunlimited" ) ) then
		limit ${MEMORYUSE} 4095m
		limit datasize 4095m
	endif
	unset LIMITED
	unset MEMORYUSE
endif
unset PHYS_MEM

umask 0077
