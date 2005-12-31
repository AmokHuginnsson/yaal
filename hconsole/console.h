/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	console.h - this file is integral part of `stdhapi' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __HCONSOLE_CONSOLE_H
#define __HCONSOLE_CONSOLE_H

#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif /* HAVE_CONFIG_H */

#include "mouse.h"

namespace stdhapi
{

namespace hconsole
{

extern int const C_OK;
extern int const C_ERR;

struct KEY_CODES
	{
	int const static D_ESC					= 27;
	int const static D_PAGE_UP			= 256;
	int const static D_PAGE_DOWN		= D_PAGE_UP		+ 1;
	int const static D_DOWN					= D_PAGE_DOWN	+ 1;
	int const static D_UP						= D_DOWN			+ 1;
	int const static D_LEFT					= D_UP				+ 1;
	int const static D_RIGHT				= D_LEFT			+ 1;
	int const static D_HOME					= D_RIGHT			+ 1;
	int const static D_END					= D_HOME			+ 1;
	int const static D_DELETE				= D_END				+ 1;
	int const static D_INSERT				= D_DELETE		+ 1;
	int const static D_BACKSPACE		= D_INSERT		+ 1;
	int const static D_MOUSE				=	D_BACKSPACE	+ 1;
	int const static D_META_BASE		= 0x04000;
	int const static D_COMMAND_BASE	= 0x08000;
	};

struct GLYPHS
	{
	typedef enum
		{
		D_DOWN_ARROW,
		D_UP_ARROW,
		D_VERTICAL_LINE
		} glyph_t;
	};

#define D_CURSOR_INVISIBLE		0
#define D_CURSOR_VISIBLE			1
#define D_CURSOR_VERY_VISIBLE	2

#define D_ATTR_DEFAULT			( - 1 )

struct COLORS
	{
	int const static D_FG_BLACK					= 0;
	int const static D_FG_RED						= 1;
	int const static D_FG_GREEN					= 2;
	int const static D_FG_BROWN					= 3;
	int const static D_FG_BLUE					= 4;
	int const static D_FG_MAGENTA				= 5;
	int const static D_FG_CYAN					= 6;
	int const static D_FG_LIGHTGRAY			= 7;
	int const static D_FG_GRAY					= 8;
	int const static D_FG_BRIGHTRED			= 9;
	int const static D_FG_BRIGHTGREEN		= 10;
	int const static D_FG_YELLOW				= 11;
	int const static D_FG_BRIGHTBLUE		= 12;
	int const static D_FG_BRIGHTMAGENTA	= 13;
	int const static D_FG_BRIGHTCYAN		= 14;
	int const static D_FG_WHITE					= 15;

	int const static D_BG_BLACK					= 0;
	int const static D_BG_RED						= 16;
	int const static D_BG_GREEN					= 32;
	int const static D_BG_BROWN					= 48;
	int const static D_BG_BLUE					= 64;
	int const static D_BG_MAGENTA				= 80;
	int const static D_BG_CYAN					= 96;
	int const static D_BG_LIGHTGRAY			= 112;
	int const static D_BG_BLINK					= 128;
	int const static D_BG_GRAY					= D_BG_BLINK;
	int const static D_ATTR_NORMAL			= ( D_FG_LIGHTGRAY | D_BG_BLACK );
	};

#define D_KEY_REFRESH				D_KEY_CTRL_L
#define D_KEY_RESIZE				D_KEY_CTRL_L

#define	D_KEY_META_(n)			( ( n ) + KEY_CODES::D_META_BASE )
#define D_KEY_CTRL_(n)			( ( n ) - 96 )
#define D_KEY_COMMAND_(n)		( ( n ) + KEY_CODES::D_COMMAND_BASE )

#define D_EVENT_ERROR			0
#define D_EVENT_KEYBOARD	1
#define D_EVENT_MOUSE			2

void enter_curses( void );
void leave_curses( void );
void set_attr( int );
int c_move ( int const &, int const & );
int curs_set ( int const & );
int c_addch ( GLYPHS::glyph_t const & );
int c_refresh ( void );
int endwin ( void );
void c_getmaxyx ( int &, int & );
void c_getyx ( int &, int & );
void c_clrtoeol ( void );
int c_mvprintf ( int, int, char const * const, ... );
int c_vmvprintf ( int, int, char const * const, va_list & );
int c_printf ( int, int, int, char const * const, ... );
int c_vprintf ( int, int, int, char const * const, va_list & );
int ungetch ( int );
int get_key( void );
int kbhit( void );
unsigned char get_attr( void );
void clrscr( void );
bool is_enabled ( void );
int wait_for_user_input ( int &, mouse::OMouse &, int = 0, int = 0 );
void bell ( void );

extern bool n_bNeedRepaint;
extern bool n_bInputWaiting;
extern int n_iWidth;
extern int n_iHeight;
extern int n_iMouseDes;

}

}

#endif /* not __HCONSOLE_CONSOLE_H */

