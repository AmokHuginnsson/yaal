### ! /bin/sh

clean() {
	if [ ${#} -ne 1 -o ! -d "${1}" ] ; then
		echo "bad arg for clean() call."
		return 1
	fi
	find "$1" -type f \( \
							-name '*~' \
					-or -name 'a.out' \
					-or -name '.*.sw[n-p]' \
					-or -name '*.o' \
					-or -name '*.oo' \
					-or -name '.*~' \
					-or -name 'gmon.out' \
					-or -name 'core' \
					-or -name '*.core' \
					-or -name '*.core.*' \
					-or -name '~=+*=' \
					-or -name '=+*=' \) -print0 \
			| xargs -0 /bin/rm -f
	return 0
}

# vim: ft=sh
