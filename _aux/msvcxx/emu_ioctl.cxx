#include "../msvcxx/unistd.h"
#include <windows.h>
#include <cerrno>
#include "../msvcxx/sys/ioctl.h"
#include "../msvcxx/msio.hxx"

namespace msvcxx {

int ioctl_TIOCGWINSZ( int fd_, void* param_ ) {
	int val( 0 );
	if ( ( fd_ == STDIN_FILENO ) || ( fd_ == STDOUT_FILENO ) || ( fd_ == STDERR_FILENO ) ) {
		winsize* w( static_cast<winsize*>( param_ ) );
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		::GetConsoleScreenBufferInfo( ::GetStdHandle( fd_to_handle( fd_ ) ), &csbi );
		w->ws_col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		w->ws_row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	} else {
		val = EINVAL;
	}
	return ( 0 );
}
int ioctl( int fd_, int request_, void* param_ ) {
	int val( ENOSYS );
	switch ( request_ ) {
		case ( TIOCGWINSZ ): {
			val = ioctl_TIOCGWINSZ( fd_, param_ );
		} break;
	}
	return val;
}

}

