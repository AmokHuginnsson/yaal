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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "console.h"

#include "../hcore/hlog.h"
#include "../hcore/hexception.h"
#include "../hcore/rc_file.h"

namespace console
{

/* public: */
bool n_bNeedRepaint = false;
bool n_bInputWaiting = false;
int n_iWidth = 0;
int n_iHeight = 0;
int n_iLatency = 1;
int n_iDisabledAttribute = 256 * ( D_FG_GREEN | D_BG_BLACK | D_BG_BLINK )
															+ ( D_FG_LIGHTGRAY | D_BG_BLACK | D_BG_BLINK );
int n_iEnabledAttribute = 256 * ( D_FG_BRIGHTGREEN | D_BG_BLACK | D_BG_BLINK )
															+ ( D_FG_WHITE | D_BG_BLACK | D_BG_BLINK );
int n_iFocusedAttribute = 256 * ( D_FG_BRIGHTGREEN | D_BG_BLACK | D_BG_BLINK )
															+ ( D_FG_BLACK | D_BG_LIGHTGRAY ); 
/* private: */
WINDOW * n_psWindow = NULL;
bool	n_bEnabled = false;
bool	n_bUseMouse = false;
char	n_cCommandComposeCharacter = 'x';
int		n_iCommandComposeDelay = 16;

OVariable n_psVariables [ ] =
	{
		{ D_TYPE_BOOL, "use_mouse", & n_bUseMouse },
		{ D_TYPE_INT, "latency", & n_iLatency },
		{ D_TYPE_CHAR, "command_compose_character", & n_cCommandComposeCharacter },
		{ D_TYPE_INT, "command_compose_delay", & n_iCommandComposeDelay },
		{ 0, NULL, NULL }
	};
	
/* public: */
void enter_curses( void )
	{
	M_PROLOG
/*	def_shell_mode(); */
/* this is done automaticly by initscr ( ), read man next time */
	use_env ( true );
	if ( ! n_psWindow )n_psWindow = initscr();
	cbreak();
	start_color();
	standout();
	nonl();
	keypad( stdscr, true );
	intrflush( stdscr, false );
/*	scrollok( stdscr, true ); */
	scrollok( stdscr, false );
	leaveok( stdscr, true );
	immedok( stdscr, false );
	fflush( 0 );
	flushinp();
	curs_set ( 0 );
	refresh();
	/* black background */
	assume_default_colors ( COLOR_BLACK, COLOR_BLACK );
	init_pair( COLOR_BLACK * 8 + COLOR_BLACK, COLOR_BLACK, COLOR_BLACK );
	init_pair( COLOR_BLACK * 8 + COLOR_RED, COLOR_RED, COLOR_BLACK );
	init_pair( COLOR_BLACK * 8 + COLOR_GREEN, COLOR_GREEN, COLOR_BLACK );
	init_pair( COLOR_BLACK * 8 + COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK );
	init_pair( COLOR_BLACK * 8 + COLOR_BLUE, COLOR_BLUE, COLOR_BLACK );
	init_pair( COLOR_BLACK * 8 + COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK );
	init_pair( COLOR_BLACK * 8 + COLOR_CYAN, COLOR_CYAN, COLOR_BLACK );
	init_pair( COLOR_BLACK * 8 + COLOR_WHITE, COLOR_WHITE, COLOR_BLACK );
	/* red background */
	init_pair( COLOR_RED * 8 + COLOR_BLACK, COLOR_BLACK, COLOR_RED );
	init_pair( COLOR_RED * 8 + COLOR_RED, COLOR_RED, COLOR_RED );
	init_pair( COLOR_RED * 8 + COLOR_GREEN, COLOR_GREEN, COLOR_RED );
	init_pair( COLOR_RED * 8 + COLOR_YELLOW, COLOR_YELLOW, COLOR_RED );
	init_pair( COLOR_RED * 8 + COLOR_BLUE, COLOR_BLUE, COLOR_RED );
	init_pair( COLOR_RED * 8 + COLOR_MAGENTA, COLOR_MAGENTA, COLOR_RED );
	init_pair( COLOR_RED * 8 + COLOR_CYAN, COLOR_CYAN, COLOR_RED );
	init_pair( COLOR_RED * 8 + COLOR_WHITE, COLOR_WHITE, COLOR_RED );
	/* green background */
	init_pair( COLOR_GREEN * 8 + COLOR_BLACK, COLOR_BLACK, COLOR_GREEN );
	init_pair( COLOR_GREEN * 8 + COLOR_RED, COLOR_RED, COLOR_GREEN );
	init_pair( COLOR_GREEN * 8 + COLOR_GREEN, COLOR_GREEN, COLOR_GREEN );
	init_pair( COLOR_GREEN * 8 + COLOR_YELLOW, COLOR_YELLOW, COLOR_GREEN );
	init_pair( COLOR_GREEN * 8 + COLOR_BLUE, COLOR_BLUE, COLOR_GREEN );
	init_pair( COLOR_GREEN * 8 + COLOR_MAGENTA, COLOR_MAGENTA, COLOR_GREEN );
	init_pair( COLOR_GREEN * 8 + COLOR_CYAN, COLOR_CYAN, COLOR_GREEN );
	init_pair( COLOR_GREEN * 8 + COLOR_WHITE, COLOR_WHITE, COLOR_GREEN );
	/* yellow background */
	init_pair( COLOR_YELLOW * 8 + COLOR_BLACK, COLOR_BLACK, COLOR_YELLOW );
	init_pair( COLOR_YELLOW * 8 + COLOR_RED, COLOR_RED, COLOR_YELLOW );
	init_pair( COLOR_YELLOW * 8 + COLOR_GREEN, COLOR_GREEN, COLOR_YELLOW );
	init_pair( COLOR_YELLOW * 8 + COLOR_YELLOW, COLOR_YELLOW, COLOR_YELLOW );
	init_pair( COLOR_YELLOW * 8 + COLOR_BLUE, COLOR_BLUE, COLOR_YELLOW );
	init_pair( COLOR_YELLOW * 8 + COLOR_MAGENTA, COLOR_MAGENTA, COLOR_YELLOW );
	init_pair( COLOR_YELLOW * 8 + COLOR_CYAN, COLOR_CYAN, COLOR_YELLOW );
	init_pair( COLOR_YELLOW * 8 + COLOR_WHITE, COLOR_WHITE, COLOR_YELLOW );
	/* blue background */
	init_pair( COLOR_BLUE * 8 + COLOR_BLACK, COLOR_BLACK, COLOR_BLUE );
	init_pair( COLOR_BLUE * 8 + COLOR_RED, COLOR_RED, COLOR_BLUE );
	init_pair( COLOR_BLUE * 8 + COLOR_GREEN, COLOR_GREEN, COLOR_BLUE );
	init_pair( COLOR_BLUE * 8 + COLOR_YELLOW, COLOR_YELLOW, COLOR_BLUE );
	init_pair( COLOR_BLUE * 8 + COLOR_BLUE, COLOR_BLUE, COLOR_BLUE );
	init_pair( COLOR_BLUE * 8 + COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLUE );
	init_pair( COLOR_BLUE * 8 + COLOR_CYAN, COLOR_CYAN, COLOR_BLUE );
	init_pair( COLOR_BLUE * 8 + COLOR_WHITE, COLOR_WHITE, COLOR_BLUE );
	/* magenta background */
	init_pair( COLOR_MAGENTA * 8 + COLOR_BLACK, COLOR_BLACK, COLOR_MAGENTA );
	init_pair( COLOR_MAGENTA * 8 + COLOR_RED, COLOR_RED, COLOR_MAGENTA );
	init_pair( COLOR_MAGENTA * 8 + COLOR_GREEN, COLOR_GREEN, COLOR_MAGENTA );
	init_pair( COLOR_MAGENTA * 8 + COLOR_YELLOW, COLOR_YELLOW, COLOR_MAGENTA );
	init_pair( COLOR_MAGENTA * 8 + COLOR_BLUE, COLOR_BLUE, COLOR_MAGENTA );
	init_pair( COLOR_MAGENTA * 8 + COLOR_MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA );
	init_pair( COLOR_MAGENTA * 8 + COLOR_CYAN, COLOR_CYAN, COLOR_MAGENTA );
	init_pair( COLOR_MAGENTA * 8 + COLOR_WHITE, COLOR_WHITE, COLOR_MAGENTA );
	/* cyan background */
	init_pair( COLOR_CYAN * 8 + COLOR_BLACK, COLOR_BLACK, COLOR_CYAN );
	init_pair( COLOR_CYAN * 8 + COLOR_RED, COLOR_RED, COLOR_CYAN );
	init_pair( COLOR_CYAN * 8 + COLOR_GREEN, COLOR_GREEN, COLOR_CYAN );
	init_pair( COLOR_CYAN * 8 + COLOR_YELLOW, COLOR_YELLOW, COLOR_CYAN );
	init_pair( COLOR_CYAN * 8 + COLOR_BLUE, COLOR_BLUE, COLOR_CYAN );
	init_pair( COLOR_CYAN * 8 + COLOR_MAGENTA, COLOR_MAGENTA, COLOR_CYAN );
	init_pair( COLOR_CYAN * 8 + COLOR_CYAN, COLOR_CYAN, COLOR_CYAN );
	init_pair( COLOR_CYAN * 8 + COLOR_WHITE, COLOR_WHITE, COLOR_CYAN );
	/* white background */
	init_pair( COLOR_WHITE * 8 + COLOR_BLACK, COLOR_BLACK, COLOR_WHITE );
	init_pair( COLOR_WHITE * 8 + COLOR_RED, COLOR_RED, COLOR_WHITE );
	init_pair( COLOR_WHITE * 8 + COLOR_GREEN, COLOR_GREEN, COLOR_WHITE );
	init_pair( COLOR_WHITE * 8 + COLOR_YELLOW, COLOR_YELLOW, COLOR_WHITE );
	init_pair( COLOR_WHITE * 8 + COLOR_BLUE, COLOR_BLUE, COLOR_WHITE );
	init_pair( COLOR_WHITE * 8 + COLOR_MAGENTA, COLOR_MAGENTA, COLOR_WHITE );
	init_pair( COLOR_WHITE * 8 + COLOR_CYAN, COLOR_CYAN, COLOR_WHITE );
	init_pair( COLOR_WHITE * 8 + COLOR_WHITE, COLOR_WHITE, COLOR_WHITE );
	attrset( COLOR_PAIR( 7 ) );
	n_bEnabled = true;
	getmaxyx ( stdscr, n_iHeight, n_iWidth );
	if ( n_bUseMouse
			&& ! mousemask ( ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL ) )
		throw new HException ( __WHERE__, "mousemask ( ) returned 0", g_iErrNo );
	return;
	M_EPILOG
	}
	
void leave_curses( void )
	{
	M_PROLOG
	if ( ! n_bEnabled )
		throw new HException ( __WHERE__, "not in curses mode", g_iErrNo );
//	if ( ! mousemask ( 0, NULL ) )
//		throw new HException ( __WHERE__, "mousemask ( ) returned 0", g_iErrNo );
	use_default_colors ( );
	printw ( "" );
	fflush ( 0 );
	flushinp ( );
	intrflush ( stdscr, true );
	leaveok ( stdscr, false );
	immedok ( stdscr, true );
	refresh ( );
	nl ( );
	standend ( );
	keypad ( stdscr, false );
	nocbreak ( );
	curs_set ( 1 );
/*	reset_shell_mode ( ); */
/* see comment near def_shell_mode ( ), ( automagicly by endwin ( ) ) */
/*
	if ( n_psWindow )delwin ( n_psWindow );
	n_psWindow = NULL;
*/
	endwin ( );
	n_bEnabled = false;
	return ;
	M_EPILOG
	}
	
void set_attr( unsigned char a_ucAttr )
	{
	M_PROLOG
	if ( ! n_bEnabled )
		throw new HException ( __WHERE__, "not in curses mode", g_iErrNo );
	
	attrset ( COLOR_PAIR( ( ( a_ucAttr & 112 ) >> 1 ) | a_ucAttr & 7 ) | ( a_ucAttr & 8 ? A_BOLD : 0 ) | ( a_ucAttr & 128 ? A_BLINK : 0 ) );
	return ;
	M_EPILOG
	}
	
int c_printf ( int a_iRow, int a_iColumn, int a_iAttribute,
							 const char * a_pcFormat, ... )
	{
	M_PROLOG
	int l_iError = 0;
	int l_iOrigRow = 0;
	int l_iOrigColumn = 0;
	int l_iOrigAttribute = 0;
	int l_iOrigCursState = 0;
	va_list l_sAp;
	getyx ( stdscr, l_iOrigRow, l_iOrigColumn );
	l_iOrigAttribute = get_attr ( );
	leaveok ( stdscr, true );
	l_iOrigCursState = curs_set ( 0 );
#ifdef __DEBUG__
	if ( a_iColumn < 0 )
		{
		move ( a_iRow, 0 );
		clrtoeol ( );
		}
	else move ( a_iRow, a_iColumn );
#else /* __DEBUG__ */
	if ( ( a_iColumn < 0 ) || ( a_iColumn >= n_iWidth ) )
		throw new HException ( __WHERE__, "bad column.", a_iColumn );
#endif /* not __DEBUG__ */
	if ( ( a_iRow < 0 ) || ( a_iRow >= n_iHeight ) )
		throw new HException ( __WHERE__, "bad row.", a_iRow );
	set_attr ( a_iAttribute );
	va_start ( l_sAp, a_pcFormat );
	vwprintw ( stdscr, a_pcFormat, l_sAp );
	va_end ( l_sAp );
	move ( l_iOrigRow, l_iOrigColumn );
	curs_set ( l_iOrigCursState );
	leaveok ( stdscr, false );
	set_attr ( l_iOrigAttribute );
	return ( l_iError );
	M_EPILOG
	}

int get_key( void )
	{
	M_PROLOG
	int l_iKey = 0;
	int l_iChar = 0;
	if ( ! n_bEnabled )
		throw new HException ( __WHERE__, "not in curses mode", g_iErrNo );
	noecho();
	fflush( 0 );
	l_iKey = getch ( );
	if ( l_iKey == D_KEY_ESC )
		{
		nodelay ( stdscr, true );
		l_iKey = getch ( );
		nodelay ( stdscr, false );
		if ( l_iKey == ERR )l_iKey = D_KEY_ESC;
		else l_iKey = D_KEY_META_(l_iKey);
		}
	if ( l_iKey == D_KEY_CTRL_(n_cCommandComposeCharacter) )
		{
		c_printf ( console::n_iHeight - 1, -1, D_FG_WHITE, "ctrl-%c",
				n_cCommandComposeCharacter );
		timeout ( n_iCommandComposeDelay * 100 );
		l_iKey = getch ( );
		timeout ( -1 );
		if ( l_iKey == ERR )
			{
			l_iKey = D_KEY_CTRL_(n_cCommandComposeCharacter);
			c_printf ( console::n_iHeight - 1, 0, D_FG_LIGHTGRAY, "      " );
			}
		else
			{
			if ( l_iKey < D_KEY_ESC )l_iKey = D_KEY_COMMAND_( l_iChar = l_iKey + 96 );
			else if ( l_iKey == D_KEY_ESC )
				{
				nodelay ( stdscr, true );
				l_iKey = getch ( );
				nodelay ( stdscr, false );
				if ( l_iKey == ERR )l_iKey = D_KEY_COMMAND_(l_iChar = D_KEY_ESC);
				else l_iKey = D_KEY_COMMAND_(D_KEY_META_(l_iChar = l_iKey));
				}
			else l_iKey = D_KEY_COMMAND_(l_iChar = l_iKey);
			c_printf ( console::n_iHeight - 1, 6, D_FG_WHITE, " %c", l_iChar );
			}
		}
	echo ( );
	if ( l_iKey == 347 ) l_iKey = 360;
	else if ( ( l_iKey == 127 ) || ( l_iKey == 8 ) ) l_iKey = 263;
	return ( l_iKey );
	M_EPILOG
	}
	
int kbhit()
	{
	M_PROLOG
	int l_iKey;
	if ( ! n_bEnabled )
		throw new HException ( __WHERE__, "not in curses mode", g_iErrNo );
	nodelay( stdscr, true );
	l_iKey = get_key ( );
	nodelay( stdscr, false );
	if ( l_iKey == ERR ) return ( 0 );
	return ( l_iKey );
	M_EPILOG
	}
	
char get_attr( void )
	{
	M_PROLOG
	if ( ! n_bEnabled )
		throw new HException ( __WHERE__, "not in curses mode", g_iErrNo );
	attr_t l_xAttr;
	short l_hColor = 0;
	int l_iAttribute = 0;
	attr_get( & l_xAttr, & l_hColor, 0 );
	l_iAttribute = ( l_hColor << 1 ) & 56;
	l_iAttribute |= ( l_hColor & 7 );
	if ( l_xAttr & A_BOLD )l_iAttribute |= 8;
	if ( l_xAttr & A_BLINK )l_iAttribute |= 128;
	return ( l_iAttribute );
	M_EPILOG
	}
	
void clrscr()
	{
	M_PROLOG
	if ( ! n_bEnabled )
		throw new HException ( __WHERE__, "not in curses mode", g_iErrNo );
	leaveok( stdscr, false );
	clear();
	refresh();
	leaveok( stdscr, true );
	return ;
	M_EPILOG
	}

bool is_enabled ( void )
	{
	M_PROLOG
	return ( n_bEnabled );
	M_EPILOG
	}

}

extern "C"
{

extern void _init ( void );
extern void _fini ( void );

void set_color_bits ( int & a_riWord, int a_iBits, int a_iWhat )
	{
	int l_iMask = 0x000f;
	if ( a_iWhat )
		{
		a_iWhat <<= 2;
		a_iBits <<= a_iWhat;
		l_iMask <<= a_iWhat;
		}
	a_riWord &= ~ l_iMask;
	a_riWord |= a_iBits;
	return;
	}

int get_color_bits ( HString & a_roValue, int a_iWhat )
	{
	HString l_oStr = a_roValue.split ( " \t", a_iWhat );
	if ( l_oStr.is_empty ( ) )return ( 0 );
	if ( ! strcasecmp ( l_oStr, "BLACK" ) )return ( D_FG_BLACK );
	else if ( ! strcasecmp ( l_oStr, "RED" ) )return ( D_FG_RED );
	else if ( ! strcasecmp ( l_oStr, "GREEN" ) )return ( D_FG_GREEN );
	else if ( ! strcasecmp ( l_oStr, "BROWN" ) )return ( D_FG_BROWN );
	else if ( ! strcasecmp ( l_oStr, "BLUE" ) )return ( D_FG_BLUE );
	else if ( ! strcasecmp ( l_oStr, "MAGENTA" ) )return ( D_FG_MAGENTA );
	else if ( ! strcasecmp ( l_oStr, "CYAN" ) )return ( D_FG_CYAN );
	else if ( ! strcasecmp ( l_oStr, "LIGHTGRAY" ) )return ( D_FG_LIGHTGRAY );
	else if ( ! strcasecmp ( l_oStr, "GRAY" ) )return ( D_FG_GRAY );
	else if ( ! strcasecmp ( l_oStr, "BRGIHTRED" ) )return ( D_FG_BRIGHTRED );
	else if ( ! strcasecmp ( l_oStr, "BRIGHTGREEN" ) )return ( D_FG_BRIGHTGREEN );
	else if ( ! strcasecmp ( l_oStr, "YELLOW" ) )return ( D_FG_YELLOW );
	else if ( ! strcasecmp ( l_oStr, "BRIGHTBLUE" ) )return ( D_FG_BRIGHTBLUE );
	else if ( ! strcasecmp ( l_oStr, "BRIGHTMAGENTA" ) )
		return ( D_FG_BRIGHTMAGENTA );
	else if ( ! strcasecmp ( l_oStr, "BRIGHTCYAN" ) )return ( D_FG_BRIGHTCYAN );
	else if ( ! strcasecmp ( l_oStr, "WHITE" ) )return ( D_FG_WHITE );
	return ( 0 );
	}

void set_color ( HString & a_roValue, int & a_riAttribute )
	{
	int l_iCtr = 0;
	int l_piTab [ 4 ] = { 2, 3, 0, 1 };
	for ( l_iCtr = 0; l_iCtr < 4; l_iCtr ++ )
		set_color_bits ( a_riAttribute,
				get_color_bits ( a_roValue, l_iCtr ), l_piTab [ l_iCtr ] );
	return;
	}

bool set_console_variables ( HString & a_roOption, HString & a_roValue )
	{
	if ( ! strcasecmp ( a_roOption, "disabled_attribute" ) )
		set_color ( a_roValue, console::n_iDisabledAttribute );
	else if ( ! strcasecmp ( a_roOption, "enabled_attribute" ) )
		set_color ( a_roValue, console::n_iEnabledAttribute );
	else if ( ! strcasecmp ( a_roOption, "focused_attribute" ) )
		set_color ( a_roValue, console::n_iFocusedAttribute );
	else return ( true );
	return ( false );
	}

void console_init ( void )
	{
/* I do not know why but when _init ( ) is called explictly, like here would,
 * process hangs just before dbwrapper_fini ( ) */
/*	_init ( ); */
	g_iErrNo = 0;
	if ( sizeof ( int ) < 4 )
		{
		::log << "Your CPU or compiler does not support proper size of int." << endl;
		exit ( 0 );
		}
	rc_file::process_rc_file ( "stdhapi", "console",
			console::n_psVariables, set_console_variables );
	return;
	}

void console_fini ( void )
	{
	_fini ( );
	return;
	}

}

