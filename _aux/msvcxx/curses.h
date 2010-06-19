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

static int vw_printw( WINDOW* win_, char const* format_, va_list va_ )
	{
	static int const maxCols( 1024 ); /* not very likely that someone has more that 1024 column wide terminal */
	char buf[maxCols]; 
	int const size( vsnprintf( buf, maxCols - 1, format_, va_ ) );
	wprintw( win_, buf );
	return ( size );
	}

typedef int mmask_t;
static mmask_t mousemask(mmask_t, mmask_t *)
	{ return ( 0 ); }

struct MEVENT
	{	int y, x, bstate;	};

static int getmouse( MEVENT* ev_ )
	{
	ev_->x = ev_->y = ev_->bstate = 0;
	return ( ERR );
	}

#endif /* not YAAL_MSVCXX_CURSES_H_INCLUDED */
