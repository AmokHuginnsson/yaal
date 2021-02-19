### ! /bin/sh

set -ue

physMem=0
osType="$(uname -s)"

case "x${osType}" in
	x*Linux)
		physMem="$(free -m | awk '/^Mem:/{print $2}')"
	;;
	xFreeBSD)
		physMem="$(/sbin/sysctl -n hw.physmem | awk '{print int( $1 / 1024 / 1024 )}')"
	;;
	xSolaris|xSunOS)
		physMem="$(prtconf | awk '/Memory/{print $3}')"
	;;
	xDarwin)
		physMem="$(/usr/sbin/sysctl -n hw.memsize | awk '{print int( $1 / 1024 / 1024 )}')"
	;;
esac

physMem="$(expr "${physMem}" \* 1024)"

ulimit -c unlimited

if [ -z "${procLimit+x}" ] ; then
	procLimit=2000
fi

# bash proc limit is set with -u, dash proc limit is set with -p
isBash="$(ulimit -a | grep 'max user processes' || true)"

if [ "x${isBash}" != "x" ] ; then
	ulimit -u ${procLimit} > /dev/null 2>&1 || true
else
	ulimit -p ${procLimit} > /dev/null 2>&1 || true
fi
ulimit -s 8192

limited="$(ulimit -v)"
if [ ${physMem} -ne 0 -a -z "${limited}" ] ; then
	if [ -z "${max32BitPhysMem}" ] ; then
		max32BitPhysMem="$(expr 4095 \* 1024)"
	fi
	limited="$(ulimit -v ${physMem} ; ulimit -v)"
	if [ \( ${physMem} -gt ${max32BitPhysMem} \) -a \( "x${limited}" = "xunlimited" \) ] ; then
		physMem=${max32BitPhysMem}
	fi
	ulimit -v ${physMem}
	ulimit -d ${physMem}
fi
max64BitPhysMem="$(expr 16 \* 1024 \* 1024 - 1)"
if [ ${physMem} -gt ${max64BitPhysMem} ] ; then
	physMem=${max64BitPhysMem}
	ulimit -v ${physMem} || test "x${osType}" = "xDarwin"
	ulimit -d ${physMem} || test "x${osType}" = "xDarwin"
fi

umask 0077

# vim: ft=sh
