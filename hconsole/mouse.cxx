/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	mouse.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <libintl.h>

#include "config.hxx"

#ifdef HAVE_SYS_CONSIO_H
#	include <sys/consio.h>
#	include <csignal>
#	include <fcntl.h>
#elif defined ( HAVE_GPM_H )
#	include <gpm.h>
#endif /* HAVE_GPM_H */

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "mouse.hxx"
#include "console.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hlog.hxx"

#ifdef HAVE_CURSES_H
#	include <curses.h>
#elif defined ( HAVE_NCURSES_CURSES_H )
#	include <ncurses/curses.h>
#else /* HAVE_NCURSES_CURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_CURSES_H */

using namespace yaal::hcore;

namespace yaal
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
	char const* l_pcTtyName = NULL;
	char const* l_pcPtr = NULL;
	l_pcTtyName = ttyname( STDIN_FILENO );
	if ( l_pcTtyName && ! ::strncmp( l_pcTtyName, "/dev/ttyv", 8 + a_iOffset ) )
		l_iVC = lexical_cast<int>( l_pcTtyName + 8 + a_iOffset );
	else
		{
		l_pcTtyName = ::getenv( "STY" );
		if ( l_pcTtyName )
			{
			if ( ( l_pcPtr = ::strstr( l_pcTtyName, ".tty" ) ) )
				l_iVC = lexical_cast<int>( l_pcPtr + 4 + a_iOffset );
			else if ( ( l_pcPtr = ::strstr( l_pcTtyName, ".pts" ) ) )
				l_iVC = lexical_cast<int>( l_pcPtr + 4 + a_iOffset );
			}
		else
			M_THROW( "cannot find controling virtual console", errno );
		}
	return ( l_iVC );
	M_EPILOG
	}

#ifdef HAVE_SYS_CONSIO_H

int n_iMouse = 0;

int console_mouse_open( void )
	{
	M_PROLOG
	int l_iVC = 0;
	char l_pcTty[] = "/dev/ttyv0";
	HString l_oError;
	mouse_info l_sMouse;
	l_sMouse.operation = MOUSE_MODE;
	l_sMouse.u.mode.mode = 0;
	l_sMouse.u.mode.signal = SIGUSR1;
	l_iVC = hunt_tty( 1 );
	l_pcTty[ 9 ] = '0' + l_iVC;
	n_iMouse = ::open( l_pcTty, O_RDWR );
	if ( n_iMouse < 0 )
		{
		l_oError.format( _( "cannot open mouse, %s" ), error_message( errno ) );
		M_THROW( l_oError, l_iVC );
		}
	if ( ::ioctl( n_iMouse, CONS_MOUSECTL, &l_sMouse ) < 0 )
		{
		l_oError.format( _( "cannot setup mouse mode, %s" ), error_message( errno ) );
		TEMP_FAILURE_RETRY( ::close( n_iMouse ) );
		M_THROW( l_oError, errno );
		}

	log( LOG_TYPE::INFO ) << "i have opened device: `" << l_pcTty << '\'' << endl;

	return ( 0 );
	M_EPILOG
	}

int console_mouse_get ( OMouse & a_rsMouse )
	{
	M_PROLOG
	mouse_info l_sMouse;
	l_sMouse.operation = MOUSE_GETINFO;
	if ( ::ioctl( n_iMouse, CONS_MOUSECTL, &l_sMouse ) < 0 )
		M_THROW( "cannot get mouse data", errno );
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
		M_THROW( "mouse not opened", errno );
	TEMP_FAILURE_RETRY( ::close( n_iMouse ) );
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
	l_sGpm.pid = ::getpid();
	l_sGpm.vc = l_iVC = hunt_tty( 0 );
	l_sGpm.eventMask = static_cast<int>( GPM_UP );
	l_sGpm.defaultMask = static_cast<int short unsigned>( ~l_sGpm.eventMask );
	gpm_zerobased = true;
	if ( Gpm_Open( &l_sGpm, l_iVC ) == -1 )
		{
		l_oError.format( "Can't open mouse connection: %s", error_message( errno ) );
		M_THROW( l_oError, l_iVC );
		}
	log( LOG_TYPE::INFO ) << "i have opened device: `" << l_iVC << '\'' << endl;
	return ( gpm_fd );
	M_EPILOG
	}

int console_mouse_get( OMouse& a_rsMouse )
	{
	M_PROLOG
	Gpm_Event l_sEvent;
	if ( Gpm_GetEvent( &l_sEvent ) != 1 )
		M_THROW( _( "cannot retrieve event") , errno );
	a_rsMouse.f_iButtons = l_sEvent.buttons;
	a_rsMouse.f_iRow = l_sEvent.y;
	a_rsMouse.f_iColumn = l_sEvent.x;
	return ( 0 );
	M_EPILOG
	}

int console_mouse_close( void )
	{
	M_PROLOG
	while ( Gpm_Close() )
		;
	return ( 0 );
	M_EPILOG
	}

#else /* HAVE_GPM_H */

int console_mouse_open ( void )
	{
	M_PROLOG
	int a( 1 );
	if ( a )
		M_THROW( _( "console mouse support not compiled" ), errno );
	return ( 0 );
	M_EPILOG
	}

int console_mouse_get( OMouse& )
	{
	M_PROLOG
	int a( 1 );
	if ( a )
		M_THROW( _( "console mouse support not compiled" ), errno );
	return ( 0 );
	M_EPILOG
	}

int console_mouse_close( void )
	{
	M_PROLOG
/*	I cannot throw exception here bacause exception was probably
 *	thrown by console_mouse_open in enter_curses and now this
 *	function is probably called from leave_curses from exception
 *	catch block, so new exception would be recursive and would
 *	overload the stack because of infinite number of exceptions */
	return ( 0 );
	M_EPILOG
	}

#endif /* ! HAVE_GPM_H */

int x_mouse_open( void )
	{
	M_PROLOG
	mmask_t l_xMouseMask, l_xDesiredMouseMask;
	l_xDesiredMouseMask = BUTTON1_CLICKED | BUTTON2_CLICKED | BUTTON3_CLICKED;
	l_xMouseMask = mousemask( l_xDesiredMouseMask, NULL );
	if ( ! l_xMouseMask )
		M_THROW( "mousemask() returned 0", errno );
	else if ( ( l_xMouseMask & l_xDesiredMouseMask ) < l_xDesiredMouseMask )
		{
		HString l_oError;
		HException l_oException( __FILE__, __PRETTY_FUNCTION__, __LINE__, "could not set up apropriate mask",
				static_cast<int>( l_xMouseMask ) );
		l_oError.format( "1 = %d, 2 = %d, 3 = %d",
				l_xMouseMask & BUTTON1_CLICKED, l_xMouseMask & BUTTON2_CLICKED,
				l_xMouseMask & BUTTON3_CLICKED );
		l_oException.set( l_oError );
		throw ( l_oException );
		}
	return ( 0 );
	M_EPILOG
	}

int x_mouse_get( OMouse& a_rsMouse )
	{
	M_PROLOG
	MEVENT l_sMouse;
	if ( getmouse( &l_sMouse ) != OK )
		M_THROW( "cannot get mouse data", errno );
	else
		{
		a_rsMouse.f_iButtons = static_cast<int>( l_sMouse.bstate );
		a_rsMouse.f_iRow = l_sMouse.y;
		a_rsMouse.f_iColumn = l_sMouse.x;
		}
	return ( 0 );
	M_EPILOG
	}

int x_mouse_close( void )
	{
	return ( 0 );
	}

}

}

}

