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

#include "config.h"

#include "hcore/hexception.h" /* M_PROLOG, M_EPILOG */
M_CVSID ( "$CVSHeader$" )
#include "hcore/xalloc.h"
#include "hconsole/console.h" /* conio (ncurses) ability */
#include "hconsole/hconsole.h" /* n_bUseMouse */
#include "hcore/hlog.h"       /* log object */
#include "hcore/hstring.h"    /* HString class */
#include "signals.h"
#include "tools.h"                /* tools namespace */

using namespace stdhapi::hcore;
using namespace stdhapi::hconsole;

namespace stdhapi
{

namespace tools
{

namespace signals
{

/* Some notes about signal reinstalling.
 * Default behavior on BSD and GNU systems is to keep current signal
 * handler after signal delivery, only SVID systems reinstall SIG_DFL
 * after signal delivery. So we do not need to be concernd about signal
 * reinstallation inside our signal handlers, because we support only
 * BSD and GNU compatibile systems. */

typedef void ( * SIGNAL_HANDLER_t ) ( int );

class HSigStackWrapper
	{
protected:
/*{*/
	stack_t f_sStack;
/*}*/
public:
/*{*/
	HSigStackWrapper ( void );
	virtual ~HSigStackWrapper ( void );
	void create_stack ( void );
/*}*/
	} g_oSigStack;

HSigStackWrapper::HSigStackWrapper ( void ) : f_sStack ( )
	{
	f_sStack.ss_sp = NULL;
	f_sStack.ss_size = 0;
	f_sStack.ss_flags = 0;
	return;
	}

HSigStackWrapper::~HSigStackWrapper ( void )
	{
	if ( f_sStack.ss_sp )
		{
		while ( f_sStack.ss_flags )
			; /* we wait til last signal returns */
		xfree ( f_sStack.ss_sp );
		f_sStack.ss_sp = NULL;
		}
	return;
	}

void HSigStackWrapper::create_stack ( void )
	{
	M_PROLOG
	int l_iError = 0;
	f_sStack.ss_sp = xmalloc < char > ( SIGSTKSZ );
	f_sStack.ss_size = SIGSTKSZ;
	f_sStack.ss_flags = 0;
	l_iError = sigaltstack ( & f_sStack, NULL );
	if ( l_iError )
		M_THROW ( "sigaltstack ( )", l_iError );
	if ( f_sStack.ss_flags )
		M_THROW ( "sigaltstack ( ) failed to set stack", g_iErrNo );
	return;
	M_EPILOG
	}

void install_special ( SIGNAL_HANDLER_t HANDLER, int a_iSignum )
	{
	M_PROLOG
	int l_iError = 0;
	struct sigaction l_sHandler, l_sOldHandler;
	HString l_oError;
#if 0 
/* We need to wait til signal handling with sigaltstack
 * will be fixed in LinuxThreads environment. */
	l_sHandler.sa_flags = SA_ONSTACK;
#else
	l_sHandler.sa_flags = 0;
#endif
/* order of calls of siginterrupt, sigaction does matter */
	siginterrupt ( a_iSignum, 1 ); /* 1 means true */
	sigemptyset ( & l_sHandler.sa_mask );
	sigaddset ( & l_sHandler.sa_mask, a_iSignum );
	l_sHandler.sa_handler = HANDLER;
	l_iError = sigaction ( a_iSignum, & l_sHandler, & l_sOldHandler );
	if ( l_iError )
		{
		l_oError.format ( "sigaction ( SIG(%d), ... )", a_iSignum );
		M_THROW ( l_oError, l_iError );
		}
	if ( l_sOldHandler.sa_handler == SIG_IGN )
		sigaction ( a_iSignum, & l_sOldHandler, & l_sHandler );
	return;
	M_EPILOG
	}

/* singnal handler definitions */

void signal_WINCH ( int a_iSignum )
	{
	M_PROLOG
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	l_oMessage = "Terminal size changed: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
#ifdef __HCORE_HLOG_H
	log << l_oMessage << endl;
#endif /* __HCORE_HLOG_H */
#ifdef __HCONSOLE_CONSOLE_H
	if ( is_enabled ( ) )
		{
		n_bInputWaiting = true;
		ungetch ( D_KEY_CTRL_('l') );
		}
	else
		fprintf ( stderr, "\n%s", l_pcSignalMessage );
#else /* __HCONSOLE_CONSOLE_H */
	fprintf ( stderr, "\n%s", l_pcSignalMessage );
#endif /* not __HCONSOLE_CONSOLE_H */
	return;
	M_EPILOG
	}

void signal_INT ( int a_iSignum )
	{
	M_PROLOG
	if ( tools::n_bIgnoreSignalSIGINT )
		return;
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	l_oMessage = "Interrupt signal caught, process broken: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
#ifdef __HCORE_HLOG_H
	log << l_oMessage << endl;
#endif /* __HCORE_HLOG_H */
#ifdef __HCONSOLE_CONSOLE_H
	if ( is_enabled ( ) )
		leave_curses();
#endif /* __HCONSOLE_CONSOLE_H */
	fprintf ( stderr, "\n%s", l_pcSignalMessage );
	signal ( SIGINT, SIG_DFL );
	raise ( SIGINT );
	return;
	M_EPILOG
	}
	
void signal_TERM ( int a_iSignum )
	{
	M_PROLOG
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	l_oMessage = "Process was explictly killed: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
#ifdef __HCORE_HLOG_H
	log << l_oMessage << endl;
#endif /* __HCORE_HLOG_H */
#ifdef __HCONSOLE_CONSOLE_H
	if ( is_enabled ( ) )
		leave_curses();
#endif
	fprintf ( stderr, "\n%s", l_pcSignalMessage );
	signal ( SIGTERM, SIG_DFL );
	raise ( SIGTERM );
	return;
	M_EPILOG
	}
	
void signal_QUIT ( int a_iSignum )
	{
	M_PROLOG
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	if ( tools::n_bIgnoreSignalSIGQUIT )
		{
#ifdef __HCONSOLE_CONSOLE_H
		if ( is_enabled ( ) )
			c_printf ( n_iHeight - 1, 0, COLORS::D_FG_BRIGHTRED,
					"Hard Quit is disabled by stdhapi configuration." );
#endif /* __HCONSOLE_CONSOLE_H */
		return;
		}
	l_oMessage = "Abnormal program quit forced: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
#ifdef __HCORE_HLOG_H
	log << l_oMessage << endl;
#endif /* __HCORE_HLOG_H */
#ifdef __HCONSOLE_CONSOLE_H
	if ( is_enabled ( ) )
		leave_curses();
#endif
	fprintf ( stderr, "\n%s", l_pcSignalMessage );
	signal ( SIGQUIT, SIG_DFL );
	raise ( SIGQUIT );
	return;
	M_EPILOG
	}

void signal_TSTP ( int a_iSignum )
	{
	M_PROLOG
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	if ( tools::n_bIgnoreSignalSIGINT )
		{
#ifdef __HCONSOLE_CONSOLE_H
		if ( is_enabled ( ) )
			c_printf ( n_iHeight - 1, 0, COLORS::D_FG_BRIGHTRED,
					"Suspend is disabled by stdhapi configuration." );
#endif /* __HCONSOLE_CONSOLE_H */
		return;
		}
	l_oMessage = "Stop signal caught, process suspended: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
#ifdef __HCORE_HLOG_H
	log << l_oMessage << endl;
#endif /* __HCORE_HLOG_H */
#ifdef __HCONSOLE_CONSOLE_H
	if ( is_enabled ( ) )
		leave_curses();
#endif
	fprintf ( stderr, "\n%s", l_pcSignalMessage );
	signal ( SIGTSTP, SIG_DFL );
	raise ( SIGTSTP );
	return;
	M_EPILOG
	}

void signal_CONT ( int a_iSignum )
	{
	M_PROLOG
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	l_oMessage = "Process was resurected: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
#ifdef __HCORE_HLOG_H
	log << l_oMessage << endl;
#endif /* __HCORE_HLOG_H */
#ifdef __HCONSOLE_CONSOLE_H
	if ( ! is_enabled ( ) )
		enter_curses();
	if ( is_enabled ( ) )
		{
		n_bInputWaiting = true;
		ungetch ( D_KEY_CTRL_('l') );
		}
#endif /* __HCONSOLE_CONSOLE_H */
	fprintf ( stderr, "\n%s", l_pcSignalMessage );
	if ( signal ( SIGTSTP, signals::signal_TSTP ) == SIG_IGN )
		signal ( SIGTSTP, SIG_IGN );
	return;
	M_EPILOG
	}

void signal_fatal ( int a_iSignum )
	{
	M_PROLOG
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	l_oMessage = "Process caused FATAL ERROR: ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
#ifdef __HCORE_HLOG_H
	log << l_oMessage << endl;
#endif /* __HCORE_HLOG_H */
#ifdef __HCONSOLE_CONSOLE_H
	if ( is_enabled ( ) )
		leave_curses();
#endif /* __HCONSOLE_CONSOLE_H */
	fprintf ( stderr, "\n%s", l_pcSignalMessage );
	signal ( a_iSignum, SIG_DFL );
	raise ( a_iSignum );
	return;
	M_EPILOG
	}

void signal_USR1 ( int a_iSignum )
	{
	M_PROLOG
#ifdef __HCONSOLE_CONSOLE_H
	if ( n_bUseMouse )
		{
		if ( is_enabled ( ) )
			{
			n_bInputWaiting = true;
			ungetch ( KEY_CODES::D_MOUSE );
			return;
			}
		}
#endif /* __HCONSOLE_CONSOLE_H */
	char const * l_pcSignalMessage = NULL;
	HString l_oMessage;
	l_oMessage = "\nDo you play with the mouse under FreeBSD ? ";
	l_oMessage += strsignal ( a_iSignum );
	l_oMessage += '.';
	l_pcSignalMessage = l_oMessage;
#ifdef __HCORE_HLOG_H
	log << l_oMessage << endl;
#endif /* __HCORE_HLOG_H */
#ifdef __HCONSOLE_CONSOLE_H
	if ( is_enabled ( ) )
		leave_curses();
#endif /* __HCONSOLE_CONSOLE_H */
	fprintf ( stderr, "\n%s", l_pcSignalMessage );
	signal ( a_iSignum, SIG_DFL );
	raise ( a_iSignum );
	return;
	M_EPILOG
	}

/* end of signal handler definitions */

void set_handlers ( void )
	{
	M_PROLOG
	g_oSigStack.create_stack ( );
	install_special ( signals::signal_USR1, SIGUSR1 );
	install_special ( signals::signal_WINCH, SIGWINCH );
	if ( signal ( SIGINT, signals::signal_INT ) == SIG_IGN )
		signal ( SIGINT, SIG_IGN );
	if ( signal ( SIGTERM, signals::signal_TERM ) == SIG_IGN )
		signal ( SIGTERM, SIG_IGN );
	if ( signal ( SIGQUIT, signals::signal_QUIT ) == SIG_IGN )
		signal ( SIGQUIT, SIG_IGN );
	if ( signal ( SIGTSTP, signals::signal_TSTP ) == SIG_IGN )
		signal ( SIGTSTP, SIG_IGN );
	if ( signal ( SIGCONT, signals::signal_CONT ) == SIG_IGN )
		signal ( SIGCONT, SIG_IGN );
	if ( signal ( SIGPIPE, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGPIPE, SIG_IGN );
	if ( signal ( SIGFPE, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGFPE, SIG_IGN );
	if ( signal ( SIGILL, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGILL, SIG_IGN );
	if ( signal ( SIGSEGV, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGSEGV, SIG_IGN );
	if ( signal ( SIGBUS, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGBUS, SIG_IGN );
	if ( signal ( SIGABRT, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGABRT, SIG_IGN );
	if ( signal ( SIGIOT, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGIOT, SIG_IGN );
	if ( signal( SIGTRAP, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGTRAP, SIG_IGN );
	if ( signal ( SIGSYS, signals::signal_fatal ) == SIG_IGN )
		signal ( SIGSYS, SIG_IGN );
	return;
	M_EPILOG
	}

}

}

}

