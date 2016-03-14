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
#include <csignal>  /* signal handling */
#include <unistd.h>
#include <libintl.h>

#undef ECHO

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "console.hxx"
#include "hcore/memory.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hcore.hxx"
#include "hcore/numeric.hxx"
#include "hcore/system.hxx"
#include "tools/tools.hxx"
#include "tools/hterminal.hxx"
#include "hconsole.hxx"

/* curses system header is polluted with huge number of macros
 * though it cannot be included at the top together with rest of sane
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
using namespace yaal::meta;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

STATIC_ASSERT( KEY_CODES::SPECIAL_KEY > KEY_MAX );

#ifdef HAVE_ASCII_GRAPHICS
int GLYPHS::ARROW::DOWN;
int GLYPHS::ARROW::UP;
int GLYPHS::ARROW::LEFT;
int GLYPHS::ARROW::RIGHT;
int GLYPHS::ARROW::HORIZONTAL;
int GLYPHS::ARROW::VERTICAL;

int GLYPHS::CHECKER::SPARSE;
int GLYPHS::CHECKER::MEDIUM;
int GLYPHS::CHECKER::DENSE;

int GLYPHS::LINE::SINGLE::VERTICAL;
int GLYPHS::LINE::SINGLE::HORIZONTAL;
int GLYPHS::LINE::SINGLE::BOTTOM_LEFT;
int GLYPHS::LINE::SINGLE::BOTTOM_RIGHT;
int GLYPHS::LINE::SINGLE::TOP_LEFT;
int GLYPHS::LINE::SINGLE::TOP_RIGHT;
int GLYPHS::LINE::SINGLE::VERTICAL_LEFT;
int GLYPHS::LINE::SINGLE::VERTICAL_RIGHT;
int GLYPHS::LINE::SINGLE::HORIZONTAL_TOP;
int GLYPHS::LINE::SINGLE::HORIZONTAL_BOTTOM;
int GLYPHS::LINE::SINGLE::CROSS;

int GLYPHS::LINE::DOUBLE::VERTICAL;
int GLYPHS::LINE::DOUBLE::HORIZONTAL;
int GLYPHS::LINE::DOUBLE::BOTTOM_LEFT;
int GLYPHS::LINE::DOUBLE::BOTTOM_RIGHT;
int GLYPHS::LINE::DOUBLE::TOP_LEFT;
int GLYPHS::LINE::DOUBLE::TOP_RIGHT;
int GLYPHS::LINE::DOUBLE::VERTICAL_LEFT;
int GLYPHS::LINE::DOUBLE::VERTICAL_RIGHT;
int GLYPHS::LINE::DOUBLE::HORIZONTAL_TOP;
int GLYPHS::LINE::DOUBLE::HORIZONTAL_BOTTOM;
int GLYPHS::LINE::DOUBLE::CROSS;

int GLYPHS::MATH::NOT_EQUAL;
int GLYPHS::MATH::LESS_EQUAL;
int GLYPHS::MATH::GREATER_EQUAL;
int GLYPHS::MATH::APPROXIMATELY;
int GLYPHS::MATH::PI;

#if 0
int const GLYPHS::MATH::SQUARE_ROOT   = static_cast<int>( ACS_ );
int const GLYPHS::MATH::INFINITY      = static_cast<int>( ACS_ );
#endif

int GLYPHS::FACE::WHITE = '@';
int GLYPHS::FACE::BLACK = '@';
#else /* than HAVE_ASCII_GRAPHICS */
#error No ASCII box drawing.
#endif /* not HAVE_ASCII_GRAPHICS */

namespace {
void init_glyphs( void ) {
	GLYPHS::ARROW::DOWN       = static_cast<int>( ACS_DARROW );
	GLYPHS::ARROW::UP         = static_cast<int>( ACS_UARROW );
	GLYPHS::ARROW::LEFT       = static_cast<int>( ACS_LARROW );
	GLYPHS::ARROW::RIGHT      = static_cast<int>( ACS_RARROW );
	GLYPHS::ARROW::HORIZONTAL = static_cast<int>( ACS_HLINE );
	GLYPHS::ARROW::VERTICAL   = static_cast<int>( ACS_VLINE );

	GLYPHS::CHECKER::SPARSE = static_cast<int>( ACS_BOARD );
	GLYPHS::CHECKER::MEDIUM = static_cast<int>( ACS_BOARD );
	GLYPHS::CHECKER::DENSE  = static_cast<int>( ACS_CKBOARD );

	GLYPHS::LINE::SINGLE::VERTICAL          = static_cast<int>( ACS_VLINE );
	GLYPHS::LINE::SINGLE::HORIZONTAL        = static_cast<int>( ACS_HLINE );
	GLYPHS::LINE::SINGLE::BOTTOM_LEFT       = static_cast<int>( ACS_URCORNER );
	GLYPHS::LINE::SINGLE::BOTTOM_RIGHT      = static_cast<int>( ACS_ULCORNER );
	GLYPHS::LINE::SINGLE::TOP_LEFT          = static_cast<int>( ACS_LRCORNER );
	GLYPHS::LINE::SINGLE::TOP_RIGHT         = static_cast<int>( ACS_LLCORNER );
	GLYPHS::LINE::SINGLE::VERTICAL_LEFT     = static_cast<int>( ACS_RTEE );
	GLYPHS::LINE::SINGLE::VERTICAL_RIGHT    = static_cast<int>( ACS_LTEE );
	GLYPHS::LINE::SINGLE::HORIZONTAL_TOP    = static_cast<int>( ACS_BTEE );
	GLYPHS::LINE::SINGLE::HORIZONTAL_BOTTOM = static_cast<int>( ACS_TTEE );
	GLYPHS::LINE::SINGLE::CROSS             = static_cast<int>( ACS_PLUS );

	GLYPHS::LINE::DOUBLE::VERTICAL          = static_cast<int>( ACS_VLINE );
	GLYPHS::LINE::DOUBLE::HORIZONTAL        = static_cast<int>( ACS_HLINE );
	GLYPHS::LINE::DOUBLE::BOTTOM_LEFT       = static_cast<int>( ACS_URCORNER );
	GLYPHS::LINE::DOUBLE::BOTTOM_RIGHT      = static_cast<int>( ACS_ULCORNER );
	GLYPHS::LINE::DOUBLE::TOP_LEFT          = static_cast<int>( ACS_LRCORNER );
	GLYPHS::LINE::DOUBLE::TOP_RIGHT         = static_cast<int>( ACS_LLCORNER );
	GLYPHS::LINE::DOUBLE::VERTICAL_LEFT     = static_cast<int>( ACS_RTEE );
	GLYPHS::LINE::DOUBLE::VERTICAL_RIGHT    = static_cast<int>( ACS_LTEE );
	GLYPHS::LINE::DOUBLE::HORIZONTAL_TOP    = static_cast<int>( ACS_BTEE );
	GLYPHS::LINE::DOUBLE::HORIZONTAL_BOTTOM = static_cast<int>( ACS_TTEE );
	GLYPHS::LINE::DOUBLE::CROSS             = static_cast<int>( ACS_PLUS );

	GLYPHS::MATH::NOT_EQUAL     = static_cast<int>( ACS_NEQUAL );
	GLYPHS::MATH::LESS_EQUAL    = static_cast<int>( ACS_LEQUAL );
	GLYPHS::MATH::GREATER_EQUAL = static_cast<int>( ACS_GEQUAL );
	GLYPHS::MATH::APPROXIMATELY = '~';
	GLYPHS::MATH::PI            = static_cast<int>( ACS_PI );
	return;
}
}

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
	inline static int encode( int attr_ ) {
		return (
			static_cast<int>(
				COLOR_PAIR(
					static_cast<int unsigned>(
						( ( attr_ & COLORS::BG_MASK ) >> 1 )            /* background */
						| ( attr_ & COLORS::FG_MASK )                   /* foreground */
					)
				) | ( ( attr_ & COLORS::FG_BOLD ) ? A_BOLD : 0 )    /* brighter foreground */
					| ( ( attr_ & COLORS::BG_BLINK ) ? A_BLINK : 0 )  /* brighter background */
			)
		);
	}
	inline static int decode( int color_, int attr_ ) {
		return (
			static_cast<int>(
				( color_ & static_cast<int>( obinary<0111>::value ) )
				| ( ( color_ & static_cast<int>( obinary<0111000>::value ) ) << 1 )
				| ( ( attr_ & static_cast<int>( A_BOLD ) ) ? COLORS::FG_BOLD : 0 )
				| ( attr_ & static_cast<int>( A_BLINK ) ? COLORS::BG_BLINK : 0 )
			)
		);
	}
	inline static int encode_fix( int attr_ ) {
		int attr( 0 );
		/*
		 * On broken terminals we use trick to get bright background,
		 * first we swap foreground and background colors and then
		 * we add REVERSE attribure.
		 */
		if ( attr_ & COLORS::BG_BLINK ) {
			attr = (
				static_cast<int>(
					COLOR_PAIR(
						static_cast<int unsigned>(
								( ( attr_ & COLORS::FG_MASK ) << 3 )
							| ( ( attr_ & COLORS::BG_MASK ) >> 4 )
						)
					) | ( ( attr_ & COLORS::FG_BOLD ) ? A_BLINK : 0 )
						| A_BOLD | A_REVERSE
				)
			);
		} else {
			attr = encode( attr_ );
		}
		return ( attr );
	}
	inline static int decode_fix( int color_, int attr_ ) {
		int attr( 0 );
		if ( attr_ & static_cast<int>( A_REVERSE ) ) {
			attr = static_cast<int>(
				( ( color_ & static_cast<int>( obinary<0111>::value ) ) << 4 )
				| ( ( color_ & static_cast<int>( obinary<0111000>::value ) ) >> 3 )
				| ( attr_ & static_cast<int>( A_BLINK ) ? COLORS::FG_BOLD : 0 )
				| COLORS::BG_BLINK
			);
		} else {
			attr = decode( color_, attr_ );
		}
		return ( attr );
	}
};

int COLORS::fg_to_bg( int fg_ ) {
	M_PROLOG
	int bg( 0 );
	switch ( fg_ ) {
		case ( FG_BLACK ):         bg = BG_BLACK; break;
		case ( FG_RED ):           bg = BG_RED; break;
		case ( FG_GREEN ):         bg = BG_GREEN; break;
		case ( FG_BROWN ):         bg = BG_BROWN; break;
		case ( FG_BLUE ):          bg = BG_BLUE; break;
		case ( FG_MAGENTA ):       bg = BG_MAGENTA; break;
		case ( FG_CYAN ):          bg = BG_CYAN; break;
		case ( FG_LIGHTGRAY ):     bg = BG_LIGHTGRAY; break;
		case ( FG_GRAY ):          bg = BG_GRAY; break;
		case ( FG_BRIGHTRED ):     bg = BG_BRIGHTRED; break;
		case ( FG_BRIGHTGREEN ):   bg = BG_BRIGHTGREEN; break;
		case ( FG_YELLOW ):        bg = BG_YELLOW; break;
		case ( FG_BRIGHTBLUE ):    bg = BG_BRIGHTBLUE; break;
		case ( FG_BRIGHTMAGENTA ): bg = BG_BRIGHTMAGENTA; break;
		case ( FG_BRIGHTCYAN ):    bg = BG_BRIGHTCYAN; break;
		case ( FG_WHITE ):         bg = BG_WHITE; break;
		default: {
			M_THROW( "Invalid color:", fg_ );
		}
	}
	return ( bg );
	M_EPILOG
}

namespace {

bool has_broken_bright_background( void ) {
	bool hasBrokenBrightBackground(
		( ::getenv( "YAAL_HAS_BROKEN_BRIGHT_BACKGROUND" ) != nullptr )
		|| ( ::getenv( "MRXVT_TABTITLE" ) != nullptr )
		|| ( ::getenv( "TERMINATOR_UUID" ) != nullptr )
	);
	return ( hasBrokenBrightBackground );
}

}

/* public: */

HConsole::HConsole( void )
	: _initialized( false )
	, _enabled( false )
	, _brokenBrightBackground( has_broken_bright_background() )
	, _width( 0 )
	, _height( 0 )
	, _mouseDes( -1 )
	, _window( NULL )
	, _event() {
	return;
}

HConsole::~HConsole( void ) {
	M_PROLOG
	if ( _initialized ) {
		/*
		 * Because of bug in how ::initscr() handles invalid terminal setup and performs
		 * program abortion ::isendwin() return true even if initscr() fails.
		 * The only way to call ::isendwin() in case of initscr() failure
		 * is from global destructor or atexit() callback.
		 */
		if ( is_enabled() ) {
			leave_curses();
		}
		delwin( static_cast<WINDOW*>( _window ) );
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
	if ( ! _terminal_.exists() ) {
		throw HConsoleException( "Not connected to a terminal." );
	}
	static short const colors[] = {
		COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
		COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
	};
/*	def_shell_mode(); */
/* this is done automatically by initscr(), read man next time */
	if ( ! _initialized ) {
		init();
	}
	_terminal_.init();
	use_env( true );
	if ( ! _window ) {
		_window = initscr();
	}
	init_glyphs();
	M_ENSURE( cbreak() != ERR );
	M_ENSURE( start_color() != ERR );
	M_ENSURE( static_cast<int>( wstandend( static_cast<WINDOW*>( _window ) ) ) != ERR );
	M_ENSURE( nonl() == OK );
	M_ENSURE( keypad( static_cast<WINDOW*>( _window ), true ) != ERR );
	M_ENSURE( intrflush( static_cast<WINDOW*>( _window ), false ) != ERR );
	M_ENSURE( scrollok( static_cast<WINDOW*>( _window ), false ) != ERR );
	M_ENSURE( leaveok( static_cast<WINDOW*>( _window ), false ) != ERR );
	immedok( static_cast<WINDOW*>( _window ), false );
	M_ENSURE( fflush( NULL ) == 0 );
	flushinp(); /* Always returns OK */
	curs_set( CURSOR::INVISIBLE );
	/* init color pairs */
	M_ENSURE( use_default_colors() == OK );
	M_ENSURE( assume_default_colors( COLOR_BLACK, COLOR_BLACK ) == OK );
	static int const COLOR_MAX( countof ( colors ) );
	for ( int bg( 0 ); bg < COLOR_MAX; ++ bg ) {
		for ( int fg( 0 ); fg < COLOR_MAX; ++ fg ) {
			init_pair( static_cast<short>( bg * COLOR_MAX + fg ),
					colors[ fg ], colors[ bg ] );
		}
	}
	_enabled = true;
	set_attr( COLORS::ATTR_NORMAL );
	set_background( _screenBackground_ );
	if ( ::getenv( "YAAL_NO_MOUSE" ) ) {
		_useMouse_ = USE_MOUSE::NO;
	}
	if ( _useMouse_ != USE_MOUSE::NO ) {
		if ( ::getenv( "DISPLAY" ) != NULL ) {
			mouse::mouse_open = mouse::x_mouse_open;
			mouse::mouse_get = mouse::x_mouse_get;
			mouse::mouse_close = mouse::x_mouse_close;
		} else {
			mouse::mouse_open = mouse::console_mouse_open;
			mouse::mouse_get = mouse::console_mouse_get;
			mouse::mouse_close = mouse::console_mouse_close;
		}
		try {
			if ( ( _mouseDes = mouse::mouse_open() ) >= 0 ) {
				log( LOG_LEVEL::INFO )
					<< ( ( mouse::mouse_open == mouse::x_mouse_open ) ? _( "using ncurses mouse support, at fd(" ) : _( "using console mouse support, at fd(" ) )
					<< _mouseDes << ')' << endl;
			} else if ( _useMouse_ == USE_MOUSE::YES ) {
				M_THROW(
					_( "mouse is console type and we did not received file descriptor" ),
					errno
				);
			}
		} catch ( HConsoleException const& ) {
			if ( _useMouse_ == USE_MOUSE::YES ) {
				throw;
			}
		} catch ( mouse::HMouseException const& ) {
			if ( _useMouse_ == USE_MOUSE::YES ) {
				throw;
			}
		}
	}
	refresh();
	return;
	M_EPILOG
}

void HConsole::leave_curses( void ) {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	if ( _mouseDes >= 0 ) {
		static_cast<void>( mouse::mouse_close() );
		_mouseDes = -1;
	}
	set_attr( COLORS::ATTR_NORMAL );
	set_background( COLORS::ATTR_NORMAL );
	M_ENSURE( ::wprintw( static_cast<WINDOW*>( _window ), "" ) != ERR );
	M_ENSURE( ::fflush( NULL ) == 0 );
	flushinp(); /* Always returns OK */
	M_ENSURE( ::intrflush( static_cast<WINDOW*>( _window ), true ) != ERR );
	leaveok( static_cast<WINDOW*>( _window ), false ); /* Always OK */
	immedok( static_cast<WINDOW*>( _window ), true ); /* Always OK */
	nl(); /* Always OK */
	M_ENSURE( static_cast<int>( wstandend( static_cast<WINDOW*>( _window ) ) ) != ERR );
	M_ENSURE( keypad( static_cast<WINDOW*>( _window ), false ) != ERR );
	M_ENSURE( nocbreak() != ERR );
	curs_set( CURSOR::VISIBLE );
	refresh();
/*	reset_shell_mode(); */
/* see comment near def_shell_mode(), ( automagicly by endwin() ) */
/*
	if ( static_cast<WINDOW*>( _window ) )
		delwin ( static_cast<WINDOW*>( static_cast<WINDOW*>( _window ) ) );
	static_cast<WINDOW*>( _window ) = NULL;
*/
	M_ENSURE( endwin() == OK );
	_terminal_.flush();
	_enabled = false;
	return;
	M_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wsign-conversion"
template <typename T>
inline void fwd_wattrset( WINDOW* win_, T val_ ) {
	static_cast<void>( wattrset( win_, val_ ) );
}
#pragma GCC diagnostic error "-Wsign-conversion"
}

void HConsole::set_attr( int attr_ ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	char unsigned byte( static_cast<char unsigned>( attr_ ) );
	fwd_wattrset(
		static_cast<WINDOW*>( _window ),
		_brokenBrightBackground
			? static_cast<attr_t>( ATTR::encode_fix( byte ) )
			: static_cast<attr_t>( ATTR::encode( byte ) )
	);
	return;
	M_EPILOG
}

void HConsole::set_background( int color_ ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	wbkgd(
		static_cast<WINDOW*>( _window ),
		' ' | static_cast<chtype>(
			_brokenBrightBackground
				? ATTR::encode_fix( COLORS::FG_BLACK | color_ )
				: ATTR::encode( COLORS::FG_BLACK | color_ )
		)
	); /* meaningless value from macro */
	return;
	M_EPILOG
}

void HConsole::move( int row_, int column_ ) const {
	M_ENSURE( ::wmove( static_cast<WINDOW*>( _window ), row_, column_ ) != ERR );
	return;
}

CURSOR HConsole::curs_set( CURSOR cursor_ ) const {
	int cursor = ::curs_set( cursor_ == CURSOR::VISIBLE ? 1 : ( cursor_ == CURSOR::INVISIBLE ? 0 : 2 ) );
	if ( cursor == 1 )
		return ( CURSOR::VISIBLE );
	else if ( cursor == 2 )
		return ( CURSOR::VERY_VISIBLE );
	return ( CURSOR::INVISIBLE );
}

inline int waddch_fwd( WINDOW* win_, int char_ ) {
	return ( ::waddch( win_, static_cast<chtype>( char_ ) ) );
}

#undef addch
void HConsole::addch( int char_ ) {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	M_ENSURE( waddch_fwd( static_cast<WINDOW*>( _window ), char_ ) != ERR );
	return;
	M_EPILOG
}

void HConsole::refresh( void ) {
	M_PROLOG
	M_ENSURE( ::wrefresh( static_cast<WINDOW*>( _window ) ) != ERR );
	getmaxyx( static_cast<WINDOW*>( _window ), _height, _width );
	return;
	M_EPILOG
}

int HConsole::endwin( void ) {
	return ( ::endwin() );
}

inline void getyx_fwd( WINDOW* win_, int& height_, int& width_ ) {
	getyx( win_, height_, width_ );
	return;
}

#undef getyx
void HConsole::getyx( int& height_, int& width_ ) const {
	getyx_fwd( static_cast<WINDOW*>( _window ), height_, width_ );
	return;
}

#undef clrtoeol
void HConsole::clrtoeol( void ) const {
	M_PROLOG
	M_ENSURE( wclrtoeol( static_cast<WINDOW*>( _window ) ) != ERR );
	return;
	M_EPILOG
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

yaal::hcore::HStreamInterface::ptr_t const& HConsole::get_event_source( void ) const {
	return ( _event->out() );
}

void HConsole::vmvprintf( int row_, int column_,
							 char const* format_, void* ap_ ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	if ( column_ >= _width ) {
		M_THROW( "bad column.", column_ );
	}
	if ( ( row_ < 0 ) || ( row_ >= _height ) ) {
		M_THROW( "bad row.", row_ );
	}
	int origRow( 0 );
	int origColumn( 0 );
	getyx( origRow, origColumn );
	if ( column_ < 0 ) {
		move( row_, 0 );
		clrtoeol(); /* Always OK */
	} else {
		move( row_, column_ );
	}
	va_list& ap( *static_cast<va_list*>( ap_ ) );
	M_ENSURE( vw_printw( static_cast<WINDOW*>( _window ), format_, ap ) != ERR );
	move( origRow, origColumn );
	return;
	M_EPILOG
}

void HConsole::vcmvprintf( int row_, int column_, int attribute_,
							 char const* format_, void* ap_ ) const {
	M_PROLOG
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	int origAttribute( get_attr() );
	set_attr( attribute_ );
	vmvprintf( row_, column_, format_, ap_ );
	set_attr( origAttribute );
	return;
	M_EPILOG
}

void HConsole::vprintf( char const* format_, void* ap_ ) const {
	M_PROLOG
	va_list& ap = *static_cast<va_list*>( ap_ );
	if ( ! _enabled )
		M_THROW( "not in curses mode", errno );
	M_ENSURE( vw_printw( static_cast<WINDOW*>( _window ), format_, ap ) != ERR );
	return;
	M_EPILOG
}

void HConsole::printf( char const* format_, ... ) const {
	M_PROLOG
	va_list ap;
	va_start( ap, format_ );
	vprintf( format_, &ap );
	va_end( ap );
	return;
	M_EPILOG
}

void HConsole::mvprintf( int row_, int column_, char const* format_,
		... ) const {
	M_PROLOG
	va_list ap;
	va_start( ap, format_ );
	vmvprintf( row_, column_, format_, &ap );
	va_end( ap );
	return;
	M_EPILOG
}

void HConsole::cmvprintf( int row_, int column_, int attribute_,
							 char const* format_, ... ) const {
	M_PROLOG
	va_list ap;
	va_start( ap, format_ );
	vcmvprintf( row_, column_, attribute_, format_, &ap );
	va_end( ap );
	return;
	M_EPILOG
}

inline int waddstr_fwd( WINDOW* win_, char const* str_ ) {
	return ( ::waddstr( win_, str_ ) );
}

#undef addstr
void HConsole::addstr( char const* str_ ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	M_ENSURE( waddstr_fwd( static_cast<WINDOW*>( _window ), str_ ) != ERR );
	return;
	M_EPILOG
}

int HConsole::ungetch( int code_ ) {
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
		case ( KEY_CODES::F1 ):        code_ = KEY_F(1);      break;
		case ( KEY_CODES::F2 ):        code_ = KEY_F(2);      break;
		case ( KEY_CODES::F3 ):        code_ = KEY_F(3);      break;
		case ( KEY_CODES::F4 ):        code_ = KEY_F(4);      break;
		case ( KEY_CODES::F5 ):        code_ = KEY_F(5);      break;
		case ( KEY_CODES::F6 ):        code_ = KEY_F(6);      break;
		case ( KEY_CODES::F7 ):        code_ = KEY_F(7);      break;
		case ( KEY_CODES::F8 ):        code_ = KEY_F(8);      break;
		case ( KEY_CODES::F9 ):        code_ = KEY_F(9);      break;
		case ( KEY_CODES::F10 ):       code_ = KEY_F(10);      break;
		case ( KEY_CODES::F11 ):       code_ = KEY_F(11);      break;
		case ( KEY_CODES::F12 ):       code_ = KEY_F(12);      break;
		case ( KEY_CODES::F13 ):       code_ = KEY_F(13);      break;
		case ( KEY_CODES::F14 ):       code_ = KEY_F(14);      break;
		case ( KEY_CODES::F15 ):       code_ = KEY_F(15);      break;
		case ( KEY_CODES::F16 ):       code_ = KEY_F(16);      break;
		case ( KEY_CODES::F17 ):       code_ = KEY_F(17);      break;
		case ( KEY_CODES::F18 ):       code_ = KEY_F(18);      break;
		case ( KEY_CODES::F19 ):       code_ = KEY_F(19);      break;
		case ( KEY_CODES::F20 ):       code_ = KEY_F(20);      break;
		case ( KEY_CODES::F21 ):       code_ = KEY_F(21);      break;
		case ( KEY_CODES::F22 ):       code_ = KEY_F(22);      break;
		case ( KEY_CODES::F23 ):       code_ = KEY_F(23);      break;
		case ( KEY_CODES::F24 ):       code_ = KEY_F(24);      break;
		case ( KEY_CODES::MOUSE ):     code_ = KEY_MOUSE;     break;
		default:
		break;
	}
	return ( ::ungetch( code_ ) );
}

int HConsole::get_key( void ) const {
	M_PROLOG
	CURSOR origCursState = CURSOR::INVISIBLE;
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	M_ENSURE( noecho() != ERR );
	M_ENSURE( ::fflush( NULL ) == 0 );
	int key( getch() );
	M_ASSERT( key < KEY_CODES::SPECIAL_KEY );
	if ( key == KEY_CODES::ESCAPE ) {
		M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), true ) != ERR );
		key = getch();
		M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), false ) != ERR );
		if ( key == ERR ) {
			key = KEY_CODES::ESCAPE;
		} else {
			key = KEY<>::meta_r( key );
		}
	}
	if ( key == KEY<>::ctrl_r( _commandComposeCharacter_ ) ) {
		origCursState = curs_set( CURSOR::INVISIBLE );
		cmvprintf( _height - 1, -1, COLORS::FG_WHITE, "ctrl-%c",
					_commandComposeCharacter_ );
		timeout( _commandComposeDelay_ * 100 );
		key = getch();
		timeout( -1 );
		if ( key == ERR ) {
			key = KEY<>::ctrl_r( _commandComposeCharacter_ );
			cmvprintf( _height - 1, 0, COLORS::FG_LIGHTGRAY, "      " );
		} else {
			int character = 0;
			if ( key < KEY_CODES::ESCAPE ) {
				key = KEY<>::command_r( character = key + 96 );
			} else if ( key == KEY_CODES::ESCAPE ) {
				M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), true ) != ERR );
				key = getch();
				M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), false ) != ERR );
				if ( key == ERR ) {
					key = KEY<>::command_r( character = KEY_CODES::ESCAPE );
				} else {
					key = KEY<>::command_r( KEY<>::meta_r( character = key ) );
				}
			} else {
				key = KEY<>::command_r( character = key );
			}
			cmvprintf( _height - 1, 6, COLORS::FG_WHITE, " %c", character );
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
		case ( KEY_F(1) ):      key = KEY_CODES::F1;        break;
		case ( KEY_F(2) ):      key = KEY_CODES::F2;        break;
		case ( KEY_F(3) ):      key = KEY_CODES::F3;        break;
		case ( KEY_F(4) ):      key = KEY_CODES::F4;        break;
		case ( KEY_F(5) ):      key = KEY_CODES::F5;        break;
		case ( KEY_F(6) ):      key = KEY_CODES::F6;        break;
		case ( KEY_F(7) ):      key = KEY_CODES::F7;        break;
		case ( KEY_F(8) ):      key = KEY_CODES::F8;        break;
		case ( KEY_F(9) ):      key = KEY_CODES::F9;        break;
		case ( KEY_F(10) ):     key = KEY_CODES::F10;       break;
		case ( KEY_F(11) ):     key = KEY_CODES::F11;       break;
		case ( KEY_F(12) ):     key = KEY_CODES::F12;       break;
		case ( KEY_F(13) ):     key = KEY_CODES::F13;       break;
		case ( KEY_F(14) ):     key = KEY_CODES::F14;       break;
		case ( KEY_F(15) ):     key = KEY_CODES::F15;       break;
		case ( KEY_F(16) ):     key = KEY_CODES::F16;       break;
		case ( KEY_F(17) ):     key = KEY_CODES::F17;       break;
		case ( KEY_F(18) ):     key = KEY_CODES::F18;       break;
		case ( KEY_F(19) ):     key = KEY_CODES::F19;       break;
		case ( KEY_F(20) ):     key = KEY_CODES::F20;       break;
		case ( KEY_F(21) ):     key = KEY_CODES::F21;       break;
		case ( KEY_F(22) ):     key = KEY_CODES::F22;       break;
		case ( KEY_F(23) ):     key = KEY_CODES::F23;       break;
		case ( KEY_F(24) ):     key = KEY_CODES::F24;       break;
		case ( KEY_MOUSE ):     key = KEY_CODES::MOUSE;     break;
		default:
		break;
	}
	return ( key );
	M_EPILOG
}

int HConsole::kbhit( void ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), true ) != ERR );
	int key( get_key() );
	M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), false ) != ERR );
	return ( key != ERR ? key : 0 );
	M_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
template <typename T1, typename T2>
inline void fwd_attr_get( WINDOW* win_, T1& val1_, T2& val2_, void* ) {
	static_cast<void>( wattr_get( win_, val1_, val2_, NULL ) ); /* Ugly macro */
}
#pragma GCC diagnostic error "-Wold-style-cast"
}

char unsigned HConsole::get_attr( void ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	attr_t attr = 0;
	NCURSES_ATTR_GET_SECOND_ARG_TYPE color = 0;
	/* A workaround for compiler warning regarding
	 * &attr and &color never being NULL. */
	attr_t* pa( &attr );
	NCURSES_ATTR_GET_SECOND_ARG_TYPE* pc( &color );
	fwd_attr_get( static_cast<WINDOW*>( _window ), pa, pc, NULL );
	int attribute(
		_brokenBrightBackground
			? ATTR::decode_fix( static_cast<int>( color ), static_cast<int>( attr ) )
			: ATTR::decode( static_cast<int>( color ), static_cast<int>( attr ) )
	);
	return ( static_cast<char unsigned>( attribute ) );
	M_EPILOG
}

void HConsole::clrscr( void ) {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	wclear( static_cast<WINDOW*>( _window ) ); /* Always returns OK */
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
	if ( is_enabled() ) {
		_event->write( "r", 1 );
	} else {
		::fprintf( stderr, "\n%s", message.raw() );
	}
	return ( 0 );
	M_EPILOG
}

int HConsole::console_cleanup( int sigNo_ ) {
	M_PROLOG
	if ( ( sigNo_ == SIGINT ) && ( tools::_ignoreSignalSIGINT_ ) ) {
		return ( 0 );
	}
	if ( is_enabled() ) {
		leave_curses();
	}
	return ( 0 );
	M_EPILOG
}

int HConsole::on_quit( int ) {
	M_PROLOG
	if ( is_enabled() ) {
		if ( tools::_ignoreSignalSIGQUIT_ ) {
			cmvprintf( get_height() - 1, 0, COLORS::FG_BRIGHTRED,
					"Hard Quit is disabled by yaal configuration." );
		} else {
			leave_curses();
		}
	}
	return ( 0 );
	M_EPILOG
}

int HConsole::on_tstp( int ) {
	M_PROLOG
	if ( is_enabled() ) {
		if ( tools::_ignoreSignalSIGTSTP_ ) {
			cmvprintf( get_height() - 1, 0, COLORS::FG_BRIGHTRED,
					"Suspend is disabled by yaal configuration." );
		} else {
			leave_curses();
		}
	}
	return ( 0 );
	M_EPILOG
}

int HConsole::on_cont( int ) {
	M_PROLOG
	if ( ! is_enabled() )
		enter_curses();
	if ( is_enabled() ) {
		_event->write( "r", 1 );
	}
	return ( 0 );
	M_EPILOG
}

int HConsole::on_mouse( int ) {
	M_PROLOG
	int res( 0 );
	if ( _mouseDes >= 0 ) {
		if ( is_enabled() ) {
			res = static_cast<int>( _event->write( "m", 1 ) );
		}
	}
	if ( is_enabled() ) {
		leave_curses();
	}
	return ( res );
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

