/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcollector.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cstdlib>
#include <termios.h>
#include <sys/types.h> /* FD_* macros */
#include <libintl.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hcore/hlog.h"
#include "tools.h"
#include "hcollector.h"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

char const * const HCollector::PROTOCOL::D_SYN = "SYN\n";
char const * const HCollector::PROTOCOL::D_ACK = "ACK\n";
char const * const HCollector::PROTOCOL::D_DTA = "DTA"; /* warrning! no endline */
char const * const HCollector::PROTOCOL::D_FIN = "FIN"; /* warrning! no endline, but \0 at end,
																																	so sizeof ( ) retruns 4 */
char const * const HCollector::PROTOCOL::D_ERR = "ERR\n";

int const HCollector::PROTOCOL::D_RECV_BUF_SIZE; /* 5 should be enought but you never know */
int const HCollector::PROTOCOL::D_BUF_SIZE;

char const * const n_pcError = _ ( "collector device not opened" );

HCollector::HCollector ( char const * a_pcDevicePath )
					: HSerial ( a_pcDevicePath ), f_iLines ( 0 ),
						f_oLine ( PROTOCOL::D_BUF_SIZE, true )
	{
	M_PROLOG
	memset ( f_pcReadBuf, 0, PROTOCOL::D_RECV_BUF_SIZE );
	set_flags ( HSerial::D_FLAGS_TEXT );
	return;
	M_EPILOG
	}

bool HCollector::test_char ( char const * a_pcBuffer, int a_iIndex ) const
	{
	return (
			a_pcBuffer [ a_iIndex ]
							&& ( ( ( a_pcBuffer [ a_iIndex ] >= '0' )
									&& ( a_pcBuffer [ a_iIndex ] <= '9' ) )
								|| ( ( a_pcBuffer [ a_iIndex ] >= 'A' )
									&& ( a_pcBuffer [ a_iIndex ] <= 'Z' ) )
								|| ( ( ( a_pcBuffer [ a_iIndex ] == 10 )
										|| ( a_pcBuffer [ a_iIndex ] == 13 )
										|| ( a_pcBuffer [ a_iIndex ] == '?' ) )
									&& a_iIndex ) )
							);
	}

int HCollector::send_line ( char const * a_pcLine )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCRC = 0;
	int l_iError = -1;
	int l_iLength = strlen ( a_pcLine );
	char * l_pcSpeedUp = NULL;
	HString l_oLine, l_oLocalCopy;
	if ( l_iLength < 1 )
		return ( 0 );
	l_oLocalCopy = a_pcLine;
	l_pcSpeedUp = l_oLocalCopy.raw ( );
	if ( a_pcLine [ l_iLength - 1 ] == '\n' )
		{
		l_iLength --;
		l_pcSpeedUp [ l_iLength ] = 0;
		}
	for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
		l_iCRC += l_pcSpeedUp [ l_iCtr ];
	l_oLine.format ( "%s%02x%02x%s\n", PROTOCOL::D_DTA,
			l_iLength & 0x0ff, l_iCRC & 0x0ff, l_pcSpeedUp );
	memset ( f_pcReadBuf, 0, PROTOCOL::D_RECV_BUF_SIZE );
	l_iLength += strlen ( PROTOCOL::D_DTA );
	l_iLength += ( 2 /* for lenght */ + 2 /* for crc */ + 1 /* for newline */ );
	while ( strncmp ( f_pcReadBuf, PROTOCOL::D_ACK, strlen ( PROTOCOL::D_ACK ) ) )
		{
		flush ( TCOFLUSH );
		l_iCtr = HRawFile::write ( l_oLine, l_iLength );
		wait_for_eot ( );
		memset ( f_pcReadBuf, 0, PROTOCOL::D_RECV_BUF_SIZE );
		HRawFile::read ( f_pcReadBuf, PROTOCOL::D_RECV_BUF_SIZE );
		flush ( TCIFLUSH );
		l_iError ++;
		}
	return ( l_iError + l_iLength - l_iCtr );
	M_EPILOG
	}

int HCollector::receive_line ( char * & a_pcLine )
	{
	M_PROLOG
	int l_iError = -1;
	int l_iCtr = 0;
	int l_iCRC = 0, l_iPCRC = -1;
	int l_iLength = 0, l_iPLength = -1;
	int l_iAckLenght = strlen ( PROTOCOL::D_ACK );
	int l_iErrLenght = strlen ( PROTOCOL::D_ERR );
	/* P prefix means sender transmission side data */
	while ( ( l_iPCRC != l_iCRC ) || ( l_iPLength != l_iLength ) )
		{
		memset ( f_oLine.raw ( ), 0, PROTOCOL::D_BUF_SIZE );
		HRawFile::read ( f_oLine.raw ( ), PROTOCOL::D_BUF_SIZE );
		flush ( TCIFLUSH );
		a_pcLine = f_oLine.raw ( )
			+ strlen ( PROTOCOL::D_DTA ) + 2 /* for lenght */ + 2 /* for crc */;
		l_iLength = strlen ( a_pcLine ) - 1;
		a_pcLine [ l_iLength ] = 0;
		for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
			l_iCRC += a_pcLine [ l_iCtr ];
		l_iLength &= 0x0ff;
		l_iCRC &= 0x0ff;
		memset ( f_pcReadBuf, 0, PROTOCOL::D_RECV_BUF_SIZE );
		strncpy ( f_pcReadBuf, static_cast < char const * const > ( f_oLine ) + strlen ( PROTOCOL::D_DTA ), 2 );
		l_iPLength = strtol ( f_pcReadBuf, NULL, 0x10 );
		memset ( f_pcReadBuf, 0, PROTOCOL::D_RECV_BUF_SIZE );
		strncpy ( f_pcReadBuf, static_cast < char const * const > ( f_oLine )
				+ strlen ( PROTOCOL::D_DTA ) + 2 /* for Plength */, 2 );
		l_iPCRC = strtol ( f_pcReadBuf, NULL, 0x10 );
		if ( ( l_iPCRC != l_iCRC ) || ( l_iPLength != l_iLength ) )
			l_iError += ( l_iErrLenght - HRawFile::write ( PROTOCOL::D_ERR, l_iErrLenght ) );
		l_iError ++;
		}
	flush ( TCOFLUSH );
	l_iError += ( l_iAckLenght - HRawFile::write ( PROTOCOL::D_ACK, l_iAckLenght ) );
	wait_for_eot ( );
	f_iLines ++;
	return ( l_iError );
	M_EPILOG
	}

int HCollector::establish_connection ( int a_iTimeOut )
	{
	M_PROLOG
/*
	 We have small problem here.
	 There are two ways that communication can start.
	 Either waiting part runs before initializing part (the easy case),
	 or initializing part runs before waiting part (here comes the trouble).
*/
	int l_iLenght = strlen ( PROTOCOL::D_SYN ), l_iError = -1;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, f_iFileDescriptor );
	memset ( f_pcReadBuf, 0, PROTOCOL::D_RECV_BUF_SIZE );
	while ( strncmp ( f_pcReadBuf, PROTOCOL::D_ACK, strlen ( PROTOCOL::D_ACK ) ) )
		{
		flush ( TCOFLUSH );
		if ( HRawFile::write ( PROTOCOL::D_SYN, l_iLenght ) != l_iLenght )
			M_THROW ( "write", l_iLenght );
		wait_for_eot ( );
		if ( tcsendbreak ( f_iFileDescriptor, 0 ) )
			M_THROW ( "tcsendbreak", errno );
		memset ( f_pcReadBuf, 0, PROTOCOL::D_RECV_BUF_SIZE );
		timed_read ( f_pcReadBuf, PROTOCOL::D_RECV_BUF_SIZE, 1 );
		flush ( TCIFLUSH );
		l_iError ++;
		if ( l_iError > a_iTimeOut )
			return ( -1 );
		}
	::log ( LOG_TYPE::D_DEBUG ) << "Collector: Connected ! (estab)" << endl;
	return ( l_iError );
	M_EPILOG
	}

int HCollector::wait_for_connection ( int a_iTimeOut )
	{
	M_PROLOG
	int l_iError = - 1;
	int l_iLenght = strlen ( PROTOCOL::D_ACK );
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, f_iFileDescriptor );
	memset ( f_pcReadBuf, 0, PROTOCOL::D_RECV_BUF_SIZE );
	while ( strncmp ( f_pcReadBuf, PROTOCOL::D_SYN, strlen ( PROTOCOL::D_SYN ) ) )
		{
		if ( timed_read ( f_pcReadBuf, PROTOCOL::D_RECV_BUF_SIZE, a_iTimeOut ) >= 0 )
			l_iError ++;
		else
			return ( -1 );
		}
	l_iError += ( l_iLenght - HRawFile::write ( PROTOCOL::D_ACK, l_iLenght ) );
	::log ( LOG_TYPE::D_DEBUG ) << "Collector: Connected ! (wait)" << endl;
	return ( l_iError );
	M_EPILOG
	}

int HCollector::read_collector ( void ( * process_line ) ( char *, int ) ) 
	{
	M_PROLOG
	int l_iError = 0;
	char * l_pcLine = NULL;
	f_iLines = 0;
	l_iError = wait_for_connection ( tools::n_iCollectorConnectionTimeOut );
	while ( l_iError >= 0 )
		{
		l_iError += receive_line ( l_pcLine );
		/* '\n' is stripped from each line so we need to FIN treat special */
		if ( ! strncmp ( l_pcLine, PROTOCOL::D_FIN, sizeof ( PROTOCOL::D_FIN ) ) )
			break;
		process_line ( l_pcLine, f_iLines );
		}
	return ( l_iError );
	M_EPILOG
	}

}

}

