/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	mouse.c - this file is integral part of `stdhapi' project.

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

#include "../config.h"

#ifdef HAVE_SYS_CONSIO_H
#	include <sys/consio.h>
#	include <signal.h>
#	include <fcntl.h>
#	include <unistd.h>
#endif /* HAVE_SYS_CONSIO_H */

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "mouse.h"

namespace mouse
{

#ifdef HAVE_SYS_CONSIO_H

int n_iMouse = 0;

int mouse_open ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	char l_pcTty [ ] = "/dev/ttyv0";
	mouse_info l_sMouse;
	l_sMouse.operation = MOUSE_MODE;
	l_sMouse.u.mode.mode = 1;
	l_sMouse.u.mode.signal = SIGUSR1;
	
	/* this hack allows to guess current controling virtual terminal screen */

	while ( l_iCtr < 12 )
		{
		l_pcTty [ 9 ] = '0' + l_iCtr ++;
		n_iMouse = open ( l_pcTty, O_RDONLY );
		if ( n_iMouse < 0 )continue;
		if ( ioctl ( n_iMouse, CONS_MOUSECTL, & l_sMouse ) >= 0 )break;
		close ( n_iMouse );
		n_iMouse = -1;
		}

	if ( n_iMouse < 0 )
		M_THROW ( "can not open mouse", g_iErrNo );

	return ( n_iMouse );
	M_EPILOG
	}

int mouse_get ( OMouse & a_sMouse )
	{
	mouse_info l_sMouse;
	if ( ioctl ( n_iMouse, CONS_MOUSECTL, & l_sMouse ) < 0 )
		M_THROW ( "can not get mouse data", g_iErrNo );
	else
		{
		a_sMouse.f_iButtons = l_sMouse.u.data.buttons;
		a_sMouse.f_iRow = l_sMouse.u.data.y / 16;
		a_sMouse.f_iColumn = l_sMouse.u.data.x / 8;
		}
	return ( 0 );
	}

int mouse_close ( void )
	{
	if ( ! n_iMouse )
		M_THROW ( "mouse not opened", g_iErrNo );
	close ( n_iMouse );
	n_iMouse = 0;
	return ( 0 );
	}

#endif /* HAVE_SYS_CONSIO_H */

}

