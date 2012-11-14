#include "../msvcxx/sys/cdefs.h"
#include <windows.h>
#include <cerrno>
#include "../msvcxx/sys/ioctl.h"

namespace msvcxx {
	int ioctl_TIOCGWINSZ( int fd_, void* param_ ) {
		int val( 0 );
		if ( fd_ == 0 ) {
			winsize* w( static_cast<winsize*>( param_ ) );
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			::GetConsoleScreenBufferInfo( ::GetStdHandle( STD_OUTPUT_HANDLE ), &csbi );
			w->ws_col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
			w->ws_row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		} else
			val = EINVAL;
		return ( 0 );
	}
	int ioctl( int fd_, int request_, void* param_ ) {
		int val( ENOSYS );
		switch ( request_ ) {
			case ( TIOCGWINSZ ): {
				val = ioctl_TIOCGWINSZ( fd_, param_ );
			} break;
		}
		return ( val );
	}
}

