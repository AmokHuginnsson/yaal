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

#include <cstdarg>

#include "hcore/hsingleton.hxx"
#include "hcore/hpipe.hxx"
#include "tools/signals.hxx"
#include "hconsole/mouse.hxx"

namespace yaal {

/*! \brief User inteface.
 *
 * This namespace contains library API required to build
 * terminal based user interface (TUI).
 */
namespace hconsole {

extern int const C_OK;
extern int const C_ERR;

/*! \brief Special key codes.
 */
struct KEY_CODES {
	static int const ESC					= 27;
	/* Coincidentaly KEY_MAX from ncurses is 0777 which is 511. */
	static int const SPECIAL_KEY  = 0x400;
	static int const PAGE_UP			= SPECIAL_KEY;
	static int const PAGE_DOWN		= PAGE_UP		+ 1;
	static int const DOWN					= PAGE_DOWN	+ 1;
	static int const UP						= DOWN			+ 1;
	static int const LEFT					= UP				+ 1;
	static int const RIGHT				= LEFT			+ 1;
	static int const HOME					= RIGHT			+ 1;
	static int const END					= HOME			+ 1;
	static int const DELETE				= END				+ 1;
	static int const INSERT				= DELETE		+ 1;
	static int const BACKSPACE		= INSERT		+ 1;
	static int const MOUSE				=	BACKSPACE	+ 1;
	static int const META_BASE		= 0x04000;
	static int const COMMAND_BASE	= 0x08000;
};

/*! \brief Quasi graphic glyphs.
 */
struct GLYPHS {
	static int DOWN_ARROW;
	static int UP_ARROW;
	static int VERTICAL_LINE;
};

/*! \brief Cursor visibility types.
 */
struct CURSOR {
	typedef enum {
		INVISIBLE,
		VISIBLE,
		VERY_VISIBLE
	} cursor_t;
};

/*! \brief TUI colors definitions.
 */
struct COLORS {
	enum {
		FG_BLACK         = 0,
		FG_RED           = 1,
		FG_GREEN         = 2,
		FG_BROWN         = 3,
		FG_BLUE          = 4,
		FG_MAGENTA       = 5,
		FG_CYAN          = 6,
		FG_LIGHTGRAY     = 7,
		FG_GRAY          = 8,
		FG_BRIGHTRED     = 9,
		FG_BRIGHTGREEN   = 10,
		FG_YELLOW        = 11,
		FG_BRIGHTBLUE    = 12,
		FG_BRIGHTMAGENTA = 13,
		FG_BRIGHTCYAN    = 14,
		FG_WHITE         = 15,

		BG_BLACK         = 0,
		BG_RED           = 16,
		BG_GREEN         = 32,
		BG_BROWN         = 48,
		BG_BLUE          = 64,
		BG_MAGENTA       = 80,
		BG_CYAN          = 96,
		BG_LIGHTGRAY     = 112,
		BG_BLINK         = 128,
		BG_GRAY          = BG_BLINK,
		BG_BRIGHTRED     = 16 | BG_BLINK,
		BG_BRIGHTGREEN   = 32 | BG_BLINK,
		BG_YELLOW        = 48 | BG_BLINK,
		BG_BRIGHTBLUE    = 64 | BG_BLINK,
		BG_BRIGHTMAGENTA = 80 | BG_BLINK,
		BG_BRIGHTCYAN    = 96 | BG_BLINK,
		BG_WHITE         = 112 | BG_BLINK,
		ATTR_NORMAL      = ( FG_LIGHTGRAY | BG_BLACK ),
		ATTR_DEFAULT     = -1
	};
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
	static int const meta = code + KEY_CODES::META_BASE;
	static int meta_r( int code_ ) {
		return ( code_ + KEY_CODES::META_BASE );
	}
	static int const ctrl = code - 96;
	static int ctrl_r( int code_ ) {
		return ( code_ - 96 );
	}
	static int const command = code + KEY_CODES::COMMAND_BASE;
	static int command_r( int code_ ) {
		return ( code_ + KEY_CODES::COMMAND_BASE );
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

extern M_YAAL_HCONSOLE_PUBLIC_API bool _needRepaint_;

/*! \brief Low level TUI description and modifier.
 */
class HConsole : public yaal::hcore::HSingleton<HConsole> {
public:
	typedef HConsole this_type;
	typedef yaal::hcore::HSingleton<HConsole> base_type;
private:
	bool _initialized;
	int _width;
	int _height;
	int _mouseDes;
	yaal::hcore::HPipe::ptr_t _event;
public:
	int get_height( void ) const;
	int get_width( void ) const;
	void enter_curses( void );
	void leave_curses( void );
	void set_attr( int ) const;
	void move( int, int ) const;
	CURSOR::cursor_t curs_set( CURSOR::cursor_t const& ) const;
	void addch( int );
	void refresh( void );
	int endwin( void );
	void getmaxyx( void );
	void getyx( int&, int& ) const;
	void clrtoeol( void ) const;
	void printf( char const* const, ... ) const;
	void mvprintf( int, int, char const* const, ... ) const;
	void cmvprintf( int, int, int, char const* const, ... ) const;
	int ungetch( int );
	int get_key( void ) const;
	int kbhit( void ) const;
	char unsigned get_attr( void ) const;
	void clrscr( void );
	bool is_enabled( void ) const;
	int wait_for_user_input( int&, mouse::OMouse&, int = 0 ) const;
	void bell( void ) const;
	int get_mouse_fd( void ) const;
	int get_event_fd( void ) const;
	int on_terminal_resize( int );
	int console_cleanup( int );
	int on_quit( int );
	int on_tstp( int );
	int on_cont( int );
	int on_mouse( int );
	static void set_escdelay( int );
protected:
	void vmvprintf ( int, int, char const* const, void* ) const;
	void vcmvprintf ( int, int, int, char const* const, void* ) const;
	void vprintf ( char const* const, void* ) const;
	void init( void );
private:
	HConsole( void );
	~HConsole( void );
	friend class yaal::hcore::HSingleton<HConsole>;
	friend class yaal::hcore::HDestructor<HConsole>;
};

typedef yaal::hcore::HExceptionT<HConsole, yaal::hcore::HSingletonException> HConsoleException;

extern M_YAAL_HCONSOLE_PUBLIC_API char const* const bold;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const reset;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const black;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const red;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const green;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brown;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const blue;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const magenta;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const cyan;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const lightgray;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const gray;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightred;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightgreen;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const yellow;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightblue;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightmagenta;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightcyan;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const white;

}

}

#endif /* #ifndef YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED */

