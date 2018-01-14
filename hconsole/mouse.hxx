/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_MOUSE_HXX_INCLUDED
#define YAAL_HCONSOLE_MOUSE_HXX_INCLUDED 1

#include "hcore/hexception.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Console (TUI) and X mouse handling.
 */
namespace mouse {

/*! \brief Meta-data for mouse interface.
 */
struct MOUSE_BITS {
	/*! \brief Mouse button identifiers.
	 */
	struct BUTTON {
		static int const ONE        = 1;       /*!< Button 1, single click. */
		static int const ONE_2      = 1 | 64;  /*!< Button 1, double clicked. */
		static int const TWO        = 2;       /*!< Button 2, single click. */
		static int const TWO_2      = 2 | 64;  /*!< Button 2, double clicked. */
		static int const THREE      = 4;       /*!< Button 3, single click. */
		static int const THREE_2    = 4 | 64;  /*!< Button 3, double clicked. */
		static int const WHEEL_UP   = 8;       /*!< Wheel rotated up. */
		static int const WHEEL_DOWN = 16;      /*!< Wheel rotated down. */
	};
};

/*! \brief Mouse input event data.
 */
typedef struct {
	int _buttons; /*!< Button state mask. */
	int _row;     /*!< Cursor Y position. */
	int _column;  /*!< Cursor X position. */
} OMouse;

typedef yaal::hcore::HExceptionT<OMouse> HMouseException;

typedef int ( * fun_console_mouse_open_t )( void );
typedef int ( * fun_console_mouse_get_t )( OMouse& );
typedef int ( * fun_console_mouse_close_t )( void );

extern fun_console_mouse_open_t mouse_open;
extern fun_console_mouse_get_t mouse_get;
extern fun_console_mouse_close_t mouse_close;

int console_mouse_open( void );
int console_mouse_get( OMouse& );
int console_mouse_close( void );
int x_mouse_open( void );
int x_mouse_get( OMouse& );
int x_mouse_close( void );

}

}

}

#endif /* #ifndef YAAL_HCONSOLE_MOUSE_HXX_INCLUDED */

