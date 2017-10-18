### ! /bin/sh

PHYS_MEM=0
OSTYPE=`uname -s`

case "x${OSTYPE}" in
	x*Linux)
		PHYS_MEM=`free -m | awk '/^Mem:/{print $2}'`
	;;
	xFreeBSD)
		PHYS_MEM=`/sbin/sysctl -n hw.physmem | awk '{print int( $1 / 1024 / 1024 )}'`
	;;
	xSolaris|xSunOS)
		PHYS_MEM=`prtconf | awk '/Memory/{print $3}'`
	;;
	xDarwin)
		PHYS_MEM=`/usr/sbin/sysctl -n hw.memsize | awk '{print int( $1 / 1024 / 1024 )}'`
	;;
esac

PHYS_MEM=`expr "${PHYS_MEM}" \* 1024`

ulimit -c unlimited

# bash proc limit is set with -u, dash proc limit is set with -p
IS_BASH=`ulimit -a | grep 'max user processes'`
if [ "x${IS_BASH}" != "x" ] ; then
	ulimit -u 400
else
	ulimit -p 400
fi
ulimit -s 8192
if [ ${PHYS_MEM} -ne 0 ] ; then
	ulimit -v ${PHYS_MEM}
	ulimit -d ${PHYS_MEM}
	LIMITED=`ulimit -v`
	MAX_32BIT_PHYS_MEM=`expr 4095 \* 1024`
	if [ \( ${PHYS_MEM} -gt ${MAX_32BIT_PHYS_MEM} \) -a \( "x${LIMITED}" = "xunlimited" \) ] ; then
		ulimit -v ${MAX_32BIT_PHYS_MEM}
		ulimit -d ${MAX_32BIT_PHYS_MEM}
	fi
fi

# vim: ft=sh
