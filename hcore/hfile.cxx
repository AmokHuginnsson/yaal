/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hfile.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdio>
#include <cstring>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hfile.hxx"

namespace yaal
{

namespace hcore
{

HFile::HFile( OPEN::open_t const& a_eOpen, void* const a_pvHandle )
	: HStreamInterface(), f_eOpen( a_eOpen ),
	f_pvHandle( a_pvHandle ), f_oPath(), f_oError(),
	f_bExternal( a_pvHandle ? true : false )
	{
	M_PROLOG
	if ( ( ( a_eOpen & OPEN::D_APPEND ) && ( a_eOpen & OPEN::D_TRUNCATE ) )
			|| ( ( a_eOpen & OPEN::D_READING ) && ( a_eOpen & OPEN::D_TRUNCATE ) )
			|| ( ( a_eOpen & OPEN::D_READING ) && ( a_eOpen & OPEN::D_APPEND ) ) )
		M_THROW( _( "inconsistient mode flags" ), a_eOpen );
	return;
	M_EPILOG
	}

HFile::HFile( yaal::hcore::HString const& path, OPEN::open_t const& a_eOpen )
	: HStreamInterface(), f_eOpen( a_eOpen ),
	f_pvHandle( NULL ), f_oPath(), f_oError(),
	f_bExternal( false )
	{
	M_PROLOG
	if ( ( ( a_eOpen & OPEN::D_APPEND ) && ( a_eOpen & OPEN::D_TRUNCATE ) )
			|| ( ( a_eOpen & OPEN::D_READING ) && ( a_eOpen & OPEN::D_TRUNCATE ) )
			|| ( ( a_eOpen & OPEN::D_READING ) && ( a_eOpen & OPEN::D_APPEND ) ) )
		M_THROW( _( "inconsistient mode flags" ), a_eOpen );
	open( path );
	return;
	M_EPILOG
	}

HFile::~HFile( void )
	{
	M_PROLOG
	if ( f_pvHandle && ! f_bExternal )
		close();
	return;
	M_EPILOG
	}

int HFile::open( HString const& a_oPath )
	{
	M_PROLOG
	int l_iError = 0;
	char const * l_pcMode = NULL;
	if ( f_eOpen == OPEN::D_READING )
		l_pcMode = "r";
	else if ( ( f_eOpen == OPEN::D_WRITING ) || ( f_eOpen == ( OPEN::D_WRITING | OPEN::D_TRUNCATE ) ) )
		l_pcMode = "w";
	else if ( f_eOpen == ( OPEN::D_WRITING | OPEN::D_APPEND ) )
		l_pcMode = "a";
	else if ( f_eOpen == ( OPEN::D_READING | OPEN::D_WRITING ) )
		l_pcMode = "r+";
	else if ( f_eOpen == ( OPEN::D_READING | OPEN::D_WRITING | OPEN::D_TRUNCATE ) )
		l_pcMode = "w+";
	else if ( f_eOpen == ( OPEN::D_READING | OPEN::D_WRITING | OPEN::D_APPEND ) )
		l_pcMode = "a+";
	else
		M_THROW ( "unexpected mode setting", f_eOpen );
	f_oPath = a_oPath;
	f_pvHandle = ::fopen( a_oPath.raw(), l_pcMode );
	if ( ! f_pvHandle )
		{
		l_iError = errno;
		f_oError = error_message( l_iError );
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
		M_THROW( "file is not opened", errno );
	l_iError = ::std::fclose( static_cast < FILE * > ( f_pvHandle ) );
	if ( l_iError )
		{
		f_oError = error_message( l_iError );
		return ( l_iError );
		}
	f_pvHandle = NULL;
	return ( 0 );
	M_EPILOG
	}

int long HFile::tell( void ) const
	{
	M_PROLOG
	if ( ! f_pvHandle )
		M_THROW( _( "no file is opened" ), errno );
	int long pos = 0;
	M_ENSURE( ( pos = ::std::ftell( static_cast<FILE*>( f_pvHandle ) ) ) >= 0 );
	return ( pos );
	M_EPILOG
	}

void HFile::seek( int long const& pos, SEEK::seek_t const& a_eSeek )
	{
	M_PROLOG
	if ( ! f_pvHandle )
		M_THROW( _( "no file is opened" ), errno );
	int s = 0;
	switch ( a_eSeek )
		{
		case ( SEEK::D_SET ):
			s = SEEK_SET;
		break;
		case ( SEEK::D_CUR ):
			s = SEEK_CUR;
		break;
		case ( SEEK::D_END ):
			s = SEEK_END;
		break;
		default:
			M_ASSERT( ! "bad seek type" );
		break;
		}
	M_ENSURE( ::fseek( static_cast<FILE*>( f_pvHandle ), pos, s ) == 0 );
	return;
	M_EPILOG
	}

int long HFile::read_line( HString& a_roLine, READ::read_t const& a_eRead,
		int const a_iMaximumLength )
	{
	M_PROLOG
	READ::read_t l_eRead = a_eRead;
	if ( ( l_eRead & READ::D_KEEP_NEWLINES ) && ( l_eRead & READ::D_STRIP_NEWLINES ) )
		M_THROW ( _ ( "bad newlines setting" ), l_eRead );
	if ( ! ( l_eRead & ( READ::D_KEEP_NEWLINES | READ::D_STRIP_NEWLINES ) ) )
		l_eRead |= READ::D_KEEP_NEWLINES;
	if ( ( l_eRead & READ::D_BUFFERED_READS ) && ( l_eRead & READ::D_UNBUFFERED_READS ) )
		M_THROW ( _ ( "bad buffering setting" ), l_eRead );
	if ( ! ( l_eRead & ( READ::D_BUFFERED_READS | READ::D_UNBUFFERED_READS ) ) )
		l_eRead |= READ::D_BUFFERED_READS;
	if ( ! f_pvHandle )
		M_THROW( _( "no file is opened" ), errno );
	char * l_pcPtr = NULL;
	int long l_iLength = -1;
	if ( l_eRead & READ::D_BUFFERED_READS )
		{
		l_iLength = get_line_length();
		if ( l_iLength )
			{
			if ( a_iMaximumLength && ( l_iLength > a_iMaximumLength ) )
				M_THROW( _( "line too long" ), l_iLength );
			f_oCache.pool_realloc( l_iLength );
			l_pcPtr = f_oCache.raw();
			M_ENSURE( static_cast<int>( ::fread( l_pcPtr,
							sizeof ( char ), l_iLength,
							static_cast<FILE*>( f_pvHandle ) ) ) == l_iLength );
			l_pcPtr[ l_iLength ] = 0;
			a_roLine = l_pcPtr;
			}
		}
	else /* D_UNBUFFERED_READS */
		{
		l_iLength = read_until( a_roLine, HStreamInterface::eols, false ).octets;
		if ( a_iMaximumLength && ( l_iLength > a_iMaximumLength ) )
			M_THROW( _( "line too long" ), l_iLength );
		}
	if ( l_iLength > 0 )
		{
		int long newLen = l_iLength;
		if ( ( l_eRead & READ::D_STRIP_NEWLINES ) && ( newLen > 0 ) )
			{
			-- newLen;
			if ( ( newLen > 0 ) && ( a_roLine[ newLen - 1 ] == '\r' ) )
				-- newLen;
			}
		if ( newLen < l_iLength )
			a_roLine.set_at( newLen, 0 );
		}
	else
		l_iLength = -1;
	return ( l_iLength );
	M_EPILOG
	}

int long HFile::get_line_length( void )
	{
	M_PROLOG
	static int const D_SCAN_BUFFER_SIZE = 8;
	int long l_iLength = 0, l_iSize = 0;
	char l_pcBuffer [ D_SCAN_BUFFER_SIZE ];
	char const * l_pcPtr = NULL;
	do
		{
		l_iSize = ::fread( l_pcBuffer, sizeof ( char ),
				D_SCAN_BUFFER_SIZE, static_cast<FILE*>( f_pvHandle ) );
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
	M_ENSURE( ::fflush( static_cast<FILE*>( f_pvHandle ) ) == 0 );
	return;
	M_EPILOG
	}

bool HFile::operator ! ( void ) const
	{
	M_PROLOG
	return ( ! f_pvHandle );
	M_EPILOG
	}

int long HFile::do_read( void* const a_pcBuffer, int long const& a_lSize )
	{
	M_PROLOG
	return ( ::fread( a_pcBuffer, sizeof ( char ), a_lSize,
				static_cast<FILE*>( f_pvHandle ) ) );
	M_EPILOG
	}

int long HFile::do_write( void const* const a_pcString, int long const& a_lSize )
	{
	M_PROLOG
	return ( ::fwrite( a_pcString, sizeof ( char ), a_lSize, static_cast<FILE*>( f_pvHandle ) ) );
	M_EPILOG
	}

HFile cout( HFile::OPEN::D_WRITING, stdout );

}

}

