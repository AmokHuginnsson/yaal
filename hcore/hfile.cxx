/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hfile.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>
#include <cstring>
#include <libintl.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hfile.h"

namespace yaal
{

namespace hcore
{

HFile::HFile ( mode_open_t const a_eMode, void* const a_pvHandle )
	: HStreamInterface(), f_eMode( a_eMode ),
	f_pvHandle( a_pvHandle ), f_oPath(), f_oError(),
	f_bExternal( a_pvHandle ? true : false )
	{
	M_PROLOG
	if ( ( ( a_eMode & D_APPEND ) && ( a_eMode & D_TRUNCATE ) )
			|| ( ( a_eMode & D_READING ) && ( a_eMode & D_TRUNCATE ) )
			|| ( ( a_eMode & D_READING ) && ( a_eMode & D_APPEND ) ) )
		M_THROW ( _ ( "inconsistient mode flags" ), a_eMode );
	return;
	M_EPILOG
	}

HFile::~HFile ( void )
	{
	M_PROLOG
	if ( f_pvHandle && ! f_bExternal )
		close();
	return;
	M_EPILOG
	}

int HFile::open( char const* const a_pcPath )
	{
	M_PROLOG
	int l_iError = 0;
	char const * l_pcMode = NULL;
	if ( f_eMode == D_READING )
		l_pcMode = "r";
	else if ( ( f_eMode == D_WRITING ) || ( f_eMode == ( D_WRITING | D_TRUNCATE ) ) )
		l_pcMode = "w";
	else if ( f_eMode == ( D_WRITING | D_APPEND ) )
		l_pcMode = "a";
	else if ( f_eMode == ( D_READING | D_WRITING ) )
		l_pcMode = "r+";
	else if ( f_eMode == ( D_READING | D_WRITING | D_TRUNCATE ) )
		l_pcMode = "w+";
	else if ( f_eMode == ( D_READING | D_WRITING | D_APPEND ) )
		l_pcMode = "a+";
	else
		M_THROW ( "unexpected mode setting", f_eMode );
	f_oPath = a_pcPath;
	f_pvHandle = fopen ( a_pcPath, l_pcMode );
	if ( ! f_pvHandle )
		{
		l_iError = errno;
		f_oError = strerror ( l_iError );
		return ( l_iError );
		}
	return ( 0 );
	M_EPILOG
	}

int HFile::close( void )
	{
	M_PROLOG
	int l_iError = 0;
	if ( ! f_pvHandle )
		M_THROW ( "file is not opened", errno );
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

int HFile::read_line( HString& a_roLine, mode_read_t a_eMode,
		int const a_iMaximumLength )
	{
	M_PROLOG
	int l_iLength = 0;
	char * l_pcPtr = NULL;
	if ( ( a_eMode & D_KEEP_NEWLINES ) && ( a_eMode & D_STRIP_NEWLINES ) )
		M_THROW ( _ ( "bad newlines setting" ), a_eMode );
	if ( ! ( a_eMode & ( D_KEEP_NEWLINES | D_STRIP_NEWLINES ) ) )
		a_eMode |= D_KEEP_NEWLINES;
	if ( ( a_eMode & D_BUFFERED_READS ) && ( a_eMode & D_UNBUFFERED_READS ) )
		M_THROW ( _ ( "bad buffering setting" ), a_eMode );
	if ( ! ( a_eMode & ( D_BUFFERED_READS | D_UNBUFFERED_READS ) ) )
		a_eMode |= D_BUFFERED_READS;
	if ( ! f_pvHandle )
		M_THROW ( _ ( "no file is opened" ), errno );
	if ( a_eMode & D_BUFFERED_READS )
		{
		l_iLength = get_line_length();
		if ( l_iLength )
			{
			if ( a_iMaximumLength && ( l_iLength > a_iMaximumLength ) )
				M_THROW ( _ ( "line too long" ), l_iLength );
			a_roLine.hs_realloc ( l_iLength );
			l_pcPtr = a_roLine.raw();
			M_ENSURE ( static_cast<int>( ::fread( l_pcPtr,
							sizeof ( char ), l_iLength,
							static_cast<FILE*>( f_pvHandle ) ) ) == l_iLength );
			}
		}
	else /* D_UNBUFFERED_READS */
		{
		l_iLength = read_until( a_roLine );
		if ( a_iMaximumLength && ( l_iLength > a_iMaximumLength ) )
			M_THROW ( _ ( "line too long" ), l_iLength );
		l_pcPtr = a_roLine.raw();
		}
	if ( l_iLength )
		{
		if ( ( a_eMode & D_STRIP_NEWLINES ) && ( l_iLength > 0 ) )
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

int HFile::get_line_length( void )
	{
	M_PROLOG
	static int const D_SCAN_BUFFER_SIZE = 8;
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

HString const& HFile::get_path( void ) const
	{
	M_PROLOG
	return ( f_oPath );
	M_EPILOG
	}

HString const& HFile::get_error( void ) const
	{
	M_PROLOG
	return ( f_oError );
	M_EPILOG
	}

void HFile::flush( void ) const
	{
	M_PROLOG
	do_flush();
	return;
	M_EPILOG
	}

void HFile::do_flush( void ) const
	{
	M_PROLOG
	M_ENSURE ( fflush ( static_cast < FILE * > ( f_pvHandle ) ) == 0 );
	return;
	M_EPILOG
	}

HFile::operator opcm_t const ( void ) const
	{
	M_PROLOG
	return ( f_pvHandle ? &OPointerConversion::f_iValid : NULL );
	M_EPILOG
	}

int HFile::do_read( void* const a_pcBuffer, int a_iSize )
	{
	M_PROLOG
	return ( ::fread ( a_pcBuffer, sizeof ( char ), a_iSize,
				static_cast<FILE*>( f_pvHandle ) ) );
	M_EPILOG
	}

int HFile::do_write( void const* const a_pcString, int const a_iSize )
	{
	M_PROLOG
	return ( ::fwrite( a_pcString, sizeof ( char ), a_iSize, static_cast<FILE*>( f_pvHandle ) ) );
	M_EPILOG
	}


}

}

