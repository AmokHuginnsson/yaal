/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hprocess.c - this file is integral part of `stdhapi' project.

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
#include "hprocess.h"
#include "hconsole.h"
#include "hmainwindow.h"

#ifdef __DEBUGGER_BABUNI__
#include "../hcore/hlog.h"
#endif /* __DEBUGGER_BABUNI__ */

HProcess::HProcess ( void )
	{
	M_PROLOG
	f_bInitialised = false;
	f_bLoop = true;
	f_iIdleCycles = 0;
	f_poForegroundWindow = NULL;
	f_poWindows = NULL;
	return;
	M_EPILOG
	}

HProcess::~HProcess ( void )
	{
	M_PROLOG
	HMainWindow * l_poMainWindow = NULL;
	if ( f_poWindows )
		l_poMainWindow = ( HMainWindow * )( void * ) f_poWindows->go ( -1 ) [ 0 ];
	if ( l_poMainWindow )delete l_poMainWindow;
	f_poWindows = NULL;
#ifdef __DEBUGGER_BABUNI__
	M_LOG ( "ok!" );
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_EPILOG
	}

int HProcess::init ( const char * a_pcProcessName )
	{
	M_PROLOG
	int l_iCtr = 0;
	HMainWindow * l_poMainWindow = NULL;
	if ( f_bInitialised )
		throw new HException ( __WHERE__,
				"you can initialise your main process only once, dumbass", g_iErrNo );
	f_bInitialised = true;
	l_poMainWindow = new HMainWindow ( );
	l_poMainWindow->init ( );
	f_poWindows = l_poMainWindow->_disclose_window_list ( );
	add_window ( l_poMainWindow, a_pcProcessName );
	register_file_descriptor_handler ( STDIN_FILENO, & HProcess::process_stdio );
	register_postprocess_handler ( D_KEY_CTRL_('l'),
			& HProcess::handler_refresh );
	register_postprocess_handler ( D_KEY_COMMAND_('x'),
			& HProcess::handler_quit );
	register_postprocess_handler ( D_KEY_META_( '\t' ),
			& HProcess::handler_jump_meta_tab );
	register_postprocess_handler ( D_KEY_COMMAND_('q'),
			& HProcess::handler_close_window );
	for ( l_iCtr = 0; l_iCtr < 10; l_iCtr ++ )
		register_postprocess_handler ( D_KEY_META_( '0' + l_iCtr ),
				& HProcess::handler_jump_meta_direct );
	handler_refresh ( 0 );
	return ( 1 );
	M_EPILOG
	}

int HProcess::add_window ( HWindow * a_poWindow, const char * a_pcTitle )
	{
	M_PROLOG
	HInfo l_oInfo;
	HItem l_oItem ( 1 );
	l_oInfo = ( void * ) a_poWindow;
	l_oInfo = HString ( a_pcTitle );
	l_oItem [ 0 ] = l_oInfo;
	f_poWindows->add_tail ( l_oItem );
	f_poForegroundWindow = a_poWindow;
	f_poForegroundWindow->init ( );
	::refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HProcess::register_file_descriptor_handler ( int a_iFileDescriptor,
		int ( HProcess::* HANDLER ) ( int ) )
	{
	M_PROLOG
	HHandler < int ( HProcess::* ) ( int ) > l_oHandler ( a_iFileDescriptor,
			HANDLER );
	f_oFileDescriptorHandlers.add_tail ( l_oHandler );
	return ( 0 );
	M_EPILOG
	}

int HProcess::unregister_file_descriptor_handler ( int a_iFileDescriptor )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < int ( HProcess::* ) ( int ) > l_oHandler;
	if ( ! f_oFileDescriptorHandlers.quantity ( ) )return ( -1 );
	f_oFileDescriptorHandlers.go ( 0 );
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		{
		l_oHandler = f_oFileDescriptorHandlers.to_tail ( 1, & l_iFlag );
		if ( l_oHandler == a_iFileDescriptor )
			{
			f_oFileDescriptorHandlers.remove_element ( D_FORCE_REMOVE_ELEMENT );
			return ( 0 );
			}
		}
	return ( -1 );
	M_EPILOG
	}

int HProcess::register_preprocess_handler ( int a_iCode,
		int ( HProcess::* HANDLER ) ( int, void * ) )
	{
	M_PROLOG
	HHandler < int ( HProcess::* ) ( int, void * ) > l_oHandler ( a_iCode, HANDLER );
	f_oPreprocessHandlers.add_tail ( l_oHandler );
	return ( 0 );
	M_EPILOG
	}

int HProcess::register_postprocess_handler ( int a_iCode,
		int ( HProcess::* HANDLER ) ( int, void * ) )
	{
	M_PROLOG
	HHandler < int ( HProcess::* ) ( int, void * ) > l_oHandler ( a_iCode, HANDLER );
	f_oPostprocessHandlers.add_tail ( l_oHandler );
	return ( 0 );
	M_EPILOG
	}

int HProcess::reconstruct_fdset ( void )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < int ( HProcess::* ) ( int ) > l_oHandler;
	f_sLatency.tv_sec = console::n_iLatency;
	f_sLatency.tv_usec = 0;
	FD_ZERO ( & f_xFileDescriptorSet );
	if ( ! f_oFileDescriptorHandlers.quantity ( ) )return ( -1 );
	f_oFileDescriptorHandlers.go ( 0 );
/* FD_SET is a macro and first argument is evaluated twice ! */
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		{
		l_oHandler = f_oFileDescriptorHandlers.to_tail ( 1, & l_iFlag );
		FD_SET ( l_oHandler, & f_xFileDescriptorSet );
		}
	return ( 0 );
	M_EPILOG
	}

int HProcess::preprocess_input ( int a_iCode )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < int ( HProcess::* ) ( int, void * ) > l_oHandler;
	if ( ! f_oPreprocessHandlers.quantity ( ) )return ( a_iCode );
	f_oPreprocessHandlers.go ( 0 );
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		{
		l_oHandler = f_oPreprocessHandlers.to_tail ( 1, & l_iFlag );
		if ( l_oHandler == a_iCode )
			a_iCode = ( this->*( ( int ( HProcess::* ) ( int, void * ) ) l_oHandler ) ) ( a_iCode );
		}
	return ( a_iCode );
	M_EPILOG
	}

int HProcess::postprocess_input ( int a_iCode )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	HHandler < int ( HProcess::* ) ( int, void * ) > l_oHandler;
	if ( ! f_oPostprocessHandlers.quantity ( ) )return ( a_iCode );
	f_oPostprocessHandlers.go ( 0 );
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		{
		l_oHandler = f_oPostprocessHandlers.to_tail ( 1, & l_iFlag );
		if ( l_oHandler == a_iCode )
			a_iCode = ( this->*( ( int ( HProcess::* ) ( int, void * ) ) l_oHandler ) ) ( a_iCode );
		}
	return ( a_iCode );
	M_EPILOG
	}

int HProcess::process_stdio ( int a_iCode )
	{
	M_PROLOG
	console::n_bInputWaiting = false;
	if ( ! a_iCode )a_iCode = console::get_key ( );
	if ( a_iCode )a_iCode = preprocess_input ( a_iCode );
	if ( f_poForegroundWindow )
		{
		if ( a_iCode )
			a_iCode = f_poForegroundWindow->preprocess_input ( a_iCode );
		if ( a_iCode )
			a_iCode = f_poForegroundWindow->process_input ( a_iCode );
		if ( a_iCode )
			a_iCode = f_poForegroundWindow->postprocess_input ( a_iCode );
		}
	if ( a_iCode )a_iCode = postprocess_input ( a_iCode );
#ifdef __DEBUGGER_BABUNI__
	console::n_bNeedRepaint = true;
	if ( a_iCode )
		{
		if ( a_iCode > D_KEY_COMMAND_(D_KEY_META_(0)) )
			console::c_printf ( 0, 0, D_FG_GREEN,
					"COMMAND-META-%c: %5d       ",
					a_iCode - D_KEY_COMMAND_(D_KEY_META_(0)), a_iCode );
		else if ( a_iCode > D_KEY_COMMAND_(0) )
			console::c_printf ( 0, 0, D_FG_GREEN,
					"     COMMAND-%c: %5d       ",
					a_iCode - D_KEY_COMMAND_(0), a_iCode );
		else if ( a_iCode > D_KEY_META_(0) )
			console::c_printf ( 0, 0, D_FG_GREEN,
					"        META-%c: %5d       ",
					a_iCode - D_KEY_META_(0), a_iCode );
		else if ( a_iCode < D_KEY_ESC )
			console::c_printf ( 0, 0, D_FG_GREEN,
					"        CTRL-%c: %5d       ",
					a_iCode + 96, a_iCode);
		else console::c_printf ( 0, 0, D_FG_GREEN,
					"             %c: %5d       ",
					a_iCode, a_iCode );
		}
	else console::c_printf ( 0, 0, D_FG_GREEN, "                           " );
#endif /* __DEBUGGER_BABUNI__ */
	if ( a_iCode )
		console::c_printf ( console::n_iHeight - 1, -1, D_FG_RED,
				"unknown function, err code(%d)", a_iCode );
	return ( a_iCode );
	M_EPILOG
	}

/* this makro is ripped from unistd.h, I seriously doubt if it is portable */

#define M_STDHAPI_TEMP_FAILURE_RETRY(expression) \
  (__extension__ \
    ({ long int __result; \
       do \
			 	{ \
				if ( console::n_bInputWaiting )process_stdio ( STDIN_FILENO ); \
				__result = (long int) (expression); \
				} \
       while (__result == -1L && errno == EINTR); \
       __result; }))

int HProcess::run ( void )
	{
	M_PROLOG
	int l_iError = 0;
	int l_iFlag = 0;
	HHandler < int ( HProcess::* ) ( int ) > l_oHandler;
	if ( ! f_bInitialised )
		throw new HException ( __WHERE__,
				"you have to call HProcess::init ( ) first, dumbass", g_iErrNo );
	::refresh ( );
	while ( f_bLoop )
		{
		reconstruct_fdset ( );
		if ( ( l_iError = M_STDHAPI_TEMP_FAILURE_RETRY ( select ( FD_SETSIZE,
							& f_xFileDescriptorSet,	NULL, NULL, & f_sLatency ) ) ) )
			{
			if ( l_iError < 0 )
				throw new HException ( __WHERE__, "select ( ) returned", l_iError );
			if ( ! f_oFileDescriptorHandlers.quantity ( ) )return ( -1 );
			l_iFlag = ( int ) D_TREAT_AS_OPENED;
			f_oFileDescriptorHandlers.go ( 0 );
			while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
				{
				l_oHandler = f_oFileDescriptorHandlers.to_tail ( 1, & l_iFlag );
				if ( FD_ISSET ( l_oHandler, & f_xFileDescriptorSet ) )
					{
					( this->*( ( int ( HProcess::* ) ( int ) ) l_oHandler ) ) ( l_oHandler );
					f_iIdleCycles = 0;
					}
				}
			}
		else handler_idle ( 0 );
		if ( console::n_bNeedRepaint )
			{
			console::n_bNeedRepaint = false;
			::refresh ( );
			}
		}
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_idle ( int a_iCode, void * )
	{
	M_PROLOG
#ifdef __DEBUG__
	HString l_oClock ( ( const char * ) HTime ( ) );
	console::c_printf ( 0, console::n_iWidth - l_oClock.get_length ( ),
			D_FG_BLACK | D_BG_LIGHTGRAY, l_oClock );
	console::n_bNeedRepaint = true;
#endif /* __DEBUG__ */
	f_iIdleCycles ++;
	return ( a_iCode );
	M_EPILOG
	}

int HProcess::handler_refresh ( int, void * )
	{
	M_PROLOG
	::endwin ( );
	console::n_bNeedRepaint = false;
	console::clrscr ( ); /* there is ::refresh ( ) call inside */
	console::kbhit ( ); /* cleans all trash from stdio buffer */
	getmaxyx ( stdscr, console::n_iHeight, console::n_iWidth );
	if ( f_poForegroundWindow )
		f_poForegroundWindow->refresh ( );
	::refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_quit ( int a_iCode, void * )
	{
	M_PROLOG
	if ( a_iCode == D_KEY_COMMAND_('x') )f_bLoop = false;
	console::clrscr ( );
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_jump_meta_tab ( int a_iCode, void * )
	{
	M_PROLOG
	if ( f_iIdleCycles < 5 )
		{
		f_poWindows->to_tail ( );
		HInfo & l_oInfo = f_poWindows->present ( ) [ 0 ];
		f_poForegroundWindow = ( HWindow * ) ( void * ) l_oInfo;
		}
	else f_poForegroundWindow = ( HWindow * )( void * ) f_poWindows->
																															go ( -1 ) [ 0 ];
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HProcess::handler_jump_meta_direct ( int a_iCode, void * )
	{
	M_PROLOG
	a_iCode = ( a_iCode & 0xff ) - '0';
	if ( a_iCode >= f_poWindows->quantity ( ) )return ( 0 );
	f_poForegroundWindow = ( HWindow * ) ( void * ) f_poWindows->
																										go ( a_iCode - 1 ) [ 0 ];
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HProcess::handler_close_window ( int a_iCode, void * )
	{
	M_PROLOG
	f_poWindows->remove_element ( D_EMPTY_IF_NOT_EMPTIED );
	f_poForegroundWindow = ( HWindow * ) ( void * ) f_poWindows->go ( -1 ) [ 0 ];
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

