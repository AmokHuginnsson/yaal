/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	sighand.c - this file is integral part of `stdhapi' project.

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

#include <string.h>          /* strsignal */
#include <unistd.h>          /* kill function           */
#include <stdio.h>		       /* perror function         */
#include <signal.h>		       /* signal handling         */

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif HAVE_NCURSES_NCURSES_H
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "signals.h"

#include "../hcore/xalloc.h"
#include "../hcore/rc_file.h"
#include "../hcore/hexception.h"
#include "../hconsole/console.h"         /* conio (ncurses) ability */
#include "../hcore/hlog.h"            /* log object */
#include "../hcore/hstring.h"         /* HString class */

namespace signals
{

bool n_bCtrlCDoesNotBreak = false;
	
/* singnal handler definitions */
	
void signal_terminal_size_changed ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	if ( signal( SIGWINCH, signals::signal_terminal_size_changed ) == SIG_IGN )
		signal( SIGWINCH, SIG_IGN );
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nTerminal size changed: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
#ifdef __CONSOLE_H
	if ( console::is_enabled ( ) )
		{
		console::n_bInputWaiting = true;
		ungetch ( D_KEY_CTRL_('l') );
		}
	else fprintf ( stderr, l_oMessage );
#else /* __CONSOLE_H */
	fprintf ( stderr, l_oMessage );
#endif /* not __CONSOLE_H */
	return;
	M_EPILOG
	}

void signal_ctrlc ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
#ifdef __CONSOLE_H
	if ( console::is_enabled ( ) && n_bCtrlCDoesNotBreak )
		{
		if ( signal( SIGINT, signals::signal_ctrlc ) == SIG_IGN )
			signal( SIGINT, SIG_IGN );
		console::n_bInputWaiting = true;
		ungetch ( D_KEY_CTRL_('c') );
		return;
		}
	if ( console::is_enabled ( ) )console::leave_curses();
#endif /* __CONSOLE_H */
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nUser typed ^C, process broken: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
	fprintf ( stderr, l_oMessage );
	signal ( SIGINT, SIG_DFL );
	raise ( SIGINT );
	return;
	M_EPILOG
	}
	
void signal_kill ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nProcess was explictly killed: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
#ifdef __CONSOLE_H
	if ( console::is_enabled ( ) )console::leave_curses();
#endif
	fprintf ( stderr, l_oMessage );
	signal ( SIGTERM, SIG_DFL );
	raise ( SIGTERM );
	return;
	M_EPILOG
	}
	
void signal_quit ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nAbnormal program quit forced: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
#ifdef __CONSOLE_H
	if ( console::is_enabled ( ) )console::leave_curses();
#endif
	fprintf ( stderr, l_oMessage );
	signal ( SIGQUIT, SIG_DFL );
	raise ( SIGQUIT );
	return;
	M_EPILOG
	}

void signal_stop ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nUser typed ^Z, process suspended: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
#ifdef __CONSOLE_H
	if ( console::is_enabled ( ) )console::leave_curses();
#endif /* __CONSOLE_H */
	fprintf ( stderr, l_oMessage );
	signal ( SIGTSTP, SIG_DFL );
	raise ( SIGTSTP );
	return;
	M_EPILOG
	}

void signal_continue ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	if ( signal( SIGTSTP, signals::signal_stop ) == SIG_IGN )
		signal( SIGTSTP, SIG_IGN );
	if ( signal( SIGCONT, signals::signal_continue ) == SIG_IGN )
		signal( SIGCONT, SIG_IGN );
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nProcess was resurected: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
#ifdef __CONSOLE_H
	if ( ! console::is_enabled ( ) )console::enter_curses();
	if ( console::is_enabled ( ) )
		{
		console::n_bInputWaiting = true;
		ungetch ( D_KEY_CTRL_('l') );
		}
#endif /* __CONSOLE_H */
	fprintf ( stderr, l_oMessage );
	return;
	M_EPILOG
	}

void signal_fatal ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nProcess caused FATAL ERROR: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += ", bailing out.";
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
#ifdef __CONSOLE_H
	if ( console::is_enabled ( ) )console::leave_curses();
#endif /* __CONSOLE_H */
	fprintf ( stderr, l_oMessage );
	signal ( a_iSignum, SIG_DFL );
	raise ( a_iSignum );
	return;
	M_EPILOG
	}

/*  end of signal handler definitions */

void set_handlers ( void )
	{
	M_PROLOG
/* 	signals handling (defining particular functions) */
/*
	int l_iError = 0;
	sigset_t l_sMask;
	static stack_t l_sSigStack;
	struct sigaction l_sHandler;
	l_sSigStack.ss_sp = xmalloc ( SIGSTKSZ );
	l_sSigStack.ss_size = SIGSTKSZ;
	l_sSigStack.ss_flags = 0;
	l_iError = sigaltstack ( & l_sSigStack, NULL );
	if ( l_iError )throw HException ( __WHERE__, "sigaltstack ( )", l_iError );
	sigemptyset ( & l_sMask );
	l_sHandler.sa_handler = signal_ctrlc;
	l_sHandler.sa_mask = l_sMask;
	l_sHandler.sa_flags = SA_RESTART | SA_ONSTACK;
	l_iError = sigaction( SIGINT, & l_sHandler, NULL );
	if ( l_iError )throw HException ( __WHERE__, "sigaction ( SIGINT, ... )", l_iError );
*/
	if ( signal( SIGWINCH, signals::signal_terminal_size_changed ) == SIG_IGN )
		signal( SIGWINCH, SIG_IGN );
	if ( signal( SIGINT, signals::signal_ctrlc ) == SIG_IGN )
		signal( SIGINT, SIG_IGN );
	if ( signal( SIGTERM, signals::signal_kill ) == SIG_IGN )
		signal( SIGTERM, SIG_IGN );
	if ( signal( SIGQUIT, signals::signal_quit ) == SIG_IGN )
		signal( SIGQUIT, SIG_IGN );
	if ( signal( SIGTSTP, signals::signal_stop ) == SIG_IGN )
		signal( SIGTSTP, SIG_IGN );
	if ( signal( SIGCONT, signals::signal_continue ) == SIG_IGN )
		signal( SIGCONT, SIG_IGN );
	if ( signal( SIGFPE, signals::signal_fatal ) == SIG_IGN )
		signal( SIGFPE, SIG_IGN );
	if ( signal( SIGILL, signals::signal_fatal ) == SIG_IGN )
		signal( SIGILL, SIG_IGN );
	if ( signal( SIGSEGV, signals::signal_fatal ) == SIG_IGN )
		signal( SIGSEGV, SIG_IGN );
	if ( signal( SIGBUS, signals::signal_fatal ) == SIG_IGN )
		signal( SIGBUS, SIG_IGN );
	if ( signal( SIGABRT, signals::signal_fatal ) == SIG_IGN )
		signal( SIGABRT, SIG_IGN );
	if ( signal( SIGIOT, signals::signal_fatal ) == SIG_IGN )
		signal( SIGIOT, SIG_IGN );
	if ( signal( SIGTRAP, signals::signal_fatal ) == SIG_IGN )
		signal( SIGTRAP, SIG_IGN );
	if ( signal( SIGSYS, signals::signal_fatal ) == SIG_IGN )
		signal( SIGSYS, SIG_IGN );
/*	all signal handler definitions finished	                              */
	return;
	M_EPILOG
	}

}

extern "C"
{

extern void _init ( void );
extern void _fini ( void );

void set_tools_variables ( HString & a_roOption, HString & a_roValue )
	{
	if ( ! strcasecmp ( a_roOption, "ctrl_c_does_not_break" ) )
		rc_file::rc_set_variable ( a_roValue, signals::n_bCtrlCDoesNotBreak );
	}

void tools_init ( void )
	{
/* I do not know why but when _init ( ) is called explictly, like here would,
 * process hangs just before dbwrapper_fini ( ) */
/*	_init ( ); */
	g_iErrNo = 0;
	rc_file::process_rc_file ( "stdhapi", set_tools_variables );
	return;
	}

void tools_fini ( void )
	{
	_fini ( );
	return;
	}

}

