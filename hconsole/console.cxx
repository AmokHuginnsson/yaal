/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	console.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib> /* getenv */
#include <cstring>
#include <cstdio>
#include <csignal>  /* signal handling */
#include <unistd.h>
#include <termios.h>
#include <libintl.h>

#undef ECHO

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "console.hxx"
#include "hcore/memory.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hcore.hxx"
#include "hcore/system.hxx"
#include "tools/tools.hxx"
#include "hconsole.hxx"

/* curses system header is poluted with huge number of macros
 * though it cannot be included at hte top together with rest of sane
 * system headers. */

#ifdef HAVE_CURSES_H
#	include <curses.h>
#elif defined ( HAVE_NCURSES_CURSES_H )
#	include <ncurses/curses.h>
#else /* HAVE_NCURSES_CURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_CURSES_H */

#undef COLORS

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

STATIC_ASSERT( KEY_CODES::SPECIAL_KEY > KEY_MAX );

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
struct ATTR {
	inline static int value( int const attr_ ) {
		return ( static_cast<int>( COLOR_PAIR(
						( ( attr_ & 0x70 ) >> 1 )             /* background */
						| ( attr_ & 0x07 ) )                  /* foreground */
					| ( ( attr_ & 0x08 ) ? A_BOLD : 0 )     /* brighter foreground */
					| ( ( attr_ & 0x80 ) ? A_BLINK : 0 ) ) ); /* brighter background */
	}
	inline static int value_fix( int const attr_ ) {
		if ( attr_ & 0x80 )
			return ( static_cast<int>( COLOR_PAIR(
							( (   attr_ & 0x07 ) << 3 )
						| ( ( attr_ & 0x70 ) >> 4 ) )
						| ( (   attr_ & 0x08 ) ? A_BLINK : 0 )
						| A_BOLD | A_REVERSE ) );
		return ( value( attr_ ) );
	}
};

char const* const bold = "\033[1m";
char const* const reset = "\033[0m";
char const* const black = "\033[0;30m";
char const* const red = "\033[0;31m";
char const* const green = "\033[0;32m";
char const* const brown = "\033[0;33m";
char const* const blue = "\033[0;34m";
char const* const magenta = "\033[0;35m";
char const* const cyan = "\033[0;36m";
char const* const lightgray = "\033[0;37m";
char const* const gray = "\033[1;30m";
char const* const brightred = "\033[1;31m";
char const* const brightgreen = "\033[1;32m";
char const* const yellow = "\033[1;33m";
char const* const brightblue = "\033[1;34m";
char const* const brightmagenta = "\033[1;35m";
char const* const brightcyan = "\033[1;36m";
char const* const white = "\033[1;37m";

/* public: */

/* private: */
WINDOW* _window = NULL;
bool	_enabled = false;
bool	_brokenBrightBackground = false;
termios	_termios;

int const C_OK = OK;
int const C_ERR = ERR;
bool _needRepaint_( false );

/* public: */

HConsole::HConsole( void ) : _initialized( false ), _width( 0 ), _height( 0 ), _mouseDes( 0 ), _event() {
	return;
}

HConsole::~HConsole( void ) {
	M_PROLOG
	if ( _initialized ) {
		if ( ! isendwin() )
			leave_curses();
		delwin( stdscr );
		delwin( curscr );
		delwin( newscr );
	}
	return;
	M_DESTRUCTOR_EPILOG
}

void HConsole::init( void ) {
	M_PROLOG
	_initialized = true;
	HSignalService& signalService = HSignalService::get_instance();
	signalService.register_handler( SIGWINCH, call( &HConsole::on_terminal_resize, this, _1 ) );
	signalService.register_handler( SIGQUIT, call( &HConsole::on_quit, this, _1 ) );
	signalService.register_handler( SIGTSTP, call( &HConsole::on_tstp, this, _1 ) );
	signalService.register_handler( SIGCONT, call( &HConsole::on_cont, this, _1 ) );
	signalService.register_handler( SIGUSR1, call( &HConsole::on_mouse, this, _1 ) );
	HSignalService::handler_t cleanup( call( &HConsole::console_cleanup, this, _1 ) );
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
	_event = make_pointer<HPipe>();
	M_EPILOG
}

void HConsole::enter_curses( void ) {
	M_PROLOG
	short colors[] = { COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
		COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE };
	int fg = 0, bg = 0;
	termios termios;
/*	def_shell_mode(); */
/* this is done automaticly by initscr(), read man next time */
	if ( ! isatty( STDIN_FILENO ) )
		M_THROW( "stdin in not a tty", 0 );
	if ( ! _initialized )
		init();
	if ( _disableXON_ ) {
		M_ENSURE( tcgetattr( STDIN_FILENO, &_termios ) == 0 );
		M_ENSURE( tcgetattr( STDIN_FILENO, &termios ) == 0 );
		termios.c_iflag &= ~IXON;
		if ( _leaveCtrlC_ )
			termios.c_cc [ VINTR ] = 0;
		if ( _leaveCtrlZ_ )
			termios.c_cc [ VSUSP ] = 0;
		if ( _leaveCtrlS_ )
			termios.c_cc [ VSTOP ] = 0;
		if ( _leaveCtrlQ_ )
			termios.c_cc [ VSTART ] = 0;
		if ( _leaveCtrlBackSlash_ )
			termios.c_cc [ VQUIT ] = 0;
		M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &termios ) == 0 );
	}
	use_env( true );
	if ( ! _window )
		_window = initscr();
	M_ENSURE( cbreak() != ERR );
	M_ENSURE( start_color() != ERR );
	standout(); /* Macro, returned value without meaning */
	M_ENSURE( nonl() == OK );
	M_ENSURE( keypad( stdscr, true ) != ERR );
	M_ENSURE( intrflush( stdscr, false ) != ERR );
	M_ENSURE( scrollok( stdscr, false ) != ERR );
	M_ENSURE( leaveok( stdscr, false ) != ERR );
	immedok( stdscr, false );
	M_ENSURE( fflush( NULL ) == 0 );
	flushinp(); /* Always returns OK */
	curs_set( CURSOR::INVISIBLE );
	M_ENSURE( refresh() != ERR );
	/* init color pairs */
	M_ENSURE( assume_default_colors( COLOR_BLACK, COLOR_BLACK ) == OK );
	for ( bg = 0; bg < 8; bg ++ )
		for ( fg = 0; fg < 8; fg ++ )
			init_pair( static_cast<short>( bg * 8 + fg ),
					colors[ fg ], colors[ bg ] );
	attrset( COLOR_PAIR( 7 ) );
	bkgd( ' ' | ATTR::value( COLORS::FG_BLACK | COLORS::BG_BLACK ) | A_INVIS ); /* meaningless value from macro */
	_brokenBrightBackground = ( ::getenv( "MRXVT_TABTITLE" ) != NULL );
	_enabled = true;
	getmaxyx( stdscr, _height, _width );
	if ( getenv( "YAAL_NO_MOUSE" ) )
		_useMouse_ = false;
	if ( _useMouse_ ) {
		if ( ::getenv( "DISPLAY" ) ) {
			mouse::mouse_open = mouse::x_mouse_open;
			mouse::mouse_get = mouse::x_mouse_get;
			mouse::mouse_close = mouse::x_mouse_close;
		} else {
			mouse::mouse_open = mouse::console_mouse_open;
			mouse::mouse_get = mouse::console_mouse_get;
			mouse::mouse_close = mouse::console_mouse_close;
		}
		if ( ( _mouseDes = mouse::mouse_open() ) < 0 )
			M_THROW( _( "mouse is console type"
						" and we did not recived file descriptor" ), errno );
		log( LOG_TYPE::INFO )
			<< ( ( mouse::mouse_open == mouse::x_mouse_open ) ? _( "using ncurses mouse support, at fd(" ) : _( "using console mouse support, at fd(" ) )
			<< _mouseDes << ')' << endl;
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
	
void HConsole::leave_curses( void ) {
	M_PROLOG
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	if ( _useMouse_ )
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
	if ( _window )
	delwin ( _window );
	_window = NULL;
*/
	M_ENSURE( endwin() == OK );
	if ( _disableXON_ )
		M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &_termios ) == 0 );
	_enabled = false;
	return;
	M_EPILOG
}
	
void HConsole::set_attr( int attr_ ) const {
	M_PROLOG
	char unsigned byte = 0;
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	byte = static_cast<char unsigned>( attr_ );
	if ( _brokenBrightBackground )
		static_cast<void>( attrset( ATTR::value_fix( byte ) ) );
	else
		static_cast<void>( attrset( ATTR::value( byte ) ) );
	return;
	M_EPILOG
}

int HConsole::c_move( int row_, int column_ ) {
	return ( ::move( row_, column_ ) );
}

CURSOR::cursor_t HConsole::curs_set( CURSOR::cursor_t const &cursor_ ) const {
	int cursor = ::curs_set( cursor_ == CURSOR::VISIBLE ? 1 : ( cursor_ == CURSOR::INVISIBLE ? 0 : 2 ) );
	if ( cursor == 1 )
		return ( CURSOR::VISIBLE );
	else if ( cursor == 2 )
		return ( CURSOR::VERY_VISIBLE );
	return ( CURSOR::INVISIBLE );
}

int HConsole::c_addch( int char_ ) {
	return ( ::addch( char_ ) );
}

int HConsole::c_refresh( void ) {
	return ( ::refresh() );
}

int HConsole::endwin ( void ) {
	return ( ::endwin() );
}

void HConsole::c_getmaxyx( void ) {
	M_PROLOG
	getmaxyx( stdscr, _height, _width );
	log( LOG_TYPE::INFO ) << "New terminal dimenstions: " << _height << "x" << _width << "." << endl;
	return;
	M_EPILOG
}

void HConsole::c_getyx( int& height_, int& width_ ) {
	getyx( stdscr, height_, width_ );
	return;
}

void HConsole::c_clrtoeol( void ) {
	::clrtoeol();
	return;
}

int HConsole::get_height( void ) const {
	return ( _height );
}

int HConsole::get_width( void ) const {
	return ( _width );
}

int HConsole::get_mouse_fd( void ) const {
	return ( _mouseDes );
}

int HConsole::get_event_fd( void ) const {
	return ( _event->get_reader_fd() );
}

int HConsole::c_vmvprintf( int row_, int column_,
							 char const* const format_, void* ap_ ) const {
	M_PROLOG
	int origRow = 0;
	int origColumn = 0;
	int error = 0;
	va_list& ap = *static_cast<va_list*>( ap_ );
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	if ( column_ >= _width )
		M_THROW( "bad column.", column_ );
	if ( ( row_ < 0 ) || ( row_ >= _height ) )
		M_THROW( "bad row.", row_ );
	getyx( stdscr, origRow, origColumn );
	if ( column_ < 0 ) {
		M_ENSURE( move( row_, 0 ) != ERR );
		clrtoeol(); /* Always OK */
	} else
		M_ENSURE( move( row_, column_ ) != ERR );
	error = vw_printw( stdscr, format_, ap );
	M_ENSURE( move( origRow, origColumn ) != ERR );
	return ( error );
	M_EPILOG
}

int HConsole::c_vcmvprintf( int row_, int column_, int attribute_,
							 char const* const format_, void* ap_ ) const {
	M_PROLOG
	int error = 0;
	int origAttribute = 0;
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	origAttribute = get_attr();
	set_attr( attribute_ );
	error = c_vmvprintf( row_, column_, format_, ap_ );
	set_attr( origAttribute );
	return ( error );
	M_EPILOG
}
	
int HConsole::c_vprintf ( char const* const format_, void* ap_ ) const {
	M_PROLOG
	int error = 0;
	va_list& ap = *static_cast<va_list*>( ap_ );
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	error = vw_printw( stdscr, format_, ap );
	return ( error );
	M_EPILOG
}

int HConsole::c_printf( char const* const format_, ... ) const {
	M_PROLOG
	int error = 0;
	va_list ap;
	va_start( ap, format_ );
	error = c_vprintf( format_, &ap );
	va_end( ap );
	return ( error );
	M_EPILOG
}

int HConsole::c_mvprintf( int row_, int column_, char const* const format_,
		... ) const {
	M_PROLOG
	int error = 0;
	va_list ap;
	va_start( ap, format_ );
	error = c_vmvprintf( row_, column_, format_, &ap );
	va_end( ap );
	return ( error );
	M_EPILOG
}

int HConsole::c_cmvprintf( int row_, int column_, int attribute_,
							 char const* const format_, ... ) const {
	M_PROLOG
	int error = 0;
	va_list ap;
	va_start( ap, format_ );
	error = c_vcmvprintf( row_, column_, attribute_, format_, &ap );
	va_end( ap );
	return ( error );
	M_EPILOG
}

int HConsole::ungetch( int code_ ) const {
	switch ( code_ ) {
		case ( KEY_CODES::PAGE_DOWN ): code_ = KEY_NPAGE;     break;
		case ( KEY_CODES::PAGE_UP ):   code_ = KEY_PPAGE;     break;
		case ( KEY_CODES::HOME ):      code_ = KEY_HOME;      break;
		case ( KEY_CODES::END ):       code_ = KEY_END;       break;
		case ( KEY_CODES::BACKSPACE ): code_ = KEY_BACKSPACE; break;
		case ( KEY_CODES::UP ):        code_ = KEY_UP;        break;
		case ( KEY_CODES::DOWN ):      code_ = KEY_DOWN;      break;
		case ( KEY_CODES::LEFT ):      code_ = KEY_LEFT;      break;
		case ( KEY_CODES::RIGHT ):     code_ = KEY_RIGHT;     break;
		case ( KEY_CODES::DELETE ):    code_ = KEY_DC;        break;
		case ( KEY_CODES::INSERT ):    code_ = KEY_IC;        break;
		case ( KEY_CODES::MOUSE ):     code_ = KEY_MOUSE;     break;
		default:
		break;
	}
	return ( ::ungetch( code_ ) );
}

int HConsole::get_key( void ) const {
	M_PROLOG
	CURSOR::cursor_t origCursState = CURSOR::INVISIBLE;
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	M_ENSURE( noecho() != ERR );
	M_ENSURE( ::fflush( NULL ) == 0 );
	int key( getch() );
	M_ASSERT( key < KEY_CODES::SPECIAL_KEY );
	if ( key == KEY_CODES::ESC ) {
		M_ENSURE( nodelay( stdscr, true ) != ERR );
		key = getch();
		M_ENSURE( nodelay( stdscr, false ) != ERR );
		if ( key == ERR )
			key = KEY_CODES::ESC;
		else
			key = KEY<>::meta_r( key );
	}
	if ( key == KEY<>::ctrl_r( _commandComposeCharacter_ ) ) {
		origCursState = curs_set( CURSOR::INVISIBLE );
		c_cmvprintf( _height - 1, -1, COLORS::FG_WHITE, "ctrl-%c",
					_commandComposeCharacter_ );
		timeout( _commandComposeDelay_ * 100 );
		key = getch();
		timeout( -1 );
		if ( key == ERR ) {
			key = KEY<>::ctrl_r( _commandComposeCharacter_ );
			c_cmvprintf( _height - 1, 0, COLORS::FG_LIGHTGRAY, "      " );
		} else {
			int character = 0;
			if ( key < KEY_CODES::ESC )
				key = KEY<>::command_r( character = key + 96 );
			else if ( key == KEY_CODES::ESC ) {
				M_ENSURE( nodelay( stdscr, true ) != ERR );
				key = getch();
				M_ENSURE( nodelay( stdscr, false ) != ERR );
				if ( key == ERR )
					key = KEY<>::command_r( character = KEY_CODES::ESC );
				else
					key = KEY<>::command_r( KEY<>::meta_r( character = key ) );
			} else
				key = KEY<>::command_r( character = key );
			c_cmvprintf( _height - 1, 6, COLORS::FG_WHITE, " %c", character );
		}
		curs_set( origCursState );
	}
	M_ENSURE( echo() != ERR );
	M_ASSERT( ( key < KEY_CODES::SPECIAL_KEY ) || ( key > KEY_CODES::META_BASE ) );
	switch ( key ) {
		case ( KEY_NPAGE ):     key = KEY_CODES::PAGE_DOWN; break;
		case ( KEY_PPAGE ):     key = KEY_CODES::PAGE_UP;   break;
		case ( KEY_HOME ):      key = KEY_CODES::HOME;      break;
		case ( 347 ):
		case ( KEY_END ):       key = KEY_CODES::END;       break;
		case ( 8 ):
		case ( 127 ):
		case ( KEY_BACKSPACE ): key = KEY_CODES::BACKSPACE; break;
		case ( KEY_UP ):        key = KEY_CODES::UP;        break;
		case ( KEY_DOWN ):      key = KEY_CODES::DOWN;      break;
		case ( KEY_LEFT ):      key = KEY_CODES::LEFT;      break;
		case ( KEY_RIGHT ):     key = KEY_CODES::RIGHT;     break;
		case ( KEY_DC ):        key = KEY_CODES::DELETE;    break;
		case ( KEY_IC ):        key = KEY_CODES::INSERT;    break;
		case ( KEY_MOUSE ):     key = KEY_CODES::MOUSE;     break;
		default:
		break;
	}
	return ( key );
	M_EPILOG
}
	
int HConsole::kbhit( void ) const {
	M_PROLOG
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	M_ENSURE( nodelay( stdscr, true ) != ERR );
	int key( get_key() );
	M_ENSURE( nodelay( stdscr, false ) != ERR );
	if ( key == ERR )
		return ( 0 );
	return ( key );
	M_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
template <typename T1, typename T2>
inline void fwd_attr_get( T1& val1_, T2& val2_, void* ) {
	static_cast<void>( attr_get( val1_, val2_, NULL ) ); /* Ugly macro */
}
#pragma GCC diagnostic error "-Wold-style-cast"
}
	
char unsigned HConsole::get_attr( void ) const {
	M_PROLOG
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	attr_t attr;
	NCURSES_ATTR_GET_SECOND_ARG_TYPE color = 0;
	/* A workaround for compiler warning regarding
	 * &attr and &color never being NULL. */
	attr_t* pa( &attr );
	NCURSES_ATTR_GET_SECOND_ARG_TYPE* pc( &color );
	fwd_attr_get( pa, pc, NULL );
	int unsigned attribute = ( color << 1 ) & 56;
	attribute |= ( color & 7 );
	if ( attr & A_BOLD )
		attribute |= 8;
	if ( attr & A_BLINK )
		attribute |= 128;
	return ( static_cast<char unsigned>( attribute ) );
	M_EPILOG
}
	
void HConsole::clrscr( void ) const {
	M_PROLOG
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	clear(); /* Always returns OK */
	M_ENSURE( refresh() != ERR );
	return;
	M_EPILOG
}

bool HConsole::is_enabled( void ) const {
	M_PROLOG
	return ( _enabled );
	M_EPILOG
}

int HConsole::wait_for_user_input( int& key_, mouse::OMouse& mouse_, int timeOut_ ) const {
	M_PROLOG
	int fds[2] = { STDIN_FILENO, _mouseDes };
	int long wait( timeOut_ );
	int error( hcore::system::wait_for_io( fds, _mouseDes ? 2 : 1, NULL, 0, timeOut_ ? &wait : NULL ) );
	int eventType( 0 );
	if ( error > 0 ) {
		if ( fds[0] != -1 ) {
			key_ = get_key(), eventType = EVENT::KEYBOARD;
			if ( key_ == KEY_MOUSE )
				eventType = 0;
		}
		if ( ( key_ == KEY_MOUSE )
				|| ( _mouseDes && ( fds[1] != -1 ) ) )
			eventType |= EVENT::MOUSE, static_cast<void>( mouse::mouse_get( mouse_ ) );
	}
	return ( eventType );
	M_EPILOG
}

void HConsole::bell( void ) const {
	M_PROLOG
	M_ENSURE( putchar( '\a' ) == '\a' );
	return;
	M_EPILOG
}

int HConsole::on_terminal_resize( int signum_ ) {
	M_PROLOG
	HString message;
	message = "Terminal size changed: ";
	message += strsignal( signum_ );
	message += '.';
	log << message << endl;
	if ( is_enabled() )
		*_event << 'r';
	else
		::fprintf( stderr, "\n%s", message.raw() );
	return ( 0 );
	M_EPILOG
}

int HConsole::console_cleanup( int sigNo_ ) {
	M_PROLOG
	if ( ( sigNo_ == SIGINT ) && ( tools::_ignoreSignalSIGINT_ ) )
		return ( 0 );
	if ( is_enabled() )
		leave_curses();
	return ( 0 );
	M_EPILOG
}

int HConsole::on_quit( int ) {
	M_PROLOG
	if ( is_enabled() ) {
		if ( tools::_ignoreSignalSIGQUIT_ )
			c_cmvprintf ( get_height() - 1, 0, COLORS::FG_BRIGHTRED,
					"Hard Quit is disabled by yaal configuration." );
		else
			leave_curses();
	}
	return ( 0 );
	M_EPILOG
}

int HConsole::on_tstp( int ) {
	M_PROLOG
	if ( is_enabled() ) {
		if ( tools::_ignoreSignalSIGTSTP_ )
			c_cmvprintf ( get_height() - 1, 0, COLORS::FG_BRIGHTRED,
					"Suspend is disabled by yaal configuration." );
		else
			leave_curses();
	}
	return ( 0 );
	M_EPILOG
}

int HConsole::on_cont( int ) {
	M_PROLOG
	if ( ! is_enabled() )
		enter_curses();
	if ( is_enabled() )
		*_event << 'r';
	return ( 0 );
	M_EPILOG
}

int HConsole::on_mouse( int ) {
	M_PROLOG
	if ( _useMouse_ ) {
		if ( is_enabled() ) {
			*_event << 'm';
			return ( 1 );
		}
	}
	if ( is_enabled() )
		leave_curses();
	return ( 0 );
	M_EPILOG
}

void HConsole::set_escdelay( int escdelay_ ) {
	M_PROLOG
	set_env( "ESCDELAY", escdelay_ );
	return;
	M_EPILOG
}

}

}

