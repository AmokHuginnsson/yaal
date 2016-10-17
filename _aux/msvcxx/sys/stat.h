#ifndef YAAL_MSVCXX_SYS_STAT_H_INCLUDED
#define YAAL_MSVCXX_SYS_STAT_H_INCLUDED 1

#include <../ucrt/sys/stat.h>

static int const F_OK = 0;
static int const X_OK = 1;
static int const W_OK = 2;
static int const R_OK = 4;

static int const S_IRUSR = 0400;
static int const S_IWUSR = 0200;
static int const S_IXUSR = 0100;
static int const S_IRGRP = 0040;
static int const S_IWGRP = 0020;
static int const S_IXGRP = 0010;
static int const S_IROTH = 0004;
static int const S_IWOTH = 0002;
static int const S_IXOTH = 0001;

static int const TYPE_MASK = 0170000;
static int const TYPE_FIFO = 0010000;
static int const TYPE_CHR  = 0020000;
static int const TYPE_DIR  = 0040000;
static int const TYPE_BLK  = 0060000;
static int const TYPE_REG  = 0100000;
static int const TYPE_LNK  = 0120000;
static int const TYPE_SOCK = 0140000;

#ifndef MODE_T_DEFINED
typedef int unsigned mode_t;
#define MODE_T_DEFINED 1
#endif

constexpr bool is_file_type( mode_t mode_, mode_t mask_ ) {
	return ( ( mode_ & TYPE_MASK ) == mask_ );
}

constexpr bool S_ISDIR( mode_t mode_ ) {
	return ( is_file_type( mode_, TYPE_DIR ) );
}

constexpr bool S_ISREG( mode_t mode_ ) {
	return ( is_file_type( mode_, TYPE_REG ) );
}

constexpr bool S_ISLNK( mode_t mode_ ) {
	return ( is_file_type( mode_, TYPE_LNK ) );
}

constexpr bool S_ISSOCK( mode_t mode_ ) {
	return ( is_file_type( mode_, TYPE_SOCK ) );
}

namespace msvcxx {
struct unix_stat : public stat {};
int unix_stat( char const*, struct stat* );
mode_t umask( mode_t );
}

inline int lstat( char const* path_, struct stat* s_ )
	{ return ( msvcxx::unix_stat( path_, s_ ) ); }

inline mode_t umask( mode_t umask_ )
	{ return ( msvcxx::umask( umask_ ) ); }

#ifdef YAAL_USES_STAT
#define stat msvcxx::unix_stat
#endif /* #ifdef YAAL_USES_STAT */

#endif /* not YAAL_MSVCXX_SYS_STAT_H_INCLUDED */

