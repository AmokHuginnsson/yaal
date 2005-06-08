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
#include <string.h>

#include "config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hprocess.h"
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

#define D_CTRLS_COUNT	2
#define D_ALTS_COUNT	10

HProcess::HProcess ( size_t a_uiFileHandlers, size_t a_uiKeyHandlers,
		size_t a_uiCommandHandlers )
				: HHandler ( a_uiKeyHandlers, a_uiCommandHandlers ),
	f_bInitialised ( false ), f_bLoop ( true ), f_iIdleCycles ( 0 ),
	f_sLatency ( ), f_xFileDescriptorSet ( ), f_poForegroundWindow ( NULL ),
	f_poWindows ( NULL ), f_oFileDescriptorHandlers ( a_uiFileHandlers )
	{
	M_PROLOG
	memset ( & f_sLatency, 0, sizeof ( f_sLatency ) );
	FD_ZERO ( & f_xFileDescriptorSet );
	return;
	M_EPILOG
	}

HProcess::~HProcess ( void )
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

int HProcess::init ( char const * a_pcProcessName )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_piAlts [ D_ALTS_COUNT ], l_piCtrls [ ] = { D_KEY_CTRL_('l'), D_KEY_CTRL_('x') };
	HMainWindow * l_poMainWindow = NULL;
	if ( f_bInitialised )
		M_THROW ( "you can initialise your main process only once, dumbass",
				g_iErrNo );
	f_bInitialised = true;
	l_poMainWindow = new HMainWindow ( a_pcProcessName );
	M_IRV ( l_poMainWindow->init ( ) );
	f_poWindows = l_poMainWindow->_disclose_window_list ( );
	M_IRV ( add_window ( l_poMainWindow, a_pcProcessName ) );
	M_IRV ( register_file_descriptor_handler ( STDIN_FILENO, & HProcess::process_stdin ) );
	if ( n_bUseMouse && n_iMouseDes )
		register_file_descriptor_handler ( n_iMouseDes,
				& HProcess::process_mouse );
	M_REGISTER_POSTPROCESS_HANDLER ( D_CTRLS_COUNT, l_piCtrls,
			HProcess::handler_refresh );
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_COMMAND_('x'), NULL,
			HProcess::handler_quit );
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_META_( '\t' ), NULL,
			HProcess::handler_jump_meta_tab );
	M_REGISTER_POSTPROCESS_HANDLER ( D_KEY_COMMAND_('q'), NULL,
			HProcess::handler_close_window );
	if ( n_bUseMouse )
		M_REGISTER_POSTPROCESS_HANDLER ( KEY_MOUSE, NULL,
				HProcess::handler_mouse );
	for ( l_iCtr = 0; l_iCtr < D_ALTS_COUNT; l_iCtr ++ )
		l_piAlts [ l_iCtr ] = D_KEY_META_( '0' + l_iCtr );
	M_REGISTER_POSTPROCESS_HANDLER ( D_ALTS_COUNT, l_piAlts,
			HProcess::handler_jump_meta_direct );
	f_oCommandHandlers [ "quit" ] = static_cast < HANDLER_t > ( & HProcess::handler_quit );
	handler_refresh ( 0 );
	return ( 1 );
	M_EPILOG
	}

int HProcess::add_window ( HWindow * a_poWindow, char const * a_pcTitle )
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
	::refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HProcess::register_file_descriptor_handler ( int a_iFileDescriptor,
		PROCESS_HANDLER_FILEDES_t HANDLER )
	{
	M_PROLOG
	f_oFileDescriptorHandlers [ a_iFileDescriptor ] = HANDLER;
	return ( 0 );
	M_EPILOG
	}

int HProcess::unregister_file_descriptor_handler ( int a_iFileDescriptor )
	{
	M_PROLOG
	return ( f_oFileDescriptorHandlers.remove ( a_iFileDescriptor ) );
	M_EPILOG
	}

int HProcess::reconstruct_fdset ( void )
	{
	M_PROLOG
	int l_iFileDes = 0;
	PROCESS_HANDLER_FILEDES_t DUMMY = NULL;
	f_sLatency.tv_sec = n_iLatency;
	f_sLatency.tv_usec = 0;
	FD_ZERO ( & f_xFileDescriptorSet );
	if ( ! f_oFileDescriptorHandlers.quantity ( ) )
		return ( -1 );
	f_oFileDescriptorHandlers.rewind ( );
/* FD_SET is a macro and first argument is evaluated twice ! */
	while ( f_oFileDescriptorHandlers.iterate ( l_iFileDes, DUMMY ) )
		FD_SET ( l_iFileDes, & f_xFileDescriptorSet );
	return ( 0 );
	M_EPILOG
	}

int HProcess::process_stdin ( int a_iCode )
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
		if ( f_oCommand && static_cast < char * > ( f_oCommand ) [ 0 ] )
			l_oCommand = process_command ( );
		if ( l_oCommand && static_cast < char * > ( l_oCommand ) [ 0 ] && f_poForegroundWindow )
			f_poForegroundWindow->status_bar ( )->message ( D_FG_RED,
					"unknown command: `%s'", static_cast < char * > ( l_oCommand ) );
		}
#ifdef __DEBUGGER_BABUNI__
	n_bNeedRepaint = true;
	if ( a_iCode )
		{
		if ( a_iCode > D_KEY_COMMAND_(D_KEY_META_(0)) )
			c_printf ( 0, 0, D_FG_GREEN,
					"COMMAND-META-%c: %5d       ",
					a_iCode - D_KEY_COMMAND_(D_KEY_META_(0)), a_iCode );
		else if ( a_iCode > D_KEY_COMMAND_(0) )
			c_printf ( 0, 0, D_FG_GREEN,
					"     COMMAND-%c: %5d       ",
					a_iCode - D_KEY_COMMAND_(0), a_iCode );
		else if ( a_iCode > D_KEY_META_(0) )
			c_printf ( 0, 0, D_FG_GREEN,
					"        META-%c: %5d       ",
					a_iCode - D_KEY_META_(0), a_iCode );
		else if ( a_iCode < D_KEY_ESC )
			c_printf ( 0, 0, D_FG_GREEN,
					"        CTRL-%c: %5d       ",
					a_iCode + 96, a_iCode);
		else
			c_printf ( 0, 0, D_FG_GREEN,
					"             %c: %5d       ",
					a_iCode, a_iCode );
		}
	else
		c_printf ( 0, 0, D_FG_GREEN, "                           " );
#endif /* __DEBUGGER_BABUNI__ */
	if ( a_iCode && f_poForegroundWindow )
		f_poForegroundWindow->status_bar ( )->message ( D_FG_RED,
				"unknown function, err code(%d)", a_iCode );
	return ( a_iCode );
	M_EPILOG
	}

/* this makro is ripped from unistd.h, I seriously doubt if it is portable */

#define M_REFRESH( )	if ( n_bNeedRepaint )\
	{\
	n_bNeedRepaint = false;\
	::refresh ( );\
	}\

#ifdef __GNUC__
#	define M_STDHAPI_TEMP_FAILURE_RETRY( expression )	(__extension__ ( \
	{ long int __result; \
	do \
		{ \
		if ( n_bInputWaiting ) \
			{ \
			process_stdin ( STDIN_FILENO ); \
			M_REFRESH ( ); \
			} \
		__result = (long int) (expression); \
		} \
	while (__result == -1L && errno == EINTR); \
	__result; \
	}))
#else /* __GNUC__ */
#	define M_STDHAPI_TEMP_FAILURE_RETRY( expression ) ( expression )
#endif /* not __GNUC__ */

int HProcess::run ( void )
	{
	M_PROLOG
	int l_iError = 0;
	int l_iFileDes = 0;
	PROCESS_HANDLER_FILEDES_t HANDLER = NULL;
	if ( ! f_bInitialised )
		M_THROW ( "you have to call HProcess::init ( ) first, dumbass", g_iErrNo );
	::refresh ( );
	while ( f_bLoop )
		{
		reconstruct_fdset ( );
		if ( ( l_iError = M_STDHAPI_TEMP_FAILURE_RETRY ( select ( FD_SETSIZE,
							& f_xFileDescriptorSet,	NULL, NULL, & f_sLatency ) ) ) )
			{
			if ( l_iError < 0 )
				M_THROW ( "select ( ) returned", l_iError );
			if ( ! f_oFileDescriptorHandlers.quantity ( ) )
				return ( -1 );
			f_oFileDescriptorHandlers.rewind ( );
			while ( f_oFileDescriptorHandlers.iterate ( l_iFileDes, HANDLER ) )
				{
				if ( FD_ISSET ( l_iFileDes, & f_xFileDescriptorSet ) )
					{
					M_IRV ( ( this->*HANDLER ) ( l_iFileDes ) );
					f_iIdleCycles = 0;
					}
				}
			}
		else
			handler_idle ( 0 );
		if ( n_bNeedRepaint )
			{
			n_bNeedRepaint = false;
			::refresh ( );
			}
		}
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_idle ( int a_iCode, void * )
	{
	M_PROLOG
	HStatusBarControl * l_poStatusBar = NULL;
#ifdef __DEBUG__
	HString l_oClock ( static_cast < char const * > ( HTime ( ) ) );
	c_printf ( 0, n_iWidth - l_oClock.get_length ( ),
			D_FG_BLACK | D_BG_LIGHTGRAY, l_oClock );
	n_bNeedRepaint = true;
#endif /* __DEBUG__ */
	if ( f_poForegroundWindow )
		{
		l_poStatusBar = f_poForegroundWindow->status_bar ( );
		if ( l_poStatusBar )
			l_poStatusBar->refresh ( );
		}
	f_iIdleCycles ++;
	return ( a_iCode );
	M_EPILOG
	}

int HProcess::process_mouse ( int )
	{
	M_PROLOG
	handler_mouse ( 0 );
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_mouse ( int a_iCode, void * )
	{
	M_PROLOG
	a_iCode = 0;
	mouse::OMouse l_sMouse;
	M_IRV ( mouse::mouse_get ( l_sMouse ) );
#ifdef __DEBUGGER_BABUNI__
	c_printf ( 0, 0,	D_FG_BLACK | D_BG_LIGHTGRAY, "mouse: %6d, %3d, %3d",
			l_sMouse.f_iButtons, l_sMouse.f_iRow, l_sMouse.f_iColumn );
	n_bNeedRepaint = true;
#endif /* __DEBUGGER_BABUNI__ */
	if ( f_poForegroundWindow )
		f_poForegroundWindow->click ( l_sMouse );
	return ( a_iCode );
	M_EPILOG
	}

int HProcess::handler_refresh ( int, void * )
	{
	M_PROLOG
	::endwin ( );
	n_bNeedRepaint = false;
	clrscr ( ); /* there is ::refresh ( ) call inside */
	kbhit ( ); /* cleans all trash from stdio buffer */
	getmaxyx ( stdscr, n_iHeight, n_iWidth );
	if ( f_poForegroundWindow )
		f_poForegroundWindow->refresh ( );
	::refresh ( );
	return ( 0 );
	M_EPILOG
	}

int HProcess::handler_quit ( int, void * )
	{
	M_PROLOG
	f_bLoop = false;
	clrscr ( );
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
		f_poForegroundWindow = static_cast < HWindow * > ( l_oInfo.get < void * > ( ) );
		}
	else
		f_poForegroundWindow = static_cast < HWindow * > ( f_poWindows->go ( -1 ) [ 0 ].get < void * > ( ) );
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

int HProcess::handler_jump_meta_direct ( int a_iCode, void * )
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

int HProcess::handler_close_window ( int a_iCode, void * )
	{
	M_PROLOG
	f_poWindows->remove_element ( D_EMPTY_IF_NOT_EMPTIED );
	f_poForegroundWindow = static_cast < HWindow * > ( f_poWindows->go ( -1 ) [ 0 ].get < void * > ( ) );
	handler_refresh ( 0 );
	a_iCode = 0;
	return ( a_iCode );
	M_EPILOG
	}

}

}

