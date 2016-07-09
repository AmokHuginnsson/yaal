#ifndef YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED
#define YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/numeric.hxx"

typedef int rusage_who_t;
typedef int rlimit_resource_t;
typedef yaal::i64_t rlim_t;
static rusage_who_t RUSAGE_SELF = 0;
static rlimit_resource_t RLIMIT_DATA = 2;
static rlimit_resource_t RLIMIT_STACK = 3;
static rlimit_resource_t RLIMIT_NOFILE = 8;
static rlim_t RLIM_INFINITY = yaal::meta::max_signed<rlim_t>::value;

struct rusage {
	int long ru_maxrss;
};

struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

int getrusage( rusage_who_t, struct rusage* );
int getrlimit( rlimit_resource_t, struct rlimit* );

#endif /* not YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED */
