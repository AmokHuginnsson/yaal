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
	_handle( NULL ), _path(), _error(),
	_external( false )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HFile::HFile( void* const handle_ )
	: HStreamInterface(),
	_handle( handle_ ), _path(), _error(),
	_external( handle_ ? true : false )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HFile::HFile( yaal::hcore::HString const& path, open_t const& open_ )
	: HStreamInterface(),
	_handle( NULL ), _path(), _error(),
	_external( false )
	{
	M_PROLOG
	open( path, open_ );
	return;
	M_EPILOG
	}

HFile::~HFile( void )
	{
	M_PROLOG
	if ( _handle && ! _external )
		close();
	return;
	M_EPILOG
	}

int HFile::open( HString const& path_, open_t const& open_ )
	{
	M_PROLOG
	int saveErrno( errno );
	char const* mode = NULL;
	if ( open_ == OPEN::READING )
		mode = "rb";
	else if ( ( open_ == OPEN::WRITING ) || ( open_ == ( open_t( OPEN::WRITING ) | open_t( OPEN::TRUNCATE ) ) ) )
		mode = "wb";
	else if ( open_ == ( open_t( OPEN::WRITING ) | open_t( OPEN::APPEND ) ) )
		mode = "ab";
	else if ( open_ == ( open_t( OPEN::READING ) | open_t( OPEN::WRITING ) ) )
		mode = "r+b";
	else if ( open_ == ( open_t( OPEN::READING ) | open_t( OPEN::WRITING ) | open_t( OPEN::TRUNCATE ) ) )
		mode = "w+b";
	else if ( open_ == ( open_t( OPEN::READING ) | open_t( OPEN::WRITING ) | open_t( OPEN::APPEND ) ) )
		mode = "a+b";
	else
		M_THROW( "unexpected mode setting", open_.value() );
	M_ENSURE_EX( ! _handle, "stream already opened" );
	_path = path_;
	_external = false;
	_handle = ::std::fopen( path_.raw(), mode );
	int error( 0 );
	if ( ! _handle )
		{
		error = errno;
		_error = error_message( error );
		_error += ": " + path_;
		}
	errno = saveErrno;
	return ( error );
	M_EPILOG
	}

int HFile::open( void* const handle )
	{
	M_PROLOG
	M_ENSURE_EX( ! _handle, "stream already opened" );
	_handle = handle;
	_external = true;
	_path.clear();
	return ( 0 );
	M_EPILOG
	}

int HFile::close( void )
	{
	M_PROLOG
	M_ASSERT( _handle );
	int error = 0;
	error = ::std::fclose( static_cast<FILE*>( _handle ) );
	if ( error )
		{
		_error = error_message( error );
		return ( error );
		}
	_handle = NULL;
	return ( 0 );
	M_EPILOG
	}

void* HFile::release( void )
	{
	M_PROLOG
	M_ASSERT( _handle && _external );
	void* handle( NULL );
	using yaal::swap;
	swap( _handle, handle );
	return ( handle );
	M_EPILOG
	}

int long HFile::tell( void ) const
	{
	M_PROLOG
	M_ASSERT( _handle );
	int long pos = 0;
	M_ENSURE( ( pos = ::std::ftell( static_cast<FILE*>( _handle ) ) ) >= 0 );
	return ( pos );
	M_EPILOG
	}

void HFile::seek( int long pos, seek_t const& seek_ )
	{
	M_PROLOG
	M_ASSERT( _handle );
	int s = 0;
	switch ( seek_.value() )
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
	M_ENSURE( ::std::fseek( static_cast<FILE*>( _handle ), pos, s ) == 0 );
	return;
	M_EPILOG
	}

int long HFile::read_line( HString& line_, read_t const& read_,
		int const maximumLength_ )
	{
	M_PROLOG
	M_ASSERT( _handle );
	read_t readMode = read_;
	if ( ( !!( readMode & READ::KEEP_NEWLINES ) ) && ( !!( readMode & READ::STRIP_NEWLINES ) ) )
		M_THROW( _( "bad newlines setting" ), readMode.value() );
	if ( ! ( ( !!( readMode & READ::KEEP_NEWLINES ) ) || ( !!( readMode & READ::STRIP_NEWLINES ) ) ) )
		readMode |= READ::KEEP_NEWLINES;
	if ( ( !!( readMode & READ::BUFFERED_READS ) ) && ( !!( readMode & READ::UNBUFFERED_READS ) ) )
		M_THROW( _( "bad buffering setting" ), readMode.value() );
	if ( ! ( ( !!( readMode & READ::BUFFERED_READS ) ) || ( !!( readMode & READ::UNBUFFERED_READS ) ) ) )
		readMode |= READ::BUFFERED_READS;
	if ( ! _handle )
		M_THROW( _( "no file is opened" ), errno );
	char * ptr = NULL;
	int long length = -1;
	if ( !!( readMode & READ::BUFFERED_READS ) )
		{
		length = get_line_length();
		if ( length )
			{
			if ( maximumLength_ && ( length > maximumLength_ ) )
				M_THROW( _( "line too long" ), length );
			ptr = static_cast<char*>( _cache.realloc( length ) );
			M_ENSURE( static_cast<int>( ::std::fread( ptr,
							sizeof ( char ), length,
							static_cast<FILE*>( _handle ) ) ) == length );
			ptr[ length ] = 0;
			line_ = ptr;
			length = line_.get_length();
			}
		}
	else /* UNBUFFERED_READS */
		{
		length = read_until( line_, HStreamInterface::eols, false );
		if ( maximumLength_ && ( length > maximumLength_ ) )
			M_THROW( _( "line too long" ), length );
		}
	if ( length > 0 )
		{
		int long newLen = length;
		if ( ( !!( readMode & READ::STRIP_NEWLINES ) ) && ( newLen > 0 ) )
			{
			-- newLen;
			if ( ( newLen > 0 ) && ( line_[ newLen - 1 ] == '\r' ) )
				-- newLen;
			}
		if ( newLen < length )
			line_.set_at( newLen, 0 );
		}
	else
		length = -1;
	return ( length );
	M_EPILOG
	}

int long HFile::get_line_length( void )
	{
	M_PROLOG
	M_ASSERT( _handle );
	static int const SCAN_BUFFER_SIZE = 8;
	int long length = 0, size = 0;
	char buffer[ SCAN_BUFFER_SIZE ];
	char const* ptr = NULL;
	do
		{
		size = static_cast<int long>( ::std::fread( buffer, sizeof ( char ),
			SCAN_BUFFER_SIZE, static_cast<FILE*>( _handle ) ) );
		length += size;
		ptr = static_cast<char*>( ::std::memchr( buffer,
					'\n', size ) );
		}
	while ( ! ptr && ( size == SCAN_BUFFER_SIZE ) );
	M_ENSURE( ::std::fseek( static_cast<FILE*>( _handle ),
				- length, SEEK_CUR ) == 0 );
	if ( ptr )
		length -= ( size - static_cast<int long>( ptr + 1 - buffer ) ); /* + 1 for \n */
	return ( length );
	M_EPILOG
	}

HString const& HFile::get_path( void ) const
	{
	M_PROLOG
	return ( _path );
	M_EPILOG
	}

HString const& HFile::get_error( void ) const
	{
	M_PROLOG
	return ( _error );
	M_EPILOG
	}

void HFile::do_flush( void ) const
	{
	M_PROLOG
	M_ASSERT( _handle );
	M_ENSURE( ::std::fflush( static_cast<FILE*>( _handle ) ) == 0 );
	return;
	M_EPILOG
	}

bool HFile::operator ! ( void ) const
	{
	M_PROLOG
	return ( ! _handle );
	M_EPILOG
	}

int long HFile::do_read( void* const buffer_, int long size_ )
	{
	M_PROLOG
	int long len( static_cast<int long>( ::std::fread( buffer_, sizeof ( char ), size_, static_cast<FILE*>( _handle ) ) ) );
	return ( len ? len : ( ::std::ferror( static_cast<FILE*>( _handle ) ) ? -1 : len ) );
	M_EPILOG
	}

int long HFile::do_write( void const* const string_, int long size_ )
	{
	M_PROLOG
	M_ASSERT( _handle );
	int long len( static_cast<int long>( ::std::fwrite( string_, sizeof ( char ), size_, static_cast<FILE*>( _handle ) ) ) );
	return ( len ? len : ( ::std::ferror( static_cast<FILE*>( _handle ) ) ? -1 : len ) );
	M_EPILOG
	}

bool HFile::do_is_valid( void ) const
	{
	M_PROLOG
	return ( _handle != NULL );
	M_EPILOG
	}

HFile cinInstance( stdin );
HSynchronizedStream cin( cinInstance );
HSynchronizedStream cout( HStreamInterface::ptr_t( new HFile( stdout ) ) );
HFile cerrInstance( stderr );
HSynchronizedStream cerr( cerrInstance );
HSynchronizedStream clog;

}

}

