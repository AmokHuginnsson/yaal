### ! /bin/sh

PHYS_MEM=0
OSTYPE=$(uname -s)

case "x${OSTYPE}" in
	x*Linux)
		PHYS_MEM=$(free -m | awk '/^Mem:/{print $2}')
	;;
	xFreeBSD)
		PHYS_MEM=$(/sbin/sysctl -n hw.physmem | awk '{print int( $1 / 1024 / 1024 )}')
	;;
	xSolaris|xSunOS)
		PHYS_MEM=$(prtconf | awk '/Memory/{print $3}')
	;;
	xDarwin)
		PHYS_MEM=$(/usr/sbin/sysctl -n hw.memsize | awk '{print int( $1 / 1024 / 1024 )}')
	;;
esac

PHYS_MEM=$(expr "${PHYS_MEM}" \* 1024)

ulimit -c unlimited

if [ -z "${PROC_LIMIT}" ] ; then
	PROC_LIMIT=400
fi

# bash proc limit is set with -u, dash proc limit is set with -p
IS_BASH=$(ulimit -a | grep 'max user processes')
if [ "x${IS_BASH}" != "x" ] ; then
	ulimit -u ${PROC_LIMIT} > /dev/null 2>&1 || true
else
	ulimit -p ${PROC_LIMIT} > /dev/null 2>&1 || true
fi
ulimit -s 8192

LIMITED=$(ulimit -v)
if [ ${PHYS_MEM} -ne 0 -a -z "${LIMITED}" ] ; then
	if [ -z "${MAX_32BIT_PHYS_MEM}" ] ; then
		MAX_32BIT_PHYS_MEM=$(expr 4095 \* 1024)
	fi
	LIMITED=$(ulimit -v ${PHYS_MEM} ; ulimit -v)
	if [ \( ${PHYS_MEM} -gt ${MAX_32BIT_PHYS_MEM} \) -a \( "x${LIMITED}" = "xunlimited" \) ] ; then
		PHYS_MEM=${MAX_32BIT_PHYS_MEM}
	fi
	ulimit -v ${PHYS_MEM}
	ulimit -d ${PHYS_MEM}
fi
MAX_64BIT_PHYS_MEM=$(expr 16 \* 1024 \* 1024 - 1)
if [ ${PHYS_MEM} -gt ${MAX_64BIT_PHYS_MEM} ] ; then
	PHYS_MEM=${MAX_64BIT_PHYS_MEM}
	ulimit -v ${PHYS_MEM}
	ulimit -d ${PHYS_MEM}
fi

# vim: ft=sh
