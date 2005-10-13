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
	typedef enum
		{
		D_PAGE_UP = 256,
		D_PAGE_DOWN,
		D_DOWN,
		D_UP,
		D_LEFT,
		D_RIGHT,
		D_HOME,
		D_END,
		D_DELETE,
		D_INSERT,
		D_BACKSPACE,
		D_MOUSE
		} key_code_t;
	};

typedef struct
	{
	typedef enum
		{
		D_DOWN_ARROW,
		D_UP_ARROW,
		D_VERTICAL_LINE
		} glyph_t;
	} GLYPHS;

#define D_CURSOR_INVISIBLE		0
#define D_CURSOR_VISIBLE			1
#define D_CURSOR_VERY_VISIBLE	2

#define D_ATTR_DEFAULT			( - 1 )

#define D_FG_BLACK					0
#define D_FG_RED						1
#define D_FG_GREEN					2
#define D_FG_BROWN					3
#define D_FG_BLUE						4
#define D_FG_MAGENTA				5
#define D_FG_CYAN						6
#define D_FG_LIGHTGRAY			7
#define D_FG_GRAY						8
#define D_FG_BRIGHTRED			9
#define D_FG_BRIGHTGREEN		10
#define D_FG_YELLOW					11
#define D_FG_BRIGHTBLUE			12
#define D_FG_BRIGHTMAGENTA	13
#define D_FG_BRIGHTCYAN			14
#define D_FG_WHITE					15

#define D_BG_BLACK					0
#define D_BG_RED						16
#define D_BG_GREEN					32
#define D_BG_BROWN					48
#define D_BG_BLUE						64
#define D_BG_MAGENTA				80
#define D_BG_CYAN						96
#define D_BG_LIGHTGRAY			112

#define D_BG_BLINK					128
#define D_BG_GRAY						D_BG_BLINK

#define D_ATTR_NORMAL				( D_FG_LIGHTGRAY | D_BG_BLACK )

#define D_KEY_ESC						27
#define D_KEY_ENTER					13
#define D_KEY_REFRESH				D_KEY_CTRL_L
#define D_KEY_RESIZE				D_KEY_CTRL_L

#define D_KEY_META_BASE			0x04000
#define D_KEY_COMMAND_BASE	0x08000
	
#define	D_KEY_META_(n)			( ( n ) + D_KEY_META_BASE )
#define D_KEY_CTRL_(n)			( ( n ) - 96 )
#define D_KEY_COMMAND_(n)		( ( n ) + D_KEY_COMMAND_BASE )

#define D_EVENT_ERROR			0
#define D_EVENT_KEYBOARD	1
#define D_EVENT_MOUSE			2

#define KEY_DELETE KEY_DC
#define KEY_BS KEY_BACKSPACE

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

