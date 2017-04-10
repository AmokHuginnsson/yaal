/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  console.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED
#define YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED 1

#include "hcore/hsingleton.hxx"
#include "hcore/hpipe.hxx"
#include "hcore/hformat.hxx"
#include "tools/ansi.hxx"
#include "tools/signals.hxx"
#include "hconsole/mouse.hxx"

namespace yaal {

/*! \brief User interface.
 *
 * This namespace contains library API required to build
 * terminal based user interface (TUI).
 */
namespace hconsole {

/*! \brief Special key codes.
 */
struct KEY_CODE {
	static int const ESCAPE       = 27;
	/* Coincidentally KEY_MAX from ncurses is 0777 which is 511. */
	static int const SPECIAL_KEY  = 0x400;
	static int const PAGE_UP      = SPECIAL_KEY;
	static int const PAGE_DOWN    = PAGE_UP   + 1;
	static int const DOWN         = PAGE_DOWN + 1;
	static int const UP           = DOWN      + 1;
	static int const LEFT         = UP        + 1;
	static int const RIGHT        = LEFT      + 1;
	static int const HOME         = RIGHT     + 1;
	static int const END          = HOME      + 1;
	static int const DELETE       = END       + 1;
	static int const INSERT       = DELETE    + 1;
	static int const BACKSPACE    = INSERT    + 1;
	static int const F1           = BACKSPACE + 1;
	static int const F2           = F1        + 1;
	static int const F3           = F2        + 1;
	static int const F4           = F3        + 1;
	static int const F5           = F4        + 1;
	static int const F6           = F5        + 1;
	static int const F7           = F6        + 1;
	static int const F8           = F7        + 1;
	static int const F9           = F8        + 1;
	static int const F10          = F9        + 1;
	static int const F11          = F10       + 1;
	static int const F12          = F11       + 1;
	static int const F13          = F12       + 1;
	static int const F14          = F13       + 1;
	static int const F15          = F14       + 1;
	static int const F16          = F15       + 1;
	static int const F17          = F16       + 1;
	static int const F18          = F17       + 1;
	static int const F19          = F18       + 1;
	static int const F20          = F19       + 1;
	static int const F21          = F20       + 1;
	static int const F22          = F21       + 1;
	static int const F23          = F22       + 1;
	static int const F24          = F23       + 1;
	static int const MOUSE        = F24       + 1;
	static int const CONTROL_BASE = 96;
	static int const META_BASE    = 0x04000;
	static int const COMMAND_BASE = 0x08000;
};

/*! \brief Quasi graphic glyphs.
 */
struct GLYPH {
	struct ARROW {
		static int DOWN;
		static int UP;
		static int LEFT;
		static int RIGHT;
		static int HORIZONTAL;
		static int VERTICAL;
	};
	struct CHECKER {
		static int SPARSE;
		static int MEDIUM;
		static int DENSE;
	};
	struct LINE {
		struct SINGLE {
			static int VERTICAL;
			static int HORIZONTAL;
			static int BOTTOM_LEFT;
			static int BOTTOM_RIGHT;
			static int TOP_LEFT;
			static int TOP_RIGHT;
			static int VERTICAL_LEFT;
			static int VERTICAL_RIGHT;
			static int HORIZONTAL_TOP;
			static int HORIZONTAL_BOTTOM;
			static int CROSS;
		};
		struct DOUBLE {
			static int VERTICAL;
			static int HORIZONTAL;
			static int BOTTOM_LEFT;
			static int BOTTOM_RIGHT;
			static int TOP_LEFT;
			static int TOP_RIGHT;
			static int VERTICAL_LEFT;
			static int VERTICAL_RIGHT;
			static int HORIZONTAL_TOP;
			static int HORIZONTAL_BOTTOM;
			static int CROSS;
		};
	};
	struct MATH {
		static int NOT_EQUAL;
		static int LESS_EQUAL;
		static int GREATER_EQUAL;
		static int APPROXIMATELY;
		static int SQUARE_ROOT;
		static int PI;
#undef INFINITY
		static int INFINITY;
	};
	struct FACE {
		static int WHITE;
		static int BLACK;
	};
};

/*! \brief Cursor visibility types.
 */
enum class CURSOR {
	INVISIBLE,
	VISIBLE,
	VERY_VISIBLE
};

/*! \brief TUI colors definitions.
 */
struct COLOR {
	typedef enum {
		FG_BLACK         = 0,
		FG_RED           = 1,
		FG_GREEN         = 2,
		FG_BROWN         = 3,
		FG_BLUE          = 4,
		FG_MAGENTA       = 5,
		FG_CYAN          = 6,
		FG_LIGHTGRAY     = 7,
		FG_BOLD          = 8,
		FG_GRAY          = FG_BLACK     | FG_BOLD,
		FG_BRIGHTRED     = FG_RED       | FG_BOLD,
		FG_BRIGHTGREEN   = FG_GREEN     | FG_BOLD,
		FG_YELLOW        = FG_BROWN     | FG_BOLD,
		FG_BRIGHTBLUE    = FG_BLUE      | FG_BOLD,
		FG_BRIGHTMAGENTA = FG_MAGENTA   | FG_BOLD,
		FG_BRIGHTCYAN    = FG_CYAN      | FG_BOLD,
		FG_WHITE         = FG_LIGHTGRAY | FG_BOLD,
		FG_MASK          = 0x07,

		BG_BLACK         = 0,
		BG_RED           = 16,
		BG_GREEN         = 32,
		BG_BROWN         = 48,
		BG_BLUE          = 64,
		BG_MAGENTA       = 80,
		BG_CYAN          = 96,
		BG_LIGHTGRAY     = 112,
		BG_BLINK         = 128,
		BG_GRAY          = BG_BLACK     | BG_BLINK,
		BG_BRIGHTRED     = BG_RED       | BG_BLINK,
		BG_BRIGHTGREEN   = BG_GREEN     | BG_BLINK,
		BG_YELLOW        = BG_BROWN     | BG_BLINK,
		BG_BRIGHTBLUE    = BG_BLUE      | BG_BLINK,
		BG_BRIGHTMAGENTA = BG_MAGENTA   | BG_BLINK,
		BG_BRIGHTCYAN    = BG_CYAN      | BG_BLINK,
		BG_WHITE         = BG_LIGHTGRAY | BG_BLINK,
		BG_MASK          = 0x70,
		ATTR_NORMAL      = ( FG_LIGHTGRAY | BG_BLACK ),
		ATTR_DEFAULT     = -1
	} color_t;
	static color_t complementary( color_t );
	static color_t combine( color_t, color_t );
	static color_t fg_to_bg( color_t );
	static color_t from_string( yaal::hcore::HString const& );
	static yaal::ansi::HSequence const& to_ansi( color_t );
};

/*! \brief Get special key code values.
 *
 * \tparam code - basic input code.
 * \retval meta - META modified code.
 * \retval ctrl - CONTROL modified code.
 * \retval commercial - CONTROL-x modified code (command).
 */
template<int code = 0>
struct KEY {
	static int const meta = code + KEY_CODE::META_BASE;
	static int meta_r( int code_ ) {
		return ( code_ + KEY_CODE::META_BASE );
	}
	static int const ctrl = code - KEY_CODE::CONTROL_BASE;
	static int ctrl_r( int code_ ) {
		return ( code_ - KEY_CODE::CONTROL_BASE );
	}
	static int const command = code + KEY_CODE::COMMAND_BASE;
	static int command_r( int code_ ) {
		return ( code_ + KEY_CODE::COMMAND_BASE );
	}
};

/*! \brief TUI input events types.
 */
struct EVENT {
	enum {
		ERROR,
		KEYBOARD,
		MOUSE
	};
};


/*! \brief Low level TUI description and modifier.
 */
class M_YAAL_HCONSOLE_PUBLIC_API HConsole : public yaal::hcore::HSingleton<HConsole>, yaal::hcore::HFormatter {
public:
	typedef HConsole this_type;
	typedef yaal::hcore::HSingleton<HConsole> base_type;
private:
	bool _initialized;
	bool _enabled;
	bool _brokenBrightBackground;
	int _width;
	int _height;
	int _mouseDes;
	void* _window;
	yaal::hcore::HPipe::ptr_t _event;
	mutable yaal::hcore::HUTF8String _utf8;
public:
	int get_height( void ) const;
	int get_width( void ) const;
	void enter_curses( void );
	void leave_curses( void );
	void set_attr( COLOR::color_t ) const;
	void set_background( COLOR::color_t ) const;
	void move( int, int ) const;
	CURSOR curs_set( CURSOR ) const;
	void addch( int );
	void refresh( bool = false );
	int endwin( void );
	void getyx( int&, int& ) const;
	void clrtoeol( void ) const;
	void addstr( yaal::hcore::HString const& ) const;
	template<typename... T>
	void printf( T const&... a_ ) const {
		return ( addstr( do_format( a_... ) ) );
	}
	template<typename... T>
	void mvprintf( int row_, int col_, T const&... a_ ) const {
		return ( do_mvprintf( row_, col_, do_format( a_... ) ) );
	}
	template<typename... T>
	void cmvprintf( int row_, int col_, COLOR::color_t attr_, T const&... a_ ) const {
		return ( do_cmvprintf( row_, col_, attr_, do_format( a_... ) ) );
	}
	void ungetch( int );
	int get_key( void ) const;
	int kbhit( void ) const;
	COLOR::color_t get_attr( void ) const;
	void clear_terminal( void );
	void clear( void );
	bool is_enabled( void ) const;
	int wait_for_user_input( int&, mouse::OMouse&, int = 0 ) const;
	void bell( void ) const;
	int get_mouse_fd( void ) const;
	yaal::hcore::HStreamInterface::ptr_t const& get_event_source( void ) const;
	void notify_terminal( void );
	void notify_mouse( void );
	void notify_keyboard( void );
	int on_terminal_resize( int );
	int console_cleanup( int );
	int on_quit( int );
	int on_tstp( int );
	int on_cont( int );
	int on_mouse( int );
	static void set_escdelay( int );
protected:
	void do_mvprintf( int, int, yaal::hcore::HString const& ) const;
	void do_cmvprintf( int, int, COLOR::color_t, yaal::hcore::HString const& ) const;
	void init( void );
private:
	HConsole( void );
	~HConsole( void );
	HConsole( HConsole const& );
	HConsole& operator = ( HConsole const& );
	friend class yaal::hcore::HSingleton<HConsole>;
	friend class yaal::hcore::HDestructor<HConsole>;
};

typedef yaal::hcore::HExceptionT<HConsole, yaal::hcore::HExceptionT<yaal::hcore::HSingleton<HConsole>>> HConsoleException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED */

