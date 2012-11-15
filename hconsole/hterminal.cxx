/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hterminal.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib> /* getenv */
#include <cstdio>
#include <unistd.h>
#include <termios.h>

#include "config.hxx"

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif /* #ifdef HAVE_SYS_IOCTL_H */
#ifdef HAVE_TERMIO_H
#include <termio.h>
#endif /* #ifdef HAVE_TERMIO_H */
#ifdef HAVE_TTY_H
#include <tty.h>
#endif /* #ifdef HAVE_TTY_H */

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hterminal.hxx"
#include "hconsole.hxx"
#include "hcore/hrawfile.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;

namespace yaal {

namespace hconsole {

HTerminal::HTerminal( void )
	: _termios( chunk_size<struct termios>( 1 ) )
	{}

bool HTerminal::exists( void ) const {
	return ( ::isatty( STDIN_FILENO ) && ::isatty( STDOUT_FILENO ) && ::isatty( STDERR_FILENO ) );
}

void HTerminal::init( void ) {
	M_PROLOG
	termios termios;
	if ( _disableXON_ ) {
		M_ENSURE( tcgetattr( STDIN_FILENO, _termios.get<struct termios>() ) == 0 );
		M_ENSURE( tcgetattr( STDIN_FILENO, &termios ) == 0 );
		termios.c_iflag &= ~IXON;
		if ( _leaveCtrlC_ )
			termios.c_cc[ VINTR ] = 0;
		if ( _leaveCtrlZ_ )
			termios.c_cc[ VSUSP ] = 0;
		if ( _leaveCtrlS_ )
			termios.c_cc[ VSTOP ] = 0;
		if ( _leaveCtrlQ_ )
			termios.c_cc[ VSTART ] = 0;
		if ( _leaveCtrlBackSlash_ )
			termios.c_cc[ VQUIT ] = 0;
		M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &termios ) == 0 );
	}
	return;
	M_EPILOG
}

void HTerminal::flush( void ) {
	M_PROLOG
	if ( _disableXON_ )
		M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, _termios.get<termios>() ) == 0 );
	return;
	M_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int const FWD_TIOCGWINSZ = TIOCGWINSZ;
#pragma GCC diagnostic error "-Wold-style-cast"
}

HTerminal::coord_t HTerminal::size( void ) const {
	M_PROLOG
	M_ENSURE( exists() );
	winsize w;
	M_ENSURE( ioctl( 0, FWD_TIOCGWINSZ, &w ) >= 0 );
	return ( coord_t( w.ws_row, w.ws_col ) );
	M_EPILOG
}

HTerminal _terminal_;

}

}

