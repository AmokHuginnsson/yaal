/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	signals.c - this file is integral part of `stdhapi' project.

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

#include <string.h>  /* strsignal */
#include <unistd.h>  /* kill function */
#include <stdio.h>	 /* perror function */
#include <signal.h>	 /* signal handling */
#include <libintl.h>

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif defined ( HAVE_NCURSES_NCURSES_H )
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "../hcore/hexception.h" /* M_PROLOG, M_EPILOG */
M_CVSID ( "$CVSHeader$" );
#include "../hcore/xalloc.h"
#include "../hconsole/console.h" /* conio (ncurses) ability */
#include "../hconsole/hconsole.h" /* n_bUseMouse */
#include "../hcore/hlog.h"       /* log object */
#include "../hcore/hstring.h"    /* HString class */
#include "signals.h"
#include "tools.h"                /* tools namespace */

namespace signals
{

typedef void ( * SIGNAL_HANDLER_t ) ( int );

class HSigStackWrapper
	{
public:
	stack_t m_sData;
	HSigStackWrapper ( void )
		{
		m_sData.ss_sp = ( char * ) xmalloc ( SIGSTKSZ );
		m_sData.ss_size = SIGSTKSZ;
		m_sData.ss_flags = 0;
		}
	virtual ~HSigStackWrapper ( void )
		{
		if ( m_sData.ss_sp )
			{
			while ( m_sData.ss_flags ); /* we wait til last signal returns */
			xfree ( m_sData.ss_sp );
			m_sData.ss_sp = NULL;
			}
		}
	} g_oSigStack;

/* singnal handler definitions */
	
void signal_WINCH ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	if ( signal( SIGWINCH, signals::signal_WINCH ) == SIG_IGN )
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

void signal_INT ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	if ( tools::n_bIgnoreSignalSIGINT )
		{
		if ( signal( SIGINT, signals::signal_INT ) == SIG_IGN )
			signal( SIGINT, SIG_IGN );
		return;
		}
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nInterrupt signal caught, process broken: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
#ifdef __CONSOLE_H
	if ( console::is_enabled ( ) )console::leave_curses();
#endif /* __CONSOLE_H */
	fprintf ( stderr, l_oMessage );
	signal ( SIGINT, SIG_DFL );
	raise ( SIGINT );
	return;
	M_EPILOG
	}
	
void signal_TERM ( int a_iSignum )
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
	
void signal_QUIT ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	if ( tools::n_bIgnoreSignalSIGQUIT )
		{
		if ( signal( SIGQUIT, signals::signal_QUIT ) == SIG_IGN )
			signal( SIGQUIT, SIG_IGN );
#ifdef __CONSOLE_H
		if ( console::is_enabled ( ) )
			console::c_printf ( console::n_iHeight - 1, 0, D_FG_BRIGHTRED,
					"Hard Quit is disabled by stdhapi configuration." );
#endif /* __CONSOLE_H */
		return;
		}
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

void signal_TSTP ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	if ( tools::n_bIgnoreSignalSIGINT )
		{
		if ( signal( SIGTSTP, signals::signal_TSTP ) == SIG_IGN )
			signal( SIGTSTP, SIG_IGN );
#ifdef __CONSOLE_H
		if ( console::is_enabled ( ) )
			console::c_printf ( console::n_iHeight - 1, 0, D_FG_BRIGHTRED,
					"Suspend is disabled by stdhapi configuration." );
#endif /* __CONSOLE_H */
		return;
		}
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nStop signal caught, process suspended: ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
#ifdef __HLOG_H
	log << ( ( char * ) l_oMessage ) + 1 << endl;
#endif /* __HLOG_H */
#ifdef __CONSOLE_H
	if ( console::is_enabled ( ) )console::leave_curses();
#endif
	fprintf ( stderr, l_oMessage );
	signal ( SIGTSTP, SIG_DFL );
	raise ( SIGTSTP );
	return;
	M_EPILOG
	}

void signal_CONT ( int a_iSignum )
	{
	M_PROLOG
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	if ( signal( SIGTSTP, signals::signal_TSTP ) == SIG_IGN )
		signal( SIGTSTP, SIG_IGN );
	if ( signal( SIGCONT, signals::signal_CONT ) == SIG_IGN )
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

void signal_USR1 ( int a_iSignum )
	{
	M_PROLOG
#ifdef __CONSOLE_H
	if ( console::n_bUseMouse )
		{
		if ( console::is_enabled ( ) )
			{
			console::n_bInputWaiting = true;
			ungetch ( KEY_MOUSE );
			return;
			}
		}
#endif /* __CONSOLE_H */
	char * l_pcSignalMessage = 0;
	HString l_oMessage;
	if ( signal( SIGUSR1, signals::signal_USR1 ) == SIG_IGN )
		signal( SIGUSR1, SIG_IGN );
	l_pcSignalMessage = strsignal ( a_iSignum );
	l_oMessage = "\nDo you play with the mouse under FreeBSD ? ";
	l_oMessage += l_pcSignalMessage;
	l_oMessage += '.';
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

void install_special ( SIGNAL_HANDLER_t HANDLER, int a_iSignum )
	{
	M_PROLOG
	int l_iError = 0;
	sigset_t l_sMask;
	struct sigaction l_sHandler, l_sOldHandler;
	HString l_oError;
	l_sHandler.sa_flags = SA_ONSTACK;
	sigemptyset ( & l_sMask );
	sigaddset ( & l_sMask, a_iSignum );
	l_sHandler.sa_handler = HANDLER;
	l_sHandler.sa_mask = l_sMask;
	l_iError = sigaction( a_iSignum, & l_sHandler, & l_sOldHandler );
	if ( l_iError )
		{
		l_oError.format ( "sigaction ( SIG(%d), ... )", a_iSignum );
		M_THROW ( l_oError, l_iError );
		}
	siginterrupt ( a_iSignum, true );
	if ( l_sOldHandler.sa_handler == SIG_IGN )
		signal ( a_iSignum, SIG_IGN );
	return;
	M_EPILOG
	}

void set_handlers ( void )
	{
	M_PROLOG
	int l_iError = 0;
	l_iError = sigaltstack ( & g_oSigStack.m_sData, NULL );
	if ( l_iError )M_THROW ( "sigaltstack ( )", l_iError );
/* 	signals handling (defining particular functions) */
	install_special ( signals::signal_USR1, SIGUSR1 );
	install_special ( signals::signal_WINCH, SIGWINCH );
	if ( signal( SIGINT, signals::signal_INT ) == SIG_IGN )
		signal( SIGINT, SIG_IGN );
	if ( signal( SIGTERM, signals::signal_TERM ) == SIG_IGN )
		signal( SIGTERM, SIG_IGN );
	if ( signal( SIGQUIT, signals::signal_QUIT ) == SIG_IGN )
		signal( SIGQUIT, SIG_IGN );
	if ( signal( SIGTSTP, signals::signal_TSTP ) == SIG_IGN )
		signal( SIGTSTP, SIG_IGN );
	if ( signal( SIGCONT, signals::signal_CONT ) == SIG_IGN )
		signal( SIGCONT, SIG_IGN );
	if ( signal( SIGPIPE, signals::signal_fatal ) == SIG_IGN )
		signal( SIGPIPE, SIG_IGN );
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

