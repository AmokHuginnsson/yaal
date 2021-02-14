#! /bin/sh

# Set cross compilation build environment.

if [ "x${1}" == "x" ] ; then
	echo "You must specify toolchain directory with toolchain={dir}."
	return
fi

toolchain="$(echo "${1}" | awk -F '=' '{print $1}')"

if [ "x${toolchain}" != "xtoolchain" ] ; then
	echo "First argument must be toolchain."
	return
fi

toolchain="$(echo "${1}" | awk -F '=' '{OFS="";$1 = "";print}')"

if [ -z "${toolchain}" ] ; then
	echo "toolchain needs to have a value."
	return
fi

if [ ! -d "${toolchain}" ] ; then
	echo "toolchain directory must exist."
	return
fi

if [ "x${2}" == "x" ] ; then
	sysroot="${toolchain}/sysroot"
else
	sysroot="$(echo "${2}" | awk -F '=' '{print $1}')"

	if [ "x${sysroot}" != "xsysroot" ] ; then
		echo "Second argument must be sysroot."
		return
	fi

	sysroot="$(echo "${2}" | awk -F '=' '{OFS="";$1 = "";print}')"

	if [ -z "${sysroot}" ] ; then
		echo "sysroot needs to have a value."
		return
	fi

	if [ ! -d "${sysroot}" ] ; then
		echo "sysroot directory must exist."
		return
	fi
fi

# set path = ( "${toolchain}/bin" ${path} )

hostTriplet="$(find "${toolchain}" -path '*/bin/*-cpp' | xargs basename | sed -e 's/-cpp//')"

echo "toolchain = ${toolchain}"
echo "sysroot = ${sysroot}"
echo "hostTriplet = ${hostTriplet}"

unset PKG_CONFIG_DIR
export PKG_CONFIG_LIBDIR="${sysroot}/usr/lib/${hostTriplet}/pkgconfig:${sysroot}/usr/lib/pkgconfig:${sysroot}/usr/share/pkgconfig"
export PKG_CONFIG_SYSROOT_DIR="${sysroot}"
export CONFIGURE="--host=${hostTriplet}"
export EXTRA_CXXFLAGS="--sysroot=${sysroot}"
export EXTRA_LXXFLAGS="--sysroot=${sysroot}"
export SYSROOT="${sysroot}"
unset CC
unset CXX

