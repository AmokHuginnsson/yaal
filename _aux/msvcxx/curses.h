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

int attr_get( attr_t*, int*, void* );

int vw_printw( WINDOW*, char const*, va_list );

typedef int mmask_t;
static mmask_t mousemask(mmask_t, mmask_t *)
	{ return ( 0 ); }

struct MEVENT
	{	int y, x, bstate;	};

int getmouse( MEVENT* );

#endif /* not YAAL_MSVCXX_CURSES_H_INCLUDED */
