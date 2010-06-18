#ifndef YAAL_MSVCXX_CURSES_H_INCLUDED
#define YAAL_MSVCXX_CURSES_H_INCLUDED 1

#include <../include/curses.h>
#define newscr NULL
#define assume_default_colors( x, y ) OK
#define use_default_colors() OK
#define use_env( x ) /**/
#undef ungetch
inline int ungetch( int ch_ )
	{ return ( PDC_ungetch( ch_ ) ); }

inline int attr_get( attr_t*, int*, void* ) { return ( 0 ); }

#endif /* not YAAL_MSVCXX_CURSES_H_INCLUDED */
