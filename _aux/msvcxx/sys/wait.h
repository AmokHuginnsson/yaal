#ifndef YAAL_MSVCXX_SYS_WAIT_H_INCLUDED
#define YAAL_MSVCXX_SYS_WAIT_H_INCLUDED 1

namespace msvcxx {
int waitpid( int, int*, int );
}

inline int waitpid( int pid_, int* status_, int options_ )
	{ return ( msvcxx::waitpid( pid_, status_, options_ ) ); }

#endif /* not YAAL_MSVCXX_SYS_WAIT_H_INCLUDED */

