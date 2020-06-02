/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib> /* getenv */
#include <cstring>
#include <csignal>  /* signal handling */
#include <unistd.h>
#include <poll.h>
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
#include "hcore/unicode.hxx"
#include "tools/tools.hxx"
#include "tools/hterminal.hxx"
#include "tools/keycode.hxx"
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

#undef COLOR

using namespace yaal::hcore;
using namespace yaal::meta;
using namespace yaal::tools;
using namespace yaal::ansi;

namespace yaal {

namespace hconsole {

static_assert( KEY_CODE::BASE > KEY_MAX, "ncurses key codes overlap with KEY_CODE codes" );

#ifdef HAVE_ASCII_GRAPHICS
int GLYPH::ARROW::DOWN;
int GLYPH::ARROW::UP;
int GLYPH::ARROW::LEFT;
int GLYPH::ARROW::RIGHT;
int GLYPH::ARROW::HORIZONTAL;
int GLYPH::ARROW::VERTICAL;

int GLYPH::CHECKER::SPARSE;
int GLYPH::CHECKER::MEDIUM;
int GLYPH::CHECKER::DENSE;

int GLYPH::LINE::SINGLE::VERTICAL;
int GLYPH::LINE::SINGLE::HORIZONTAL;
int GLYPH::LINE::SINGLE::BOTTOM_LEFT;
int GLYPH::LINE::SINGLE::BOTTOM_RIGHT;
int GLYPH::LINE::SINGLE::TOP_LEFT;
int GLYPH::LINE::SINGLE::TOP_RIGHT;
int GLYPH::LINE::SINGLE::VERTICAL_LEFT;
int GLYPH::LINE::SINGLE::VERTICAL_RIGHT;
int GLYPH::LINE::SINGLE::HORIZONTAL_TOP;
int GLYPH::LINE::SINGLE::HORIZONTAL_BOTTOM;
int GLYPH::LINE::SINGLE::CROSS;

int GLYPH::LINE::DOUBLE::VERTICAL;
int GLYPH::LINE::DOUBLE::HORIZONTAL;
int GLYPH::LINE::DOUBLE::BOTTOM_LEFT;
int GLYPH::LINE::DOUBLE::BOTTOM_RIGHT;
int GLYPH::LINE::DOUBLE::TOP_LEFT;
int GLYPH::LINE::DOUBLE::TOP_RIGHT;
int GLYPH::LINE::DOUBLE::VERTICAL_LEFT;
int GLYPH::LINE::DOUBLE::VERTICAL_RIGHT;
int GLYPH::LINE::DOUBLE::HORIZONTAL_TOP;
int GLYPH::LINE::DOUBLE::HORIZONTAL_BOTTOM;
int GLYPH::LINE::DOUBLE::CROSS;

int GLYPH::MATH::NOT_EQUAL;
int GLYPH::MATH::LESS_EQUAL;
int GLYPH::MATH::GREATER_EQUAL;
int GLYPH::MATH::APPROXIMATELY;
int GLYPH::MATH::PI;

#if 0
int const GLYPH::MATH::SQUARE_ROOT   = static_cast<int>( ACS_ );
int const GLYPH::MATH::INFINITY      = static_cast<int>( ACS_ );
#endif

int GLYPH::FACE::WHITE = '@';
int GLYPH::FACE::BLACK = '@';
#else /* than HAVE_ASCII_GRAPHICS */
#error No ASCII box drawing.
#endif /* not HAVE_ASCII_GRAPHICS */

namespace {
void init_glyphs( void ) {
	GLYPH::ARROW::DOWN       = static_cast<int>( ACS_DARROW );
	GLYPH::ARROW::UP         = static_cast<int>( ACS_UARROW );
	GLYPH::ARROW::LEFT       = static_cast<int>( ACS_LARROW );
	GLYPH::ARROW::RIGHT      = static_cast<int>( ACS_RARROW );
	GLYPH::ARROW::HORIZONTAL = static_cast<int>( ACS_HLINE );
	GLYPH::ARROW::VERTICAL   = static_cast<int>( ACS_VLINE );

	GLYPH::CHECKER::SPARSE = static_cast<int>( ACS_BOARD );
	GLYPH::CHECKER::MEDIUM = static_cast<int>( ACS_BOARD );
	GLYPH::CHECKER::DENSE  = static_cast<int>( ACS_CKBOARD );

	GLYPH::LINE::SINGLE::VERTICAL          = static_cast<int>( ACS_VLINE );
	GLYPH::LINE::SINGLE::HORIZONTAL        = static_cast<int>( ACS_HLINE );
	GLYPH::LINE::SINGLE::BOTTOM_LEFT       = static_cast<int>( ACS_URCORNER );
	GLYPH::LINE::SINGLE::BOTTOM_RIGHT      = static_cast<int>( ACS_ULCORNER );
	GLYPH::LINE::SINGLE::TOP_LEFT          = static_cast<int>( ACS_LRCORNER );
	GLYPH::LINE::SINGLE::TOP_RIGHT         = static_cast<int>( ACS_LLCORNER );
	GLYPH::LINE::SINGLE::VERTICAL_LEFT     = static_cast<int>( ACS_RTEE );
	GLYPH::LINE::SINGLE::VERTICAL_RIGHT    = static_cast<int>( ACS_LTEE );
	GLYPH::LINE::SINGLE::HORIZONTAL_TOP    = static_cast<int>( ACS_BTEE );
	GLYPH::LINE::SINGLE::HORIZONTAL_BOTTOM = static_cast<int>( ACS_TTEE );
	GLYPH::LINE::SINGLE::CROSS             = static_cast<int>( ACS_PLUS );

	GLYPH::LINE::DOUBLE::VERTICAL          = static_cast<int>( ACS_VLINE );
	GLYPH::LINE::DOUBLE::HORIZONTAL        = static_cast<int>( ACS_HLINE );
	GLYPH::LINE::DOUBLE::BOTTOM_LEFT       = static_cast<int>( ACS_URCORNER );
	GLYPH::LINE::DOUBLE::BOTTOM_RIGHT      = static_cast<int>( ACS_ULCORNER );
	GLYPH::LINE::DOUBLE::TOP_LEFT          = static_cast<int>( ACS_LRCORNER );
	GLYPH::LINE::DOUBLE::TOP_RIGHT         = static_cast<int>( ACS_LLCORNER );
	GLYPH::LINE::DOUBLE::VERTICAL_LEFT     = static_cast<int>( ACS_RTEE );
	GLYPH::LINE::DOUBLE::VERTICAL_RIGHT    = static_cast<int>( ACS_LTEE );
	GLYPH::LINE::DOUBLE::HORIZONTAL_TOP    = static_cast<int>( ACS_BTEE );
	GLYPH::LINE::DOUBLE::HORIZONTAL_BOTTOM = static_cast<int>( ACS_TTEE );
	GLYPH::LINE::DOUBLE::CROSS             = static_cast<int>( ACS_PLUS );

	GLYPH::MATH::NOT_EQUAL     = static_cast<int>( ACS_NEQUAL );
	GLYPH::MATH::LESS_EQUAL    = static_cast<int>( ACS_LEQUAL );
	GLYPH::MATH::GREATER_EQUAL = static_cast<int>( ACS_GEQUAL );
	GLYPH::MATH::APPROXIMATELY = '~';
	GLYPH::MATH::PI            = static_cast<int>( ACS_PI );
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
		return ( static_cast<int>( COLOR_PAIR( static_cast<int unsigned>( attr_ ) ) ) );
	}
	inline static int encode_bright_by_bold( int attr_ ) {
		return (
			static_cast<int>(
				COLOR_PAIR(
					static_cast<int unsigned>(
						( ( attr_ & COLOR::BG_MASK ) >> 1 )            /* background */
						| ( attr_ & COLOR::FG_MASK )                   /* foreground */
					)
				) | ( ( attr_ & COLOR::FG_BOLD ) ? A_BOLD : 0 )    /* brighter foreground */
					| ( ( attr_ & COLOR::BG_BLINK ) ? A_BLINK : 0 )  /* brighter background */
			)
		);
	}
	inline static int decode( int color_, int ) {
		return ( static_cast<int>( color_ ) );
	}
	inline static int decode_bright_by_bold( int color_, int attr_ ) {
		return (
			static_cast<int>(
				( color_ & static_cast<int>( obinary<0111>::value ) )
				| ( ( color_ & static_cast<int>( obinary<0111000>::value ) ) << 1 )
				| ( ( attr_ & static_cast<int>( A_BOLD ) ) ? COLOR::FG_BOLD : 0 )
				| ( attr_ & static_cast<int>( A_BLINK ) ? COLOR::BG_BLINK : 0 )
			)
		);
	}
	inline static int encode_broken_bright_background( int attr_ ) {
		int attr( 0 );
		/*
		 * On broken terminals we use trick to get bright background,
		 * first we swap foreground and background colors and then
		 * we add REVERSE attribute.
		 */
		if ( attr_ & COLOR::BG_BLINK ) {
			attr = (
				static_cast<int>(
					COLOR_PAIR(
						static_cast<int unsigned>(
								( ( attr_ & COLOR::FG_MASK ) << 3 )
							| ( ( attr_ & COLOR::BG_MASK ) >> 4 )
						)
					) | ( ( attr_ & COLOR::FG_BOLD ) ? A_BLINK : 0 )
						| A_BOLD | A_REVERSE
				)
			);
		} else {
			attr = encode_bright_by_bold( attr_ );
		}
		return ( attr );
	}
	inline static int decode_broken_bright_background( int color_, int attr_ ) {
		int attr( 0 );
		if ( attr_ & static_cast<int>( A_REVERSE ) ) {
			attr = static_cast<int>(
				( ( color_ & static_cast<int>( obinary<0111>::value ) ) << 4 )
				| ( ( color_ & static_cast<int>( obinary<0111000>::value ) ) >> 3 )
				| ( attr_ & static_cast<int>( A_BLINK ) ? COLOR::FG_BOLD : 0 )
				| COLOR::BG_BLINK
			);
		} else {
			attr = decode_bright_by_bold( color_, attr_ );
		}
		return ( attr );
	}
};

namespace {

bool make_bright_by_bold( void ) {
	return ( ::getenv( "YAAL_MAKE_BRIGHT_BY_BOLD" ) != nullptr );
}

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
	, _brightByBold( make_bright_by_bold() )
	, _brokenBrightBackground( has_broken_bright_background() )
	, _width( 0 )
	, _height( 0 )
	, _mouseDes( -1 )
	, _window( nullptr )
	, _event()
	, _utf8() {
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
	if ( ! HTerminal::get_instance().exists() ) {
		throw HConsoleException( "Not connected to a terminal." );
	}
	static int const BASE_COLOR_COUNT = 8;
	static short const colors[] = {
		COLOR_BLACK,
		COLOR_RED,
		COLOR_GREEN,
		COLOR_YELLOW,
		COLOR_BLUE,
		COLOR_MAGENTA,
		COLOR_CYAN,
		COLOR_WHITE,
		COLOR_BLACK   + BASE_COLOR_COUNT,
		COLOR_RED     + BASE_COLOR_COUNT,
		COLOR_GREEN   + BASE_COLOR_COUNT,
		COLOR_YELLOW  + BASE_COLOR_COUNT,
		COLOR_BLUE    + BASE_COLOR_COUNT,
		COLOR_MAGENTA + BASE_COLOR_COUNT,
		COLOR_CYAN    + BASE_COLOR_COUNT,
		COLOR_WHITE   + BASE_COLOR_COUNT
	};
/* def_shell_mode(); */
/* this is done automatically by initscr(), read man next time */
	if ( ! _initialized ) {
		init();
	}
	HTerminal::get_instance().init();
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
	M_ENSURE( fflush( nullptr ) == 0 );
	flushinp(); /* Always returns OK */
	curs_set( CURSOR::INVISIBLE );
	/* init color pairs */
	M_ENSURE( use_default_colors() == OK );
	M_ENSURE( assume_default_colors( COLOR_BLACK, COLOR_BLACK ) == OK );
	static int const COLOR_MAX( yaal::size( colors ) / ( _brightByBold ? 2 : 1 ) );
	for ( int bg( 0 ); bg < COLOR_MAX; ++ bg ) {
		for ( int fg( 0 ); fg < COLOR_MAX; ++ fg ) {
			init_pair(
				static_cast<short>( bg * COLOR_MAX + fg ),
				colors[ fg ],
				colors[ bg ]
			);
		}
	}
	_enabled = true;
	set_attr( COLOR::ATTR_NORMAL );
	set_background( _screenBackground_ );
	if ( ::getenv( "YAAL_NO_MOUSE" ) ) {
		_useMouse_ = USE_MOUSE::NO;
	}
	if ( _useMouse_ != USE_MOUSE::NO ) {
		if ( ::getenv( "DISPLAY" ) != nullptr ) {
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
	set_attr( COLOR::ATTR_NORMAL );
	set_background( COLOR::ATTR_NORMAL );
	M_ENSURE( ::waddstr( static_cast<WINDOW*>( _window ), "" ) != ERR );
	M_ENSURE( ::fflush( nullptr ) == 0 );
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
/* see comment near def_shell_mode(), ( automagically by endwin() ) */
/*
	if ( static_cast<WINDOW*>( _window ) )
		delwin ( static_cast<WINDOW*>( static_cast<WINDOW*>( _window ) ) );
	static_cast<WINDOW*>( _window ) = nullptr;
*/
	M_ENSURE( endwin() == OK );
	HTerminal::get_instance().reset();
	_enabled = false;
	return;
	M_EPILOG
}

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
template <typename T>
inline void fwd_wattrset( WINDOW* win_, T val_ ) {
	static_cast<void>( wattrset( win_, val_ ) );
}
#pragma GCC diagnostic pop
}

void HConsole::set_attr( COLOR::color_t attr_ ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	char unsigned byte( static_cast<char unsigned>( attr_ ) );
	fwd_wattrset(
		static_cast<WINDOW*>( _window ),
		! _brightByBold
			? static_cast<attr_t>( ATTR::encode( byte ) )
			: (
				_brokenBrightBackground
					? static_cast<attr_t>( ATTR::encode_broken_bright_background( byte ) )
					: static_cast<attr_t>( ATTR::encode_bright_by_bold( byte ) )
			)
	);
	return;
	M_EPILOG
}

void HConsole::set_background( COLOR::color_t color_ ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	wbkgd(
		static_cast<WINDOW*>( _window ),
		' ' | static_cast<chtype>(
			! _brightByBold
				? ATTR::encode( COLOR::FG_BLACK | color_ )
				: (
					_brokenBrightBackground
						? ATTR::encode_broken_bright_background( COLOR::FG_BLACK | color_ )
						: ATTR::encode_bright_by_bold( COLOR::FG_BLACK | color_ )
				)
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

void HConsole::refresh( bool repaint_ ) {
	M_PROLOG
	if ( repaint_ ) {
		::clearok( static_cast<WINDOW*>( _window ), true );
	}
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

#undef addstr
void HConsole::do_mvprintf( int row_, int column_, yaal::hcore::HString const& str_ ) const {
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
	addstr( str_ );
	move( origRow, origColumn );
	return;
	M_EPILOG
}

void HConsole::do_cmvprintf( int row_, int column_, COLOR::color_t attribute_, yaal::hcore::HString const& str_ ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	COLOR::color_t origAttribute( get_attr() );
	set_attr( attribute_ );
	do_mvprintf( row_, column_, str_ );
	set_attr( origAttribute );
	return;
	M_EPILOG
}

void HConsole::addstr( yaal::hcore::HString const& str_ ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	_utf8 = str_;
	M_ENSURE( ::waddstr( static_cast<WINDOW*>( _window ), _utf8.c_str() ) != ERR );
	return;
	M_EPILOG
}

void HConsole::ungetch( int code_ ) {
	M_PROLOG
	switch ( code_ ) {
		case ( KEY_CODE::PAGE_DOWN ): code_ = KEY_NPAGE;     break;
		case ( KEY_CODE::PAGE_UP ):   code_ = KEY_PPAGE;     break;
		case ( KEY_CODE::HOME ):      code_ = KEY_HOME;      break;
		case ( KEY_CODE::END ):       code_ = KEY_END;       break;
		case ( KEY_CODE::BACKSPACE ): code_ = KEY_BACKSPACE; break;
		case ( KEY_CODE::UP ):        code_ = KEY_UP;        break;
		case ( KEY_CODE::DOWN ):      code_ = KEY_DOWN;      break;
		case ( KEY_CODE::LEFT ):      code_ = KEY_LEFT;      break;
		case ( KEY_CODE::RIGHT ):     code_ = KEY_RIGHT;     break;
		case ( KEY_CODE::DELETE ):    code_ = KEY_DC;        break;
		case ( KEY_CODE::INSERT ):    code_ = KEY_IC;        break;
		case ( KEY_CODE::F1 ):        code_ = KEY_F(1);      break;
		case ( KEY_CODE::F2 ):        code_ = KEY_F(2);      break;
		case ( KEY_CODE::F3 ):        code_ = KEY_F(3);      break;
		case ( KEY_CODE::F4 ):        code_ = KEY_F(4);      break;
		case ( KEY_CODE::F5 ):        code_ = KEY_F(5);      break;
		case ( KEY_CODE::F6 ):        code_ = KEY_F(6);      break;
		case ( KEY_CODE::F7 ):        code_ = KEY_F(7);      break;
		case ( KEY_CODE::F8 ):        code_ = KEY_F(8);      break;
		case ( KEY_CODE::F9 ):        code_ = KEY_F(9);      break;
		case ( KEY_CODE::F10 ):       code_ = KEY_F(10);      break;
		case ( KEY_CODE::F11 ):       code_ = KEY_F(11);      break;
		case ( KEY_CODE::F12 ):       code_ = KEY_F(12);      break;
		case ( KEY_CODE::F13 ):       code_ = KEY_F(13);      break;
		case ( KEY_CODE::F14 ):       code_ = KEY_F(14);      break;
		case ( KEY_CODE::F15 ):       code_ = KEY_F(15);      break;
		case ( KEY_CODE::F16 ):       code_ = KEY_F(16);      break;
		case ( KEY_CODE::F17 ):       code_ = KEY_F(17);      break;
		case ( KEY_CODE::F18 ):       code_ = KEY_F(18);      break;
		case ( KEY_CODE::F19 ):       code_ = KEY_F(19);      break;
		case ( KEY_CODE::F20 ):       code_ = KEY_F(20);      break;
		case ( KEY_CODE::F21 ):       code_ = KEY_F(21);      break;
		case ( KEY_CODE::F22 ):       code_ = KEY_F(22);      break;
		case ( KEY_CODE::F23 ):       code_ = KEY_F(23);      break;
		case ( KEY_CODE::F24 ):       code_ = KEY_F(24);      break;
		case ( KEY_CODE::MOUSE ):     code_ = KEY_MOUSE;     break;
		default:
		break;
	}
	int codes[] = { 0, 0, 0 };
	int codeCount( 0 );
	if ( code_ >= KEY_CODE::COMMAND_BASE ) {
		code_ -= KEY_CODE::COMMAND_BASE;
		codes[codeCount ++] = KEY<>::ctrl_r( _commandComposeCharacter_ );
	}
	if ( code_ >= KEY_CODE::META_BASE ) {
		code_ -= KEY_CODE::META_BASE;
		codes[codeCount ++] = KEY_CODE::ESCAPE;
	}
	codes[codeCount ++] = code_;
	for ( int i( codeCount - 1 ); i >= 0; -- i ) {
		M_ENSURE( ::ungetch( codes[i] ) != ERR );
	}
	if ( ! _event.unique() ) {
		notify_keyboard();
	}
	return;
	M_EPILOG
}

inline int complete_unicode( WINDOW* window_, int key_ ) {
	M_PROLOG
	int tailLength( key_ >= 0 && key_ < 256 ? unicode::utf8_declared_length( static_cast<char>( key_ ) ) - 1 : 0 );
	yaal::u32_t character( static_cast<yaal::u32_t>( key_ ) );
	if ( tailLength > 0 ) {
		M_ENSURE( nodelay( window_, true ) != ERR );
		static u8_t const mask[] = { 0xff, unicode::ENC_2_BYTES_VALUE_MASK, unicode::ENC_3_BYTES_VALUE_MASK, unicode::ENC_4_BYTES_VALUE_MASK };
		character &= mask[tailLength];
		for ( int i( 0 ); i < tailLength; ++ i ) {
			character <<= 6;
			int key( wgetch( window_ ) );
			character |= ( static_cast<u8_t>( key ) & unicode::TAIL_BYTES_VALUE_MASK );
		}
		M_ENSURE( nodelay( window_, false ) != ERR );
	}
	return ( static_cast<int>( character ) );
	M_EPILOG
}

int HConsole::get_key( void ) const {
	M_PROLOG
	CURSOR origCursState = CURSOR::INVISIBLE;
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	M_ENSURE( noecho() != ERR );
	M_ENSURE( ::fflush( nullptr ) == 0 );
	int key( wgetch( static_cast<WINDOW*>( _window ) ) );
	M_ASSERT( key < KEY_CODE::BASE );
	if ( key == KEY_CODE::ESCAPE ) {
		M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), true ) != ERR );
		key = wgetch( static_cast<WINDOW*>( _window ) );
		M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), false ) != ERR );
		if ( key == ERR ) {
			key = KEY_CODE::ESCAPE;
		} else {
			key = KEY<>::meta_r( key );
		}
	}
	if ( key == KEY<>::ctrl_r( _commandComposeCharacter_ ) ) {
		origCursState = curs_set( CURSOR::INVISIBLE );
		cmvprintf( _height - 1, -1, COLOR::FG_WHITE, "ctrl-%c", _commandComposeCharacter_ );
		wtimeout( static_cast<WINDOW*>( _window ), _commandComposeDelay_ * 100 );
		key = complete_unicode( static_cast<WINDOW*>( _window ), wgetch( static_cast<WINDOW*>( _window ) ) );
		wtimeout( static_cast<WINDOW*>( _window ), -1 );
		if ( key == ERR ) {
			key = KEY<>::ctrl_r( _commandComposeCharacter_ );
			cmvprintf( _height - 1, 0, COLOR::FG_LIGHTGRAY, "      " );
		} else {
			int character = 0;
			if ( key < KEY_CODE::ESCAPE ) {
				key = KEY<>::command_r( character = key + KEY_CODE::CONTROL_BASE );
			} else if ( key == KEY_CODE::ESCAPE ) {
				M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), true ) != ERR );
				key = complete_unicode( static_cast<WINDOW*>( _window ), wgetch( static_cast<WINDOW*>( _window ) ) );
				M_ENSURE( nodelay( static_cast<WINDOW*>( _window ), false ) != ERR );
				if ( key == ERR ) {
					key = KEY<>::command_r( character = KEY_CODE::ESCAPE );
				} else {
					key = KEY<>::command_r( KEY<>::meta_r( character = key ) );
				}
			} else {
				key = KEY<>::command_r( character = key );
			}
			cmvprintf( _height - 1, 6, COLOR::FG_WHITE, " %c", static_cast<char>( character ) );
		}
		curs_set( origCursState );
	} else {
		switch ( key ) {
			case ( KEY_NPAGE ):     key = KEY_CODE::PAGE_DOWN; break;
			case ( KEY_PPAGE ):     key = KEY_CODE::PAGE_UP;   break;
			case ( KEY_HOME ):      key = KEY_CODE::HOME;      break;
			case ( 347 ):
			case ( KEY_END ):       key = KEY_CODE::END;       break;
			case ( 8 ):
			case ( 127 ):
			case ( KEY_BACKSPACE ): key = KEY_CODE::BACKSPACE; break;
			case ( KEY_UP ):        key = KEY_CODE::UP;        break;
			case ( KEY_DOWN ):      key = KEY_CODE::DOWN;      break;
			case ( KEY_LEFT ):      key = KEY_CODE::LEFT;      break;
			case ( KEY_RIGHT ):     key = KEY_CODE::RIGHT;     break;
			case ( KEY_DC ):        key = KEY_CODE::DELETE;    break;
			case ( KEY_IC ):        key = KEY_CODE::INSERT;    break;
			case ( KEY_F(1) ):      key = KEY_CODE::F1;        break;
			case ( KEY_F(2) ):      key = KEY_CODE::F2;        break;
			case ( KEY_F(3) ):      key = KEY_CODE::F3;        break;
			case ( KEY_F(4) ):      key = KEY_CODE::F4;        break;
			case ( KEY_F(5) ):      key = KEY_CODE::F5;        break;
			case ( KEY_F(6) ):      key = KEY_CODE::F6;        break;
			case ( KEY_F(7) ):      key = KEY_CODE::F7;        break;
			case ( KEY_F(8) ):      key = KEY_CODE::F8;        break;
			case ( KEY_F(9) ):      key = KEY_CODE::F9;        break;
			case ( KEY_F(10) ):     key = KEY_CODE::F10;       break;
			case ( KEY_F(11) ):     key = KEY_CODE::F11;       break;
			case ( KEY_F(12) ):     key = KEY_CODE::F12;       break;
			case ( KEY_F(13) ):     key = KEY_CODE::F13;       break;
			case ( KEY_F(14) ):     key = KEY_CODE::F14;       break;
			case ( KEY_F(15) ):     key = KEY_CODE::F15;       break;
			case ( KEY_F(16) ):     key = KEY_CODE::F16;       break;
			case ( KEY_F(17) ):     key = KEY_CODE::F17;       break;
			case ( KEY_F(18) ):     key = KEY_CODE::F18;       break;
			case ( KEY_F(19) ):     key = KEY_CODE::F19;       break;
			case ( KEY_F(20) ):     key = KEY_CODE::F20;       break;
			case ( KEY_F(21) ):     key = KEY_CODE::F21;       break;
			case ( KEY_F(22) ):     key = KEY_CODE::F22;       break;
			case ( KEY_F(23) ):     key = KEY_CODE::F23;       break;
			case ( KEY_F(24) ):     key = KEY_CODE::F24;       break;
			case ( KEY_MOUSE ):     key = KEY_CODE::MOUSE;     break;
			default: {
				key = complete_unicode( static_cast<WINDOW*>( _window ), key );
			} break;
		}
	}
	M_ENSURE( echo() != ERR );
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
template <typename T1, typename T2>
inline void fwd_attr_get( WINDOW* win_, T1& val1_, T2& val2_, void* ) {
	static_cast<void>( wattr_get( win_, val1_, val2_, nullptr ) ); /* Ugly macro */
}
#pragma GCC diagnostic pop
}

COLOR::color_t HConsole::get_attr( void ) const {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	attr_t attr = 0;
	NCURSES_ATTR_GET_SECOND_ARG_TYPE color = 0;
	/* A workaround for compiler warning regarding
	 * &attr and &color never being nullptr. */
	attr_t* pa( &attr );
	NCURSES_ATTR_GET_SECOND_ARG_TYPE* pc( &color );
	fwd_attr_get( static_cast<WINDOW*>( _window ), pa, pc, nullptr );
	int attribute(
		! _brightByBold
			? ATTR::decode( static_cast<int>( color ), static_cast<int>( attr ) )
			: ( _brokenBrightBackground
				? ATTR::decode_broken_bright_background( static_cast<int>( color ), static_cast<int>( attr ) )
				: ATTR::decode_bright_by_bold( static_cast<int>( color ), static_cast<int>( attr ) )
			)
	);
	return ( static_cast<COLOR::color_t>( attribute ) );
	M_EPILOG
}

void HConsole::clear( void ) {
	M_PROLOG
	if ( ! _enabled ) {
		M_THROW( "not in curses mode", errno );
	}
	werase( static_cast<WINDOW*>( _window ) ); /* Always returns OK */
	return;
	M_EPILOG
}

void HConsole::clear_terminal( void ) {
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
	pollfd pfd[2] = {
		{ STDIN_FILENO, POLLIN, 0 },
		{ _mouseDes, POLLIN, 0 }
	};
	int error( ::poll( pfd, _mouseDes ? 2 : 1, timeOut_ ? timeOut_ : -1 ) );
	int eventType( 0 );
	if ( error > 0 ) {
		if ( pfd[0].revents & POLLIN ) {
			key_ = get_key();
			eventType = EVENT::KEYBOARD;
			if ( key_ == KEY_MOUSE ) {
				eventType = 0;
			}
		}
		if ( ( key_ == KEY_MOUSE ) || ( _mouseDes && ( pfd[1].revents & POLLIN ) ) ) {
			eventType |= EVENT::MOUSE, static_cast<void>( mouse::mouse_get( mouse_ ) );
		}
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

void HConsole::notify_terminal( void ) {
	M_PROLOG
	_event->write( "t", 1 );
	_event->flush();
	return;
	M_EPILOG
}

void HConsole::notify_mouse( void ) {
	M_PROLOG
	_event->write( "m", 1 );
	_event->flush();
	return;
	M_EPILOG
}

void HConsole::notify_keyboard( void ) {
	M_PROLOG
	_event->write( "k", 1 );
	_event->flush();
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
		notify_terminal();
	} else {
		cerr << endl << message;
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
			cmvprintf( get_height() - 1, 0, COLOR::FG_BRIGHTRED,
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
			cmvprintf( get_height() - 1, 0, COLOR::FG_BRIGHTRED,
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
		notify_terminal();
	}
	return ( 0 );
	M_EPILOG
}

int HConsole::on_mouse( int ) {
	M_PROLOG
	int res( 0 );
	if ( _mouseDes >= 0 ) {
		if ( is_enabled() ) {
			notify_mouse();
			res = 1;
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

