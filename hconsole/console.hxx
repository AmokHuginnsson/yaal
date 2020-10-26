/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED
#define YAAL_HCONSOLE_CONSOLE_HXX_INCLUDED 1

#include "hcore/hsingleton.hxx"
#include "hcore/hpipe.hxx"
#include "hcore/hformat.hxx"
#include "tools/color.hxx"
#include "tools/signals.hxx"
#include "hconsole/mouse.hxx"

namespace yaal {

/*! \brief User interface.
 *
 * This namespace contains library API required to build
 * terminal based user interface (TUI).
 */
namespace hconsole {

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
#pragma push_macro( "INFINITY" )
#undef INFINITY
		static int INFINITY;
#pragma pop_macro( "INFINITY" )
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
	bool _brightByBold;
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
	void set_attr( tools::COLOR::color_t ) const;
	void set_background( tools::COLOR::color_t ) const;
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
	void cmvprintf( int row_, int col_, tools::COLOR::color_t attr_, T const&... a_ ) const {
		return ( do_cmvprintf( row_, col_, attr_, do_format( a_... ) ) );
	}
	void ungetch( int );
	int get_key( void ) const;
	int kbhit( void ) const;
	tools::COLOR::color_t get_attr( void ) const;
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
	void do_cmvprintf( int, int, tools::COLOR::color_t, yaal::hcore::HString const& ) const;
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

