/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	mouse.hxx - this file is integral part of `yaal' project.

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
	struct BUTTONS {
		static int const ONE 		= 1; /*!< Button 1. */
		static int const TWO		= 2; /*!< Button 2. */
		static int const THREE	= 4; /*!< Button 3. */
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

