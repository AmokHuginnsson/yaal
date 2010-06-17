#ifndef YAAL_MSVCXX_FIX_HXX_INCLUDED
#define YAAL_MSVCXX_FIX_HXX_INCLUDED 1

#define _USE_32BIT_TIME_T 1
#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#define ip_opts CONCATENATE( ip_opts, __COUNTER__ )
#define unlink CONCATENATE( unlink, __COUNTER__ )
#define unlink1 unlink

#define swab CONCATENATE( swab, __COUNTER__ )
#define swab0 swab

#endif /* not YAAL_MSVCXX_FIX_HXX_INCLUDED */

