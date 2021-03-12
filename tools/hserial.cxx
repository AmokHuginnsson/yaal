/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h> /* timeval */
#include <libintl.h>

static int const ECHO_VAL = ECHO;
#undef ECHO

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hserial.hxx"
#include "tools.hxx"

#include "hcore/hlog.hxx"

using namespace yaal::hcore;
using namespace yaal::hcore::system;

namespace yaal {

namespace tools {

namespace {
	char const* const _eAlreadyOpened_ = _( "serial port already opened" );
	char const* const _eNotOpened_ = _( "serial port not opened" );
	HSerial::flag_t drop = HSerial::flag_t::new_flag();
}

HSerial::flag_t const HSerial::FLAG::DEFAULT = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::FLOW_CONTROL_HARDWARE = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::FLOW_CONTROL_SOFTWARE = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::STOP_BITS_1 = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::STOP_BITS_2 = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::PARITY_CHECK = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::PARITY_ODD = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::BITS_PER_BYTE_8 = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::BITS_PER_BYTE_7 = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::BITS_PER_BYTE_6 = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::BITS_PER_BYTE_5 = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::CANONICAL = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::ECHO = HSerial::flag_t::new_flag();
HSerial::flag_t const HSerial::FLAG::CR2NL = HSerial::flag_t::new_flag();

HSerial::flag_t HSerial::FLAG_TEXT = HSerial::FLAG::DEFAULT | HSerial::FLAG::CANONICAL | HSerial::FLAG::CR2NL;

HSerial::HSerial( HString const& devicePath_ )
	: HRawFile()
	, _speed( SPEED::DEFAULT )
	, _flags( FLAG::DEFAULT )
	, _devicePath()
	, _tIO( chunk_size<termios>( 1 ) )
	, _backUpTIO( chunk_size<termios>( 1 ) ) {
	M_PROLOG
	if ( !! devicePath_ ) {
		_devicePath = devicePath_;
	} else {
		_devicePath = tools::_serialDevice_;
	}
	compile();
	return;
	M_EPILOG
}

HSerial::~HSerial( void ) {
	M_PROLOG
	if ( _fileDescriptor >= 0 ) {
		close();
	}
	M_ASSERT ( _fileDescriptor < 0 );
	return;
	M_DESTRUCTOR_EPILOG
}

void HSerial::open( void ) {
	M_PROLOG
	if ( _fileDescriptor >= 0 ) {
		M_THROW( _eAlreadyOpened_, errno );
	}
	compile();
	/* O_NONBLOCK allow open device even if nothing seats on other side */
	HUTF8String utf8( _devicePath );
	_fileDescriptor = ::open( utf8.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
	M_ENSURE( _fileDescriptor >= 0, _devicePath );
	if ( ! ::isatty( _fileDescriptor ) ) {
		M_THROW( HString( _devicePath ).append( _( " is not a tty device" ) ), _fileDescriptor );
	}
	M_ENSURE( ::tcgetattr( _fileDescriptor, _backUpTIO.get<termios>() ) == 0 );
	M_ENSURE( ::fcntl( _fileDescriptor, F_SETFD, 0 ) == 0 );
	M_ENSURE( ::fcntl( _fileDescriptor, F_SETFL, 0 ) == 0 );
	M_ENSURE( ::tcflush( _fileDescriptor, TCIOFLUSH ) == 0 );
	M_ENSURE( ::tcsetattr( _fileDescriptor, TCSANOW, _tIO.get<termios const>() ) == 0 );
	return;
	M_EPILOG
}

int HSerial::do_close( void ) {
	M_PROLOG
	if ( _fileDescriptor >= 0 ) {
		::tcsetattr( _fileDescriptor, TCSANOW, _backUpTIO.get<termios const>() );
	}
	return ( HRawFile::do_close() );
	M_EPILOG
}

void HSerial::compile( void ) {
	M_PROLOG
	termios& tIO = *_tIO.get<termios>();
	::memset( &tIO, 0, sizeof ( termios ) );
	::memset( _backUpTIO.get<void>(), 0, sizeof ( termios ) );
/*
 *   initialize all control characters
 *   default values can be found in /usr/include/termios.h, and are given
 *   in the comments, but we don't need them here
 */
	tIO.c_cc [ VINTR ]    = 0;    /* Ctrl-c */
	tIO.c_cc [ VQUIT ]    = 0;    /* Ctrl-\ */
	tIO.c_cc [ VERASE ]   = 0;    /* del */
	tIO.c_cc [ VKILL ]    = 0;    /* @ */
	tIO.c_cc [ VEOF ]     = 4;    /* Ctrl-d */
	tIO.c_cc [ VTIME ]    = 0;    /* inter-character timer unused */
	tIO.c_cc [ VMIN ]     = 1;    /* blocking read until 1 character arrives */
#if HAVE_DECL_VSWTC
	tIO.c_cc [ VSWTC ]    = 0;    /* '\0' */
#endif /* HAVE_DECL_VSWTC */
	tIO.c_cc [ VSTART ]   = 0;    /* Ctrl-q */
	tIO.c_cc [ VSTOP ]    = 0;    /* Ctrl-s */
	tIO.c_cc [ VSUSP ]    = 0;    /* Ctrl-z */
	tIO.c_cc [ VEOL ]     = 0;    /* '\0' */
	tIO.c_cc [ VREPRINT ] = 0;    /* Ctrl-r */
	tIO.c_cc [ VDISCARD ] = 0;    /* Ctrl-u */
	tIO.c_cc [ VWERASE ]  = 0;    /* Ctrl-w */
	tIO.c_cc [ VLNEXT ]   = 0;    /* Ctrl-v */
	tIO.c_cc [ VEOL2 ]    = 0;    /* '\0' */
	compile_flags();
	compile_speed();
	return;
	M_EPILOG
}

void HSerial::set_speed( SPEED speed_ ) {
	M_PROLOG
	_speed = speed_;
	return;
	M_EPILOG
}

void HSerial::compile_speed( void ) {
	M_PROLOG
	if ( _fileDescriptor >= 0 ) {
		M_THROW( _eAlreadyOpened_, errno );
	}
	termios& tIO = *_tIO.get<termios>();
	int baudRate = 0;
	if ( _speed == SPEED::DEFAULT ) {
		_speed = tools::_baudRate_;
	}
	switch ( _speed ) {
		case ( SPEED::B_230400 ): baudRate = B230400; break;
		case ( SPEED::B_115200 ): baudRate = B115200; break;
#if defined( HAVE_DECL_B76800 ) && ( HAVE_DECL_B76800 == 1 )
		case ( SPEED::B_76800 ):  baudRate = B76800;  break;
#endif /* HAVE_DECL_B76800 */
		case ( SPEED::B_57600 ):  baudRate = B57600;  break;
		case ( SPEED::B_38400 ):  baudRate = B38400;  break;
#if defined( HAVE_DECL_B28800 ) && ( HAVE_DECL_B28800 == 1 )
		case ( SPEED::B_28800 ):  baudRate = B28800;  break;
#endif /* HAVE_DECL_B28800 */
		case ( SPEED::B_19200 ):  baudRate = B19200;  break;
#if defined( HAVE_DECL_B14400 ) && ( HAVE_DECL_B14400 == 1 )
		case ( SPEED::B_14400 ):  baudRate = B14400;  break;
#endif /* HAVE_DECL_B14400 */
		case ( SPEED::B_9600 ):   baudRate = B9600;   break;
#if defined( HAVE_DECL_B7200 ) && ( HAVE_DECL_B7200 == 1 )
		case ( SPEED::B_7200 ):   baudRate = B7200;   break;
#endif /* HAVE_DECL_B7200 */
		case ( SPEED::B_4800 ):   baudRate = B4800;   break;
		case ( SPEED::B_2400 ):   baudRate = B2400;   break;
		case ( SPEED::DEFAULT ): break;
		default : {
			M_THROW( _( "unknown speed" ), _speed );
			break;
		}
	}
	cfsetispeed( &tIO, static_cast< ::speed_t>( baudRate ) );
	cfsetospeed( &tIO, static_cast< ::speed_t>( baudRate ) );
	return;
	M_EPILOG
}

void HSerial::set_flags( flag_t flags_ ) {
	M_PROLOG
	_flags = flags_;
	return;
	M_EPILOG
}

void HSerial::compile_flags( void ) {
	M_PROLOG
	if ( _fileDescriptor >= 0 ) {
		M_THROW( _eAlreadyOpened_, errno );
	}
	termios& tIO = *_tIO.get<termios>();
	if ( !!( _flags & FLAG::DEFAULT ) ) {
		_flags |= tools::_serialFlags_;
	}
	/* consistency tests */
	if ( ( !!( _flags & FLAG::STOP_BITS_1 ) ) && ( !!( _flags & FLAG::STOP_BITS_2 ) ) ) {
		M_THROW( _( "stop bits setup inconsistent" ), _flags.value() );
	}
	if ( ( !!( _flags & FLAG::FLOW_CONTROL_HARDWARE ) ) && ( !!( _flags & FLAG::FLOW_CONTROL_SOFTWARE ) ) ) {
		M_THROW( _( "flow control inconsistent" ), _flags.value() );
	}
	int ctr( 0 );
	if ( !!( _flags & FLAG::BITS_PER_BYTE_8 ) ) {
		++ ctr;
		tIO.c_cflag = CS8;
	}
	if ( !!( _flags & FLAG::BITS_PER_BYTE_7 ) ) {
		++ ctr;
		tIO.c_cflag = CS7;
	}
	if ( !!( _flags & FLAG::BITS_PER_BYTE_6 ) ) {
		++ ctr;
		tIO.c_cflag = CS6;
	}
	if ( !!( _flags & FLAG::BITS_PER_BYTE_5 ) ) {
		++ ctr;
		tIO.c_cflag = CS5;
	}
	if ( ctr != 1 ) {
		M_THROW( to_string( _( "bits per byte inconsistent: " ) ).append( _flags.value() ), ctr );
	}

	/* compiling settings */
	/* setting c_cflag */
/*
 *   BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
 *   CRTSCTS : output hardware flow control (only used if the cable has
 *             all necessary lines. See sect. 7 of Serial-HOWTO)
 *             CS8     : 8n1 (8bit, no parity, 1 stopbit)
 *             CLOCAL  : local connection, no modem control
 *             CREAD   : enable receiving characters
 */
/*
 *   statement above is *FALSE*, I cannot use cfsetispeed and cfsetospeed,
 *   i *MUST* use it. On newer systems c_cflag and BAUDRATE simply does not work.
 *   Newer interface for setting speed (baudrate)
 */
	tIO.c_cflag |= CSIZE | CREAD /* | CLOCAL */;
	if ( !!( _flags & FLAG::FLOW_CONTROL_HARDWARE ) ) {
		tIO.c_cflag |= CRTSCTS;
	}

	/* setting c_iflag */
/*
 *   IGNPAR  : ignore bytes with parity errors
 *   ICRNL   : map CR to NL (otherwise a CR input on the other computer
 *             will not terminate input)
 *             otherwise make device raw (no other input processing)
 *   IXANY   : (not in POSIX.1; XSI) Enable any character to restart output.
 *   IGNBRK  : Ignore BREAK condition on input.
 *   INPCK   : Enable input parity checking.
 */
	tIO.c_iflag = IGNPAR | IGNBRK | IXANY;
	if ( !!( _flags & FLAG::CR2NL ) ) {
		tIO.c_iflag |= ICRNL;
	}
	if ( !!( _flags & FLAG::FLOW_CONTROL_SOFTWARE ) ) {
		tIO.c_iflag |= IXON | IXOFF;
	}
	if ( !!( _flags & FLAG::PARITY_CHECK ) ) {
		tIO.c_iflag |= INPCK;
	}

	/* setting c_oflag */
/*
 *  Raw output.
 *  CSTOPB  : Set two stop bits, rather than one.
 *  PARENB  : Enable parity generation on output and parity checking for input.
 *  PARODD  : Parity for input and output is odd.
 */
	tIO.c_oflag = 0;
	if ( !!( _flags & FLAG::STOP_BITS_2 ) ) {
		tIO.c_oflag |= CSTOPB;
	}
	if ( !!( _flags & FLAG::PARITY_CHECK ) ) {
		tIO.c_oflag |= PARENB;
	}
	if ( !!( _flags & FLAG::PARITY_ODD ) ) {
		tIO.c_oflag |= PARODD;
	}

/*
 *   ICANON  : enable canonical input disable all echo functionality,
 *             and don't send signals to calling program
 *             more over: return from read() only after EOL
 *   IEXTEN  : Enable implementation-defined input processing.
 *             This flag, as well as ICANON must be enabled for the special
 *             characters EOL2,  LNEXT,  REPRINT,  WERASE to be interpreted,
 *             and for the IUCLC flag to be effective.
 *   ECHO    : Echo input characters.
 */
	tIO.c_lflag = IEXTEN;
	if ( !!( _flags & FLAG::CANONICAL ) ) {
		tIO.c_lflag |= ICANON;
	}
	if ( !!( _flags & FLAG::ECHO ) ) {
		tIO.c_lflag = tIO.c_iflag | ECHO_VAL;
	}
	return;
	M_EPILOG
}

void HSerial::flush( int type_ ) {
	M_PROLOG
	HString errMsg;
	if ( _fileDescriptor < 0 )
		M_THROW( _eNotOpened_, errno );
	if ( tcflush( _fileDescriptor, type_ ) ) {
		switch ( type_ ) {
			case ( TCIFLUSH ):
				M_THROW( "tcflush ( TCIFLUSH )", errno );
			break;
			case ( TCOFLUSH ):
				M_THROW( "tcflush ( TCOFLUSH )", errno );
			break;
			case ( TCIOFLUSH ):
				M_THROW( "tcflush ( TCIOFLUSH )", errno );
			break;
			default : {
				M_THROW( "tcflush( "_ys.append( type_ ).append( " )" ), errno );
			}
			break;
		}
	}
	return;
	M_EPILOG
}

void HSerial::wait_for_eot( void ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _eNotOpened_, errno );
	}
	if ( tcdrain( _fileDescriptor ) ) {
		M_THROW( "tcdrain", errno );
	}
	return;
	M_EPILOG
}

int HSerial::timed_read( void* buffer_, int const size_, int timeOut_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _eNotOpened_, errno );
	}
	int nRead( -1 );
	if ( system::wait_for_io( _fileDescriptor, IO_EVENT_TYPE::READ, timeOut_ ) == IO_EVENT_TYPE::READ ) {
		nRead = static_cast<int>( HRawFile::read( buffer_, size_ ) );
	}
	return nRead;
	M_EPILOG
}

}

}

