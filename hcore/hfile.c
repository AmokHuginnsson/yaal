/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfile.c - this file is integral part of `stdhapi' project.

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

#include <stdio.h>
#include <string.h>
#include <libintl.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hfile.h"

namespace stdhapi
{

namespace hcore
{

HFile::HFile ( int a_iMode ) : f_iMode ( a_iMode ), f_pvHandle ( NULL ),
															 f_oPath ( ), f_oError ( )
	{
	M_PROLOG
	if ( ( ( a_iMode & D_APPEND ) && ( a_iMode & D_TRUNCATE ) )
			|| ( ( a_iMode & D_READING ) && ( a_iMode & D_TRUNCATE ) )
			|| ( ( a_iMode & D_READING ) && ( a_iMode & D_APPEND ) ) )
		M_THROW ( _ ( "inconsistient mode flags" ), a_iMode );
	return;
	M_EPILOG
	}

HFile::~HFile ( void )
	{
	M_PROLOG
	if ( f_pvHandle )
		close ( );
	return;
	M_EPILOG
	}

int HFile::open ( char const * a_pcPath )
	{
	M_PROLOG
	int l_iError = 0;
	char const * l_pcMode = NULL;
	switch ( f_iMode )
		{
		case ( D_READING ):
			{
			l_pcMode = "r";
			break;
			}
		case ( D_WRITING | D_TRUNCATE ):
			{
			l_pcMode = "w";
			break;
			}
		case ( D_WRITING | D_APPEND ):
			{
			l_pcMode = "a";
			break;
			}
		case ( D_READING | D_WRITING ):
			{
			l_pcMode = "r+";
			break;
			}
		case ( D_READING | D_WRITING | D_TRUNCATE ):
			{
			l_pcMode = "w+";
			break;
			}
		case ( D_READING | D_WRITING | D_APPEND ):
			{
			l_pcMode = "a+";
			break;
			}
		default:
			{
			M_THROW ( "unexpected mode setting", f_iMode );
			break;
			}
		}
	f_oPath = a_pcPath;
	f_pvHandle = fopen ( a_pcPath, l_pcMode );
	if ( ! f_pvHandle )
		{
		l_iError = g_iErrNo;
		f_oError = strerror ( l_iError );
		return ( l_iError );
		}
	return ( 0 );
	M_EPILOG
	}

int HFile::close ( void )
	{
	M_PROLOG
	int l_iError = 0;
	if ( ! f_pvHandle )
		M_THROW ( "file is not opened", g_iErrNo );
	l_iError = fclose ( static_cast < FILE * > ( f_pvHandle ) );
	if ( l_iError )
		{
		f_oError = strerror ( l_iError );
		return ( l_iError );
		}
	f_pvHandle = NULL;
	return ( 0 );
	M_EPILOG
	}

int HFile::read_line ( HString & a_roLine, bool a_bStripNewlines, int a_iMaximumLength )
	{
	M_PROLOG
	int l_iLength = 0;
	char * l_pcPtr = NULL;
	if ( ! f_pvHandle )
		M_THROW ( _ ( "no opened file" ), g_iErrNo );
	l_iLength = scan_line ( );
	if ( l_iLength )
		{
		if ( a_iMaximumLength && ( l_iLength > a_iMaximumLength ) )
			M_THROW ( _ ( "line too long" ), l_iLength );
		a_roLine.hs_realloc ( l_iLength );
		l_pcPtr = a_roLine;
		M_ENSURE ( static_cast < int > ( fread ( l_pcPtr,
						sizeof ( char ), l_iLength,
						static_cast < FILE * > ( f_pvHandle ) ) ) == l_iLength );
		if ( a_bStripNewlines && ( l_iLength > 0 ) )
			{
			l_iLength --;
			if ( ( l_iLength > 0 ) && ( l_pcPtr [ l_iLength - 1 ] == '\r' ) )
				l_iLength --;
			}
		l_pcPtr [ l_iLength ] = 0;
		return ( l_iLength );
		}
	return ( - 1 );
	M_EPILOG
	}

int HFile::scan_line ( void )
	{
	M_PROLOG
#define D_SCAN_BUFFER_SIZE	8
	int l_iLength = 0, l_iSize = 0;
	char l_pcBuffer [ D_SCAN_BUFFER_SIZE ];
	char const * l_pcPtr = NULL;
	do
		{
		l_iSize = fread ( l_pcBuffer, sizeof ( char ),
				D_SCAN_BUFFER_SIZE, static_cast < FILE * > ( f_pvHandle ) );
		l_iLength += l_iSize;
		l_pcPtr = static_cast < char * > ( memchr ( l_pcBuffer,
					'\n', l_iSize ) );
		}
	while ( ! l_pcPtr && ( l_iSize == D_SCAN_BUFFER_SIZE ) );
	M_ENSURE ( fseek ( static_cast < FILE * > ( f_pvHandle ),
				- l_iLength, SEEK_CUR ) == 0 );
	if ( l_pcPtr )
		l_iLength -= ( l_iSize - ( l_pcPtr + 1 - l_pcBuffer ) ); /* + 1 for \n */
	return ( l_iLength );
	M_EPILOG
	}

const HString & HFile::get_path ( void ) const
	{
	M_PROLOG
	return ( f_oPath );
	M_EPILOG
	}

const HString & HFile::get_error ( void ) const
	{
	M_PROLOG
	return ( f_oError );
	M_EPILOG
	}

void HFile::flush ( void ) const
	{
	M_PROLOG
	M_ENSURE ( fflush ( static_cast < FILE * > ( f_pvHandle ) ) == 0 );
	return;
	M_EPILOG
	}

HFile & HFile::operator << ( char const * a_pcString )
	{
	M_PROLOG
	if ( a_pcString )
		fprintf ( static_cast < FILE * > ( f_pvHandle ), a_pcString );
	return ( * this );
	M_EPILOG
	}

HFile & HFile::operator << ( char const a_cChar )
	{
	M_PROLOG
	fprintf ( static_cast < FILE * > ( f_pvHandle ), "%c", a_cChar );
	return ( * this );
	M_EPILOG
	}

HFile & HFile::operator << ( const int a_iInteger )
	{
	M_PROLOG
	long int l_lTmp = a_iInteger;
	return ( operator << ( l_lTmp ) );
	M_EPILOG
	}

HFile & HFile::operator << ( const long int a_lLongInteger )
	{
	M_PROLOG
	fprintf ( static_cast < FILE * > ( f_pvHandle ), "%ld", a_lLongInteger );
	return ( * this );
	M_EPILOG
	}

HFile & HFile::operator << ( const double a_dDouble )
	{
	M_PROLOG
	fprintf ( static_cast < FILE * > ( f_pvHandle ), "%f", a_dDouble );
	return ( * this );
	M_EPILOG
	}

HFile & HFile::operator << ( void * a_pvPtr )
	{
	M_PROLOG
	fprintf ( static_cast < FILE * > ( f_pvHandle ), "%p", a_pvPtr );
	return ( * this );
	M_EPILOG
	}

HFile & HFile::operator << ( HFile & ( * HFILE ) ( HFile & ) )
	{
	M_PROLOG
	return ( HFILE ( * this ) );
	M_EPILOG
	}

HFile & endl ( HFile & a_roFile )
	{
	M_PROLOG
	a_roFile << '\n';
	return ( a_roFile );
	M_EPILOG
	}

HFile & flush ( HFile & a_roFile )
	{
	M_PROLOG
	a_roFile.flush ( );
	return ( a_roFile );
	M_EPILOG
	}

HFile::operator bool ( void ) const
	{
	M_PROLOG
	return ( f_pvHandle ? true : false );
	M_EPILOG
	}

}

}

