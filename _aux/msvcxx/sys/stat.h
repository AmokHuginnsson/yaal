#ifndef YAAL_MSVCXX_SYS_STAT_H_INCLUDED
#define YAAL_MSVCXX_SYS_STAT_H_INCLUDED 1

#if 0
#define stat stat_off
#define fstat fstat_off

#undef stat
#undef fstat
#endif

#include <../ucrt/sys/stat.h>

typedef int unsigned mode_t;

namespace msvcxx {
using ::stat;
int unix_stat( char const*, struct stat* );
mode_t umask( mode_t );
}

inline int lstat( char const* path_, struct stat* s_ )
	{ return ( msvcxx::stat( path_, s_ ) ); }

inline mode_t umask( mode_t umask_ )
	{ return ( msvcxx::umask( umask_ ) ); }

#define stat msvcxx::unix_stat

#endif /* not YAAL_MSVCXX_SYS_STAT_H_INCLUDED */

