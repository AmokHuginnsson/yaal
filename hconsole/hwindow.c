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

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hwindow.h"
#include "console.h"

#ifdef __DEBUGGER_BABUNI__
#include "../hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

HWindow::HWindow ( const char * a_pcTitle )
	{
	M_PROLOG
	if ( ! console::is_enabled ( ) )
		throw new HException ( __WHERE__, "console not initialised.", g_iErrNo );
	f_poFocusedChild = NULL;
	f_poPreviousFocusedChild = NULL;
	f_poStatusBar = NULL;
	f_oTitle = a_pcTitle;
	register_postprocess_handler ( '\t', NULL, & HWindow::handler_jump_tab );
	register_postprocess_handler ( ':', NULL, & HWindow::handler_command );
	register_postprocess_handler ( '/', NULL, & HWindow::handler_search );
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
	console::clrscr ( );
	console::n_bNeedRepaint = true;
	l_oString.format ( " [%s]& \n", ( char * ) f_oTitle );
	f_poStatusBar = init_bar ( l_oString );
	f_poStatusBar->enable ( true );
	f_oControls.remove_head ( D_FORCE_REMOVE_ELEMENT );
	return ( 0 );
	M_EPILOG
	}

HStatusBarControl * HWindow::init_bar ( const char * a_pcLabel )
	{
	M_PROLOG
	return ( new HStatusBarControl ( this, a_pcLabel ) );
	M_EPILOG
	}

int HWindow::preprocess_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCode = 0;
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < WINDOW_HANDLER_t > * l_poHandler = NULL;
	if ( ! f_oPreprocessHandlers.quantity ( ) )return ( a_iCode );
	f_oPreprocessHandlers.go ( 0 );
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		{
		l_poHandler = & f_oPreprocessHandlers.to_tail ( 1, & l_iFlag );
		l_iCtr = 0;
		while ( ( l_iCode = ( * l_poHandler ) [ l_iCtr ++ ] ) )
			if ( l_iCode == a_iCode )
				a_iCode = ( this->*( ( WINDOW_HANDLER_t ) ( * l_poHandler ) ) ) ( a_iCode );
		}
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::postprocess_input ( int a_iCode )
	{
	M_PROLOG
	int l_iCtr = 0, l_iCode = 0;
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < WINDOW_HANDLER_t > * l_poHandler = NULL;
	if ( ! f_oPostprocessHandlers.quantity ( ) )return ( a_iCode );
	f_oPostprocessHandlers.go ( 0 );
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		{
		l_poHandler = & f_oPostprocessHandlers.to_tail ( 1, & l_iFlag );
		l_iCtr = 0;
		while ( ( l_iCode = (  * l_poHandler ) [ l_iCtr ++ ] ) )
			if ( l_iCode == a_iCode )
				a_iCode = ( this->*( ( WINDOW_HANDLER_t ) ( * l_poHandler ) ) ) ( a_iCode );
		}
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::process_input ( int a_iCode )
	{
	M_PROLOG
	if ( f_poFocusedChild )
		a_iCode = f_poFocusedChild->process_input ( a_iCode );
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::register_preprocess_handler ( int a_iCodeCount, int * a_piCodes,
		WINDOW_HANDLER_t HANDLER )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < WINDOW_HANDLER_t > l_oNewHandler ( a_iCodeCount, a_piCodes, HANDLER );
	HHandler < WINDOW_HANDLER_t > * l_poHandler = NULL;
	if ( f_oPreprocessHandlers.quantity ( ) )
		{
		f_oPreprocessHandlers.go ( 0 );
		while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
			{
			l_poHandler = & f_oPreprocessHandlers.to_tail ( 1, & l_iFlag );
			if ( ( * l_poHandler ) == HANDLER )
				{
				( * l_poHandler ).add ( a_iCodeCount, a_piCodes );
				return ( 0 );
				}
			}
		}
	f_oPreprocessHandlers.add_tail ( l_oNewHandler );
	return ( 0 );
	M_EPILOG
	}

int HWindow::register_postprocess_handler ( int a_iCodeCount, int * a_piCodes,
		WINDOW_HANDLER_t HANDLER )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < WINDOW_HANDLER_t > l_oNewHandler ( a_iCodeCount, a_piCodes, HANDLER );
	HHandler < WINDOW_HANDLER_t > * l_poHandler = NULL;
	if ( f_oPostprocessHandlers.quantity ( ) )
		{
		f_oPostprocessHandlers.go ( 0 );
		while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
			{
			l_poHandler = & f_oPostprocessHandlers.to_tail ( 1, & l_iFlag );
			if ( ( * l_poHandler ) == HANDLER )
				{
				( * l_poHandler ).add ( a_iCodeCount, a_piCodes );
				return ( 0 );
				}
			}
		}
	f_oPostprocessHandlers.add_tail ( l_oNewHandler );
	return ( 0 );
	M_EPILOG
	}

int HWindow::add_control ( HControl * a_poControl, int a_iShortCut )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < int ( HWindow::* ) ( int ) > l_oHandler;
	if ( f_oPreprocessHandlers.quantity ( ) )
		{
		l_oHandler = f_oPreprocessHandlers.go ( 0 );
		do
			{
			if ( l_oHandler == a_iShortCut )
				throw new HException ( __WHERE__, "shortcut occupied", a_iShortCut );
			l_oHandler = f_oPreprocessHandlers.to_tail ( 1, & l_iFlag );
			}
		while ( l_iFlag == ( int ) D_TREAT_AS_OPENED );
		} /* if ( f_oPreprocessHandlers.quantity ( ) ) */
	f_oControls.add_tail ( a_poControl );
	register_postprocess_handler ( a_iShortCut, NULL, & HWindow::handler_jump_direct );
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
		l_poControl = f_oControls.to_tail ( );
		if ( l_poControl != f_poFocusedChild )l_poControl->refresh ( );
		}
	if ( f_poFocusedChild )f_poFocusedChild->refresh ( );
	console::n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

int HWindow::handler_jump_tab ( int a_iCode )
	{
	M_PROLOG
	f_poFocusedChild = f_oControls.next_enabled ( );
	console::n_bNeedRepaint = true;
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::handler_jump_direct ( int a_iCode )
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
		f_poFocusedChild = f_oControls.next_enabled ( a_iCode );
		if ( f_poFocusedChild != l_poControl )a_iCode = 0;
		console::n_bNeedRepaint = true;
		}
	return ( a_iCode );
	M_EPILOG
	}

void HWindow::set_focus ( HControl * a_poControl )
	{
	M_PROLOG
	if ( f_poFocusedChild == a_poControl )return;
	f_oControls.select ( a_poControl );
	f_poFocusedChild->kill_focus ( );
	f_poFocusedChild = a_poControl;
	console::n_bNeedRepaint = true;
	return;
	M_EPILOG
	}

int HWindow::handler_command ( int a_iCode )
	{
	M_PROLOG
	a_iCode = 0;
	f_poStatusBar->set_prompt ( ":" );
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::handler_search ( int a_iCode )
	{
	M_PROLOG
	a_iCode = 0;
	if ( ! f_poFocusedChild->is_searchable ( ) )return ( a_iCode );
	f_poStatusBar->set_prompt ( "/" );
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::click ( mouse::OMouse & a_rsMouse )
	{
	M_PROLOG
	int l_iCtr = 0;
	HControl * l_poControl = NULL;
	if ( f_poStatusBar && ( f_poStatusBar != f_poFocusedChild ) )
		f_poStatusBar->click ( a_rsMouse );
	l_iCtr = f_oControls.quantity ( );
	while ( l_iCtr -- )
		{
		l_poControl = f_oControls.to_tail ( );
		if ( l_poControl != f_poFocusedChild )l_poControl->click ( a_rsMouse );
		}
	if ( f_poFocusedChild )f_poFocusedChild->click ( a_rsMouse );
	return ( 0 );
	M_EPILOG
	}

