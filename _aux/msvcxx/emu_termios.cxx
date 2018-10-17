#include "../msvcxx/unistd.h"
#include <windows.h>
#include "../msvcxx/termios.h"
#include "../msvcxx/msio.hxx"

void win_to_termios( DWORD winMode_, DWORD winFlag_, tcflag_t& termiosMode_, tcflag_t termiosFlag_ ) {
	if ( winMode_ & winFlag_ ) {
		termiosMode_ |= termiosFlag_;
	} else {
		termiosMode_ &= ~termiosFlag_;
	}
}

void termios_to_win( tcflag_t termiosMode_, tcflag_t termiosFlag_, DWORD& winMode_, DWORD winFlag_ ) {
	if ( termiosMode_ & termiosFlag_ ) {
		winMode_ |= winFlag_;
	} else {
		winMode_ &= ~winFlag_;
	}
}

namespace {
#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
			static int const ENABLE_VIRTUAL_TERMINAL_INPUT      = 0x0200;
#endif
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
			static int const ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x0004;
#endif
#ifndef DISABLE_NEWLINE_AUTO_RETURN
			static int const DISABLE_NEWLINE_AUTO_RETURN        = 0x0008;
#endif
#ifndef ENABLE_LVB_GRID_WORLDWIDE
			static int const ENABLE_LVB_GRID_WORLDWIDE          = 0x0010;
#endif
}

int tcgetattr( int fd_, struct termios* term_ ) {
	int ret( -1 );
	if ( ( fd_ >= STDIN_FILENO ) && ( fd_ <= STDOUT_FILENO ) ) {
		DWORD mode( 0 );
		::GetConsoleMode( ::GetStdHandle( msvcxx::fd_to_handle( fd_ ) ), &mode );
		termios& t( *term_ );
		if ( fd_ == STDIN_FILENO ) {
			win_to_termios( mode, ENABLE_LINE_INPUT, t.c_lflag, ICANON );
			win_to_termios( mode, ENABLE_ECHO_INPUT, t.c_lflag, ECHO );
			win_to_termios( mode, ENABLE_PROCESSED_INPUT, t.c_lflag, IEXTEN | ISIG );

			win_to_termios( mode, ENABLE_INSERT_MODE, t.c_iflag, W_INSERT );
			win_to_termios( mode, ENABLE_MOUSE_INPUT, t.c_iflag, W_MOUSE );
			win_to_termios( mode, ENABLE_QUICK_EDIT_MODE, t.c_iflag, W_QUICK );
			win_to_termios( mode, ENABLE_WINDOW_INPUT, t.c_iflag, W_WININPUT );
			win_to_termios( mode, ENABLE_VIRTUAL_TERMINAL_INPUT, t.c_iflag, W_IVIRTTRM );
		} else {
			win_to_termios( mode, ENABLE_PROCESSED_OUTPUT, t.c_oflag, W_OPROCESS );
			win_to_termios( mode, ENABLE_WRAP_AT_EOL_OUTPUT, t.c_oflag, W_WRAP_EOL );
			win_to_termios( mode, ENABLE_VIRTUAL_TERMINAL_PROCESSING, t.c_oflag, W_OVIRTTRM );
			win_to_termios( mode, DISABLE_NEWLINE_AUTO_RETURN, t.c_oflag, W_DIS_NLCR );
			win_to_termios( mode, ENABLE_LVB_GRID_WORLDWIDE, t.c_oflag, W_LVB_GRID );
		}
		ret = 0;
		errno = 0;
	} else {
		errno = ENOSYS;
	}
	return ( ret );
}

int tcsetattr( int fd_, int, struct termios const* term_ ) {
	int ret( -1 );
	if ( ( fd_ >= STDIN_FILENO ) && ( fd_ <= STDOUT_FILENO ) ) {
		DWORD mode( 0 );
		termios const& t( *term_ );
		if ( fd_ == STDIN_FILENO ) {
			termios_to_win( t.c_lflag, ICANON, mode, ENABLE_LINE_INPUT );
			termios_to_win( t.c_lflag, ECHO, mode, ENABLE_ECHO_INPUT );
			termios_to_win( t.c_lflag, IEXTEN | ISIG, mode, ENABLE_PROCESSED_INPUT );

			termios_to_win( t.c_iflag, W_INSERT, mode, ENABLE_INSERT_MODE );
			termios_to_win( t.c_iflag, W_MOUSE, mode, ENABLE_MOUSE_INPUT );
			termios_to_win( t.c_iflag, W_QUICK, mode, ENABLE_QUICK_EDIT_MODE );
			termios_to_win( t.c_iflag, W_WININPUT, mode, ENABLE_WINDOW_INPUT );
			termios_to_win( t.c_iflag, W_IVIRTTRM, mode, ENABLE_VIRTUAL_TERMINAL_INPUT );
		} else {
			termios_to_win( t.c_oflag, W_OPROCESS, mode, ENABLE_PROCESSED_OUTPUT );
			termios_to_win( t.c_oflag, W_WRAP_EOL, mode, ENABLE_WRAP_AT_EOL_OUTPUT );
			termios_to_win( t.c_oflag, W_OVIRTTRM, mode, ENABLE_VIRTUAL_TERMINAL_PROCESSING );
			termios_to_win( t.c_oflag, W_DIS_NLCR, mode, DISABLE_NEWLINE_AUTO_RETURN );
			termios_to_win( t.c_oflag, W_LVB_GRID, mode, ENABLE_LVB_GRID_WORLDWIDE );
		}
		::SetConsoleMode( ::GetStdHandle( msvcxx::fd_to_handle( fd_ ) ), mode );
		ret = 0;
		errno = 0;
	} else {
		errno = ENOSYS;
	}
	return ( ret );
}

int cfsetospeed( struct termios*, speed_t ) {
	errno = ENOSYS;
	return ( -1 );
}

int cfsetispeed( struct termios*, speed_t ) {
	errno = ENOSYS;
	return ( -1 );
}

int tcflush( int, int ) {
	errno = ENOSYS;
	return ( -1 );
}

int tcdrain( int ) {
	errno = ENOSYS;
	return ( -1 );
}

int tcsendbreak( int, int ) {
	errno = ENOSYS;
	return ( -1 );
}

