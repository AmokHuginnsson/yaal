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
#elif HAVE_NCURSES_NCURSES_H
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hwindow.h"

#include "../hcore/hexception.h"
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
	f_poStatusBar = NULL;
	f_oTitle = a_pcTitle;
	register_postprocess_handler ( '\t', & HWindow::handler_jump_tab );
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
	f_poStatusBar = new HStatusBarControl ( this, l_oString );
	f_oControls.remove_head ( D_FORCE_REMOVE_ELEMENT );
	return ( 0 );
	M_EPILOG
	}

int HWindow::preprocess_input ( int a_iCode )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < int ( HWindow::* ) ( int ) > l_oHandler;
	if ( ! f_oPreprocessHandlers.quantity ( ) )return ( a_iCode );
	f_oPreprocessHandlers.go ( 0 );
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		{
		l_oHandler = f_oPreprocessHandlers.to_tail ( 1, & l_iFlag );
		if ( l_oHandler == a_iCode )
			a_iCode = ( this->*( ( int ( HWindow::* ) ( int ) ) l_oHandler ) ) ( a_iCode );
		}
	return ( a_iCode );
	M_EPILOG
	}

int HWindow::postprocess_input ( int a_iCode )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < int ( HWindow::* ) ( int ) > l_oHandler;
	if ( ! f_oPostprocessHandlers.quantity ( ) )return ( a_iCode );
	f_oPostprocessHandlers.go ( 0 );
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		{
		l_oHandler = f_oPostprocessHandlers.to_tail ( 1, & l_iFlag );
		if ( l_oHandler == a_iCode )
			a_iCode = ( this->*( ( int ( HWindow::* ) ( int ) ) l_oHandler ) ) ( a_iCode );
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

int HWindow::register_preprocess_handler ( int a_iCode,
		int ( HWindow::* HANDLER ) ( int ) )
	{
	M_PROLOG
	HHandler < int ( HWindow::* ) ( int ) > l_oHandler ( a_iCode, HANDLER );
	f_oPreprocessHandlers.add_tail ( l_oHandler );
	return ( 0 );
	M_EPILOG
	}

int HWindow::register_postprocess_handler ( int a_iCode,
		int ( HWindow::* HANDLER ) ( int ) )
	{
	M_PROLOG
	HHandler < int ( HWindow::* ) ( int ) > l_oHandler ( a_iCode, HANDLER );
	f_oPostprocessHandlers.add_tail ( l_oHandler );
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
	register_postprocess_handler ( a_iShortCut, & HWindow::handler_jump_direct );
	f_poFocusedChild = f_oControls.go ( 0 );
	return ( 0 );
	M_EPILOG
	}

void HWindow::refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( f_poStatusBar )f_poStatusBar->refresh ( );
	l_iCtr = f_oControls.quantity ( );
	while ( l_iCtr -- )f_oControls.to_tail ( )->refresh ( );
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

