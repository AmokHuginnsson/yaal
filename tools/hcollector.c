/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	collector.c - this file is integral part of `stdhapi' project.

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

#include <string.h>
#include <stdlib.h>

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "../hcore/hlog.h"
#include "collector.h"

namespace collector
{

bool test_char ( const char * a_pcBuffer, int a_iIndex )
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

int send ( HSerial & a_roPort, char * a_pcBuffer, int a_iLength )
	{
	int l_iCnt = 0;
	char l_cCRC = 0;
	char l_pcReadBuf [ 32 ];
	for ( l_iCnt = 0; l_iCnt < a_iLength; l_iCnt++ )
		l_cCRC += a_pcBuffer [ l_iCnt ];
	for ( l_iCnt = 0; l_iCnt < 32; l_iCnt++ )l_pcReadBuf [ l_iCnt ] = 0;
	l_iCnt = 0;
	while ( 1 )
		{
		a_roPort.write ( a_pcBuffer, a_iLength );
		a_roPort.read ( l_pcReadBuf, 1 );
		if ( l_pcReadBuf [ 0 ] == l_cCRC )break;
		l_iCnt ++;
		}
	a_roPort.write ( "OK\r\n", 4 );
	return ( l_iCnt );
	}

int receive ( HSerial & a_roPort, char * const a_pcBuffer )
	{
	int l_iErr = 0;
	int l_iLen = 0;
	int l_iCnt = 0;
	char l_cCRC = 0;
	char l_pcReadBuf [ 64 ];
	char l_pcBuffer [ 64 ];
	do
		{
		memset ( l_pcReadBuf, 0, 64 );
		memset ( l_pcBuffer, 0, 64 );
		l_iCnt = 0;
		l_iLen = 0;
		while ( l_iCnt < 64 )
			{
			if( a_roPort.read ( l_pcReadBuf + l_iCnt, 1 ) )
				if ( test_char ( l_pcReadBuf, l_iCnt ) )l_iCnt ++;
			if ( l_iCnt && ( l_pcReadBuf [ l_iCnt - 1 ] == '\n' ) )break;
/*			if ( console::kbhit ( ) == 'q' )return ( -1 ); */ /* FIXME */
			}
		do
			{
			l_iLen = l_iCnt;
			l_cCRC = 0;
			for ( l_iCnt = 0; l_iCnt < 64; l_iCnt++ )l_cCRC += l_pcReadBuf [ l_iCnt ];
			l_pcBuffer [ 0 ] = l_cCRC;
			a_roPort.write ( l_pcBuffer, 1 );
			l_iCnt = 0;
			l_iLen = 0;
			memset ( l_pcBuffer, 0, 64 );
			while ( l_iCnt < 64 )
				{
				if( a_roPort.read ( l_pcBuffer + l_iCnt, 1 ) )
					if ( test_char ( l_pcReadBuf, l_iCnt ) )l_iCnt ++;
				if ( l_iCnt && ( l_pcBuffer [ l_iCnt - 1 ] == '\n' ) )break;
/*				if ( console::kbhit ( ) == 'q' )return ( -1 ); */ /* FIXME */
				}
			if ( strncmp ( l_pcBuffer, "OK\r\n", 4 ) )
				{
				memset ( l_pcReadBuf, 0, 64 );
				strncpy ( l_pcReadBuf, l_pcBuffer, 64 );
				l_iErr ++;
				}
			}
		while ( strncmp ( l_pcBuffer, "OK\r\n", 4 ) );
		}
	while ( strncmp ( l_pcBuffer, "OK\r\n", 4 ) );
	l_iLen = strlen ( l_pcReadBuf );
	strncpy ( a_pcBuffer, l_pcReadBuf, l_iLen );
	return ( l_iErr );
	}

int establish_connection ( HSerial & a_roPort )
	{
	int l_iErr = 0;
	int l_iCnt = 0;
	char l_pcReadBuf [ 8 ];
	memset ( l_pcReadBuf, 0, 8 );
	while ( 1 )
		{
		strncpy ( l_pcReadBuf, "KO?\r\n", 5 );
		for ( l_iCnt = 0; l_iCnt < 5; l_iCnt ++ )
			a_roPort.write ( l_pcReadBuf + l_iCnt, 1 );
		memset ( l_pcReadBuf, 0, 8 );
		l_iCnt = 0;
		while ( l_iCnt < 64 )
			{
			if ( a_roPort.read ( l_pcReadBuf + l_iCnt, 1 ) )
				if ( test_char ( l_pcReadBuf, l_iCnt ) )l_iCnt ++;
			if ( l_iCnt && ( l_pcReadBuf [ l_iCnt - 1 ] == '\n' ) )break;
/*			if ( console::kbhit ( ) == 'q' )return ( -1 ); */ /* FIXME */
			}
		if ( ! strncmp ( l_pcReadBuf, "OK\r\n", 4 ) )break;
		l_iErr ++;
		}
	return ( l_iErr );
	}

int wait_for_connection ( HSerial & a_roPort )
	{
	int l_iErr = 0;
	int l_iCnt = 0;
	char l_pcReadBuf [ 8 ];
	memset ( l_pcReadBuf, 0, 8 );
	while ( 1 )
		{
		l_iCnt = 0;
		while ( l_iCnt < 64 )
			{
			if ( a_roPort.read ( l_pcReadBuf + l_iCnt, 1 ) )
				if ( test_char ( l_pcReadBuf, l_iCnt ) )l_iCnt ++;
			if ( l_iCnt && ( l_pcReadBuf [ l_iCnt - 1 ] == '\n' ) )break;
/*			if ( console::kbhit ( ) == 'q' )return ( -1 ); */ /* FIXME */
			}
		if ( ! strncmp ( l_pcReadBuf, "KO?\r\n", 5 ) )
			{
			strncpy ( l_pcReadBuf, "OK\r\n", 4 );
			for ( l_iCnt = 0; l_iCnt < 4; l_iCnt ++ )
				a_roPort.write ( l_pcReadBuf + l_iCnt, 1 );
			break;
			}
		l_iErr ++;
		}
	M_LOG ( "Collector: Connected !" );
	return ( 0 );
	}

void read_colector ( HSerial & a_roPort ) 
	{
	int l_iErr = 0;
	int l_iRet = 0;
	int l_iCtr = 0;
	int l_iCount = 0;
	char l_pcBuffer [ 64 ];
	HString l_oErr;
	if ( ! a_roPort.open ( )	&& ( wait_for_connection ( a_roPort ) >= 0 ) )
		{
		memset ( l_pcBuffer, 0, 64 );
		while ( strncmp ( l_pcBuffer, "QNTT", 4 ) && ( l_iRet >= 0 ) )
			l_iRet = receive ( a_roPort, l_pcBuffer );
		l_iCount = atoi ( l_pcBuffer + 4 );
		l_oErr.format ( "Collector: transmission size = %d line(s).", l_iCount );
		M_LOG ( ( char * ) l_oErr );
		if ( l_iRet < 0 )l_iCount = 0;
		while ( l_iCtr ++ < l_iCount )
			{
			memset ( l_pcBuffer, 0, 64 );
			l_iErr += receive ( a_roPort, l_pcBuffer );
			if ( l_iRet < 0 )
				{
				l_iErr -= l_iRet;
				break;
				}
			if ( ! ( strncmp ( l_pcBuffer, "END\r\n", 5 )
						&& strncmp ( l_pcBuffer, "BRK\r\n", 5 ) ) )break;
/*			external_helper_handler ( l_pcBuffer ); */ /* FIXME */
			}
		l_oErr.format ( "Collector: line(s) read = %d.", l_iCtr );
		}
	return;
	}

}

