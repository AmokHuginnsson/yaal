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

#include <unistd.h>
#include <libintl.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */
#ifdef HAVE_SYS_CONSIO_H
#	include <sys/consio.h>
#	include <signal.h>
#	include <fcntl.h>
#elif defined ( HAVE_GPM_H )
#	include <gpm.h>
#endif /* HAVE_GPM_H */

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "mouse.h"
#include "console.h"
#include "hcore/hstring.h"
#include "hcore/hlog.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

namespace mouse
{

fun_console_mouse_open_t mouse_open = NULL;
fun_console_mouse_get_t mouse_get = NULL;
fun_console_mouse_close_t mouse_close = NULL;

int hunt_tty ( int a_iOffset )
	{
	M_PROLOG
	/* this hack allows to guess current controling virtual terminal screen */
	int l_iVC = 0;
	char * l_pcTtyName = NULL;
	l_pcTtyName = ttyname ( STDIN_FILENO );
	if ( l_pcTtyName && ! strncmp ( l_pcTtyName, "/dev/ttyv", 8 + a_iOffset ) )
		l_iVC = l_pcTtyName [ 8 + a_iOffset ] - '0';
	else
		{
		l_pcTtyName = ::getenv ( "STY" );
		if ( l_pcTtyName && ( l_pcTtyName = strstr ( l_pcTtyName, ".tty" ) ) )
			l_iVC = l_pcTtyName [ 4 + a_iOffset ] - '0';
		else M_THROW ( "can not find controling virtual console", g_iErrNo );
		}
	return ( l_iVC );
	M_EPILOG
	}

#ifdef HAVE_SYS_CONSIO_H

int n_iMouse = 0;

int console_mouse_open ( void )
	{
	M_PROLOG
	int l_iVC = 0;
	char l_pcTty [ ] = "/dev/ttyv0";
	HString l_oError;
	mouse_info l_sMouse;
	l_sMouse.operation = MOUSE_MODE;
	l_sMouse.u.mode.mode = 0;
	l_sMouse.u.mode.signal = SIGUSR1;
	l_iVC = hunt_tty ( 1 );
	l_pcTty [ 9 ] = '0' + l_iVC;
	n_iMouse = open ( l_pcTty, O_RDWR );
	if ( n_iMouse < 0 )
		{
		l_oError.format ( _ ( "can not open mouse, %s" ), strerror ( g_iErrNo ) );
		M_THROW ( l_oError, l_iVC );
		}
	if ( ioctl ( n_iMouse, CONS_MOUSECTL, & l_sMouse ) < 0 )
		{
		close ( n_iMouse );
		M_THROW ( _ ( "can not setup mouse mode" ), g_iErrNo );
		}

	log ( D_LOG_INFO ) << "i have opened device: `" << l_pcTty << '\'' << endl;

	return ( 0 );
	M_EPILOG
	}

int console_mouse_get ( OMouse & a_rsMouse )
	{
	M_PROLOG
	mouse_info l_sMouse;
	l_sMouse.operation = MOUSE_GETINFO;
	if ( ioctl ( n_iMouse, CONS_MOUSECTL, & l_sMouse ) < 0 )
		M_THROW ( "can not get mouse data", g_iErrNo );
	else
		{
		a_rsMouse.f_iButtons = l_sMouse.u.data.buttons;
		a_rsMouse.f_iRow = l_sMouse.u.data.y / 16;
		a_rsMouse.f_iColumn = l_sMouse.u.data.x / 8;
		}
	return ( 0 );
	M_EPILOG
	}

int console_mouse_close ( void )
	{
	M_PROLOG
	if ( ! n_iMouse )
		M_THROW ( "mouse not opened", g_iErrNo );
	close ( n_iMouse );
	n_iMouse = 0;
	return ( 0 );
	M_EPILOG
	}

#elif defined ( HAVE_GPM_H )

int console_mouse_open ( void )
	{
	M_PROLOG
	int l_iVC = 0;
	HString l_oError;
	Gpm_Connect l_sGpm;
	l_sGpm.minMod = 0;
	l_sGpm.maxMod = 0;
	l_sGpm.pid = ::getpid ( );
	l_sGpm.vc = l_iVC = hunt_tty ( 0 );
	l_sGpm.eventMask = GPM_SINGLE | GPM_DOWN | GPM_UP;
	l_sGpm.defaultMask = ~ l_sGpm.eventMask;
	gpm_zerobased = true;
	if ( Gpm_Open ( & l_sGpm, l_iVC ) == -1 )
		{
		l_oError.format ( "Can't open mouse connection: %s",
				strerror ( g_iErrNo ) );
		M_THROW ( l_oError, l_iVC );
		}
	return ( gpm_fd );
	M_EPILOG
	}

int console_mouse_get ( OMouse & a_rsMouse )
	{
	M_PROLOG
	Gpm_Event l_sEvent;
	if ( Gpm_GetEvent ( & l_sEvent ) != 1 )
		M_THROW ( _ ( "can not retrieve event") , g_iErrNo );
	a_rsMouse.f_iButtons = l_sEvent.buttons;
	a_rsMouse.f_iRow = l_sEvent.y;
	a_rsMouse.f_iColumn = l_sEvent.x;
	return ( 0 );
	M_EPILOG
	}

int console_mouse_close ( void )
	{
	M_PROLOG
	while ( Gpm_Close ( ) );
	return ( 0 );
	M_EPILOG
	}

#else /* HAVE_GPM_H */

int console_mouse_open ( void )
	{
	M_PROLOG
	M_THROW ( _ ( "console mouse support not compiled" ), g_iErrNo );
	return ( 0 );
	M_EPILOG
	}

int console_mouse_get ( OMouse & )
	{
	M_PROLOG
	M_THROW ( _ ( "console mouse support not compiled" ), g_iErrNo );
	return ( 0 );
	M_EPILOG
	}

int console_mouse_close ( void )
	{
	M_PROLOG
/*	I can not throw exception here bacause exception was probably
 *	thrown by console_mouse_open in enter_curses and now this
 *	function is probably called from leave_curses from exception
 *	catch block, so new exception would be recursive and would
 *	overload the stack because of infinite number of exceptions */
	return ( 0 );
	M_EPILOG
	}

#endif /* ! HAVE_GPM_H */

int x_mouse_open ( void )
	{
	M_PROLOG
	mmask_t l_xMouseMask, l_xDesiredMouseMask;
	l_xDesiredMouseMask = BUTTON1_CLICKED | BUTTON2_CLICKED | BUTTON3_CLICKED;
	l_xMouseMask = mousemask ( l_xDesiredMouseMask, NULL );
	if ( ! l_xMouseMask )
		M_THROW ( "mousemask ( ) returned 0", g_iErrNo );
	else if ( ( l_xMouseMask & l_xDesiredMouseMask ) < l_xDesiredMouseMask )
		{
		HString l_oError;
		l_oError.format ( "1 = %d, 2 = %d, 3 = %d",
				l_xMouseMask & BUTTON1_CLICKED, l_xMouseMask & BUTTON2_CLICKED,
				l_xMouseMask & BUTTON3_CLICKED );
#ifdef __EXCEPTIONS_BY_REFERENCE__
		HException l_oException ( __WHERE__, "could not set up apropriate mask",
				l_xMouseMask );
		l_oException.set ( l_oError );
		throw ( l_oException );
#else /* __EXCEPTIONS_BY_REFERENCE__ */
		HException * l_poException = NULL;
		l_poException = new HException ( __WHERE__,
				"could not set up apropriate mask", l_xMouseMask );
		l_poException->set ( l_oError );
		throw ( l_poException );
#endif /* not __EXCEPTIONS_BY_REFERENCE__ */
		}
	return ( 0 );
	M_EPILOG
	}

int x_mouse_get ( OMouse & a_rsMouse )
	{
	M_PROLOG
	MEVENT l_sMouse;
	if ( getmouse ( & l_sMouse ) != OK )
		M_THROW ( "can not get mouse data", g_iErrNo );
	else
		{
		a_rsMouse.f_iButtons = l_sMouse.bstate;
		a_rsMouse.f_iRow = l_sMouse.y;
		a_rsMouse.f_iColumn = l_sMouse.x;
		}
	return ( 0 );
	M_EPILOG
	}

int x_mouse_close ( void )
	{
	return ( 0 );
	}

}

}

}

