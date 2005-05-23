/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	console.c - this file is integral part of `stdhapi' project.

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

#include <unistd.h>
#include <stdlib.h> /* getenv */
#include <stdio.h>
#include <termios.h>
#include <libintl.h>
#include <string.h> /* strerror */

#include "config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcore/hlog.h"
#include "hconsole.h"
#include "console.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

/* Bbbbffff
 * B - blink
 * b - background color bits
 * f - foreground color bits
 * 112 - 01110000
 * 7   - 00000111
 * 8   - 00001000
 * 128 - 10000000
 */
#define M_MAKE_ATTR(attr)	( COLOR_PAIR( \
		( ( ( attr ) & 112 ) >> 1 ) 					/* background */ \
			| ( attr ) & 7 ) 										/* foreground */ \
	| ( ( ( attr ) & 8 ) ? A_BOLD : 0 ) 		/* brighter foreground */ \
		| ( ( attr ) & 128 ? A_BLINK : 0 ) )	/* brighter background */

/* public: */
bool n_bNeedRepaint = false;
bool n_bInputWaiting = false;
int n_iWidth = 0;
int n_iHeight = 0;
int n_iMouseDes = 0;

/* private: */
WINDOW * n_psWindow = NULL;
bool	n_bEnabled = false;
termios	n_sTermios;

/* public: */
void enter_curses( void )
	{
	M_PROLOG
	short l_piColors [ ] = { COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
		COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE };
	int l_iFg = 0, l_iBg = 0;
	termios l_sTermios;
/*	def_shell_mode(); */
/* this is done automaticly by initscr ( ), read man next time */
	if ( ! isatty ( STDIN_FILENO ) )
		M_THROW ( "stdin in not a tty", 0 );
	if ( n_bDisableXON )
		{
		M_ENSURE ( tcgetattr ( STDIN_FILENO, & n_sTermios ) == 0 );
		M_ENSURE ( tcgetattr ( STDIN_FILENO, & l_sTermios ) == 0 );
		l_sTermios.c_iflag &= ~IXON;
		if ( n_bLeaveCtrlC )
			l_sTermios.c_cc [ VINTR ] = 0;
		if ( n_bLeaveCtrlZ )
			l_sTermios.c_cc [ VSUSP ] = 0;
		if ( n_bLeaveCtrlS )
			l_sTermios.c_cc [ VSTOP ] = 0;
		if ( n_bLeaveCtrlQ )
			l_sTermios.c_cc [ VSTART ] = 0;
		if ( n_bLeaveCtrlBackSlash )
			l_sTermios.c_cc [ VQUIT ] = 0;
		M_ENSURE ( tcsetattr ( STDIN_FILENO, TCSAFLUSH, & l_sTermios ) == 0 );
		}
	use_env ( true );
	if ( ! n_psWindow )
		n_psWindow = initscr();
	M_ENSURE ( cbreak ( ) != ERR );
	M_ENSURE ( start_color ( ) != ERR );
	M_IRV ( standout ( ) ); /* Macro, returned value without meaning */
	M_ENSURE ( nonl ( ) == OK );
	M_ENSURE ( keypad ( stdscr, true ) != ERR );
	M_ENSURE ( intrflush ( stdscr, false ) != ERR );
/*	scrollok ( stdscr, true ); */
	M_ENSURE ( scrollok ( stdscr, false ) != ERR );
	M_ENSURE ( leaveok ( stdscr, false ) != ERR );
	immedok ( stdscr, false );
	M_ENSURE ( fflush ( NULL ) == 0 );
	M_IRV ( flushinp ( ) ); /* Always returns OK */
	M_IRV ( curs_set ( D_CURSOR_INVISIBLE ) );
	M_ENSURE ( refresh ( ) != ERR );
	/* init color pairs */
	M_ENSURE ( assume_default_colors ( COLOR_BLACK, COLOR_BLACK ) == OK );
	for ( l_iBg = 0; l_iBg < 8; l_iBg ++ )
		for ( l_iFg = 0; l_iFg < 8; l_iFg ++ )
			M_IRV ( init_pair ( static_cast < short > ( l_iBg * 8 + l_iFg ),
						l_piColors [ l_iFg ], l_piColors [ l_iBg ] ) );
	attrset ( COLOR_PAIR( 7 ) );
	M_IRV ( bkgd ( ' ' | M_MAKE_ATTR ( D_FG_LIGHTGRAY | D_BG_BLACK ) | A_INVIS ) ); /* meaningless value from macro */
	n_bEnabled = true;
	getmaxyx ( stdscr, n_iHeight, n_iWidth );
	if ( getenv ( "STDHAPI_NO_MOUSE" ) )
		n_bUseMouse = false;
	if ( n_bUseMouse)
		{
		if ( ::getenv ( "DISPLAY" ) )
			{
			log ( D_LOG_INFO ) << _ ( "using X mouse support" ) << endl;
			mouse::mouse_open = mouse::x_mouse_open;
			mouse::mouse_get = mouse::x_mouse_get;
			mouse::mouse_close = mouse::x_mouse_close;
			}
		else
			{
			log ( D_LOG_INFO ) << _ ( "using console mouse support" ) << endl;
			mouse::mouse_open = mouse::console_mouse_open;
			mouse::mouse_get = mouse::console_mouse_get;
			mouse::mouse_close = mouse::console_mouse_close;
			}
		if ( ( n_iMouseDes = mouse::mouse_open ( ) ) < 0 )
			M_THROW ( _ ( "mouse is console type"
						" and we did not recived file descriptor" ), g_iErrNo );
		}
	return;
	M_EPILOG
	}
	
void leave_curses( void )
	{
	M_PROLOG
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", g_iErrNo );
//	if ( ! mousemask ( 0, NULL ) )
//		M_THROW ( "mousemask ( ) returned 0", g_iErrNo );
	if ( n_bUseMouse )
		M_IRV ( mouse::mouse_close ( ) );
	M_IRV ( bkgd ( ' ' | M_MAKE_ATTR ( ( D_FG_LIGHTGRAY | D_BG_BLACK ) ) ) );
	M_ENSURE ( use_default_colors ( ) == OK );
	M_ENSURE ( printw ( "" ) != ERR );
	M_ENSURE ( fflush ( NULL ) == 0 );
	M_IRV ( flushinp ( ) ); /* Always returns OK */
	M_ENSURE ( intrflush ( stdscr, true ) != ERR );
	M_IRV ( leaveok ( stdscr, false ) ); /* Always OK */
	immedok ( stdscr, true ); /* Always OK */
	M_ENSURE ( refresh ( ) != ERR );
	M_IRV ( nl ( ) ); /* Always OK */
	standend ( );
	M_ENSURE ( keypad ( stdscr, false ) != ERR );
	M_ENSURE ( nocbreak ( ) != ERR );
	M_IRV ( curs_set ( D_CURSOR_VISIBLE ) );
/*	reset_shell_mode ( ); */
/* see comment near def_shell_mode ( ), ( automagicly by endwin ( ) ) */
/*
	if ( n_psWindow )
	delwin ( n_psWindow );
	n_psWindow = NULL;
*/
	M_ENSURE ( endwin ( ) == OK );
	if ( n_bDisableXON )
		M_ENSURE ( tcsetattr ( STDIN_FILENO, TCSAFLUSH, & n_sTermios ) == 0 );
	n_bEnabled = false;
	return;
	M_EPILOG
	}
	
void set_attr( int a_iAttr )
	{
	M_PROLOG
	unsigned char l_ucByte = 0;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", g_iErrNo );
	l_ucByte = static_cast < unsigned char > ( a_iAttr );
	attrset ( M_MAKE_ATTR ( l_ucByte ) );
	return ;
	M_EPILOG
	}

int c_printf ( int a_iRow, int a_iColumn, int a_iAttribute,
							 char const * a_pcFormat, ... )
	{
	M_PROLOG
	int l_iError = 0;
	va_list l_xAp;
	va_start ( l_xAp, a_pcFormat );
	l_iError = c_vprintf ( a_iRow, a_iColumn, a_iAttribute, a_pcFormat, l_xAp );
	va_end ( l_xAp );
	return ( l_iError );
	M_EPILOG
	}
	
int c_vprintf ( int a_iRow, int a_iColumn, int a_iAttribute,
							 char const * a_pcFormat, va_list & a_rxAp )
	{
	M_PROLOG
	int l_iError = 0;
	int l_iOrigRow = 0;
	int l_iOrigColumn = 0;
	int l_iOrigAttribute = 0;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", g_iErrNo );
	getyx ( stdscr, l_iOrigRow, l_iOrigColumn );
	l_iOrigAttribute = get_attr ( );
	if ( a_iColumn >= n_iWidth )
		M_THROW ( "bad column.", a_iColumn );
	if ( ( a_iRow < 0 ) || ( a_iRow >= n_iHeight ) )
		M_THROW ( "bad row.", a_iRow );
	set_attr ( a_iAttribute );
	if ( a_iColumn < 0 )
		{
		M_ENSURE ( move ( a_iRow, 0 ) != ERR );
		M_IRV ( clrtoeol ( ) ); /* Always OK */
		}
	else
		M_ENSURE ( move ( a_iRow, a_iColumn ) != ERR );
	l_iError = vw_printw ( stdscr, a_pcFormat, a_rxAp );
	M_ENSURE ( move ( l_iOrigRow, l_iOrigColumn ) != ERR );
	set_attr ( l_iOrigAttribute );
	return ( l_iError );
	M_EPILOG
	}

int get_key( void )
	{
	M_PROLOG
	int l_iKey = 0;
	int l_iChar = 0;
	int l_iOrigCursState = D_CURSOR_INVISIBLE;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", g_iErrNo );
	M_ENSURE ( noecho() != ERR );
	M_ENSURE ( fflush( NULL ) == 0 );
	l_iKey = getch ( );
	if ( l_iKey == D_KEY_ESC )
		{
		M_ENSURE ( nodelay ( stdscr, true ) != ERR );
		l_iKey = getch ( );
		M_ENSURE ( nodelay ( stdscr, false ) != ERR );
		if ( l_iKey == ERR )
			l_iKey = D_KEY_ESC;
		else
			l_iKey = D_KEY_META_(l_iKey);
		}
	if ( l_iKey == D_KEY_CTRL_(n_cCommandComposeCharacter) )
		{
		l_iOrigCursState = curs_set ( D_CURSOR_INVISIBLE );
		M_IRV ( c_printf ( n_iHeight - 1, -1, D_FG_WHITE, "ctrl-%c",
					n_cCommandComposeCharacter ) );
		timeout ( n_iCommandComposeDelay * 100 );
		l_iKey = getch ( );
		timeout ( -1 );
		if ( l_iKey == ERR )
			{
			l_iKey = D_KEY_CTRL_(n_cCommandComposeCharacter);
			M_IRV ( c_printf ( n_iHeight - 1, 0, D_FG_LIGHTGRAY, "      " ) );
			}
		else
			{
			if ( l_iKey < D_KEY_ESC )
				l_iKey = D_KEY_COMMAND_( l_iChar = l_iKey + 96 );
			else if ( l_iKey == D_KEY_ESC )
				{
				M_ENSURE ( nodelay ( stdscr, true ) != ERR );
				l_iKey = getch ( );
				M_ENSURE ( nodelay ( stdscr, false ) != ERR );
				if ( l_iKey == ERR )
					l_iKey = D_KEY_COMMAND_(l_iChar = D_KEY_ESC);
				else
					l_iKey = D_KEY_COMMAND_(D_KEY_META_(l_iChar = l_iKey));
				}
			else
				l_iKey = D_KEY_COMMAND_(l_iChar = l_iKey);
			M_IRV ( c_printf ( n_iHeight - 1, 6, D_FG_WHITE, " %c", l_iChar ) );
			}
		M_IRV ( curs_set ( l_iOrigCursState ) );
		}
	M_ENSURE ( echo ( ) != ERR );
	switch ( l_iKey )
		{
		case ( 347 ):
			{
			l_iKey = KEY_END;
			break;
			}
		case ( 8 ):
		case ( 127 ):
			{
			l_iKey = KEY_BACKSPACE;
			break;
			}
		default:
			break;
		}
	return ( l_iKey );
	M_EPILOG
	}
	
int kbhit()
	{
	M_PROLOG
	int l_iKey;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", g_iErrNo );
	M_ENSURE ( nodelay( stdscr, true ) != ERR );
	l_iKey = get_key ( );
	M_ENSURE ( nodelay( stdscr, false ) != ERR );
	if ( l_iKey == ERR )
		return ( 0 );
	return ( l_iKey );
	M_EPILOG
	}
	
unsigned char get_attr( void )
	{
	M_PROLOG
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", g_iErrNo );
	attr_t l_xAttr;
	short l_hColor = 0;
	int l_iAttribute = 0;
	M_IRV ( attr_get ( & l_xAttr, & l_hColor, NULL ) ); /* Ugly macro */
	l_iAttribute = ( l_hColor << 1 ) & 56;
	l_iAttribute |= ( l_hColor & 7 );
	if ( l_xAttr & A_BOLD )
		l_iAttribute |= 8;
	if ( l_xAttr & A_BLINK )
		l_iAttribute |= 128;
	return ( static_cast < unsigned char > ( l_iAttribute ) );
	M_EPILOG
	}
	
void clrscr()
	{
	M_PROLOG
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", g_iErrNo );
	M_IRV ( clear ( ) ); /* Always returns OK */
	M_ENSURE ( refresh ( ) != ERR );
	return;
	M_EPILOG
	}

bool is_enabled ( void )
	{
	M_PROLOG
	return ( n_bEnabled );
	M_EPILOG
	}

int wait_for_user_input ( int & a_iKey, mouse::OMouse & a_rsMouse,
		int a_iTimeOutSec, int a_iTimeOutUsec )
	{
	int l_iError = - 1, l_iEventType = 0;
	timeval l_xWait;
	fd_set l_xFdSet;
	l_xWait.tv_sec = a_iTimeOutSec;
	l_xWait.tv_usec = a_iTimeOutUsec;
	FD_ZERO ( & l_xFdSet );
	FD_SET ( STDIN_FILENO, & l_xFdSet );
	if ( n_iMouseDes )
		FD_SET ( n_iMouseDes, & l_xFdSet );
	do
		{
		if ( n_bInputWaiting )
			{
			a_iKey = get_key ( );
			l_iEventType = D_EVENT_MOUSE;
			if ( a_iKey == KEY_MOUSE )
				M_IRV ( mouse::mouse_get ( a_rsMouse ) );
			else
				l_iEventType = D_EVENT_KEYBOARD;
			break;
			}
		l_iError = select ( FD_SETSIZE, & l_xFdSet, NULL, NULL,
				( a_iTimeOutSec || a_iTimeOutUsec ) ? & l_xWait : NULL );
		}
	while ( ( l_iError == -1 ) && ( g_iErrNo == EINTR ) );
	if ( l_iError > 0 )
		{
		if ( FD_ISSET ( STDIN_FILENO, & l_xFdSet ) )
			{
			a_iKey = get_key ( ), l_iEventType = D_EVENT_KEYBOARD;
			if ( a_iKey == KEY_MOUSE )
				l_iEventType = 0;
			}
		if ( ( a_iKey == KEY_MOUSE )
				|| ( n_iMouseDes && FD_ISSET ( n_iMouseDes, & l_xFdSet ) ) )
			mouse::mouse_get ( a_rsMouse ), l_iEventType |= D_EVENT_MOUSE;
		}
	return ( l_iEventType );
	}

}

}

