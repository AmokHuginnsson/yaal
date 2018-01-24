#! /bin/sh

# Set cross compilation build environment.

if [ "x${1}" == "x" ] ; then
	echo "You must specify toolchain directory with TOOLCHAIN={dir}."
	return
fi

TOOLCHAIN=`echo "${1}" | awk -F '=' '{print $1}'`

if [ "x${TOOLCHAIN}" != "xTOOLCHAIN" ] ; then
	echo "First argument must be TOOLCHAIN."
	return
fi

TOOLCHAIN=`echo "${1}" | awk -F '=' '{OFS="";$1 = "";print}'`

if [ -z "${TOOLCHAIN}" ] ; then
	echo "TOOLCHAIN needs to have a value."
	return
fi

if [ ! -d "${TOOLCHAIN}" ] ; then
	echo "TOOLCHAIN directory must exist."
	return
fi

if [ "x${2}" == "x" ] ; then
	export SYSROOT="${TOOLCHAIN}/sysroot"
else
	SYSROOT=`echo "${2}" | awk -F '=' '{print $1}'`

	if [ "x${SYSROOT}" != "xSYSROOT" ] ; then
		echo "Second argument must be SYSROOT."
		return
	fi

	SYSROOT=`echo "${2}" | awk -F '=' '{OFS="";$1 = "";print}'`

	if [ -z "${SYSROOT}" ] ; then
		echo "SYSROOT needs to have a value."
		return
	fi

	if [ ! -d "${SYSROOT}" ] ; then
		echo "SYSROOT directory must exist."
		return
	fi
fi

# set path = ( "${TOOLCHAIN}/bin" ${path} )

HOST_TRIPLET=`find "${TOOLCHAIN}" -path '*/bin/*-cpp' | xargs basename | sed -e 's/-cpp//'`

echo "TOOLCHAIN = ${TOOLCHAIN}"
echo "SYSROOT = ${SYSROOT}"
echo "HOST_TRIPLET = ${HOST_TRIPLET}"

unset PKG_CONFIG_DIR
export PKG_CONFIG_LIBDIR="${SYSROOT}/usr/lib/${HOST_TRIPLET}/pkgconfig:${SYSROOT}/usr/lib/pkgconfig:${SYSROOT}/usr/share/pkgconfig"
export PKG_CONFIG_SYSROOT_DIR="${SYSROOT}"
export CONFIGURE="--host=${HOST_TRIPLET}"
export EXTRA_CXXFLAGS="--sysroot=${SYSROOT}"
export EXTRA_LXXFLAGS="--sysroot=${SYSROOT}"
unset CC
unset CXX

