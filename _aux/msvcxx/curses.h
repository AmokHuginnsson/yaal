#ifndef YAAL_MSVCXX_CURSES_H_INCLUDED
#define YAAL_MSVCXX_CURSES_H_INCLUDED 1

#define NCURSES_MOUSE_VERSION 1

#undef MOUSE_MOVED
#include <../include/curses.h>

#define newscr nullptr
#undef ungetch

inline int ungetch( int ch_ )
	{ return ( PDC_ungetch( ch_ ) ); }

inline bool has_mouse( void )
	{ return ( true ); }

#endif /* not YAAL_MSVCXX_CURSES_H_INCLUDED */
