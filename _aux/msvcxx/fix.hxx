#ifndef YAAL_MSVCXX_FIX_HXX_INCLUDED
#define YAAL_MSVCXX_FIX_HXX_INCLUDED 1

#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)

#ifdef _WIN64
#define __intptr_t_defined 1
#endif /* #ifdef _WIN64 */

#define __dev_t_defined 1
#define __ino_t_defined 1

#define _exit CONCATENATE( _exit, __COUNTER__ )
#define _exit1 _exit

#define swab CONCATENATE( swab, __COUNTER__ )
#define swab0 swab

#define _restrict m_restrict0
#define restrict_t restrict_t0

#define tmpfile tmpfile_windows

/* It must be included before <features.h> from GnuWin32 */
#include <typeinfo>

/* warning LNK4221 goes bye bye */
namespace { char dummy_symbol_to_quiet_down_LNK4221_warning = 0; };

#endif /* not YAAL_MSVCXX_FIX_HXX_INCLUDED */

