/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	hlog.c - this file is integral part of `stdhapi' project.

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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "../config.h"

#ifdef __HOST_OS_TYPE_FREEBSD__
#	include <libgen.h>
#endif /* __HOST_OS_TYPE_FREEBSD__ */

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hlog.h"
#include "xalloc.h"

HLog::HLog ( void )
	{
	M_PROLOG
	f_bRealMode = false;
	f_psStream = NULL;
	f_bNewLine = true;
	f_pcProcessName = NULL;
	f_iBufferSize = D_BUFFER_SIZE;
	f_pcBuffer = ( char * ) xcalloc ( f_iBufferSize );
	f_pcHostName = ( char * ) xcalloc ( D_HOSTNAME_SIZE );
	f_psStream = tmpfile ( );
	if ( ! f_psStream )
		throw new HException ( __WHERE__, "tmpfile returned", ( int ) f_psStream );
	fprintf ( f_psStream, "Process started (%d).\n", getpid ( ) );
	f_pcLoginName = getenv ( "LOGNAME" );
	gethostname ( f_pcHostName, D_HOSTNAME_SIZE - 1 );
	return;
	M_EPILOG
	}

HLog::~HLog ( void )
	{
	M_PROLOG
	if ( f_bNewLine )timestamp ( );
	fprintf ( f_psStream, "Process exited normally.\n" );
	if ( ( f_psStream != stdout ) && ( f_psStream != stderr ) )
		fclose ( f_psStream );
	f_psStream = NULL;
	if ( f_pcHostName )
		xfree ( ( void * ) f_pcHostName );
	f_pcHostName = NULL;
	if ( f_pcBuffer )
		xfree ( ( void * ) f_pcBuffer );
	f_pcBuffer = NULL;
	return;
	M_EPILOG
	}

void HLog::rehash ( FILE * a_psStream, char * a_pcProcessName )
	{
	M_PROLOG
#ifndef HAVE_GETLINE
	char * l_pcPtr = NULL;
	int l_iLen = 0;
#endif /* not HAVE_GETLINE */
	FILE * l_psTmpFile;
	f_bRealMode = true;
	if ( a_pcProcessName )f_pcProcessName = basename ( a_pcProcessName );
	if ( ! a_psStream )
		throw new HException ( __WHERE__, "file parameter is", ( int ) a_psStream );
	if ( f_psStream )
		{
		fseek ( f_psStream, 0, SEEK_SET );
#ifdef HAVE_GETLINE 
		while ( getline ( & f_pcBuffer, & f_iBufferSize, f_psStream ) > 0 )
#else /* HAVE_GETLINE */
		while ( ( l_iLen = fread ( f_pcBuffer, sizeof ( char ), f_iBufferSize, f_psStream ) ) )
#endif /* not HAVE_GETLINE */
			{
#ifndef HAVE_GETLINE
			l_pcPtr = ( char * ) memchr ( f_pcBuffer, '\n', l_iLen );
			if ( ! l_pcPtr )
				{
				fprintf ( a_psStream, f_pcBuffer );
				continue;
				}
			* ++ l_pcPtr = 0;
			fseek ( f_psStream, l_pcPtr - f_pcBuffer - l_iLen, SEEK_CUR );
#endif /* not HAVE_GETLINE */
			timestamp ( a_psStream );
			fprintf ( a_psStream, f_pcBuffer );
			}
		l_psTmpFile = f_psStream;
		fclose ( l_psTmpFile );
		}
	f_psStream = a_psStream;
	return;
	M_EPILOG
	}

void HLog::rehash ( const char * a_pcLogFileName, char * a_pcProcessName )
	{
	M_PROLOG
	if ( ! a_pcLogFileName )
		throw new HException ( __WHERE__, "new file name argument is",
				( int ) a_pcLogFileName );
	rehash ( fopen ( a_pcLogFileName, "a" ), a_pcProcessName );
	return;
	M_EPILOG
	}

void HLog::timestamp ( FILE * a_psStream )
	{
	M_PROLOG
	int l_iSize = 0;
	char l_pcBuffer [ D_TIMESTAMP_SIZE ];
	time_t l_xCurrentTime;
	if ( ! f_bRealMode )return;
	tm * l_psBrokenTime = 0;
	if ( ! a_psStream )a_psStream = f_psStream;
	l_xCurrentTime = time ( NULL );
	l_psBrokenTime = localtime ( & l_xCurrentTime );
	memset ( l_pcBuffer, 0, D_TIMESTAMP_SIZE );
	l_iSize = strftime ( l_pcBuffer, D_TIMESTAMP_SIZE, "%b %e %H:%M:%S", l_psBrokenTime );
	if ( l_iSize > D_TIMESTAMP_SIZE )
		throw new HException ( __WHERE__,
				"strftime returned more than D_TIMESTAMP_SIZE.", l_iSize );
	if ( f_pcProcessName )fprintf ( a_psStream, "%s %s@%s->%s: ", l_pcBuffer, f_pcLoginName, f_pcHostName, f_pcProcessName );
	else fprintf ( a_psStream, "%s %s@%s: ", l_pcBuffer, f_pcLoginName, f_pcHostName );
	return;
	M_EPILOG
	}

int HLog::operator ( ) ( const char * a_pcFormat, ... )
	{
	M_PROLOG
	va_list l_sAp;
	if ( f_bNewLine )timestamp ( );
	memset ( f_pcBuffer, 0, f_iBufferSize );
	va_start ( l_sAp, a_pcFormat );
	vsnprintf ( f_pcBuffer, f_iBufferSize, a_pcFormat, l_sAp );
	va_end ( l_sAp );
	fprintf ( f_psStream, f_pcBuffer );
	if ( f_pcBuffer [ strlen ( f_pcBuffer ) - 1 ] != '\n' )f_bNewLine = false;
	else
		{
		f_bNewLine = true;
		fflush ( f_psStream );
		}
	return ( 0 );
	M_EPILOG
	}

HLog & HLog::operator << ( const char * a_pcString )
	{
	M_PROLOG
	if ( ! a_pcString )return ( * this );
	if ( f_bNewLine )timestamp ( );
	fprintf ( f_psStream, a_pcString );
	if ( a_pcString [ strlen ( a_pcString ) - 1 ] != '\n' )f_bNewLine = false;
	else
		{
		f_bNewLine = true;
		fflush ( f_psStream );
		}
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( const char a_cChar )
	{
	M_PROLOG
	if ( f_bNewLine )timestamp ( );
	fprintf ( f_psStream, "%c", a_cChar );
	if ( a_cChar != '\n' )f_bNewLine = false;
	else
		{
		f_bNewLine = true;
		fflush ( f_psStream );
		}
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( const int a_iInteger )
	{
	M_PROLOG
	long int l_lTmp = a_iInteger;
	return ( * this << l_lTmp );
	M_EPILOG
	}

HLog & HLog::operator << ( const long int a_lInteger )
	{
	M_PROLOG
	if ( f_bNewLine )timestamp ( );
	snprintf ( f_pcBuffer, f_iBufferSize, "%ld", a_lInteger );
	fprintf ( f_psStream, f_pcBuffer );
	f_bNewLine = false;
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( const double a_dDouble )
	{
	M_PROLOG
	if ( f_bNewLine )timestamp ( );
	snprintf ( f_pcBuffer, f_iBufferSize, "%f", a_dDouble );
	fprintf ( f_psStream, f_pcBuffer );
	f_bNewLine = false;
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( void * a_pvPtr )
	{
	M_PROLOG
	( * this ) ( "%p", a_pvPtr );
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( HLog & ( * x_log ) ( HLog & ) )
	{
	M_PROLOG
	return ( x_log ( * this ) );
	M_EPILOG
	}

HLog & endl ( HLog & a_roLog )
	{
	M_PROLOG
	a_roLog << '\n';
	return ( a_roLog );
	M_EPILOG
	}

HLog log;
