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

#undef ECHO

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hterminal.hxx"
#include "tools.hxx"
#include "hcore/hfile.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace tools {

HTerminal::HTerminal( void )
	: _termios( chunk_size<struct termios>( 1 ) ) {
	return;
}

bool HTerminal::exists( void ) const {
	return ( ::isatty( STDIN_FILENO ) || ::isatty( STDOUT_FILENO ) || ::isatty( STDERR_FILENO ) );
}

void HTerminal::init( void ) {
	M_PROLOG
	termios& termios( *_termios.get<struct termios>() );
	::memset( &termios, 0, sizeof ( termios ) );
	M_ENSURE( tcgetattr( STDIN_FILENO, &termios ) == 0 );
	if ( _disableXON_ ) {
		termios.c_iflag &= ~static_cast<int unsigned>( IXON );
	}
	if ( _leaveCtrlC_ ) {
		termios.c_cc[ VINTR ] = 0;
	}
	if ( _leaveCtrlZ_ ) {
		termios.c_cc[ VSUSP ] = 0;
	}
	if ( _leaveCtrlS_ ) {
		termios.c_cc[ VSTOP ] = 0;
	}
	if ( _leaveCtrlQ_ ) {
		termios.c_cc[ VSTART ] = 0;
	}
	if ( _leaveCtrlBackSlash_ ) {
		termios.c_cc[ VQUIT ] = 0;
	}
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &termios ) == 0 );
	return;
	M_EPILOG
}

void HTerminal::flush( void ) {
	M_PROLOG
	if ( _disableXON_ ) {
		M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, _termios.get<termios>() ) == 0 );
	}
	return;
	M_EPILOG
}

namespace {

static int const BAD_FD( -1 );

int find_term_fd( void ) {
	int fd( BAD_FD );
	if ( ::isatty( STDOUT_FILENO ) ) {
		fd = STDOUT_FILENO;
	} else if ( ::isatty( STDERR_FILENO ) ) {
		fd = STDERR_FILENO;
	} else if ( ::isatty( STDIN_FILENO ) ) {
		fd = STDIN_FILENO;
	}
	return ( fd );
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
static int const FWD_TIOCGWINSZ = TIOCGWINSZ;
#pragma GCC diagnostic error "-Wold-style-cast"

}

HTerminal::coord_t HTerminal::size( void ) const {
	M_PROLOG
	M_ENSURE( exists() );
	winsize w;
	::memset( &w, 0, sizeof ( w ) );
	int termFd( find_term_fd() );
	M_ASSERT( termFd != BAD_FD );
	M_ENSURE( ioctl( termFd, FWD_TIOCGWINSZ, &w ) >= 0 );
	return ( coord_t( w.ws_row, w.ws_col ) );
	M_EPILOG
}

HTerminal _terminal_;

template<>
bool is_a_tty( int const& fd_ ) {
	return ( ::isatty( fd_ ) ? true : false );
}

template<>
bool is_a_tty( FILE* const& file_ ) {
	bool isTty( false );
	if ( file_ == stdout ) {
		isTty = ::isatty( STDOUT_FILENO ) ? true : false;
	} else if ( file_ == stderr ) {
		isTty = ::isatty( STDERR_FILENO ) ? true : false;
	} else if ( file_ == stdin ) {
		isTty = ::isatty( STDIN_FILENO ) ? true : false;
	}
	return ( isTty );
}

template<>
bool is_a_tty( HStreamInterface const& stream_ ) {
	bool isTty( false );
	if ( ( &stream_ == &cout ) || ( &stream_ == &clog ) ) {
		isTty = ::isatty( STDOUT_FILENO ) ? true : false;
	} else if ( &stream_ == &cerr ) {
		isTty = ::isatty( STDERR_FILENO ) ? true : false;
	} else if ( &stream_ == &cin ) {
		isTty = ::isatty( STDIN_FILENO ) ? true : false;
	}
	return ( isTty );
}

template<>
bool is_a_tty( HSynchronizedStream const& stream_ ) {
	return ( is_a_tty<HStreamInterface>( stream_ ) );
}

template<>
bool is_a_tty( HFile const& stream_ ) {
	return ( is_a_tty<HStreamInterface>( stream_ ) );
}

}

}

