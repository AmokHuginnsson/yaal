/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	htuiprocess.c - this file is integral part of `stdhapi' project.

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

#include <cstring>
#include <unistd.h>
#include <libintl.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" )
#include "htuiprocess.h"
#include "hconsole.h"
#include "hmainwindow.h"
#include "mouse.h"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HTUIProcess::HTUIProcess ( size_t a_uiFileHandlers, size_t a_uiKeyHandlers,
		size_t a_uiCommandHandlers )
	: HHandler ( a_uiKeyHandlers, a_uiCommandHandlers ),
	HProcess ( a_uiFileHandlers ), f_poForegroundWindow ( NULL ),
	f_poWindows ( NULL )
	{
	M_PROLOG
	memset ( & f_sLatency, 0, sizeof ( f_sLatency ) );
	FD_ZERO ( & f_xFileDescriptorSet );
	return;
	M_EPILOG
	}

HTUIProcess::~HTUIProcess ( void )
	{
	M_PROLOG
	HMainWindow * l_poMainWindow = NULL;
	if ( f_poWindows )
		l_poMainWindow = static_cast < HMainWindow * > ( f_poWindows->go ( -1 ) [ 0 ].get < void * > ( ) );
	if ( l_poMainWindow )
		delete l_poMainWindow;
	f_poWindows = NULL;
#ifdef __DEBUGGER_BABUNI__
	M_LOG ( "ok!" );
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

int HTUIProcess::init_tui ( char const * a_pcProcessName, HWindow * a_poMainWindow )
	{
	M_PROLOG
	static int const D_CTRLS_COUNT = 2;
	static int const D_ALTS_COUNT = 10;
	int l_iCtr = 0;
	int l_piAlts [ D_ALTS_COUNT ];
	int l_piCtrls [ ] = { KEY < 'l' >::ctrl, KEY < 'x' >::ctrl };
	HMainWindow * l_poMainWindow = NULL;
	HProcess::init ( n_iLatency );
	register_file_descriptor_handler ( STDIN_FILENO, & HTUIProcess::process_stdin );
	if ( n_bUseMouse && n_iMouseDes )
		register_file_descriptor_handler ( n_iMouseDes, & HTUIProcess::process_mouse );
	register_postprocess_handler ( D_CTRLS_COUNT, l_piCtrls,
			& HTUIProcess::handler_refresh );
	register_postprocess_handler ( KEY < 'x' >::command, NULL,
			& HTUIProcess::handler_quit );
	if ( n_bUseMouse )
		register_postprocess_handler ( KEY_CODES::D_MOUSE, NULL,
				& HTUIProcess::handler_mouse );
	if ( a_poMainWindow )
		{
		f_poForegroundWindow = a_poMainWindow;
		f_poForegroundWindow->init ( );
		if ( ! f_poForegroundWindow->is_initialised ( ) )
			M_THROW ( _ ( "window has not been initialised" ), errno );
		}
	else /* Create automatically default main window. */
		{
		l_poMainWindow = new HMainWindow ( a_pcProcessName );
		l_poMainWindow->init ( );
		f_poWindows = l_poMainWindow->_disclose_window_list ( );
		add_window ( l_poMainWindow, a_pcProcessName );
		register_postprocess_handler ( KEY < '\t' >::meta, NULL,
				& HTUIProcess::handler_jump_meta_tab );
		register_postprocess_handler ( KEY < 'q' >::command, NULL,
				& HTUIProcess::handler_close_window );
		for ( l_iCtr = 0; l_iCtr < D_ALTS_COUNT; l_iCtr ++ )
			l_piAlts [ l_iCtr ] = KEY<>::meta_r ( '0' + l_iCtr );
		register_postprocess_handler ( D_ALTS_COUNT, l_piAlts,
			& HTUIProcess::handler_jump_meta_direct );
		}
	f_oCommandHandlers [ "quit" ] = static_cast < HANDLER_t > ( & HTUIProcess::handler_quit );
	handler_refresh ( 0 );
	return ( 1 );
	M_EPILOG
	}

int HTUIProcess::add_window ( HWindow * a_poWindow, char const * a_pcTitle )
	{
	M_PROLOG
	HInfo l_oInfo;
	HItem l_oItem ( 1 );
	l_oInfo = static_cast < void * > ( a_poWindow );
	l_oInfo ( a_pcTitle );
	l_oItem [ 0 ] = l_oInfo;
	f_poWindows->add_tail ( & l_oItem );
	f_poForegroundWindow = a_poWindow;
	f_poForegroundWindow->init ( );
	if ( ! f_poForegroundWindow->is_initialised ( ) )
		M_THROW ( _ ( "window has not been initialised" ), errno );
	c_refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HTUIProcess::process_stdin ( int a_iCode )
	{
	M_PROLOG
	HString l_oCommand;
	n_bInputWaiting = false;
	if ( ! a_iCode )
		a_iCode = get_key ( );
	if ( a_iCode )
		a_iCode = process_input_with_handlers ( a_iCode, f_oPreprocessHandlers );
	if ( a_iCode && f_poForegroundWindow )
			a_iCode = f_poForegroundWindow->process_input ( a_iCode );
	if ( a_iCode )
		a_iCode = process_input_with_handlers ( a_iCode, f_oPostprocessHandlers );
	if ( ! a_iCode )
		{
		if ( f_poForegroundWindow )
			f_oCommand = f_poForegroundWindow->get_command ( );
		if ( f_oCommand
				&& static_cast < char const * const > ( f_oCommand ) [ 0 ] )
			l_oCommand = process_command ( );
		if ( l_oCommand
				&& static_cast < char const * const > ( l_oCommand ) [ 0 ]
				&& f_poForegroundWindow )
			f_poForegroundWindow->status_bar ( )->message ( COLORS::D_FG_RED,
					"unknown command: `%s'",
					static_cast < char const * const > ( l_oCommand ) );
		}
#ifdef __DEBUGGER_BABUNI__
	n_bNeedRepaint = true;
	if ( a_iCode )
		{
		if ( a_iCode > D_KEY_COMMAND_(D_KEY_META_(0)) )
			c_printf ( 0, 0, COLORS::D_FG_GREEN,
					"COMMAND-META-%c: %5d       ",
					a_iCode - D_KEY_COMMAND_(D_KEY_META_(0)), a_iCode );
		else if ( a_iCode > D_KEY_COMMAND_(0) )
			c_printf ( 0, 0, COLORS::D_FG_GREEN,
					"     COMMAND-%c: %5d       ",
					a_iCode - D_KEY_COMMAND_(0), a_iCode );
		else if ( a_iCode > D_KEY_META_(0) )
			c_printf ( 0, 0, COLORS::D_FG_GREEN,
					"        META-%c: %5d       ",
					a_iCode - D_KEY_META_(0), a_iCode );
		else if ( a_iCode < D_KEY_ESC )
			c_printf ( 0, 0, COLORS::D_FG_GREEN,
					"        CTRL-%c: %5d       ",
					a_iCode + 96, a_iCode);
		else
			c_printf ( 0, 0, COLORS::D_FG_GREEN,
					"             %c: %5d       ",
					a_iCode, a_iCode );
		}
	else
		c_printf ( 0, 0, COLORS::D_FG_GREEN, "                           " );
#endif /* __DEBUGGER_BABUNI__ */
	if ( a_iCode && f_poForegroundWindow )
		f_poForegroundWindow->status_bar ( )->message ( COLORS::D_FG_RED,
				"unknown function, err code(%d)", a_iCode );
	return ( a_iCode );
	M_EPILOG
	}

int HTUIProcess::handler_alert ( int, void * )
	{
	M_PROLOG
	if ( n_bNeedRepaint )
		{
		n_bNeedRepaint = false;
		c_refresh ( );
		}
	return ( 0 );
	M_EPILOG
	}

int HTUIProcess::handler_interrupt ( int, void * )
	{
	M_PROLOG
	if ( n_bInputWaiting )
		{
		process_stdin ( STDIN_FILENO );
		handler_alert ( 0 );
		}
	return ( 0 );
	M_EPILOG
	}

int HTUIProcess::handler_idle ( int a_iCode, void * )
	{
	M_PROLOG
	HStatusBarControl * l_poStatusBar = NULL;
#ifdef __DEBUG__
	HString l_oClock ( static_cast < char const * > ( HTime ( ) ) );
	c_printf ( 0, n_iWidth - l_oClock.get_length ( ),
			COLORS::D_FG_BLACK | COLORS::D_BG_LIGHTGRAY, l_oClock );
	n_bNeedRepaint = true;
#endif /* __DEBUG__ */
	if ( f_poForegroundWindow )
		{
		l_poStatusBar = f_poForegroundWindow->status_bar ( );
		if ( l_poStatusBar )
			l_poStatusBar->refresh ( );
		}
	a_iCode = HProcess::handler_idle ( a_iCode );
	return ( a_iCode );
	M_EPILOG
	}

int HTUIProcess::process_mouse ( int )
	{
	M_PROLOG
	handler_mouse ( 0 );
	return ( 0 );
	M_EPILOG
	}

int HTUIProcess::handler_mouse ( int a_iCode, void * )
	{
	M_PROLOG
	a_iCode = 0;
	mouse::OMouse l_sMouse;
	static_cast < void > ( mouse::mouse_get ( l_sMouse ) );
#ifdef __DEBUGGER_BABUNI__
	c_printf ( 0, 0,	COLORS::D_FG_BLACK | COLORS::D_BG_LIGHTGRAY, "mouse: %6d, %3d, %3d",
			l_sMouse.f_iButtons, l_sMouse.f_iRow, l_sMouse.f_iColumn );
	n_bNeedRepaint = true;
#endif /* __DEBUGGER_BABUNI__ */
	if ( f_poForegroundWindow )
		f_poForegroundWindow->click ( l_sMouse );
	return ( a_iCode );
	M_EPILOG
	}

int HTUIProcess::handler_refresh ( int, void * )
	{
	M_PROLOG
	endwin ( );
	n_bNeedRepaint = false;
	clrscr ( ); /* there is ::refresh ( ) call inside */
	kbhit ( ); /* cleans all trash from stdio buffer */
	c_getmaxyx ( n_iHeight, n_iWidth );
	if ( f_poForegroundWindow )
		f_poForegroundWindow->refresh ( );
	c_refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HTUIProcess::handler_quit ( int, void * )
	{
	M_PROLOG
	f_bLoop = false;
	clrscr ( );
	return ( 0 );
	M_EPILOG
	}

int HTUIProcess::handler_jump_meta_tab ( int a_iCode, void * )
	{
	M_PROLOG
	if ( f_iIdleCycles < 5 )
		{
		f_poWindows->to_tail ( );
		HInfo & l_oInfo = f_poWindows->present ( ) [ 0 ];
		f_poForegroundWindow = static_cast < HWindow * > ( l_oInfo.get < void * > ( ) );
		}
	else
		f_poForegroundWindow = static_cast < HWindow * > ( f_poWindows->go ( -1 ) [ 0 ].get < void * > ( ) );
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HTUIProcess::handler_jump_meta_direct ( int a_iCode, void * )
	{
	M_PROLOG
	a_iCode = ( a_iCode & 0xff ) - '0';
	if ( a_iCode >= f_poWindows->quantity ( ) )
		return ( 0 );
	f_poForegroundWindow = static_cast < HWindow * > ( f_poWindows->go ( a_iCode - 1 ) [ 0 ].get < void * > ( ) );
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HTUIProcess::handler_close_window ( int a_iCode, void * )
	{
	M_PROLOG
	f_poWindows->remove_element ( HWindowListControl::D_EMPTY_IF_NOT_EMPTIED );
	f_poForegroundWindow = static_cast < HWindow * > ( f_poWindows->go ( -1 ) [ 0 ].get < void * > ( ) );
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

}

}

