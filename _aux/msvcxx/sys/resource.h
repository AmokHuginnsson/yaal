#ifndef YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED
#define YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED 1

typedef int rusage_who_t;
static int const RUSAGE_SELF = 0;

struct rusage {
	int long ru_maxrss;
};

#endif /* not YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED */

