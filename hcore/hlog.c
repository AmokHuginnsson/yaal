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
#include <pwd.h>
#include <time.h>
#include <libintl.h>

#include "config.h"

#ifdef HAVE_BASE_NAME_IN_LIBGEN
#	include <libgen.h>
#endif /* HAVE_BASE_NAME_IN_LIBGEN */

#include "hexception.h"
M_CVSID ( "$CVSHeader$" )
#include "hlog.h"
#include "xalloc.h"

namespace stdhapi
{

namespace hcore
{

#define D_BUFFER_SIZE			1024
#define D_HOSTNAME_SIZE		16
#define D_TIMESTAMP_SIZE	16
#define D_LOGIN_NAME_MAX	8

HLog::HLog ( void ) : f_bRealMode ( false ), f_bNewLine ( true ),
	f_lType ( 0 ), f_psStream ( NULL ), f_pcProcessName ( NULL ),
	f_pcLoginName ( NULL ), f_pcHostName ( NULL ), f_pcBuffer ( NULL ),
	f_iBufferSize ( D_BUFFER_SIZE )
	{
	M_PROLOG
	uid_t l_iUid = 0;
	passwd * l_psPasswd = NULL;
	f_pcBuffer = xcalloc ( f_iBufferSize, char );
	f_pcHostName = xcalloc ( D_HOSTNAME_SIZE, char );
	f_psStream = tmpfile ( );
	if ( ! f_psStream )
		M_THROW ( "tmpfile returned", reinterpret_cast < int > ( f_psStream ) );
	fprintf ( f_psStream, "%-10xProcess started (%d).\n",
			D_LOG_NOTICE, getpid ( ) );
	l_iUid = getuid ( );
	l_psPasswd = getpwuid ( l_iUid );
	if ( l_psPasswd )
		f_pcLoginName = xstrdup ( l_psPasswd->pw_name );
	else
		{
		f_pcLoginName = xcalloc ( D_LOGIN_NAME_MAX + 1, char );
		M_ENSURE ( snprintf ( f_pcLoginName, D_LOGIN_NAME_MAX, "%d", l_iUid ) <= D_LOGIN_NAME_MAX );
		}
	M_ENSURE ( gethostname ( f_pcHostName, D_HOSTNAME_SIZE - 1 ) == 0 );
	return;
	M_EPILOG
	}

HLog::~HLog ( void )
	{
	M_PROLOG
	if ( n_lLogMask & D_LOG_NOTICE )
		{
		if ( f_bNewLine )
			timestamp ( );
		fprintf ( f_psStream, "Process exited normally.\n" );
		}
	if ( ( f_psStream != stdout ) && ( f_psStream != stderr ) )
		fclose ( f_psStream );
	f_psStream = NULL;
	if ( f_pcLoginName )
		xfree ( f_pcLoginName );
	if ( f_pcHostName )
		xfree ( f_pcHostName );
	f_pcHostName = NULL;
	if ( f_pcBuffer )
		xfree ( f_pcBuffer );
	f_pcBuffer = NULL;
	return;
	M_EPILOG
	}

void HLog::rehash ( FILE * a_psStream,
		char const * const a_pcProcessName )
	{
	M_PROLOG
#ifndef HAVE_GETLINE
	char * l_pcPtr = NULL;
	int l_iLen = 0;
#endif /* not HAVE_GETLINE */
	FILE * l_psTmpFile;
	f_bRealMode = true;
	if ( a_pcProcessName )
		f_pcProcessName = basename ( a_pcProcessName );
	if ( ! a_psStream )
		M_THROW ( "file parameter is", reinterpret_cast < int > ( a_psStream ) );
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
			l_pcPtr = static_cast < char * > ( memchr ( f_pcBuffer, '\n', l_iLen ) );
			if ( ! l_pcPtr )
				{
				fprintf ( a_psStream, f_pcBuffer );
				continue;
				}
			* ++ l_pcPtr = 0;
			fseek ( f_psStream, l_pcPtr - f_pcBuffer - l_iLen, SEEK_CUR );
#endif /* not HAVE_GETLINE */
			f_lType = strtol ( f_pcBuffer, NULL, 0x10 );
			if ( ! ( f_lType && f_bRealMode ) || ( f_lType & n_lLogMask ) )
				{
				timestamp ( a_psStream );
				fprintf ( a_psStream, f_pcBuffer + 10 );
				}
			}
		if ( f_pcBuffer [ strlen ( f_pcBuffer ) - 1 ] == '\n' )
			f_lType = 0;
		l_psTmpFile = f_psStream;
		fclose ( l_psTmpFile );
		}
	f_psStream = a_psStream;
	return;
	M_EPILOG
	}

void HLog::rehash ( char const * const a_pcLogFileName,
		char const * const a_pcProcessName )
	{
	M_PROLOG
	if ( ! a_pcLogFileName )
		M_THROW ( "new file name argument is",
				reinterpret_cast < int > ( a_pcLogFileName ) );
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
	if ( ! a_psStream )
		a_psStream = f_psStream;
	if ( ! f_bRealMode )
		{
		if ( f_psStream )
			fprintf ( f_psStream, "%-10lx", f_lType );
		return;
		}
	tm * l_psBrokenTime = 0;
	l_xCurrentTime = time ( NULL );
	l_psBrokenTime = localtime ( & l_xCurrentTime );
	memset ( l_pcBuffer, 0, D_TIMESTAMP_SIZE );
	/* ISO C++ does not support the `%e' strftime format */
	/* `%e': The day of the month like with `%d', but padded with blank */
	/* (range ` 1' through `31'). */
	/* This format was first standardized by POSIX.2-1992 and by ISO C99.*/
	/* I will have to wait with using `%e'. */
	l_iSize = strftime ( l_pcBuffer, D_TIMESTAMP_SIZE, "%b %d %H:%M:%S",
			l_psBrokenTime );
	if ( l_iSize > D_TIMESTAMP_SIZE )
		M_THROW ( _ ( "strftime returned more than D_TIMESTAMP_SIZE" ), l_iSize );
	if ( f_pcProcessName )
		fprintf ( a_psStream, "%s %s@%s->%s: ", l_pcBuffer, f_pcLoginName,
				f_pcHostName, f_pcProcessName );
	else
		fprintf ( a_psStream, "%s %s@%s: ", l_pcBuffer, f_pcLoginName,
			f_pcHostName );
	return;
	M_EPILOG
	}

int HLog::operator ( ) ( char const * const a_pcFormat, va_list const a_xAp )
	{
	M_PROLOG
	int l_iErr = 0;
	if ( f_bNewLine )
		timestamp ( );
	memset ( f_pcBuffer, 0, f_iBufferSize );
	l_iErr = vsnprintf ( f_pcBuffer, f_iBufferSize, a_pcFormat, a_xAp );
	fprintf ( f_psStream, f_pcBuffer );
	if ( f_pcBuffer [ strlen ( f_pcBuffer ) - 1 ] != '\n' )
		f_bNewLine = false;
	else
		{
		f_lType = 0;
		f_bNewLine = true;
		M_ENSURE ( fflush ( f_psStream ) == 0 );
		}
	return ( l_iErr );
	M_EPILOG
	}

int HLog::operator ( ) ( char const * const a_pcFormat, ... )
	{
	M_PROLOG
	int l_iErr = 0;
	va_list l_xAp;
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & n_lLogMask ) )
		{
		va_start ( l_xAp, a_pcFormat );
		l_iErr = ( * this ) ( a_pcFormat, l_xAp );
		va_end ( l_xAp );
		}
	return ( l_iErr );
	M_EPILOG
	}

int HLog::operator ( ) ( int long const a_lType,
		char const * const a_pcFormat, ... )
	{
	M_PROLOG
	int l_iErr = 0;
	va_list l_xAp;
	f_lType = a_lType;
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & n_lLogMask ) )
		{
		va_start ( l_xAp, a_pcFormat );
		l_iErr = ( * this ) ( a_pcFormat, l_xAp );
		va_end ( l_xAp );
		}
	return ( l_iErr );
	M_EPILOG
	}

HLog & HLog::operator ( ) ( int long const a_lType )
	{
	M_PROLOG
	f_lType = a_lType;
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( char const * const a_pcString )
	{
	M_PROLOG
	if ( ! a_pcString )
		return ( * this );
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & n_lLogMask ) )
		{
		if ( f_bNewLine )
			timestamp ( );
		fprintf ( f_psStream, a_pcString );
		if ( a_pcString [ strlen ( a_pcString ) - 1 ] != '\n' )
			f_bNewLine = false;
		else
			{
			f_bNewLine = true;
			f_lType = 0;
			M_ENSURE ( fflush ( f_psStream ) == 0 );
			}
		}
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( char const a_cChar )
	{
	M_PROLOG
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & n_lLogMask ) )
		{
		if ( f_bNewLine )
			timestamp ( );
		fprintf ( f_psStream, "%c", a_cChar );
		if ( a_cChar != '\n' )
			f_bNewLine = false;
		else
			{
			f_bNewLine = true;
			f_lType = 0;
			M_ENSURE ( fflush ( f_psStream ) == 0 );
			}
		}
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( int const a_iInteger )
	{
	M_PROLOG
	int long l_lTmp = a_iInteger;
	return ( * this << l_lTmp );
	M_EPILOG
	}

HLog & HLog::operator << ( int unsigned const a_uiInteger )
	{
	M_PROLOG
	int long unsigned l_ulTmp = a_uiInteger;
	return ( * this << l_ulTmp );
	M_EPILOG
	}

HLog & HLog::operator << ( int long const a_lLongInteger )
	{
	M_PROLOG
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & n_lLogMask ) )
		{
		if ( f_bNewLine )
			timestamp ( );
		M_ENSURE ( snprintf ( f_pcBuffer, f_iBufferSize, "%ld",
					a_lLongInteger ) < static_cast < int > ( f_iBufferSize ) );
		fprintf ( f_psStream, f_pcBuffer );
		f_bNewLine = false;
		}
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( int long unsigned const a_ulLongInteger )
	{
	M_PROLOG
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & n_lLogMask ) )
		{
		if ( f_bNewLine )
			timestamp ( );
		M_ENSURE ( snprintf ( f_pcBuffer, f_iBufferSize, "%lu",
					a_ulLongInteger ) < static_cast < int > ( f_iBufferSize ) );
		fprintf ( f_psStream, f_pcBuffer );
		f_bNewLine = false;
		}
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( double const a_dDouble )
	{
	M_PROLOG
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & n_lLogMask ) )
		{
		if ( f_bNewLine )
			timestamp ( );
		M_ENSURE ( snprintf ( f_pcBuffer, f_iBufferSize, "%f",
					a_dDouble ) < static_cast < int > ( f_iBufferSize ) );
		fprintf ( f_psStream, f_pcBuffer );
		f_bNewLine = false;
		}
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( void * const a_pvPtr )
	{
	M_PROLOG
	( * this ) ( "%p", a_pvPtr );
	return ( * this );
	M_EPILOG
	}

HLog & HLog::operator << ( HLog & ( * const x_log ) ( HLog & ) )
	{
	M_PROLOG
	return ( x_log ( * this ) );
	M_EPILOG
	}

HLog & endl ( HLog & a_roLog )
	{
	M_PROLOG
	if ( ! ( a_roLog.f_lType && a_roLog.f_bRealMode ) || ( a_roLog.f_lType & n_lLogMask ) )
		a_roLog << '\n';
	a_roLog.f_lType = 0;
	return ( a_roLog );
	M_EPILOG
	}

}

}

