/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	console.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib> /* getenv */
#include <cstdio>
#include <cstring> /* strerror */
#include <unistd.h>
#include <termios.h>
#include <libintl.h>

#include "config.h"

#ifdef HAVE_CURSES_H
#	include <curses.h>
#elif defined ( HAVE_NCURSES_CURSES_H )
#	include <ncurses/curses.h>
#else /* HAVE_NCURSES_CURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_CURSES_H */

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hlog.h"
#include "hconsole.h"
#include "console.h"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{
	int const COLORS::D_FG_BLACK, COLORS::D_FG_RED, COLORS::D_FG_GREEN;
	int const COLORS::D_FG_BROWN, COLORS::D_FG_BLUE, COLORS::D_FG_MAGENTA;
	int const COLORS::D_FG_CYAN, COLORS::D_FG_LIGHTGRAY, COLORS::D_FG_GRAY;
	int const COLORS::D_FG_BRIGHTRED, COLORS::D_FG_BRIGHTGREEN;
	int const COLORS::D_FG_YELLOW, COLORS::D_FG_BRIGHTBLUE;
	int const COLORS::D_FG_BRIGHTMAGENTA, COLORS::D_FG_BRIGHTCYAN;
	int const COLORS::D_FG_WHITE;
	int const COLORS::D_BG_BLACK, COLORS::D_BG_RED, COLORS::D_BG_GREEN;
	int const COLORS::D_BG_BROWN, COLORS::D_BG_BLUE, COLORS::D_BG_MAGENTA;
	int const COLORS::D_BG_CYAN, COLORS::D_BG_LIGHTGRAY, COLORS::D_BG_BLINK;
	int const COLORS::D_BG_GRAY, COLORS::D_ATTR_NORMAL;
	int GLYPHS::D_DOWN_ARROW, GLYPHS::D_UP_ARROW, GLYPHS::D_VERTICAL_LINE;

/* Bbbbffff
 * B - blink
 * b - background color bits
 * f - foreground color bits
 * 112 - 01110000
 * 7   - 00000111
 * 8   - 00001000
 * 128 - 10000000
 */
template < int attr = 0 >
struct ATTR
	{
	static int const value = COLOR_PAIR(
				( ( attr & 112 ) >> 1 ) 	  		/* background */
				| attr & 7 ) 								  	/* foreground */
			| ( ( attr & 8 ) ? A_BOLD : 0 )   /* brighter foreground */
			| ( attr & 128 ? A_BLINK : 0 ); /* brighter background */
	inline static int const value_r ( int const a_iAttr )
		{
		return ( COLOR_PAIR(
					( ( a_iAttr & 112 ) >> 1 ) 	  		/* background */
					| a_iAttr & 7 ) 								  	/* foreground */
				| ( ( a_iAttr & 8 ) ? A_BOLD : 0 )   /* brighter foreground */
				| ( a_iAttr & 128 ? A_BLINK : 0 ) ); /* brighter background */
		}
	};

char const* const red = "\033[0m\033[31m";
char const* const green = "\033[0m\033[32m";
char const* const brown = "\033[0m\033[33m";
char const* const blue = "\033[0m\033[34m";
char const* const magenta = "\033[0m\033[35m";
char const* const cyan = "\033[0m\033[36m";
char const* const lightgray = "\033[0m\033[37m";
char const* const gray = "\033[40m\033[1m";
char const* const brightred = "\033[1m\033[31m";
char const* const brightgreen = "\033[1m\033[32m";
char const* const yellow = "\033[1m\033[33m";
char const* const brightblue = "\033[1m\033[34m";
char const* const brightmagenta = "\033[1m\033[35m";
char const* const brightcyan = "\033[1m\033[36m";
char const* const white = "\033[1m\033[37m";

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

int const C_OK = OK;
int const C_ERR = ERR;

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
	standout ( ); /* Macro, returned value without meaning */
	M_ENSURE ( nonl ( ) == OK );
	M_ENSURE ( keypad ( stdscr, true ) != ERR );
	M_ENSURE ( intrflush ( stdscr, false ) != ERR );
/*	scrollok ( stdscr, true ); */
	M_ENSURE ( scrollok ( stdscr, false ) != ERR );
	M_ENSURE ( leaveok ( stdscr, false ) != ERR );
	immedok ( stdscr, false );
	M_ENSURE ( fflush ( NULL ) == 0 );
	flushinp ( ); /* Always returns OK */
	curs_set ( CURSOR::D_INVISIBLE );
	M_ENSURE ( refresh ( ) != ERR );
	/* init color pairs */
	M_ENSURE ( assume_default_colors ( COLOR_BLACK, COLOR_BLACK ) == OK );
	for ( l_iBg = 0; l_iBg < 8; l_iBg ++ )
		for ( l_iFg = 0; l_iFg < 8; l_iFg ++ )
			init_pair ( static_cast < short > ( l_iBg * 8 + l_iFg ),
					l_piColors [ l_iFg ], l_piColors [ l_iBg ] );
	attrset ( COLOR_PAIR( 7 ) );
	bkgd ( ' ' | ATTR < COLORS::D_FG_BLACK | COLORS::D_BG_BLACK >::value | A_INVIS ); /* meaningless value from macro */
	n_bEnabled = true;
	getmaxyx ( stdscr, n_iHeight, n_iWidth );
	if ( getenv ( "YAAL_NO_MOUSE" ) )
		n_bUseMouse = false;
	if ( n_bUseMouse)
		{
		if ( ::getenv ( "DISPLAY" ) )
			{
			log ( LOG_TYPE::D_INFO ) << _ ( "using ncurses mouse support" ) << endl;
			mouse::mouse_open = mouse::x_mouse_open;
			mouse::mouse_get = mouse::x_mouse_get;
			mouse::mouse_close = mouse::x_mouse_close;
			}
		else
			{
			log ( LOG_TYPE::D_INFO ) << _ ( "using console mouse support" ) << endl;
			mouse::mouse_open = mouse::console_mouse_open;
			mouse::mouse_get = mouse::console_mouse_get;
			mouse::mouse_close = mouse::console_mouse_close;
			}
		if ( ( n_iMouseDes = mouse::mouse_open ( ) ) < 0 )
			M_THROW ( _ ( "mouse is console type"
						" and we did not recived file descriptor" ), errno );
		}
#ifdef HAVE_ASCII_GRAPHICS
	GLYPHS::D_DOWN_ARROW		= ACS_DARROW;
	GLYPHS::D_UP_ARROW			= ACS_UARROW;
	GLYPHS::D_VERTICAL_LINE	= ACS_VLINE;
#else /* than HAVE_ASCII_GRAPHICS */
	GLYPHS::D_DOWN_ARROW		= 'v';
	GLYPHS::D_UP_ARROW			= '^';
	GLYPHS::D_VERTICAL_LINE	= '|';
#endif /* not HAVE_ASCII_GRAPHICS */
	return;
	M_EPILOG
	}
	
void leave_curses( void )
	{
	M_PROLOG
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", errno );
//	if ( ! mousemask ( 0, NULL ) )
//		M_THROW ( "mousemask ( ) returned 0", errno );
	if ( n_bUseMouse )
		static_cast < void > ( mouse::mouse_close ( ) );
	bkgd ( ' ' | ATTR < COLORS::D_FG_LIGHTGRAY | COLORS::D_BG_BLACK >::value );
	M_ENSURE ( use_default_colors ( ) == OK );
	M_ENSURE ( printw ( "" ) != ERR );
	M_ENSURE ( fflush ( NULL ) == 0 );
	flushinp ( ); /* Always returns OK */
	M_ENSURE ( intrflush ( stdscr, true ) != ERR );
	leaveok ( stdscr, false ); /* Always OK */
	immedok ( stdscr, true ); /* Always OK */
	M_ENSURE ( refresh ( ) != ERR );
	nl ( ); /* Always OK */
	standend ( );
	M_ENSURE ( keypad ( stdscr, false ) != ERR );
	M_ENSURE ( nocbreak ( ) != ERR );
	curs_set ( CURSOR::D_VISIBLE );
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
	char unsigned l_ucByte = 0;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", errno );
	l_ucByte = static_cast < char unsigned > ( a_iAttr );
	attrset ( ATTR<>::value_r ( l_ucByte ) );
	return ;
	M_EPILOG
	}

int c_move ( int const & a_iRow, int const & a_iColumn )
	{
	return ( ::move ( a_iRow, a_iColumn ) );
	}

CURSOR::cursor_t curs_set ( CURSOR::cursor_t const & a_eCursor )
	{
	int l_iCursor = ::curs_set ( a_eCursor == CURSOR::D_VISIBLE ? 1 : ( a_eCursor == CURSOR::D_INVISIBLE ? 0 : 2 ) );
	if ( l_iCursor == 1 )
		return ( CURSOR::D_VISIBLE );
	else if ( l_iCursor == 2 )
		return ( CURSOR::D_VERY_VISIBLE );
	return ( CURSOR::D_INVISIBLE );
	}

int c_addch ( int const & a_iChar )
	{
	return ( ::addch ( a_iChar ) );
	}

int c_refresh ( void )
	{
	return ( ::refresh ( ) );
	}

int endwin ( void )
	{
	return ( ::endwin ( ) );
	}

void c_getmaxyx ( int & a_riHeight, int & a_riWidth )
	{
	getmaxyx ( stdscr, a_riHeight, a_riWidth );
	return;
	}

void c_getyx ( int & a_riHeight, int & a_riWidth )
	{
	getyx ( stdscr, a_riHeight, a_riWidth );
	return;
	}

void c_clrtoeol ( void )
	{
	::clrtoeol ( );
	return;
	}

int c_mvprintf ( int a_iRow, int a_iColumn, char const * const a_pcFormat,
		... )
	{
	int l_iError = 0;
	va_list l_xAp;
	va_start ( l_xAp, a_pcFormat );
	l_iError = c_vmvprintf ( a_iRow, a_iColumn, a_pcFormat, l_xAp );
	va_end ( l_xAp );
	return ( l_iError );
	}

int c_vmvprintf ( int a_iRow, int a_iColumn,
							 char const * const a_pcFormat, va_list & a_rxAp )
	{
	int l_iOrigRow = 0;
	int l_iOrigColumn = 0;
	int l_iError = 0;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", errno );
	if ( a_iColumn >= n_iWidth )
		M_THROW ( "bad column.", a_iColumn );
	if ( ( a_iRow < 0 ) || ( a_iRow >= n_iHeight ) )
		M_THROW ( "bad row.", a_iRow );
	getyx ( stdscr, l_iOrigRow, l_iOrigColumn );
	if ( a_iColumn < 0 )
		{
		M_ENSURE ( move ( a_iRow, 0 ) != ERR );
		clrtoeol ( ); /* Always OK */
		}
	else
		M_ENSURE ( move ( a_iRow, a_iColumn ) != ERR );
	l_iError = vw_printw ( stdscr, a_pcFormat, a_rxAp );
	M_ENSURE ( move ( l_iOrigRow, l_iOrigColumn ) != ERR );
	return ( l_iError );
	}

int c_printf ( int a_iRow, int a_iColumn, int a_iAttribute,
							 char const * const a_pcFormat, ... )
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
							 char const * const a_pcFormat, va_list & a_rxAp )
	{
	M_PROLOG
	int l_iError = 0;
	int l_iOrigAttribute = 0;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", errno );
	l_iOrigAttribute = get_attr ( );
	set_attr ( a_iAttribute );
	l_iError = c_vmvprintf ( a_iRow, a_iColumn, a_pcFormat, a_rxAp );
	set_attr ( l_iOrigAttribute );
	return ( l_iError );
	M_EPILOG
	}

int ungetch ( int a_iCode )
	{
	return ( ::ungetch ( a_iCode ) );
	}

int get_key ( void )
	{
	M_PROLOG
	int l_iKey = 0;
	int l_iChar = 0;
	CURSOR::cursor_t l_eOrigCursState = CURSOR::D_INVISIBLE;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", errno );
	M_ENSURE ( noecho() != ERR );
	M_ENSURE ( fflush( NULL ) == 0 );
	l_iKey = getch ( );
	if ( l_iKey == KEY_CODES::D_ESC )
		{
		M_ENSURE ( nodelay ( stdscr, true ) != ERR );
		l_iKey = getch ( );
		M_ENSURE ( nodelay ( stdscr, false ) != ERR );
		if ( l_iKey == ERR )
			l_iKey = KEY_CODES::D_ESC;
		else
			l_iKey = KEY<>::meta_r ( l_iKey );
		}
	if ( l_iKey == KEY<>::ctrl_r ( n_cCommandComposeCharacter ) )
		{
		l_eOrigCursState = curs_set ( CURSOR::D_INVISIBLE );
		c_printf ( n_iHeight - 1, -1, COLORS::D_FG_WHITE, "ctrl-%c",
					n_cCommandComposeCharacter );
		timeout ( n_iCommandComposeDelay * 100 );
		l_iKey = getch ( );
		timeout ( -1 );
		if ( l_iKey == ERR )
			{
			l_iKey = KEY<>::ctrl_r ( n_cCommandComposeCharacter );
			c_printf ( n_iHeight - 1, 0, COLORS::D_FG_LIGHTGRAY, "      " );
			}
		else
			{
			if ( l_iKey < KEY_CODES::D_ESC )
				l_iKey = KEY<>::command_r ( l_iChar = l_iKey + 96 );
			else if ( l_iKey == KEY_CODES::D_ESC )
				{
				M_ENSURE ( nodelay ( stdscr, true ) != ERR );
				l_iKey = getch ( );
				M_ENSURE ( nodelay ( stdscr, false ) != ERR );
				if ( l_iKey == ERR )
					l_iKey = KEY<>::command_r (l_iChar = KEY_CODES::D_ESC);
				else
					l_iKey = KEY<>::command_r (KEY<>::meta_r ( l_iChar = l_iKey ) );
				}
			else
				l_iKey = KEY<>::command_r ( l_iChar = l_iKey );
			c_printf ( n_iHeight - 1, 6, COLORS::D_FG_WHITE, " %c", l_iChar );
			}
		curs_set ( l_eOrigCursState );
		}
	M_ENSURE ( echo ( ) != ERR );
	switch ( l_iKey )
		{
		case ( KEY_NPAGE ):			l_iKey = KEY_CODES::D_PAGE_DOWN;	break;
		case ( KEY_PPAGE ):			l_iKey = KEY_CODES::D_PAGE_UP;		break;
		case ( KEY_HOME ):			l_iKey = KEY_CODES::D_HOME;				break;
		case ( 347 ):
		case ( KEY_END ):				l_iKey = KEY_CODES::D_END;				break;
		case ( 8 ):
		case ( 127 ):
		case ( KEY_BACKSPACE ):	l_iKey = KEY_CODES::D_BACKSPACE;	break;
		case ( KEY_UP ):				l_iKey = KEY_CODES::D_UP;					break;
		case ( KEY_DOWN ):			l_iKey = KEY_CODES::D_DOWN;				break;
		case ( KEY_LEFT ):			l_iKey = KEY_CODES::D_LEFT;				break;
		case ( KEY_RIGHT ):			l_iKey = KEY_CODES::D_RIGHT;			break;
		case ( KEY_DC ):				l_iKey = KEY_CODES::D_DELETE;			break;
		case ( KEY_IC ):				l_iKey = KEY_CODES::D_INSERT;			break;
		case ( KEY_MOUSE ):			l_iKey = KEY_CODES::D_MOUSE;			break;
		default:
		break;
		}
	return ( l_iKey );
	M_EPILOG
	}
	
int kbhit ( void )
	{
	M_PROLOG
	int l_iKey;
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", errno );
	M_ENSURE ( nodelay( stdscr, true ) != ERR );
	l_iKey = get_key ( );
	M_ENSURE ( nodelay( stdscr, false ) != ERR );
	if ( l_iKey == ERR )
		return ( 0 );
	return ( l_iKey );
	M_EPILOG
	}
	
char unsigned get_attr( void )
	{
	M_PROLOG
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", errno );
	attr_t l_xAttr;
	short l_hColor = 0;
	int l_iAttribute = 0;
	static_cast < void > ( attr_get ( & l_xAttr, & l_hColor, NULL ) ); /* Ugly macro */
	l_iAttribute = ( l_hColor << 1 ) & 56;
	l_iAttribute |= ( l_hColor & 7 );
	if ( l_xAttr & A_BOLD )
		l_iAttribute |= 8;
	if ( l_xAttr & A_BLINK )
		l_iAttribute |= 128;
	return ( static_cast < char unsigned > ( l_iAttribute ) );
	M_EPILOG
	}
	
void clrscr ( void )
	{
	M_PROLOG
	if ( ! n_bEnabled )
		M_THROW ( "not in curses mode", errno );
	clear ( ); /* Always returns OK */
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
			l_iEventType = EVENT::D_MOUSE;
			if ( a_iKey == KEY_MOUSE )
				static_cast < void > ( mouse::mouse_get ( a_rsMouse ) );
			else
				l_iEventType = EVENT::D_KEYBOARD;
			break;
			}
		l_iError = select ( FD_SETSIZE, & l_xFdSet, NULL, NULL,
				( a_iTimeOutSec || a_iTimeOutUsec ) ? & l_xWait : NULL );
		}
	while ( ( l_iError == -1 ) && ( errno == EINTR ) );
	if ( l_iError > 0 )
		{
		if ( FD_ISSET ( STDIN_FILENO, & l_xFdSet ) )
			{
			a_iKey = get_key ( ), l_iEventType = EVENT::D_KEYBOARD;
			if ( a_iKey == KEY_MOUSE )
				l_iEventType = 0;
			}
		if ( ( a_iKey == KEY_MOUSE )
				|| ( n_iMouseDes && FD_ISSET ( n_iMouseDes, & l_xFdSet ) ) )
			l_iEventType |= EVENT::D_MOUSE, static_cast < void > ( mouse::mouse_get ( a_rsMouse ) );
		}
	return ( l_iEventType );
	}

void bell ( void )
	{
	M_PROLOG
	M_ENSURE ( putchar ( '\a' ) == '\a' );
	return;
	M_EPILOG
	}

}

}

