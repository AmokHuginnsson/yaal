### ! /bin/sh

PHYS_MEM=0
OSTYPE=`uname`

case "${OSTYPE}" in
	*Linux)
		PHYS_MEM=`free -m | awk '/^Mem:/{print $2}'`
	;;
	FreeBSD)
		PHYS_MEM=`/sbin/sysctl hw.physmem | awk '{print int( $2 / 1024 / 1024 )}'`
	;;
	Solaris|SunOS)
		PHYS_MEM=`prtconf | awk '/Memory/{print $3}'`
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
fi

# vim: ft=sh
