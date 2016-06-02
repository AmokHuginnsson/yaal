#ifndef YAAL_MSVCXX_SYS_STAT_H_INCLUDED
#define YAAL_MSVCXX_SYS_STAT_H_INCLUDED 1

#define stat stat_off
#define lstat lstat_off
#define fstat fstat_off
#define umask umask_off

#define _INC_STAT_INL 1
#include <../ucrt/sys/stat.h>
#undef umask
#undef stat
#undef lstat
#undef fstat
#undef _INC_STAT_INL

typedef int unsigned mode_t;

namespace msvcxx {
int stat( char const*, struct stat* );
mode_t umask( mode_t );
}

inline int stat( char const* path_, struct stat* s_ )
	{ return ( msvcxx::stat( path_, s_ ) ); }

inline int lstat( char const* path_, struct stat* s_ )
	{ return ( msvcxx::stat( path_, s_ ) ); }

inline mode_t umask( mode_t umask_ )
	{ return ( msvcxx::umask( umask_ ) ); }

#endif /* not YAAL_MSVCXX_SYS_STAT_H_INCLUDED */

