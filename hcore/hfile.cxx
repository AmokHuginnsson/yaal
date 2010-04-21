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
M_VCSID( "$Id: "__TID__" $" )
#include "hfile.hxx"

namespace yaal
{

namespace hcore
{

HFile::HFile( void )
	: HStreamInterface(),
	f_pvHandle( NULL ), f_oPath(), f_oError(),
	f_bExternal( false )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HFile::HFile( void* const a_pvHandle )
	: HStreamInterface(),
	f_pvHandle( a_pvHandle ), f_oPath(), f_oError(),
	f_bExternal( a_pvHandle ? true : false )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HFile::HFile( yaal::hcore::HString const& path, open_t const& a_eOpen )
	: HStreamInterface(),
	f_pvHandle( NULL ), f_oPath(), f_oError(),
	f_bExternal( false )
	{
	M_PROLOG
	open( path, a_eOpen );
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

int HFile::open( HString const& a_oPath, open_t const& open_ )
	{
	M_PROLOG
	int l_iError = 0;
	char const* l_pcMode = NULL;
	if ( open_ == OPEN::READING )
		l_pcMode = "rb";
	else if ( ( open_ == OPEN::WRITING ) || ( open_ == ( open_t( OPEN::WRITING ) | open_t( OPEN::TRUNCATE ) ) ) )
		l_pcMode = "wb";
	else if ( open_ == ( open_t( OPEN::WRITING ) | open_t( OPEN::APPEND ) ) )
		l_pcMode = "ab";
	else if ( open_ == ( open_t( OPEN::READING ) | open_t( OPEN::WRITING ) ) )
		l_pcMode = "r+b";
	else if ( open_ == ( open_t( OPEN::READING ) | open_t( OPEN::WRITING ) | open_t( OPEN::TRUNCATE ) ) )
		l_pcMode = "w+b";
	else if ( open_ == ( open_t( OPEN::READING ) | open_t( OPEN::WRITING ) | open_t( OPEN::APPEND ) ) )
		l_pcMode = "a+b";
	else
		M_THROW( "unexpected mode setting", open_.value() );
	f_oPath = a_oPath;
	M_ENSURE_EX( ! f_pvHandle, "stream already opened" );
	f_pvHandle = ::std::fopen( a_oPath.raw(), l_pcMode );
	if ( ! f_pvHandle )
		{
		l_iError = errno;
		f_oError = error_message( l_iError );
		f_oError += ": " + a_oPath;
		return ( l_iError );
		}
	return ( 0 );
	M_EPILOG
	}

int HFile::open( void* const handle )
	{
	M_PROLOG
	M_ENSURE_EX( ! f_pvHandle, "stream already opened" );
	f_pvHandle = handle;
	f_bExternal = true;
	return ( 0 );
	M_EPILOG
	}

int HFile::close( void )
	{
	M_PROLOG
	M_ASSERT( f_pvHandle );
	int l_iError = 0;
	l_iError = ::std::fclose( static_cast<FILE*>( f_pvHandle ) );
	if ( l_iError )
		{
		f_oError = error_message( l_iError );
		return ( l_iError );
		}
	f_pvHandle = NULL;
	return ( 0 );
	M_EPILOG
	}

void* HFile::release( void )
	{
	M_PROLOG
	M_ASSERT( f_pvHandle && f_bExternal );
	void* handle( NULL );
	using yaal::swap;
	swap( f_pvHandle, handle );
	return ( handle );
	M_EPILOG
	}

int long HFile::tell( void ) const
	{
	M_PROLOG
	M_ASSERT( f_pvHandle );
	int long pos = 0;
	M_ENSURE( ( pos = ::std::ftell( static_cast<FILE*>( f_pvHandle ) ) ) >= 0 );
	return ( pos );
	M_EPILOG
	}

void HFile::seek( int long const& pos, seek_t const& a_eSeek )
	{
	M_PROLOG
	M_ASSERT( f_pvHandle );
	int s = 0;
	switch ( a_eSeek.value() )
		{
		case ( SEEK::SET ):
			s = SEEK_SET;
		break;
		case ( SEEK::CUR ):
			s = SEEK_CUR;
		break;
		case ( SEEK::END ):
			s = SEEK_END;
		break;
		default:
			M_ASSERT( ! "bad seek type" );
		break;
		}
	M_ENSURE( ::std::fseek( static_cast<FILE*>( f_pvHandle ), pos, s ) == 0 );
	return;
	M_EPILOG
	}

int long HFile::read_line( HString& a_roLine, read_t const& a_eRead,
		int const a_iMaximumLength )
	{
	M_PROLOG
	M_ASSERT( f_pvHandle );
	read_t l_eRead = a_eRead;
	if ( ( !!( l_eRead & READ::KEEP_NEWLINES ) ) && ( !!( l_eRead & READ::STRIP_NEWLINES ) ) )
		M_THROW( _( "bad newlines setting" ), l_eRead.value() );
	if ( ! ( ( !!( l_eRead & READ::KEEP_NEWLINES ) ) || ( !!( l_eRead & READ::STRIP_NEWLINES ) ) ) )
		l_eRead |= READ::KEEP_NEWLINES;
	if ( ( !!( l_eRead & READ::BUFFERED_READS ) ) && ( !!( l_eRead & READ::UNBUFFERED_READS ) ) )
		M_THROW( _( "bad buffering setting" ), l_eRead.value() );
	if ( ! ( ( !!( l_eRead & READ::BUFFERED_READS ) ) || ( !!( l_eRead & READ::UNBUFFERED_READS ) ) ) )
		l_eRead |= READ::BUFFERED_READS;
	if ( ! f_pvHandle )
		M_THROW( _( "no file is opened" ), errno );
	char * l_pcPtr = NULL;
	int long l_iLength = -1;
	if ( !!( l_eRead & READ::BUFFERED_READS ) )
		{
		l_iLength = get_line_length();
		if ( l_iLength )
			{
			if ( a_iMaximumLength && ( l_iLength > a_iMaximumLength ) )
				M_THROW( _( "line too long" ), l_iLength );
			l_pcPtr = static_cast<char*>( f_oCache.realloc( l_iLength ) );
			M_ENSURE( static_cast<int>( ::std::fread( l_pcPtr,
							sizeof ( char ), l_iLength,
							static_cast<FILE*>( f_pvHandle ) ) ) == l_iLength );
			l_pcPtr[ l_iLength ] = 0;
			a_roLine = l_pcPtr;
			}
		}
	else /* UNBUFFERED_READS */
		{
		l_iLength = read_until( a_roLine, HStreamInterface::eols, false );
		if ( a_iMaximumLength && ( l_iLength > a_iMaximumLength ) )
			M_THROW( _( "line too long" ), l_iLength );
		}
	if ( l_iLength > 0 )
		{
		int long newLen = l_iLength;
		if ( ( !!( l_eRead & READ::STRIP_NEWLINES ) ) && ( newLen > 0 ) )
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
	M_ASSERT( f_pvHandle );
	static int const SCAN_BUFFER_SIZE = 8;
	int long l_iLength = 0, l_iSize = 0;
	char l_pcBuffer[ SCAN_BUFFER_SIZE ];
	char const* l_pcPtr = NULL;
	do
		{
		l_iSize = ::std::fread( l_pcBuffer, sizeof ( char ),
				SCAN_BUFFER_SIZE, static_cast<FILE*>( f_pvHandle ) );
		l_iLength += l_iSize;
		l_pcPtr = static_cast<char*>( ::std::memchr( l_pcBuffer,
					'\n', l_iSize ) );
		}
	while ( ! l_pcPtr && ( l_iSize == SCAN_BUFFER_SIZE ) );
	M_ENSURE( ::std::fseek( static_cast<FILE*>( f_pvHandle ),
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

void HFile::do_flush( void ) const
	{
	M_PROLOG
	M_ASSERT( f_pvHandle );
	M_ENSURE( ::std::fflush( static_cast<FILE*>( f_pvHandle ) ) == 0 );
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
	int long len( ::std::fread( a_pcBuffer, sizeof ( char ), a_lSize, static_cast<FILE*>( f_pvHandle ) ) );
	return ( len ? len : ( ::std::ferror( static_cast<FILE*>( f_pvHandle ) ) ? -1 : len ) );
	M_EPILOG
	}

int long HFile::do_write( void const* const a_pcString, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( f_pvHandle );
	int long len( ::std::fwrite( a_pcString, sizeof ( char ), a_lSize, static_cast<FILE*>( f_pvHandle ) ) );
	return ( len ? len : ( ::std::ferror( static_cast<FILE*>( f_pvHandle ) ) ? -1 : len ) );
	M_EPILOG
	}

bool HFile::do_is_valid( void ) const
	{
	M_PROLOG
	return ( f_pvHandle != NULL );
	M_EPILOG
	}

HFile cinInstance( stdin, HFile::OPEN::READING );
HSynchronizedFile cin( cinInstance );
HFile coutInstance( stdout, HFile::OPEN::WRITING );
HSynchronizedFile cout( coutInstance );
HFile cerrInstance( stderr, HFile::OPEN::WRITING );
HSynchronizedFile cerr( cerrInstance );
HFile clogInstance( stderr, HFile::OPEN::WRITING );
HSynchronizedFile clog( clogInstance );


}

}

