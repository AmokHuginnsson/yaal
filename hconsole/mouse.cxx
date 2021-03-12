/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <libintl.h>

#include "config.hxx"

#ifdef HAVE_SYS_CONSIO_H
#	include <sys/consio.h>
#	include <csignal>
#	include <fcntl.h>
#elif defined ( HAVE_GPM_H )
#	include <gpm.h>
#endif /* HAVE_GPM_H */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "mouse.hxx"
#include "console.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hcore.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hformat.hxx"
#include "hcore/system.hxx"

#ifdef HAVE_CURSES_H
#	include <curses.h>
#elif defined ( HAVE_NCURSES_CURSES_H )
#	include <ncurses/curses.h>
#else /* HAVE_NCURSES_CURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_CURSES_H */

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

namespace mouse {

fun_console_mouse_open_t mouse_open = nullptr;
fun_console_mouse_get_t mouse_get = nullptr;
fun_console_mouse_close_t mouse_close = nullptr;

#if defined( HAVE_SYS_CONSIO_H ) || defined( HAVE_GPM_H )
namespace {

int hunt_tty( int offset_ ) {
	M_PROLOG
	/* this hack allows guessing current controlling virtual terminal screen */
	int vC( 0 );
	char const* ttyName( ttyname( STDIN_FILENO ) );
	if ( ttyName && ! ::strncmp( ttyName, "/dev/ttyv", static_cast<size_t>( 8 + offset_ ) ) ) {
		vC = lexical_cast<int>( ttyName + 8 + offset_ );
	} else {
		ttyName = ::getenv( "STY" );
		if ( ttyName ) {
			char const* ptr = nullptr;
			if ( ( ptr = ::strstr( ttyName, ".tty" ) ) ) {
				vC = lexical_cast<int>( ptr + 4 + offset_ );
			} else if ( ( ptr = ::strstr( ttyName, ".pts" ) ) ) {
				vC = lexical_cast<int>( ptr + 4 + offset_ );
			}
		} else {
			throw HConsoleException( "cannot find controlling virtual console", errno );
		}
	}
	return vC;
	M_EPILOG
}

}
#endif /* #if defined( HAVE_SYS_CONSIO_H ) || defined( HAVE_GPM_H ) */

#ifdef HAVE_SYS_CONSIO_H

int _mouse_ = 0;

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int long unsigned const FWD_CONS_MOUSECTL = CONS_MOUSECTL;
#pragma GCC diagnostic pop
}

int console_mouse_open( void ) {
	M_PROLOG
	int vC = 0;
	char tty[] = "/dev/ttyv0";
	mouse_info mouse;
	mouse.operation = MOUSE_MODE;
	mouse.u.mode.mode = 0;
	mouse.u.mode.signal = SIGUSR1;
	vC = hunt_tty( 1 );
	tty[ 9 ] = static_cast<char>( '0' + vC );
	_mouse_ = ::open( tty, O_RDWR );
	if ( _mouse_ < 0 ) {
		throw HMouseException( to_string( _( "cannot open mouse, " ) ).append( error_message( errno ) ), vC );
	}
	if ( ::ioctl( _mouse_, FWD_CONS_MOUSECTL, &mouse ) < 0 ) {
		int err( errno );
		TEMP_FAILURE_RETRY( hcore::system::close( _mouse_ ) );
		throw HMouseException( to_string( _( "cannot setup mouse mode, " ) ).append( err ), err );
	}

	hcore::log( LOG_LEVEL::INFO ) << "i have opened device: `" << tty << '\'' << endl;

	return ( 0 );
	M_EPILOG
}

int console_mouse_get( OMouse& mouse_ ) {
	M_PROLOG
	mouse_info mouse;
	mouse.operation = MOUSE_GETINFO;
	if ( ::ioctl( _mouse_, FWD_CONS_MOUSECTL, &mouse ) < 0 ) {
		throw HMouseException( "cannot get mouse data", errno );
	} else {
		mouse_._buttons = mouse.u.data.buttons;
		mouse_._row = mouse.u.data.y / 16;
		mouse_._column = mouse.u.data.x / 8;
	}
	return ( 0 );
	M_EPILOG
}

int console_mouse_close( void ) {
	M_PROLOG
	if ( ! _mouse_ ) {
		throw HMouseException( "mouse not opened", errno );
	}
	TEMP_FAILURE_RETRY( hcore::system::close( _mouse_ ) );
	_mouse_ = 0;
	return ( 0 );
	M_EPILOG
}

#elif defined ( HAVE_GPM_H )

int console_mouse_open( void ) {
	M_PROLOG
	int vC = 0;
	Gpm_Connect gpm;
	gpm.minMod = 0;
	gpm.maxMod = 0;
	gpm.pid = hcore::system::getpid();
	gpm.vc = vC = hunt_tty( 0 );
	gpm.eventMask = static_cast<int>( GPM_UP );
	gpm.defaultMask = static_cast<int short unsigned>( ~gpm.eventMask );
	gpm_zerobased = true;
	if ( Gpm_Open( &gpm, vC ) == -1 ) {
		throw HMouseException( "Can't open mouse connection: "_ys.append( error_message( errno ) ), vC );
	}
	hcore::log( LOG_LEVEL::INFO ) << "i have opened device: `" << vC << '\'' << endl;
	return gpm_fd;
	M_EPILOG
}

int console_mouse_get( OMouse& mouse_ ) {
	M_PROLOG
	Gpm_Event event;
	if ( Gpm_GetEvent( &event ) != 1 ) {
		throw HMouseException( _( "cannot retrieve event") , errno );
	}
	mouse_._buttons = 0;
	mouse_._buttons |= ( ( event.buttons & GPM_B_LEFT ) ? MOUSE_BITS::BUTTON::ONE : 0 );
	mouse_._buttons |= ( ( event.buttons & GPM_B_MIDDLE ) ? MOUSE_BITS::BUTTON::TWO : 0 );
	mouse_._buttons |= ( ( event.buttons & GPM_B_RIGHT ) ? MOUSE_BITS::BUTTON::THREE : 0 );
	mouse_._buttons |= ( ( event.buttons & GPM_B_UP ) ? MOUSE_BITS::BUTTON::WHEEL_UP : 0 );
	mouse_._buttons |= ( ( event.buttons & GPM_B_DOWN ) ? MOUSE_BITS::BUTTON::WHEEL_DOWN : 0 );
	mouse_._row = event.y;
	mouse_._column = event.x;
	return ( 0 );
	M_EPILOG
}

int console_mouse_close( void ) {
	M_PROLOG
	while ( Gpm_Close() ) {
		;
	}
	return ( 0 );
	M_EPILOG
}

#else /* HAVE_GPM_H */

int console_mouse_open( void ) {
	M_PROLOG
	volatile int a( 1 );
	if ( a ) {
		throw HMouseException( _( "console mouse support not compiled" ) );
	}
	return ( 0 );
	M_EPILOG
}

int console_mouse_get( OMouse& ) {
	M_PROLOG
	volatile int a( 1 );
	if ( a ) {
		throw HMouseException( _( "console mouse support not compiled" ) );
	}
	return ( 0 );
	M_EPILOG
}

int console_mouse_close( void ) {
	M_PROLOG
/* I cannot throw exception here because exception was probably
 * thrown by console_mouse_open in enter_curses and now this
 * function is probably called from leave_curses from exception
 * catch block, so new exception would be recursive and would
 * overload the stack because of infinite number of exceptions */
	return ( 0 );
	M_EPILOG
}

#endif /* ! HAVE_GPM_H */

int x_mouse_open( void ) {
	M_PROLOG
	mmask_t desiredMouseMask( ALL_MOUSE_EVENTS );
	mmask_t strictlyRequiredMask( BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED | BUTTON2_CLICKED | BUTTON3_CLICKED );

	set_env( "NCURSES_GPM_TERMS", "" );
	mmask_t mouseMask( mousemask( desiredMouseMask, nullptr ) );
	if ( ! mouseMask ) {
		throw HMouseException( "mousemask() returned 0", errno );
	} else if ( ( mouseMask & strictlyRequiredMask ) < strictlyRequiredMask ) {
		throw (
			HMouseException(
				format(
					"could not set up appropriate mask: B1C = %lu, B2C = %lu, B3C = %lu, B1DC = %lu",
					mouseMask & BUTTON1_CLICKED, mouseMask & BUTTON2_CLICKED,
					mouseMask & BUTTON3_CLICKED, mouseMask & BUTTON1_DOUBLE_CLICKED
				)
			)
		);
	}
	mouseinterval( 200 );
#if defined( HAVE_DECL_HAS_MOUSE ) && ( HAVE_DECL_HAS_MOUSE == 1 )
	if ( ! has_mouse() ) {
		throw HMouseException( "Mouse driver failed to initialize properly." );
	}
#endif /* #if defined( HAVE_DECL_HAS_MOUSE ) && ( HAVE_DECL_HAS_MOUSE == 1 ) */
	return ( 0 );
	M_EPILOG
}

int x_mouse_get( OMouse& mouse_ ) {
	M_PROLOG
	MEVENT mouse;
	if ( getmouse( &mouse ) != OK ) {
		throw HMouseException( "cannot get mouse data", errno );
	} else {
		mouse_._buttons = 0;
		mouse_._buttons |= ( ( mouse.bstate & BUTTON1_CLICKED ) ? MOUSE_BITS::BUTTON::ONE : 0 );
		mouse_._buttons |= ( ( mouse.bstate & BUTTON1_DOUBLE_CLICKED ) ? MOUSE_BITS::BUTTON::ONE_2 : 0 );
		mouse_._buttons |= ( ( mouse.bstate & BUTTON2_CLICKED ) ? MOUSE_BITS::BUTTON::TWO : 0 );
		mouse_._buttons |= ( ( mouse.bstate & BUTTON2_DOUBLE_CLICKED ) ? MOUSE_BITS::BUTTON::TWO_2 : 0 );
		mouse_._buttons |= ( ( mouse.bstate & BUTTON3_CLICKED ) ? MOUSE_BITS::BUTTON::THREE : 0 );
		mouse_._buttons |= ( ( mouse.bstate & BUTTON3_DOUBLE_CLICKED ) ? MOUSE_BITS::BUTTON::THREE_2 : 0 );
		mouse_._buttons |= ( ( mouse.bstate & BUTTON4_PRESSED ) ? MOUSE_BITS::BUTTON::WHEEL_UP : 0 );
		mouse_._buttons |= ( ( mouse.bstate >= ( BUTTON4_TRIPLE_CLICKED << 1 ) ) ? MOUSE_BITS::BUTTON::WHEEL_DOWN : 0 );
		mouse_._row = mouse.y;
		mouse_._column = mouse.x;
	}
	return ( 0 );
	M_EPILOG
}

int x_mouse_close( void ) {
	return ( 0 );
}

}

}

}

