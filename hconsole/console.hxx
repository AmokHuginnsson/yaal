/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	console.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED
#define YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED

#include <cstdarg>

#ifdef HAVE_CONFIG_H
#	include "config.hxx"
#endif /* HAVE_CONFIG_H */

#include "hcore/hsingleton.hxx"
#include "hcore/hpipe.hxx"
#include "tools/signals.hxx"
#include "hconsole/mouse.hxx"

namespace yaal
{

/*! \brief User inteface.
 * 
 * This namespace contains library API required to build
 * terminal based user interface (TUI).
 */
namespace hconsole
{

extern int const C_OK;
extern int const C_ERR;

/*! \brief Special key codes.
 */
struct KEY_CODES
	{
	static int const D_ESC					= 27;
	static int const D_PAGE_UP			= 256;
	static int const D_PAGE_DOWN		= D_PAGE_UP		+ 1;
	static int const D_DOWN					= D_PAGE_DOWN	+ 1;
	static int const D_UP						= D_DOWN			+ 1;
	static int const D_LEFT					= D_UP				+ 1;
	static int const D_RIGHT				= D_LEFT			+ 1;
	static int const D_HOME					= D_RIGHT			+ 1;
	static int const D_END					= D_HOME			+ 1;
	static int const D_DELETE				= D_END				+ 1;
	static int const D_INSERT				= D_DELETE		+ 1;
	static int const D_BACKSPACE		= D_INSERT		+ 1;
	static int const D_MOUSE				=	D_BACKSPACE	+ 1;
	static int const D_META_BASE		= 0x04000;
	static int const D_COMMAND_BASE	= 0x08000;
	};

/*! \brief Quasi graphic glyphs.
 */
struct GLYPHS
	{
	static int D_DOWN_ARROW;
	static int D_UP_ARROW;
	static int D_VERTICAL_LINE;
	};

/*! \brief Cursor visibility types.
 */
struct CURSOR
	{
	typedef enum
		{
		D_INVISIBLE, 
		D_VISIBLE, 
		D_VERY_VISIBLE
		} cursor_t;
	};

/*! \brief TUI colors definitions.
 */
struct COLORS
	{
	enum
		{
		D_FG_BLACK         = 0,
		D_FG_RED           = 1,
		D_FG_GREEN         = 2,
		D_FG_BROWN         = 3,
		D_FG_BLUE          = 4,
		D_FG_MAGENTA       = 5,
		D_FG_CYAN          = 6,
		D_FG_LIGHTGRAY     = 7,
		D_FG_GRAY          = 8,
		D_FG_BRIGHTRED     = 9,
		D_FG_BRIGHTGREEN   = 10,
		D_FG_YELLOW        = 11,
		D_FG_BRIGHTBLUE    = 12,
		D_FG_BRIGHTMAGENTA = 13,
		D_FG_BRIGHTCYAN    = 14,
		D_FG_WHITE         = 15,

		D_BG_BLACK         = 0,
		D_BG_RED           = 16,
		D_BG_GREEN         = 32,
		D_BG_BROWN         = 48,
		D_BG_BLUE          = 64,
		D_BG_MAGENTA       = 80,
		D_BG_CYAN          = 96,
		D_BG_LIGHTGRAY     = 112,
		D_BG_BLINK         = 128,
		D_BG_GRAY          = D_BG_BLINK,
		D_BG_BRIGHTRED     = 16 | D_BG_BLINK,
		D_BG_BRIGHTGREEN   = 32 | D_BG_BLINK,
		D_BG_YELLOW        = 48 | D_BG_BLINK,
		D_BG_BRIGHTBLUE    = 64 | D_BG_BLINK,
		D_BG_BRIGHTMAGENTA = 80 | D_BG_BLINK,
		D_BG_BRIGHTCYAN    = 96 | D_BG_BLINK,
		D_BG_WHITE         = 112 | D_BG_BLINK,
		D_ATTR_NORMAL      = ( D_FG_LIGHTGRAY | D_BG_BLACK ),
		D_ATTR_DEFAULT     = -1
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
struct KEY
	{
	static int const meta = code + KEY_CODES::D_META_BASE;
	static int meta_r( int a_iCode )
		{
		return ( a_iCode + KEY_CODES::D_META_BASE );
		}
	static int const ctrl = code - 96;
	static int ctrl_r( int a_iCode )
		{
		return ( a_iCode - 96 );
		}
	static int const command = code + KEY_CODES::D_COMMAND_BASE;
	static int command_r( int a_iCode )
		{
		return ( a_iCode + KEY_CODES::D_COMMAND_BASE );
		}
	};

/*! \brief TUI input events types.
 */
struct EVENT
	{
	enum
		{
		D_ERROR,
		D_KEYBOARD,
		D_MOUSE
		};
	};

extern bool n_bNeedRepaint;

/*! \brief Low level TUI description and modifier.
 */
class HConsole : public yaal::tools::HSignalHandlerInterface, private yaal::hcore::HSingletonInterface
	{
protected:
	typedef HConsole self_t;
	typedef HSignalHandlerInterface hier_t;
private:
	bool f_bInitialized;
	int f_iWidth;
	int f_iHeight;
	int f_iMouseDes;
	yaal::hcore::HPipe::ptr_t f_oEvent;
public:
	int const& get_height( void ) const;
	int const& get_width( void ) const;
	void enter_curses( void );
	void leave_curses( void );
	void set_attr( int ) const;
	int c_move ( int const&, int const& );
	CURSOR::cursor_t curs_set( CURSOR::cursor_t const& ) const;
	int c_addch( int const& );
	int c_refresh( void );
	int endwin( void );
	void c_getmaxyx( void );
	void c_getyx( int&, int& );
	void c_clrtoeol( void );
	int c_printf( char const* const, ... ) const;
	int c_mvprintf( int, int, char const* const, ... ) const;
	int c_cmvprintf( int, int, int, char const* const, ... ) const;
	int ungetch( int ) const;
	int get_key( void ) const;
	int kbhit( void ) const;
	char unsigned get_attr( void ) const;
	void clrscr( void ) const;
	bool is_enabled( void ) const;
	int wait_for_user_input( int&, mouse::OMouse&, int = 0, int = 0 ) const;
	void bell( void ) const;
	int get_mouse_fd( void ) const;
	int get_event_fd( void ) const;
	int on_terminal_resize( int );
	int console_cleanup( int );
	int on_quit( int );
	int on_tstp( int );
	int on_cont( int );
	int on_mouse( int );
protected:
	int c_vmvprintf ( int, int, char const* const, void* ) const;
	int c_vcmvprintf ( int, int, int, char const* const, void* ) const;
	int c_vprintf ( char const* const, void* ) const;
	void init( void );
private:
	HConsole( void );
	~HConsole( void );
	friend class yaal::hcore::HSingleton<HConsole>;
	friend class yaal::hcore::HDestructor<HConsole>;
	};

extern char const* const red;
extern char const* const green;
extern char const* const brown;
extern char const* const blue;
extern char const* const magenta;
extern char const* const cyan;
extern char const* const lightgray;
extern char const* const gray;
extern char const* const brightred;
extern char const* const brightgreen;
extern char const* const yellow;
extern char const* const brightblue;
extern char const* const brightmagenta;
extern char const* const brightcyan;
extern char const* const white;

typedef yaal::hcore::HSingleton<HConsole> HCons;

}

}

#endif /* not YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED */

