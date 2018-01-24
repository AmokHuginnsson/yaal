#! /bin/tcsh

# Set cross compilation build environment.

if ( "x${1}" == "x" ) then
	echo "You must specify toolchain directory with TOOLCHAIN={dir}."
	exit
endif

set TOOLCHAIN = ( ${1:s/=/ /} )

if ( "x${TOOLCHAIN[1]}" != "xTOOLCHAIN" ) then
	echo "First argument must be TOOLCHAIN."
	exit
endif

if ( ${#TOOLCHAIN} < 2 ) then
	echo "TOOLCHAIN needs to have a value."
	exit
endif

set TOOLCHAIN = "${TOOLCHAIN[2]}"

if ( ! -d "${TOOLCHAIN}" ) then
	echo "TOOLCHAIN directory must exist."
	exit
endif

if ( "x${2}" == "x" ) then
	setenv SYSROOT "${TOOLCHAIN}/sysroot"
else
	set SYSROOT = ( ${2:s/=/ /} )

	if ( "x${SYSROOT[1]}" != "xSYSROOT" ) then
		echo "Second argument must be SYSROOT."
		exit
	endif

	if ( ${#SYSROOT} < 2 ) then
		echo "SYSROOT needs to have a value."
		exit
	endif

	set SYSROOT = "${SYSROOT[2]}"

	if ( ! -d "${SYSROOT}" ) then
		echo "SYSROOT directory must exist."
		exit
	endif
endif

# set path = ( "${TOOLCHAIN}/bin" ${path} )

set HOST_TRIPLET=`find "${TOOLCHAIN}" -path '*/bin/*-cpp'`
set HOST_TRIPLET=( ${HOST_TRIPLET:t:as/-/ /} )
set HOST_TRIPLET="${HOST_TRIPLET[1-3]}"
set HOST_TRIPLET="${HOST_TRIPLET:as/ /-/}"

echo "TOOLCHAIN = ${TOOLCHAIN}"
echo "SYSROOT = ${SYSROOT}"
echo "HOST_TRIPLET = ${HOST_TRIPLET}"

unsetenv PKG_CONFIG_DIR
unset PKG_CONFIG_DIR
setenv PKG_CONFIG_LIBDIR "${SYSROOT}/usr/lib/${HOST_TRIPLET}/pkgconfig:${SYSROOT}/usr/lib/pkgconfig:${SYSROOT}/usr/share/pkgconfig"
setenv PKG_CONFIG_SYSROOT_DIR "${SYSROOT}"
setenv CONFIGURE "--host=${HOST_TRIPLET}"
setenv EXTRA_CXXFLAGS "--sysroot=${SYSROOT}"
setenv EXTRA_LXXFLAGS "--sysroot=${SYSROOT}"
unsetenv CC
unsetenv CXX

