/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	htuiprocess.cxx - this file is integral part of `yaal' project.

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
M_VCSID ( "$Id$" )
#include "htuiprocess.h"
#include "hconsole.h"
#include "hmainwindow.h"
#include "mouse.h"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HTUIProcess::HTUIProcess ( size_t a_uiFileHandlers, size_t a_uiKeyHandlers,
		size_t a_uiCommandHandlers )
	: HHandler ( a_uiKeyHandlers, a_uiCommandHandlers ),
	HProcess ( a_uiFileHandlers ), f_oMainWindow(), f_oForegroundWindow(),
	f_oWindows ( new model_t() )
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
	return;
	M_EPILOG
	}

int HTUIProcess::init_tui ( char const * a_pcProcessName, HWindow::ptr_t a_oMainWindow )
	{
	M_PROLOG
	static int const D_CTRLS_COUNT = 2;
	static int const D_ALTS_COUNT = 10;
	int l_iCtr = 0;
	int l_piAlts [ D_ALTS_COUNT ];
	int l_piCtrls [ ] = { KEY<'l'>::ctrl, KEY<'x'>::ctrl };
	HWindow::ptr_t l_oMainWindow;
	HProcess::init ( n_iLatency );
	register_file_descriptor_handler ( STDIN_FILENO, & HTUIProcess::process_stdin );
	if ( n_bUseMouse && n_iMouseDes )
		register_file_descriptor_handler ( n_iMouseDes, & HTUIProcess::process_mouse );
	register_postprocess_handler ( D_CTRLS_COUNT, l_piCtrls,
			& HTUIProcess::handler_refresh );
	register_postprocess_handler ( KEY<'x'>::command, NULL,
			& HTUIProcess::handler_quit );
	if ( n_bUseMouse )
		register_postprocess_handler ( KEY_CODES::D_MOUSE, NULL,
				& HTUIProcess::handler_mouse );
	if ( !! a_oMainWindow )
		{
		l_oMainWindow = a_oMainWindow;
		l_oMainWindow->init ( );
		f_oMainWindow = l_oMainWindow;
		if ( ! l_oMainWindow->is_initialised ( ) )
			M_THROW ( _ ( "window has not been initialised" ), errno );
		}
	else /* Create automatically default main window. */
		{
		l_oMainWindow = HWindow::ptr_t ( new HMainWindow ( a_pcProcessName, f_oWindows ) );
		l_oMainWindow->init ( );
		f_oMainWindow = l_oMainWindow;
		add_window ( l_oMainWindow );
		register_postprocess_handler ( KEY < '\t' >::meta, NULL,
				& HTUIProcess::handler_jump_meta_tab );
		register_postprocess_handler ( KEY < 'q' >::command, NULL,
				& HTUIProcess::handler_close_window );
		for ( l_iCtr = 0; l_iCtr < D_ALTS_COUNT; l_iCtr ++ )
			l_piAlts [ l_iCtr ] = KEY<>::meta_r ( '0' + l_iCtr );
		register_postprocess_handler ( D_ALTS_COUNT, l_piAlts,
			& HTUIProcess::handler_jump_meta_direct );
		}
	f_oForegroundWindow = f_oWindows->begin();
	f_oCommandHandlers [ "quit" ] = static_cast < HANDLER_t > ( & HTUIProcess::handler_quit );
	handler_refresh ( 0 );
	return ( 1 );
	M_EPILOG
	}

int HTUIProcess::add_window ( HWindow::ptr_t a_oWindow )
	{
	M_PROLOG
	a_oWindow->init();
	f_oWindows->push_back ( a_oWindow );
	f_oForegroundWindow = f_oWindows->rbegin();
	M_ASSERT( f_oForegroundWindow.is_valid() );
	f_oMainWindow->update_all();
	if ( ! (*f_oForegroundWindow)->is_initialised ( ) )
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
	if ( a_iCode && !! (*f_oForegroundWindow) )
			a_iCode = (*f_oForegroundWindow)->process_input ( a_iCode );
	if ( a_iCode )
		a_iCode = process_input_with_handlers ( a_iCode, f_oPostprocessHandlers );
	if ( ! a_iCode )
		{
		if ( !! (*f_oForegroundWindow) )
			f_oCommand = (*f_oForegroundWindow)->get_command ( );
		if ( f_oCommand
				&& static_cast < char const * const > ( f_oCommand ) [ 0 ] )
			l_oCommand = process_command ( );
		if ( l_oCommand
				&& static_cast < char const * const > ( l_oCommand ) [ 0 ]
				&& !! (*f_oForegroundWindow) )
			(*f_oForegroundWindow)->status_bar ( )->message ( COLORS::D_FG_RED,
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
	if ( a_iCode && !! (*f_oForegroundWindow) )
		(*f_oForegroundWindow)->status_bar ( )->message ( COLORS::D_FG_RED,
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
#ifdef __DEBUG__
	HString l_oClock ( static_cast < char const * > ( HTime ( ) ) );
	c_printf ( 0, n_iWidth - l_oClock.get_length ( ),
			COLORS::D_FG_BLACK | COLORS::D_BG_LIGHTGRAY, l_oClock );
	n_bNeedRepaint = true;
#endif /* __DEBUG__ */
	if ( !! (*f_oForegroundWindow) )
		{
		HStatusBarControl::ptr_t& l_oStatusBar = (*f_oForegroundWindow)->status_bar ( );
		if ( !! l_oStatusBar )
			l_oStatusBar->refresh ( );
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
	if ( !! (*f_oForegroundWindow) )
		(*f_oForegroundWindow)->click ( l_sMouse );
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
	if ( f_oForegroundWindow.is_valid() && ( !! (*f_oForegroundWindow) ) )
		(*f_oForegroundWindow)->refresh ( );
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
		++ f_oForegroundWindow;
	else
		f_oForegroundWindow = f_oWindows->begin();
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HTUIProcess::handler_jump_meta_direct ( int a_iCode, void * )
	{
	M_PROLOG
	a_iCode = ( a_iCode & 0xff ) - '0';
	if ( a_iCode >= f_oWindows->size ( ) )
		return ( 0 );
	f_oForegroundWindow = f_oWindows->begin();
	while ( -- a_iCode )
		++ f_oForegroundWindow;
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HTUIProcess::handler_close_window ( int a_iCode, void * )
	{
	M_PROLOG
	model_t::cyclic_iterator it = f_oForegroundWindow;
	-- f_oForegroundWindow;
	f_oWindows->erase ( it );
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

void HTUIProcess::select( HWindow const* const a_poWindow )
	{
	M_PROLOG
	for ( model_t::iterator it = f_oWindows->begin(); it != f_oWindows->end(); ++ it )
		{
		if ( (*it) == a_poWindow )
			{
			f_oForegroundWindow = it;
			break;
			}
		}
	return;
	M_EPILOG
	}

}

}

