/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcollector.c - this file is integral part of `stdhapi' project.

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

#include <sys/types.h> /* FD_* macros */
#include <string.h>
#include <stdlib.h>

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "../hcore/hlog.h"
#include "tools.h"
#include "hcollector.h"

HCollector::HCollector ( const char * a_pcDevicePath )
					: HSerial ( a_pcDevicePath ),
						f_oLine ( ( unsigned long int ) D_RECV_BUF_SIZE )
	{
	M_PROLOG
	memset ( f_pcReadBuf, 0, D_PROTO_RECV_BUF_SIZE );
	f_iLines = 0;
	return;
	M_EPILOG
	}

bool HCollector::test_char ( const char * a_pcBuffer, int a_iIndex )
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

int HCollector::send_line ( const char * a_pcLine )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iCRC = 0;
	int l_iError = -1;
	int l_iLength = strlen ( a_pcLine );
	char * l_pcSpeedUp = NULL;
	HString l_oLine, l_oLocalCopy;
	if ( l_iLength < 1 )return ( 0 );
	l_oLocalCopy = a_pcLine;
	l_pcSpeedUp = ( char * ) l_oLocalCopy;
	if ( a_pcLine [ l_iLength - 1 ] == '\n' )
		{
		l_iLength --;
		l_pcSpeedUp [ l_iLength ] = 0;
		}
	for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
		l_iCRC += l_pcSpeedUp [ l_iCtr ];
	l_oLine.format ( "%s%02x%02x%s\n", D_PROTO_DTA,
			l_iLength & 0x0ff, l_iCRC & 0x0ff, l_pcSpeedUp );
	memset ( f_pcReadBuf, 0, D_PROTO_RECV_BUF_SIZE );
	l_iLength += strlen ( D_PROTO_DTA );
	l_iLength += ( 2 /* for lenght */ + 2 /* for crc */ + 1 /* for newline */ );
	while ( strncmp ( f_pcReadBuf, D_PROTO_ACK, strlen ( D_PROTO_ACK ) ) )
		{
		flush ( TCOFLUSH );
		l_iCtr = write ( l_oLine, l_iLength );
		wait_for_eot ( );
		memset ( f_pcReadBuf, 0, D_PROTO_RECV_BUF_SIZE );
		read ( f_pcReadBuf, D_PROTO_RECV_BUF_SIZE );
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
	int l_iAckLenght = strlen ( D_PROTO_ACK );
	int l_iErrLenght = strlen ( D_PROTO_ERR );
	/* P prefix means sender transmission side data */
	while ( ( l_iPCRC != l_iCRC ) || ( l_iPLength != l_iLength ) )
		{
		memset ( f_oLine, 0, D_RECV_BUF_SIZE );
		read ( f_oLine, D_RECV_BUF_SIZE );
		flush ( TCIFLUSH );
		a_pcLine = ( ( char * ) f_oLine )
			+ strlen ( D_PROTO_DTA ) + 2 /* for lenght */ + 2 /* for crc */;
		l_iLength = strlen ( a_pcLine ) - 1;
		a_pcLine [ l_iLength ] = 0;
		for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
			l_iCRC += a_pcLine [ l_iCtr ];
		l_iLength &= 0x0ff;
		l_iCRC &= 0x0ff;
		memset ( f_pcReadBuf, 0, D_PROTO_RECV_BUF_SIZE );
		strncpy ( f_pcReadBuf, ( ( char * ) f_oLine ) + strlen ( D_PROTO_DTA ), 2 );
		l_iPLength = strtol ( f_pcReadBuf, NULL, 0x10 );
		memset ( f_pcReadBuf, 0, D_PROTO_RECV_BUF_SIZE );
		strncpy ( f_pcReadBuf, ( ( char * ) f_oLine )
				+ strlen ( D_PROTO_DTA ) + 2 /* for Plength */, 2 );
		l_iPCRC = strtol ( f_pcReadBuf, NULL, 0x10 );
		if ( ( l_iPCRC != l_iCRC ) || ( l_iPLength != l_iLength ) )
			l_iError += ( l_iErrLenght - write ( D_PROTO_ERR, l_iErrLenght ) );
		l_iError ++;
		}
	flush ( TCOFLUSH );
	l_iError += ( l_iAckLenght - write ( D_PROTO_ACK, l_iAckLenght ) );
	wait_for_eot ( );
	f_iLines ++;
	return ( l_iError );
	M_EPILOG
	}

#define M_STDHAPI_TEMP_FAILURE_RETRY(expression) \
  (__extension__ \
    ({ long int __result; \
       do __result = (long int) (expression); \
       while (__result == -1L && errno == EINTR); \
       __result; }))

int HCollector::establish_connection ( int a_iTimeOut )
	{
	M_PROLOG
/*
	 We have small problem here.
	 There are two ways that communication can start.
	 Ether waiting part runs before initializing part (the easy case),
	 or initializing part runs before waiting part (here comes the trouble).
*/
	int l_iLenght = strlen ( D_PROTO_SYN ), l_iError = -1;
	fd_set	l_xFileDesSet; /* serial port */
	timeval	l_sWait; /* sleep between re-selects */
	memset ( f_pcReadBuf, 0, D_PROTO_RECV_BUF_SIZE );
	while ( strncmp ( f_pcReadBuf, D_PROTO_ACK, strlen ( D_PROTO_ACK ) ) )
		{
		flush ( TCOFLUSH );
		if ( write ( D_PROTO_SYN, l_iLenght ) != l_iLenght )
			throw new HException ( __WHERE__, "write", l_iLenght );
		wait_for_eot ( );
		if ( tcsendbreak ( f_iFileDescriptor, 0 ) )
			throw new HException ( __WHERE__, "tcsendbreak", g_iErrNo );
		memset ( f_pcReadBuf, 0, D_PROTO_RECV_BUF_SIZE );
		l_sWait.tv_sec = 1;
		l_sWait.tv_usec = 0;
		FD_ZERO ( & l_xFileDesSet );
		FD_SET ( f_iFileDescriptor, & l_xFileDesSet );
		if ( ( M_STDHAPI_TEMP_FAILURE_RETRY ( select ( FD_SETSIZE,
							& l_xFileDesSet,	NULL, NULL, & l_sWait ) ) >= 0 )
				&& FD_ISSET ( f_iFileDescriptor, & l_xFileDesSet ) )
			read ( f_pcReadBuf, D_PROTO_RECV_BUF_SIZE );
		flush ( TCIFLUSH );
		l_iError ++;
		if ( l_iError > a_iTimeOut )return ( -1 );
		}
	::log ( D_LOG_DEBUG ) << "Collector: Connected ! (estab)" << endl;
	return ( l_iError );
	M_EPILOG
	}

int HCollector::wait_for_connection ( int a_iTimeOut )
	{
	M_PROLOG
	int l_iError = -1;
	int l_iLenght = strlen ( D_PROTO_ACK );
	fd_set	l_xFileDesSet; /* serial port */
	timeval	l_sWait; /* sleep between re-selects */
	memset ( f_pcReadBuf, 0, D_PROTO_RECV_BUF_SIZE );
	while ( strncmp ( f_pcReadBuf, D_PROTO_SYN, strlen ( D_PROTO_SYN ) ) )
		{
		l_sWait.tv_sec = a_iTimeOut;
		l_sWait.tv_usec = 0;
		FD_ZERO ( & l_xFileDesSet );
		FD_SET ( f_iFileDescriptor, & l_xFileDesSet );
		if ( ( M_STDHAPI_TEMP_FAILURE_RETRY ( select ( FD_SETSIZE,
							& l_xFileDesSet,	NULL, NULL, & l_sWait ) ) >= 0 )
				&& FD_ISSET ( f_iFileDescriptor, & l_xFileDesSet ) )
			read ( f_pcReadBuf, D_PROTO_RECV_BUF_SIZE ), l_iError ++;
		else return ( -1 );
		}
	l_iError += ( l_iLenght - write ( D_PROTO_ACK, l_iLenght ) );
	::log ( D_LOG_DEBUG ) << "Collector: Connected ! (wait)" << endl;
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
		if ( ! strncmp ( l_pcLine, D_PROTO_FIN, sizeof ( D_PROTO_FIN ) ) )
			break;
		process_line ( l_pcLine, f_iLines );
		}
	return ( l_iError );
	M_EPILOG
	}

