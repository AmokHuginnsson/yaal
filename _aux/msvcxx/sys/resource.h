#ifndef YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED
#define YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/numeric.hxx"

typedef int rusage_who_t;
typedef int rlimit_resource_t;
typedef yaal::i64_t rlim_t;
static rusage_who_t const RUSAGE_SELF = 0;
static rlimit_resource_t const RLIMIT_CPU    = 0;
static rlimit_resource_t const RLIMIT_FSIZE  = 1;
static rlimit_resource_t const RLIMIT_DATA   = 2;
static rlimit_resource_t const RLIMIT_STACK  = 3;
static rlimit_resource_t const RLIMIT_CORE   = 4;
static rlimit_resource_t const RLIMIT_NOFILE = 7;
static rlim_t const RLIM_INFINITY = yaal::meta::max_signed<rlim_t>::value;

struct rusage {
	int long ru_maxrss;
};

struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

int getrusage( rusage_who_t, struct rusage* );
int getrlimit( rlimit_resource_t, struct rlimit* );
int setrlimit( rlimit_resource_t, struct rlimit const* );

#ifdef YAAL_MSVCXX_SYS_STATVFS_H_INCLUDED
#define close M_CONCAT( close, __COUNTER__ )
#define close1 close
#define close2 close
#define close3 msvcxx::close
#endif /* #ifdef YAAL_MSVCXX_SYS_STATVFS_H_INCLUDED */

#endif /* not YAAL_MSVCXX_SYS_RESOURCE_H_INCLUDED */
