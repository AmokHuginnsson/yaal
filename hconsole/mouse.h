/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	mouse.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCONSOLE_MOUSE_H
#define __YAAL_HCONSOLE_MOUSE_H

namespace yaal
{

namespace hconsole
{

/*! \brief Console (TUI) and X mouse handling.
 */
namespace mouse
{

/*! \brief Meta-data for mouse interface.
 */
struct MOUSE_BITS
	{
	/*! \brief Mouse button identifiers.
	 */
	struct BUTTONS
		{
		static int const D_ONE 		= 1; /*!< Button 1. */
		static int const D_TWO		= 2; /*!< Button 2. */
		static int const D_THREE	= 4; /*!< Button 3. */
		};
	};

/*! \brief Mouse input event data.
 */
typedef struct
	{
	int f_iButtons; /*!< Button state mask. */
	int f_iRow;     /*!< Cursor Y position. */
	int f_iColumn;  /*!< Cursor X position. */
	} OMouse;

typedef int ( * fun_console_mouse_open_t ) ( void );
typedef int ( * fun_console_mouse_get_t ) ( OMouse & );
typedef int ( * fun_console_mouse_close_t ) ( void );

extern fun_console_mouse_open_t mouse_open;
extern fun_console_mouse_get_t mouse_get;
extern fun_console_mouse_close_t mouse_close;

int console_mouse_open ( void );
int console_mouse_get ( OMouse & );
int console_mouse_close ( void );
int x_mouse_open ( void );
int x_mouse_get ( OMouse & );
int x_mouse_close ( void );

}

}

}

#endif /* not __YAAL_HCONSOLE_MOUSE_H */

