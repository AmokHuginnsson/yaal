/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	console.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib> /* getenv */
#include <cstdio>
#include <csignal>  /* signal handling */
#include <unistd.h>
#include <termios.h>
#include <sys/time.h> /* timeval */
#include <libintl.h>

#undef ECHO

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hcore/hlog.hxx"
#include "tools/tools.hxx"
#include "hconsole.hxx"
#include "console.hxx"

#ifdef HAVE_CURSES_H
#	include <curses.h>
#elif defined ( HAVE_NCURSES_CURSES_H )
#	include <ncurses/curses.h>
#else /* HAVE_NCURSES_CURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_CURSES_H */

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal
{

namespace hconsole
{

int GLYPHS::DOWN_ARROW, GLYPHS::UP_ARROW, GLYPHS::VERTICAL_LINE;

/* Bbbbffff
 * B - blink
 * b - background color bits
 * f - foreground color bits
 * 112 (0x70) - 01110000
 * 7   (0x07) - 00000111
 * 8   (0x08) - 00001000
 * 128 (0x80) - 10000000
 */
struct ATTR
	{
	inline static int value( int const a_iAttr )
		{
		return ( static_cast<int>( COLOR_PAIR(
						( ( a_iAttr & 0x70 ) >> 1 )             /* background */
						| ( a_iAttr & 0x07 ) )                  /* foreground */
					| ( ( a_iAttr & 0x08 ) ? A_BOLD : 0 )     /* brighter foreground */
					| ( ( a_iAttr & 0x80 ) ? A_BLINK : 0 ) ) ); /* brighter background */
		}
	inline static int value_fix( int const a_iAttr )
		{
		if ( a_iAttr & 0x80 )
			return ( static_cast<int>( COLOR_PAIR(
							( (   a_iAttr & 0x07 ) << 3 )
						| ( ( a_iAttr & 0x70 ) >> 4 ) )
						| ( (   a_iAttr & 0x08 ) ? A_BLINK : 0 )
						| A_BOLD | A_REVERSE ) );
		return ( value( a_iAttr ) );
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

/* private: */
WINDOW* f_psWindow = NULL;
bool	f_bEnabled = false;
bool	f_bBrokenBrightBackground = false;
termios	f_sTermios;

int const C_OK = OK;
int const C_ERR = ERR;
bool n_bNeedRepaint( false );

/* public: */

HConsole::HConsole( void ) : f_bInitialized( false ), f_iWidth( 0 ), f_iHeight( 0 ), f_iMouseDes( 0 ), f_oEvent()
	{
	return;
	}

HConsole::~HConsole( void )
	{
	return;
	}

void HConsole::init( void )
	{
	M_PROLOG
	f_bInitialized = true;
	HSignalService& signalService = HSignalServiceFactory::get_instance();
	signalService.register_handler( SIGWINCH,
			HSignalService::HHandlerGeneric::ptr_t( new HSignalService::HHandlerExternal( this, &HConsole::on_terminal_resize ) ) );
	signalService.register_handler( SIGQUIT,
			HSignalService::HHandlerGeneric::ptr_t( new HSignalService::HHandlerExternal( this, &HConsole::on_quit ) ) );
	signalService.register_handler( SIGTSTP,
			HSignalService::HHandlerGeneric::ptr_t( new HSignalService::HHandlerExternal( this, &HConsole::on_tstp ) ) );
	signalService.register_handler( SIGCONT,
			HSignalService::HHandlerGeneric::ptr_t( new HSignalService::HHandlerExternal( this, &HConsole::on_cont ) ) );
	signalService.register_handler( SIGUSR1,
			HSignalService::HHandlerGeneric::ptr_t( new HSignalService::HHandlerExternal( this, &HConsole::on_mouse ) ) );
	HSignalService::HHandlerGeneric::ptr_t cleanup( new HSignalService::HHandlerExternal( this, &HConsole::console_cleanup ) );
	signalService.register_handler( SIGINT, cleanup );
	signalService.register_handler( SIGTERM, cleanup );
	signalService.register_handler( SIGSEGV, cleanup );
	signalService.register_handler( SIGBUS, cleanup );
	signalService.register_handler( SIGABRT, cleanup );
	signalService.register_handler( SIGILL, cleanup );
	signalService.register_handler( SIGFPE, cleanup );
#if defined( HAVE_DECL_SIGIOT ) && ( HAVE_DECL_SIGIOT == 1 )
	signalService.register_handler( SIGIOT, cleanup );
#endif /* HAVE_DECL_SIGIOT */
	signalService.register_handler( SIGTRAP, cleanup );
	signalService.register_handler( SIGSYS, cleanup );
	signalService.register_handler( SIGPIPE, cleanup );
	f_oEvent = HPipe::ptr_t( new HPipe );
	M_EPILOG
	}

void HConsole::enter_curses( void )
	{
	M_PROLOG
	short l_piColors[] = { COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
		COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE };
	int l_iFg = 0, l_iBg = 0;
	termios l_sTermios;
/*	def_shell_mode(); */
/* this is done automaticly by initscr(), read man next time */
	if ( ! isatty( STDIN_FILENO ) )
		M_THROW( "stdin in not a tty", 0 );
	if ( ! f_bInitialized )
		init();
	if ( n_bDisableXON )
		{
		M_ENSURE( tcgetattr( STDIN_FILENO, &f_sTermios ) == 0 );
		M_ENSURE( tcgetattr( STDIN_FILENO, &l_sTermios ) == 0 );
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
		M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &l_sTermios ) == 0 );
		}
	use_env( true );
	if ( ! f_psWindow )
		f_psWindow = initscr();
	M_ENSURE( cbreak() != ERR );
	M_ENSURE( start_color() != ERR );
	standout(); /* Macro, returned value without meaning */
	M_ENSURE( nonl() == OK );
	M_ENSURE( keypad ( stdscr, true ) != ERR );
	M_ENSURE( intrflush ( stdscr, false ) != ERR );
	M_ENSURE( scrollok ( stdscr, false ) != ERR );
	M_ENSURE( leaveok ( stdscr, false ) != ERR );
	immedok( stdscr, false );
	M_ENSURE( fflush ( NULL ) == 0 );
	flushinp(); /* Always returns OK */
	curs_set( CURSOR::INVISIBLE );
	M_ENSURE( refresh() != ERR );
	/* init color pairs */
	M_ENSURE( assume_default_colors( COLOR_BLACK, COLOR_BLACK ) == OK );
	for ( l_iBg = 0; l_iBg < 8; l_iBg ++ )
		for ( l_iFg = 0; l_iFg < 8; l_iFg ++ )
			init_pair( static_cast<short>( l_iBg * 8 + l_iFg ),
					l_piColors[ l_iFg ], l_piColors[ l_iBg ] );
	attrset( COLOR_PAIR( 7 ) );
	bkgd( ' ' | ATTR::value( COLORS::FG_BLACK | COLORS::BG_BLACK ) | A_INVIS ); /* meaningless value from macro */
	f_bBrokenBrightBackground = ( ::getenv( "MRXVT_TABTITLE" ) != NULL );
	f_bEnabled = true;
	getmaxyx( stdscr, f_iHeight, f_iWidth );
	if ( getenv( "YAAL_NO_MOUSE" ) )
		n_bUseMouse = false;
	if ( n_bUseMouse )
		{
		if ( ::getenv( "DISPLAY" ) )
			{
			log( LOG_TYPE::INFO ) << _( "using ncurses mouse support" ) << endl;
			mouse::mouse_open = mouse::x_mouse_open;
			mouse::mouse_get = mouse::x_mouse_get;
			mouse::mouse_close = mouse::x_mouse_close;
			}
		else
			{
			log( LOG_TYPE::INFO ) << _( "using console mouse support" ) << endl;
			mouse::mouse_open = mouse::console_mouse_open;
			mouse::mouse_get = mouse::console_mouse_get;
			mouse::mouse_close = mouse::console_mouse_close;
			}
		if ( ( f_iMouseDes = mouse::mouse_open() ) < 0 )
			M_THROW( _( "mouse is console type"
						" and we did not recived file descriptor" ), errno );
		}
#ifdef HAVE_ASCII_GRAPHICS
	GLYPHS::DOWN_ARROW		= static_cast<int>( ACS_DARROW );
	GLYPHS::UP_ARROW			= static_cast<int>( ACS_UARROW );
	GLYPHS::VERTICAL_LINE	= static_cast<int>( ACS_VLINE );
#else /* than HAVE_ASCII_GRAPHICS */
	GLYPHS::DOWN_ARROW		= 'v';
	GLYPHS::UP_ARROW			= '^';
	GLYPHS::VERTICAL_LINE	= '|';
#endif /* not HAVE_ASCII_GRAPHICS */
	return;
	M_EPILOG
	}
	
void HConsole::leave_curses( void )
	{
	M_PROLOG
	if ( ! f_bEnabled )
		M_THROW( "not in curses mode", errno );
	if ( n_bUseMouse )
		static_cast<void>( mouse::mouse_close() );
	bkgd( ' ' | ATTR::value( COLORS::FG_LIGHTGRAY | COLORS::BG_BLACK ) );
	M_ENSURE( use_default_colors() == OK );
	M_ENSURE( printw ( "" ) != ERR );
	M_ENSURE( fflush ( NULL ) == 0 );
	flushinp(); /* Always returns OK */
	M_ENSURE( intrflush ( stdscr, true ) != ERR );
	leaveok( stdscr, false ); /* Always OK */
	immedok( stdscr, true ); /* Always OK */
	M_ENSURE( refresh() != ERR );
	nl(); /* Always OK */
	standend();
	M_ENSURE( keypad ( stdscr, false ) != ERR );
	M_ENSURE( nocbreak() != ERR );
	curs_set( CURSOR::VISIBLE );
/*	reset_shell_mode(); */
/* see comment near def_shell_mode(), ( automagicly by endwin() ) */
/*
	if ( f_psWindow )
	delwin ( f_psWindow );
	f_psWindow = NULL;
*/
	M_ENSURE( endwin() == OK );
	if ( n_bDisableXON )
		M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &f_sTermios ) == 0 );
	f_bEnabled = false;
	return;
	M_EPILOG
	}
	
void HConsole::set_attr( int a_iAttr ) const
	{
	M_PROLOG
	char unsigned l_ucByte = 0;
	if ( ! f_bEnabled )
		M_THROW( "not in curses mode", errno );
	l_ucByte = static_cast<char unsigned>( a_iAttr );
	if ( f_bBrokenBrightBackground )
		attrset( ATTR::value_fix( l_ucByte ) );
	else
		attrset( ATTR::value( l_ucByte ) );
	return;
	M_EPILOG
	}

int HConsole::c_move( int const& a_iRow, int const& a_iColumn )
	{
	return ( ::move( a_iRow, a_iColumn ) );
	}

CURSOR::cursor_t HConsole::curs_set( CURSOR::cursor_t const &a_eCursor ) const
	{
	int l_iCursor = ::curs_set( a_eCursor == CURSOR::VISIBLE ? 1 : ( a_eCursor == CURSOR::INVISIBLE ? 0 : 2 ) );
	if ( l_iCursor == 1 )
		return ( CURSOR::VISIBLE );
	else if ( l_iCursor == 2 )
		return ( CURSOR::VERY_VISIBLE );
	return ( CURSOR::INVISIBLE );
	}

int HConsole::c_addch( int const& a_iChar )
	{
	return ( ::addch( a_iChar ) );
	}

int HConsole::c_refresh( void )
	{
	return ( ::refresh() );
	}

int HConsole::endwin ( void )
	{
	return ( ::endwin() );
	}

void HConsole::c_getmaxyx( void )
	{
	M_PROLOG
	getmaxyx( stdscr, f_iHeight, f_iWidth );
	log( LOG_TYPE::INFO ) << "New terminal dimenstions: " << f_iHeight << "x" << f_iWidth << "." << endl;
	return;
	M_EPILOG
	}

void HConsole::c_getyx( int& a_riHeight, int& a_riWidth )
	{
	getyx( stdscr, a_riHeight, a_riWidth );
	return;
	}

void HConsole::c_clrtoeol( void )
	{
	::clrtoeol();
	return;
	}

int const& HConsole::get_height( void ) const
	{
	return ( f_iHeight );
	}

int const& HConsole::get_width( void ) const
	{
	return ( f_iWidth );
	}

int HConsole::get_mouse_fd( void ) const
	{
	return ( f_iMouseDes );
	}

int HConsole::get_event_fd( void ) const
	{
	return ( f_oEvent->get_reader_fd() );
	}

int HConsole::c_vmvprintf( int a_iRow, int a_iColumn,
							 char const* const a_pcFormat, void* a_pxAp ) const
	{
	M_PROLOG
	int l_iOrigRow = 0;
	int l_iOrigColumn = 0;
	int l_iError = 0;
	va_list& l_rxAp = *static_cast<va_list*>( a_pxAp );
	if ( ! f_bEnabled )
		M_THROW( "not in curses mode", errno );
	if ( a_iColumn >= f_iWidth )
		M_THROW( "bad column.", a_iColumn );
	if ( ( a_iRow < 0 ) || ( a_iRow >= f_iHeight ) )
		M_THROW( "bad row.", a_iRow );
	getyx( stdscr, l_iOrigRow, l_iOrigColumn );
	if ( a_iColumn < 0 )
		{
		M_ENSURE( move( a_iRow, 0 ) != ERR );
		clrtoeol(); /* Always OK */
		}
	else
		M_ENSURE( move( a_iRow, a_iColumn ) != ERR );
	l_iError = vw_printw ( stdscr, a_pcFormat, l_rxAp );
	M_ENSURE( move( l_iOrigRow, l_iOrigColumn ) != ERR );
	return ( l_iError );
	M_EPILOG
	}

int HConsole::c_vcmvprintf( int a_iRow, int a_iColumn, int a_iAttribute,
							 char const* const a_pcFormat, void* a_pxAp ) const
	{
	M_PROLOG
	int l_iError = 0;
	int l_iOrigAttribute = 0;
	if ( ! f_bEnabled )
		M_THROW( "not in curses mode", errno );
	l_iOrigAttribute = get_attr();
	set_attr( a_iAttribute );
	l_iError = c_vmvprintf( a_iRow, a_iColumn, a_pcFormat, a_pxAp );
	set_attr( l_iOrigAttribute );
	return ( l_iError );
	M_EPILOG
	}
	
int HConsole::c_vprintf ( char const* const a_pcFormat, void* a_pxAp ) const
	{
	M_PROLOG
	int l_iError = 0;
	va_list& l_rxAp = *static_cast<va_list*>( a_pxAp );
	if ( ! f_bEnabled )
		M_THROW( "not in curses mode", errno );
	l_iError = vw_printw( stdscr, a_pcFormat, l_rxAp );
	return ( l_iError );
	M_EPILOG
	}

int HConsole::c_printf( char const* const a_pcFormat, ... ) const
	{
	M_PROLOG
	int l_iError = 0;
	va_list l_xAp;
	va_start( l_xAp, a_pcFormat );
	l_iError = c_vprintf( a_pcFormat, &l_xAp );
	va_end( l_xAp );
	return ( l_iError );
	M_EPILOG
	}

int HConsole::c_mvprintf( int a_iRow, int a_iColumn, char const* const a_pcFormat,
		... ) const
	{
	M_PROLOG
	int l_iError = 0;
	va_list l_xAp;
	va_start( l_xAp, a_pcFormat );
	l_iError = c_vmvprintf( a_iRow, a_iColumn, a_pcFormat, &l_xAp );
	va_end( l_xAp );
	return ( l_iError );
	M_EPILOG
	}

int HConsole::c_cmvprintf( int a_iRow, int a_iColumn, int a_iAttribute,
							 char const* const a_pcFormat, ... ) const
	{
	M_PROLOG
	int l_iError = 0;
	va_list l_xAp;
	va_start( l_xAp, a_pcFormat );
	l_iError = c_vcmvprintf( a_iRow, a_iColumn, a_iAttribute, a_pcFormat, &l_xAp );
	va_end( l_xAp );
	return ( l_iError );
	M_EPILOG
	}

int HConsole::ungetch( int a_iCode ) const
	{
	return ( ::ungetch( a_iCode ) );
	}

int HConsole::get_key( void ) const
	{
	M_PROLOG
	int l_iKey = 0;
	int l_iChar = 0;
	CURSOR::cursor_t l_eOrigCursState = CURSOR::INVISIBLE;
	if ( ! f_bEnabled )
		M_THROW ( "not in curses mode", errno );
	M_ENSURE ( noecho() != ERR );
	M_ENSURE ( fflush( NULL ) == 0 );
	l_iKey = getch();
	if ( l_iKey == KEY_CODES::ESC )
		{
		M_ENSURE ( nodelay ( stdscr, true ) != ERR );
		l_iKey = getch();
		M_ENSURE ( nodelay ( stdscr, false ) != ERR );
		if ( l_iKey == ERR )
			l_iKey = KEY_CODES::ESC;
		else
			l_iKey = KEY<>::meta_r ( l_iKey );
		}
	if ( l_iKey == KEY<>::ctrl_r ( n_cCommandComposeCharacter ) )
		{
		l_eOrigCursState = curs_set ( CURSOR::INVISIBLE );
		c_cmvprintf ( f_iHeight - 1, -1, COLORS::FG_WHITE, "ctrl-%c",
					n_cCommandComposeCharacter );
		timeout ( n_iCommandComposeDelay * 100 );
		l_iKey = getch();
		timeout ( -1 );
		if ( l_iKey == ERR )
			{
			l_iKey = KEY<>::ctrl_r ( n_cCommandComposeCharacter );
			c_cmvprintf ( f_iHeight - 1, 0, COLORS::FG_LIGHTGRAY, "      " );
			}
		else
			{
			if ( l_iKey < KEY_CODES::ESC )
				l_iKey = KEY<>::command_r ( l_iChar = l_iKey + 96 );
			else if ( l_iKey == KEY_CODES::ESC )
				{
				M_ENSURE ( nodelay ( stdscr, true ) != ERR );
				l_iKey = getch();
				M_ENSURE ( nodelay ( stdscr, false ) != ERR );
				if ( l_iKey == ERR )
					l_iKey = KEY<>::command_r (l_iChar = KEY_CODES::ESC);
				else
					l_iKey = KEY<>::command_r (KEY<>::meta_r ( l_iChar = l_iKey ) );
				}
			else
				l_iKey = KEY<>::command_r ( l_iChar = l_iKey );
			c_cmvprintf ( f_iHeight - 1, 6, COLORS::FG_WHITE, " %c", l_iChar );
			}
		curs_set ( l_eOrigCursState );
		}
	M_ENSURE ( echo() != ERR );
	switch ( l_iKey )
		{
		case ( KEY_NPAGE ):			l_iKey = KEY_CODES::PAGE_DOWN;	break;
		case ( KEY_PPAGE ):			l_iKey = KEY_CODES::PAGE_UP;		break;
		case ( KEY_HOME ):			l_iKey = KEY_CODES::HOME;				break;
		case ( 347 ):
		case ( KEY_END ):				l_iKey = KEY_CODES::END;				break;
		case ( 8 ):
		case ( 127 ):
		case ( KEY_BACKSPACE ):	l_iKey = KEY_CODES::BACKSPACE;	break;
		case ( KEY_UP ):				l_iKey = KEY_CODES::UP;					break;
		case ( KEY_DOWN ):			l_iKey = KEY_CODES::DOWN;				break;
		case ( KEY_LEFT ):			l_iKey = KEY_CODES::LEFT;				break;
		case ( KEY_RIGHT ):			l_iKey = KEY_CODES::RIGHT;			break;
		case ( KEY_DC ):				l_iKey = KEY_CODES::DELETE;			break;
		case ( KEY_IC ):				l_iKey = KEY_CODES::INSERT;			break;
		case ( KEY_MOUSE ):			l_iKey = KEY_CODES::MOUSE;			break;
		default:
		break;
		}
	return ( l_iKey );
	M_EPILOG
	}
	
int HConsole::kbhit( void ) const
	{
	M_PROLOG
	int l_iKey;
	if ( ! f_bEnabled )
		M_THROW ( "not in curses mode", errno );
	M_ENSURE ( nodelay( stdscr, true ) != ERR );
	l_iKey = get_key();
	M_ENSURE ( nodelay( stdscr, false ) != ERR );
	if ( l_iKey == ERR )
		return ( 0 );
	return ( l_iKey );
	M_EPILOG
	}
	
char unsigned HConsole::get_attr( void ) const
	{
	M_PROLOG
	if ( ! f_bEnabled )
		M_THROW( "not in curses mode", errno );
	attr_t l_xAttr;
	attr_t* pa = &l_xAttr;
	int l_hColor = 0;
	int* pc = &l_hColor;
	static_cast<void>( attr_get( pa, pc, NULL ) ); /* Ugly macro */
	int unsigned l_uiAttribute = ( l_hColor << 1 ) & 56;
	l_uiAttribute |= ( l_hColor & 7 );
	if ( l_xAttr & A_BOLD )
		l_uiAttribute |= 8;
	if ( l_xAttr & A_BLINK )
		l_uiAttribute |= 128;
	return ( static_cast<char unsigned>( l_uiAttribute ) );
	M_EPILOG
	}
	
void HConsole::clrscr( void ) const
	{
	M_PROLOG
	if ( ! f_bEnabled )
		M_THROW ( "not in curses mode", errno );
	clear(); /* Always returns OK */
	M_ENSURE ( refresh() != ERR );
	return;
	M_EPILOG
	}

bool HConsole::is_enabled( void ) const
	{
	M_PROLOG
	return ( f_bEnabled );
	M_EPILOG
	}

int HConsole::wait_for_user_input( int& a_iKey, mouse::OMouse& a_rsMouse,
		int a_iTimeOutSec, int a_iTimeOutUsec ) const
	{
	M_PROLOG
	int l_iError = - 1, l_iEventType = 0;
	timeval l_xWait;
	fd_set l_xFdSet;
	l_xWait.tv_sec = a_iTimeOutSec;
	l_xWait.tv_usec = a_iTimeOutUsec;
	FD_ZERO ( & l_xFdSet );
	FD_SET ( STDIN_FILENO, & l_xFdSet );
	if ( f_iMouseDes )
		FD_SET ( f_iMouseDes, & l_xFdSet );
	do
		{
		l_iError = ::select( FD_SETSIZE, & l_xFdSet, NULL, NULL,
				( a_iTimeOutSec || a_iTimeOutUsec ) ? & l_xWait : NULL );
		}
	while ( ( l_iError == -1 ) && ( errno == EINTR ) );
	if ( l_iError > 0 )
		{
		if ( FD_ISSET ( STDIN_FILENO, & l_xFdSet ) )
			{
			a_iKey = get_key(), l_iEventType = EVENT::KEYBOARD;
			if ( a_iKey == KEY_MOUSE )
				l_iEventType = 0;
			}
		if ( ( a_iKey == KEY_MOUSE )
				|| ( f_iMouseDes && FD_ISSET ( f_iMouseDes, & l_xFdSet ) ) )
			l_iEventType |= EVENT::MOUSE, static_cast < void > ( mouse::mouse_get ( a_rsMouse ) );
		}
	return ( l_iEventType );
	M_EPILOG
	}

void HConsole::bell( void ) const
	{
	M_PROLOG
	M_ENSURE ( putchar ( '\a' ) == '\a' );
	return;
	M_EPILOG
	}

int HConsole::on_terminal_resize( int a_iSignum )
	{
	M_PROLOG
	HString l_oMessage;
	l_oMessage = "Terminal size changed: ";
	l_oMessage += strsignal( a_iSignum );
	l_oMessage += '.';
	log << l_oMessage << endl;
	if ( is_enabled() )
		*f_oEvent << 'r';
	else
		::fprintf( stderr, "\n%s", l_oMessage.raw() );
	return ( 0 );
	M_EPILOG
	}

int HConsole::console_cleanup( int a_iSigNo )
	{
	M_PROLOG
	if ( ( a_iSigNo == SIGINT ) && ( tools::n_bIgnoreSignalSIGINT ) )
		return ( 0 );
	if ( is_enabled() )
		leave_curses();
	return ( 0 );
	M_EPILOG
	}

int HConsole::on_quit( int )
	{
	M_PROLOG
	if ( is_enabled() )
		{
		if ( tools::n_bIgnoreSignalSIGQUIT )
			c_cmvprintf ( get_height() - 1, 0, COLORS::FG_BRIGHTRED,
					"Hard Quit is disabled by yaal configuration." );
		else
			leave_curses();
		}
	return ( 0 );
	M_EPILOG
	}

int HConsole::on_tstp( int )
	{
	M_PROLOG
	if ( is_enabled() )
		{
		if ( tools::n_bIgnoreSignalSIGTSTP )
			c_cmvprintf ( get_height() - 1, 0, COLORS::FG_BRIGHTRED,
					"Suspend is disabled by yaal configuration." );
		else
			leave_curses();
		}
	return ( 0 );
	M_EPILOG
	}

int HConsole::on_cont( int )
	{
	M_PROLOG
	if ( ! is_enabled() )
		enter_curses();
	if ( is_enabled() )
		*f_oEvent << 'r';
	return ( 0 );
	M_EPILOG
	}

int HConsole::on_mouse( int )
	{
	M_PROLOG
	if ( n_bUseMouse )
		{
		if ( is_enabled() )
			{
			*f_oEvent << 'm';
			return ( 1 );
			}
		}
	if ( is_enabled() )
		leave_curses();
	return ( 0 );
	M_EPILOG
	}

}

}

