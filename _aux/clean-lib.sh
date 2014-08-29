### ! /bin/sh

clean() {
	if [ ${#} -ne 1 -o ! -d "${1}" ] ; then
		echo "bad arg for clean() call."
		return 1
	fi
	find "$1" -type f \( \
						-name '*~' \
					-o -name 'a.out' \
					-o -name '.*.sw[n-p]' \
					-o -name '*.o' \
					-o -name '*.oo' \
					-o -name '.*~' \
					-o -name 'gmon.out' \
					-o -name 'core' \
					-o -name '*.core' \
					-o -name '*.core.*' \
					-o -name '~=+*=' \
					-o -name '=+*=' \) -print0 \
			| xargs -0 /bin/rm -f
	return 0
}

# vim: ft=sh
