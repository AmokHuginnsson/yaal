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

#include "../config.h"

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hserial.h"
#include "signals.h"

HSerial::HSerial ( const char * a_pcDevice )
	{
	M_PROLOG
	f_iFileDes = 0;
	memset ( & f_sTIO, 0, sizeof ( termios ) );
	if ( a_pcDevice )f_oDevicePath = a_pcDevice;
	else f_oDevicePath = g_pcSerialDevice;
/*
 *   BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
 *   CRTSCTS : output hardware flow control  ( only used if the cable has
 *             all necessary lines. See sect. 7 of Serial-HOWTO)
 *             CS8     : 8n1  ( 8bit, no parity, 1 stopbit)
 *             CLOCAL  : local connection, no modem contol
 *             CREAD   : enable receiving characters
 */
	f_sTIO.c_cflag = g_iBaudRate | CRTSCTS | CS8 | CREAD/* | CLOCAL */;
/*
 *   statement above is *FALSE*, I can not use cfsetispeed and cfsetospeed,
 *   i *MUST* use it. On newer systes c_cflag and BAUDRATE simply does not work.
 *   Newwer interface for setting speed (baudrate)
 */
	cfsetispeed ( & f_sTIO, g_iBaudRate );
	cfsetospeed ( & f_sTIO, g_iBaudRate );
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
	f_sTIO.c_lflag = ICANON | IEXTEN;
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
#ifndef __HOST_OS_TYPE_FREEBSD__
	f_sTIO.c_cc [ VSWTC ]    = 0;    /* '\0' */
#endif /* not __HOST_OS_TYPE_FREEBSD__ */
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
	if ( f_iFileDes )close ( );
	f_iFileDes = 0;
	return;
	M_EPILOG
	}

bool HSerial::open ( void )
	{
	M_PROLOG
	if ( f_iFileDes )
		throw new HException ( __WHERE__, "serial port already openend", g_iErrNo );
	/* O_NONBLOCK allow open device even if nothing seats on other side */
	f_iFileDes = ::open ( f_oDevicePath, O_RDWR | O_NOCTTY | O_NONBLOCK );
	if ( ! f_iFileDes )
		throw new HException ( __WHERE__, strerror ( g_iErrNo ), g_iErrNo );
	if ( ! isatty ( f_iFileDes ) )
		throw new HException ( __WHERE__, "not a tty", f_iFileDes );
	tcgetattr ( f_iFileDes, & f_sBackUpTIO );
	fcntl ( f_iFileDes, F_SETFD, 0 );
	fcntl ( f_iFileDes, F_SETFL, 0 );
	tcflush ( f_iFileDes, TCIOFLUSH );
	tcsetattr ( f_iFileDes, TCSANOW, & f_sTIO );
	return ( false );
	M_EPILOG
	}

void HSerial::close ( void )
	{
	M_PROLOG
	if ( ! f_iFileDes )
		throw new HException ( __WHERE__, "serial port not opened", g_iErrNo );
	tcsetattr ( f_iFileDes, TCSANOW, & f_sBackUpTIO );
	::close ( f_iFileDes );
	f_iFileDes = 0;
	return;
	M_EPILOG
	}

int HSerial::read ( char * a_pcBuffer, int a_iSize )
	{
	M_PROLOG
	int l_iCnt = 0;
	if ( ! f_iFileDes )
		throw new HException ( __WHERE__, "serial port not opened", g_iErrNo );
	l_iCnt = ::read ( f_iFileDes, a_pcBuffer, a_iSize );
	return ( l_iCnt );
	M_EPILOG
	}

int HSerial::write ( const char * a_pcBuffer, int a_iSize )
	{
	M_PROLOG
	int l_iCnt = 0;
	if ( ! f_iFileDes )
		throw new HException ( __WHERE__, "serial port not opened", g_iErrNo );
	l_iCnt = ::write ( f_iFileDes, a_pcBuffer, a_iSize );
	return ( l_iCnt );
	M_EPILOG
	}

