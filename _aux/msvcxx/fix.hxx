#ifndef YAAL_MSVCXX_FIX_HXX_INCLUDED
#define YAAL_MSVCXX_FIX_HXX_INCLUDED 1

#pragma warning( disable : 4068 )
#pragma warning( disable : 4146 )
#pragma warning( disable : 4180 )
#pragma warning( disable : 4221 )
#pragma warning( disable : 4250 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4275 )
#pragma warning( disable : 4290 )
#pragma warning( disable : 4309 )
#pragma warning( disable : 4345 )
#pragma warning( disable : 4351 )
#pragma warning( disable : 4355 )
#pragma warning( disable : 4503 )
#pragma warning( disable : 4646 )
#pragma warning( disable : 4996 )

#ifdef __YAAL_BUILD__

#define _inline _inlineMSVCXX
#define _try _tryMSVCXX
#define _TIMESPEC_DEFINED 1
#define _USE_32BIT_TIME_T 1
#define __sync_synchronize _mm_mfence

/* warning LNK4221 goes bye bye */
namespace { char dummy_symbol_to_quiet_down_LNK4221_warning = 0; };

#define log hide_log_from_global_namespace
#include <cmath>
#undef log

#else /* #ifdef __YAAL_BUILD__ */

#define _restrict m_restrict0
#define restrict_t restrict_t0

typedef void* timer_t;

#endif /* #else #ifdef __YAAL_BUILD__ */

#endif /* not YAAL_MSVCXX_FIX_HXX_INCLUDED */

