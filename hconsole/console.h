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

#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#	include "../config.h"
#endif /* HAVE_CONFIG_H */

namespace console
	{

#ifdef HAVE_ASCII_GRAPHICS
#	define D_ASCII_DOWN_ARROW			ACS_DARROW
#	define D_ASCII_UP_ARROW				ACS_UARROW
# define D_ASCII_VERTICAL_LINE	ACS_VLINE
#else
#	define D_ASCII_DOWN_ARROW			'v'
#	define D_ASCII_UP_ARROW				'^'
# define D_ASCII_VERTICAL_LINE	'|'
#endif /* not HAVE_ASCII_GRAPHICS */

#define D_CURSOR_INVISIBLE		0
#define D_CURSOR_VISIBLE			1
#define D_CURSOR_VERY_VISIBLE	2

#define D_ATTR_DEFAULT			-1

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

#define D_KEY_ESC						27
#define D_KEY_REFRESH	D_KEY_CTRL_L
#define D_KEY_RESIZE	D_KEY_CTRL_L

#define D_KEY_META_BASE			0x04000
#define D_KEY_COMMAND_BASE	0x08000
	
#define	D_KEY_META_(n)			n + D_KEY_META_BASE
#define D_KEY_CTRL_(n)			n - 96
#define D_KEY_COMMAND_(n)		n + D_KEY_COMMAND_BASE

#define KEY_DELETE KEY_DC
#define KEY_BS KEY_BACKSPACE

void enter_curses( void );
void leave_curses( void );
void set_attr( unsigned char attr );
int c_printf ( int, int, int, const char *, ... );
int c_printf ( int, int, int, const char *, va_list );
int get_key( void );
int kbhit( void );
char get_attr( void );
void clrscr( void );
bool is_enabled ( void );

extern bool n_bNeedRepaint;
extern bool n_bInputWaiting;
extern int n_iWidth;
extern int n_iHeight;

	}

#endif /* not __CONSOLE_H */

