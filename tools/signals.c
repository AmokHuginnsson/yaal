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

#include <stdlib.h>  /* strtol */
#include <string.h>  /* strsignal */
#include <unistd.h>  /* kill function */
#include <stdio.h>	 /* perror function */
#include <signal.h>	 /* signal handling */
#include <termios.h> /* B115200 */

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
#include "../hcore/rc_file.h"    /* read conf from rc */
#include "../hconsole/console.h" /* conio (ncurses) ability */
#include "../hcore/hlog.h"       /* log object */
#include "../hcore/hstring.h"    /* HString class */
#include "signals.h"

char * g_pcSerialDevice = NULL;
int g_iBaudRate = B115200;
int g_iCollectorConnectionTimeOut = 9999;

namespace signals
{

bool n_bIgnoreSignalSIGINT = false;
bool n_bIgnoreSignalSIGTSTP = false;
bool n_bIgnoreSignalSIGQUIT = false;

OVariable n_psVariables [ ] =
	{
		{ D_TYPE_BOOL, "ignore_signal_SIGINT", & n_bIgnoreSignalSIGINT },
		{ D_TYPE_BOOL, "ignore_signal_SIGTSTP", & n_bIgnoreSignalSIGTSTP },
		{ D_TYPE_BOOL, "ignore_signal_SIGQUIT", & n_bIgnoreSignalSIGQUIT },
		{ D_TYPE_INT, "collector_connection_timeout",
		& g_iCollectorConnectionTimeOut },
		{ D_TYPE_CHAR_POINTER, "serial_device", & g_pcSerialDevice },
		{ 0, NULL, NULL }
	};
	
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
	if ( n_bIgnoreSignalSIGINT )
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
	if ( n_bIgnoreSignalSIGQUIT )
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
	if ( n_bIgnoreSignalSIGINT )
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
	l_sHandler.sa_handler = signal_INT;
	l_sHandler.sa_mask = l_sMask;
	l_sHandler.sa_flags = SA_RESTART | SA_ONSTACK;
	l_iError = sigaction( SIGINT, & l_sHandler, NULL );
	if ( l_iError )throw HException ( __WHERE__, "sigaction ( SIGINT, ... )", l_iError );
*/
	if ( signal( SIGWINCH, signals::signal_WINCH ) == SIG_IGN )
		signal( SIGWINCH, SIG_IGN );
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

/* return true means error occured, false - every thing ok */
bool set_tools_variables ( HString & a_roOption, HString & a_roValue )
	{
	int l_iBaudRate = 0;
	if ( ! strcasecmp ( a_roOption, "serial_baudrate" ) )
		{
		if ( a_roValue.get_length ( ) > 1 )
			{
			switch ( strtol ( ( ( char * ) a_roValue ) + 1, NULL, 10 ) )
				{
#ifdef __HOST_OS_TYPE_FREEBSD__
				case (  76800 ): l_iBaudRate = B76800;  break;
				case (  28800 ): l_iBaudRate = B28800;  break;
				case (  14400 ): l_iBaudRate = B14400;  break;
				case (   7200 ): l_iBaudRate = B7200;   break;
#endif /* __HOST_OS_TYPE_FREEBSD__ */
				case ( 115200 ): l_iBaudRate = B115200; break;
				case (  57600 ): l_iBaudRate = B57600;  break;
				case (  38400 ): l_iBaudRate = B38400;  break;
				case (  19200 ): l_iBaudRate = B19200;  break;
				case (   9600 ): l_iBaudRate = B9600;   break;
				case (   4800 ): l_iBaudRate = B4800;   break;
				case (   2400 ): l_iBaudRate = B2400;   break;
				}
			}
		}
	if ( l_iBaudRate )g_iBaudRate = l_iBaudRate;
	return ( ! l_iBaudRate );
	}

void tools_init ( void ); __attribute__ ( ( constructor ) )
void tools_init ( void )
	{
	g_iErrNo = 0;
	rc_file::process_rc_file ( "stdhapi", "tools", signals::n_psVariables,
			set_tools_variables );
	return;
	}

void tools_fini ( void ); __attribute__ ( ( destructor ) )
void tools_fini ( void )
	{
	if ( g_pcSerialDevice )xfree ( ( void * ) g_pcSerialDevice );
	g_pcSerialDevice = NULL;
	return;
	}

/* older versions of g++ fail to handle __attribute__((constructor))
   if no static object exists */

#if __GNUC__ < 3 || \
	 ( __GNUC__ == 3 && __GNUC_MINOR__ < 3 )

HString g_oDummyTOOLS;

#endif

