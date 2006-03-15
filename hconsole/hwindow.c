/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hwindow.c - this file is integral part of `stdhapi' project.

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

#include <libintl.h>

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" )
#include "hwindow.h"
#include "console.h"

#ifdef __DEBUGGER_BABUNI__
#include "hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace hconsole
{

HWindow::HWindow ( char const * a_pcTitle ) : f_bInitialised ( false ),
	f_oTitle ( a_pcTitle ), f_poFocusedChild ( NULL ),
	f_poPreviousFocusedChild ( NULL ), f_oControls ( ), f_poStatusBar ( NULL )
	{
	M_PROLOG
	int l_piCmds [ ] = { ':', D_KEY_COMMAND_( ':' ) };
	int l_piSearch [ ] = { '/', D_KEY_COMMAND_( '/' ), '?', D_KEY_COMMAND_( '?' ) };
	if ( ! is_enabled ( ) )
		M_THROW ( "console not initialised.", g_iErrNo );
	register_postprocess_handler ( '\t', NULL, & HWindow::handler_jump_tab );
	register_postprocess_handler ( 2, l_piCmds, & HWindow::handler_command );
	register_postprocess_handler ( 4, l_piSearch, & HWindow::handler_search );
	return;
	M_EPILOG
	}

HWindow::~HWindow ( void )
	{
	M_PROLOG
	if ( f_poStatusBar )
		{
		delete f_poStatusBar;
		f_poStatusBar = NULL;
		}
#ifdef __DEBUGGER_BABUNI__
	log << "destroing window: " << f_oTitle << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

int HWindow::init ( void )
	{
	M_PROLOG
	HString l_oString;
	clrscr ( );
	n_bNeedRepaint = true;
	l_oString.format ( " [%s]& \n", static_cast < char const * const > ( f_oTitle ) );
	f_poStatusBar = init_bar ( l_oString );
	f_poStatusBar->enable ( true );
	f_oControls.remove_head ( HControlList::D_FORCE_REMOVE_ELEMENT );
	f_bInitialised = true;
	return ( 0 );
	M_EPILOG
	}

HStatusBarControl * HWindow::init_bar ( char const * a_pcLabel )
	{
	M_PROLOG
	return ( new HStatusBarControl ( this, a_pcLabel ) );
	M_EPILOG
	}

int HWindow::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( a_iCode )
		a_iCode = process_input_with_handlers ( a_iCode, f_oPreprocessHandlers );
	if ( a_iCode && f_poFocusedChild )
		a_iCode = f_poFocusedChild->process_input ( a_iCode );
	if ( a_iCode )
		a_iCode = process_input_with_handlers ( a_iCode, f_oPostprocessHandlers );
	if ( f_oCommand && static_cast < char const * const > ( f_oCommand ) [ 0 ] )
		process_command ( );
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::add_control ( HControl * a_poControl, int a_iShortCut )
	{
	M_PROLOG
	if ( f_oPreprocessHandlers.has_key ( a_iShortCut ) )
		M_THROW ( _ ( "shortcut occupied" ), a_iShortCut );
	f_oControls.add_tail ( & a_poControl );
	register_postprocess_handler ( a_iShortCut, NULL,
			& HWindow::handler_jump_direct );
	f_poFocusedChild = f_oControls.go ( 0 );
	return ( 0 );
	M_EPILOG
	}

HStatusBarControl * HWindow::status_bar ( void )
	{
	return ( f_poStatusBar );
	}

void HWindow::refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	HControl * l_poControl = NULL;
	if ( f_poStatusBar && ( f_poStatusBar != f_poFocusedChild ) )
		f_poStatusBar->refresh ( );
	l_iCtr = f_oControls.quantity ( );
	while ( l_iCtr -- )
		{
		l_poControl = * f_oControls.to_tail ( );
		if ( l_poControl != f_poFocusedChild )
			l_poControl->refresh ( );
		}
	if ( f_poFocusedChild )
		f_poFocusedChild->refresh ( );
	n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

int HWindow::handler_jump_tab ( int a_iCode, void * )
	{
	M_PROLOG
	f_poFocusedChild = f_oControls.next_enabled ( );
	n_bNeedRepaint = true;
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::handler_jump_direct ( int a_iCode, void * )
	{
	M_PROLOG
	HControl * l_poControl = NULL;
	/* call below is _magic_, HControlList::next_enabled ( ) takes char as an
	 * argument, so a_iCode & 0x0ff is passed into the function,
	 * a_iCode is consrtructed from ordinary char by D_KEY_META_ macro,
	 * see console.h for details */
	l_poControl = f_poFocusedChild;
	if ( a_iCode & 0x0ff00 )
		{
		f_poFocusedChild = f_oControls.next_enabled ( static_cast < char > ( a_iCode ) );
		if ( f_poFocusedChild != l_poControl )
			a_iCode = 0;
		n_bNeedRepaint = true;
		}
	return ( a_iCode );
	M_EPILOG
	}

void HWindow::set_focus ( HControl * a_poControl )
	{
	M_PROLOG
	if ( f_poFocusedChild == a_poControl )
		return;
	f_oControls.select ( a_poControl );
	f_poFocusedChild->kill_focus ( );
	f_poFocusedChild = a_poControl;
	n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

int HWindow::handler_command ( int a_iCode, void * )
	{
	M_PROLOG
	a_iCode = 0;
	f_poStatusBar->set_prompt ( ":", HStatusBarControl::PROMPT::D_COMMAND );
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::handler_search ( int a_iCode, void * )
	{
	M_PROLOG
	char l_pcPrompt [ ] = "/\0";
	if ( ! f_poFocusedChild->is_searchable ( ) )
		return ( a_iCode );
	if ( a_iCode >= KEY_CODES::D_COMMAND_BASE )
		a_iCode -= KEY_CODES::D_COMMAND_BASE;
	l_pcPrompt [ 0 ] = static_cast < char > ( a_iCode );
	f_poStatusBar->set_prompt ( l_pcPrompt, HStatusBarControl::PROMPT::D_SEARCH );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::click ( mouse::OMouse & a_rsMouse )
	{
	M_PROLOG
	int l_iCtr = 0;
	HControl * l_poControl = NULL;
	if ( ! a_rsMouse.f_iButtons )
		return ( 1 );
	if ( f_poFocusedChild == f_poStatusBar )
		return ( 1 );
	if ( f_poFocusedChild->hit_test ( a_rsMouse.f_iRow, a_rsMouse.f_iColumn ) )
		return ( f_poFocusedChild->click ( a_rsMouse ) );
	l_iCtr = f_oControls.quantity ( );
	while ( l_iCtr -- )
		{
		l_poControl = * f_oControls.to_tail ( );
		if ( l_poControl->hit_test ( a_rsMouse.f_iRow, a_rsMouse.f_iColumn ) )
			{
			l_poControl->click ( a_rsMouse );
			break;
			}
		}
	return ( 0 );
	M_EPILOG
	}

HString HWindow::get_command ( void )
	{
	M_PROLOG
	HString l_oCommand;
	l_oCommand = f_oCommand;
	f_oCommand = "";
	return ( l_oCommand );
	M_EPILOG
	}

bool HWindow::is_initialised ( void ) const
	{
	return ( f_bInitialised );
	}


}

}

