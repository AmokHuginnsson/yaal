/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib> /* getenv */
#include <cstdio>
#include <cstring>
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
	termios& backupTermios( *_termios.get<struct termios>() );
	::memset( &backupTermios, 0, sizeof ( backupTermios ) );
	M_ENSURE( tcgetattr( STDIN_FILENO, &backupTermios ) == 0 );
	termios newTermios;
	::memset( &newTermios, 0, sizeof ( newTermios ) );
	M_ENSURE( tcgetattr( STDIN_FILENO, &newTermios ) == 0 );
	if ( _disableXON_ ) {
		newTermios.c_iflag &= ~static_cast<int unsigned>( IXON );
	}
	if ( _leaveCtrlC_ ) {
		newTermios.c_cc[ VINTR ] = 0;
	}
	if ( _leaveCtrlZ_ ) {
		newTermios.c_cc[ VSUSP ] = 0;
	}
	if ( _leaveCtrlS_ ) {
		newTermios.c_cc[ VSTOP ] = 0;
	}
	if ( _leaveCtrlQ_ ) {
		newTermios.c_cc[ VSTART ] = 0;
	}
	if ( _leaveCtrlBackSlash_ ) {
		newTermios.c_cc[ VQUIT ] = 0;
	}
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, &newTermios ) == 0 );
	return;
	M_EPILOG
}

void HTerminal::flush( void ) {
	M_PROLOG
	M_ENSURE( tcsetattr( STDIN_FILENO, TCSAFLUSH, _termios.get<termios>() ) == 0 );
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int const FWD_TIOCGWINSZ = TIOCGWINSZ;
#pragma GCC diagnostic pop

}

namespace {

int from_env( char const* envVarName_ ) {
	int val( -1 );
	char const* envVarValue( ::getenv( envVarName_ ) );
	if ( envVarValue ) {
		try {
			val = lexical_cast<int>( envVarValue );
		} catch ( HException const& ) {
		}
	}
	return ( val );
}

}

HTerminal::HSize HTerminal::size( void ) const {
	M_PROLOG
	M_ENSURE( exists(), "Not connected to a terminal." );
	int termFd( find_term_fd() );
	M_ASSERT( termFd != BAD_FD );
	int lines( from_env( "LINES" ) );
	int columns( from_env( "COLUMNS" ) );
	if ( ( lines < 0 ) || ( columns < 0 ) ) {
		winsize w;
		::memset( &w, 0, sizeof ( w ) );
		M_ENSURE( ioctl( termFd, FWD_TIOCGWINSZ, &w ) >= 0 );
		if ( lines < 0 ) {
			lines = w.ws_row;
		}
		if ( columns < 0 ) {
			columns = w.ws_col;
		}
	}
	return ( HSize( lines, columns ) );
	M_EPILOG
}

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

