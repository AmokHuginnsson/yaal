/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hserial.c - this file is integral part of `stdhapi' project.

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

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <libintl.h>

#include "config.h"

#include "hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hserial.h"
#include "tools.h"

using namespace stdhapi::hcore;

namespace stdhapi
{

namespace tools
{

char const * const n_pcError = _ ( "serial port not opened" );

HSerial::HSerial ( char const * a_pcDevice, bool a_bCanonical )
				: HRawFile ( ), f_oDevicePath ( ), f_sTIO ( ), f_sBackUpTIO ( )
	{
	M_PROLOG
	memset ( & f_sTIO, 0, sizeof ( termios ) );
	memset ( & f_sBackUpTIO, 0, sizeof ( termios ) );
	if ( a_pcDevice )
		f_oDevicePath = a_pcDevice;
	else
		f_oDevicePath = tools::n_pcSerialDevice;
/*
 *   BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
 *   CRTSCTS : output hardware flow control  ( only used if the cable has
 *             all necessary lines. See sect. 7 of Serial-HOWTO)
 *             CS8     : 8n1  ( 8bit, no parity, 1 stopbit)
 *             CLOCAL  : local connection, no modem contol
 *             CREAD   : enable receiving characters
 */
	f_sTIO.c_cflag = tools::n_iBaudRate | CRTSCTS | CS8 | CREAD/* | CLOCAL */;
/*
 *   statement above is *FALSE*, I can not use cfsetispeed and cfsetospeed,
 *   i *MUST* use it. On newer systes c_cflag and BAUDRATE simply does not work.
 *   Newwer interface for setting speed (baudrate)
 */
	cfsetispeed ( & f_sTIO, tools::n_iBaudRate );
	cfsetospeed ( & f_sTIO, tools::n_iBaudRate );
/*
 *   IGNPAR  : ignore bytes with parity errors
 *   ICRNL   : map CR to NL  ( otherwise a CR input on the other computer
 *             will not terminate input)
 *             otherwise make device raw  ( no other input processing)
 */
	f_sTIO.c_iflag = IGNPAR | ICRNL | IGNBRK | IXANY;
/*
 *  Raw output.
 */
	f_sTIO.c_oflag = 0;
/*
 *   ICANON  : enable canonical input disable all echo functionality,
 *             and don't send signals to calling program
 */
	f_sTIO.c_lflag = ( a_bCanonical ? ICANON : 0 ) | IEXTEN;
/*
 *   initialize all control characters
 *   default values can be found in /usr/include/termios.h,  and are given
 *   in the comments,  but we don't need them here
 */
	f_sTIO.c_cc [ VINTR ]    = 0;    /* Ctrl-c */
	f_sTIO.c_cc [ VQUIT ]    = 0;    /* Ctrl-\ */
	f_sTIO.c_cc [ VERASE ]   = 0;    /* del */
	f_sTIO.c_cc [ VKILL ]    = 0;    /* @ */
	f_sTIO.c_cc [ VEOF ]     = 4;    /* Ctrl-d */
	f_sTIO.c_cc [ VTIME ]    = 0;    /* inter-character timer unused */
	f_sTIO.c_cc [ VMIN ]     = 1;    /* blocking read until 1 character arrives */
#if HAVE_DECL_VSWTC
	f_sTIO.c_cc [ VSWTC ]    = 0;    /* '\0' */
#endif /* HAVE_DECL_VSWTC */
	f_sTIO.c_cc [ VSTART ]   = 0;    /* Ctrl-q */
	f_sTIO.c_cc [ VSTOP ]    = 0;    /* Ctrl-s */
	f_sTIO.c_cc [ VSUSP ]    = 0;    /* Ctrl-z */
	f_sTIO.c_cc [ VEOL ]     = 0;    /* '\0' */
	f_sTIO.c_cc [ VREPRINT ] = 0;    /* Ctrl-r */
	f_sTIO.c_cc [ VDISCARD ] = 0;    /* Ctrl-u */
	f_sTIO.c_cc [ VWERASE ]  = 0;    /* Ctrl-w */
	f_sTIO.c_cc [ VLNEXT ]   = 0;    /* Ctrl-v */
	f_sTIO.c_cc [ VEOL2 ]    = 0;    /* '\0' */
	return;
	M_EPILOG
	}

HSerial::~HSerial ( void )
	{
	M_PROLOG
	if ( f_iFileDescriptor >= 0 )
		HSerial::close ( );
	M_ASSERT ( f_iFileDescriptor < 0 );
	return;
	M_EPILOG
	}

bool HSerial::open ( void )
	{
	M_PROLOG
	if ( f_iFileDescriptor >= 0 )
		M_THROW ( "serial port already openend", g_iErrNo );
	/* O_NONBLOCK allow open device even if nothing seats on other side */
	f_iFileDescriptor = ::open ( f_oDevicePath, O_RDWR | O_NOCTTY | O_NONBLOCK );
	if ( ! f_iFileDescriptor )
		M_THROW ( strerror ( g_iErrNo ), g_iErrNo );
	if ( ! isatty ( f_iFileDescriptor ) )
		M_THROW ( "not a tty", f_iFileDescriptor );
	tcgetattr ( f_iFileDescriptor, & f_sBackUpTIO );
	fcntl ( f_iFileDescriptor, F_SETFD, 0 );
	fcntl ( f_iFileDescriptor, F_SETFL, 0 );
	tcflush ( f_iFileDescriptor, TCIOFLUSH );
	tcsetattr ( f_iFileDescriptor, TCSANOW, & f_sTIO );
	return ( false );
	M_EPILOG
	}

int HSerial::close ( void )
	{
	M_PROLOG
	if ( f_iFileDescriptor >= 0 )
		tcsetattr ( f_iFileDescriptor, TCSANOW, & f_sBackUpTIO );
	return ( HRawFile::close ( ) );
	M_EPILOG
	}

void HSerial::flush ( int a_iType )
	{
	M_PROLOG
	HString l_oErrMsg;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, g_iErrNo );
	if ( tcflush ( f_iFileDescriptor, a_iType ) )
		switch ( a_iType )
			{
			case ( TCIFLUSH ):
				M_THROW ( "tcflush ( TCIFLUSH )", g_iErrNo );
			case ( TCOFLUSH ):
				M_THROW ( "tcflush ( TCOFLUSH )", g_iErrNo );
			case ( TCIOFLUSH ):
				M_THROW ( "tcflush ( TCIOFLUSH )", g_iErrNo );
			default :
				{
				l_oErrMsg.format ( "tcflush ( %d )", a_iType );
				M_THROW ( l_oErrMsg, g_iErrNo );
				}
			}
	return;
	M_EPILOG
	}

void HSerial::wait_for_eot ( void )
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, g_iErrNo );
	if ( tcdrain ( f_iFileDescriptor ) )
		M_THROW ( "tcdrain", g_iErrNo );
	return;
	M_EPILOG
	}

int HSerial::read ( char * const a_pcBuffer, int const a_iSize,
		int const a_iTimeOutSec, int const a_iTimeOutUsec )
	{
	M_PROLOG
	int l_iError = 0;
	timeval l_xWait;
	fd_set l_xFdSet;
	l_xWait.tv_sec = a_iTimeOutSec;
	l_xWait.tv_usec = a_iTimeOutUsec;
	FD_ZERO ( & l_xFdSet );
	FD_SET ( f_iFileDescriptor, & l_xFdSet );
	l_iError = select ( FD_SETSIZE, & l_xFdSet, NULL, NULL, & l_xWait );
	if ( ( l_iError > 0 ) && FD_ISSET ( f_iFileDescriptor, & l_xFdSet ) )
		return ( HRawFile::read ( a_pcBuffer, a_iSize ) );
	return ( - 1 );
	M_EPILOG
	}

}

}

