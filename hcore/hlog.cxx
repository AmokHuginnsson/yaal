/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hlog.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <pwd.h>
#include <libintl.h>

#include "config.hxx"

#ifndef HAVE_BASENAME_IN_CSTRING
#	include <libgen.h>
#endif /* not HAVE_BASENAME_IN_CSTRING */

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hlog.hxx"
#include "system.hxx"

namespace yaal
{

namespace hcore
{

namespace
{

static int const BUFFER_SIZE		= 1024;
static int const HOSTNAME_SIZE	= 128;
static int const TIMESTAMP_SIZE	= 16;
static int const GETPW_R_SIZE   = 1024;

}

int long HLog::_logMask = 0;

void* DEFAULT_LOG_STREAM( stderr );

HLog::HLog( void ) : HField<HFile>( tmpfile() ), HSynchronizedFile( _file::ref() ), _realMode( false ), _newLine( true ),
	_type( 0 ), _bufferSize( BUFFER_SIZE ),
	_processName( NULL ),
	_loginName(), _hostName( HOSTNAME_SIZE ),
	_buffer( _bufferSize )
	{
	M_PROLOG
	if ( ! _file::ref() )
		M_THROW( "tmpfile() failed", errno );
	HString intro;
	intro.format( "%-10xProcess started (%ld).\n",
			LOG_TYPE::NOTICE, static_cast<int long>( get_pid() ) );
	_file::ref() << intro;
	uid_t uid( getuid() );
	passwd accountInfo;
	long int bsize = ::sysconf( _SC_GETPW_R_SIZE_MAX );
	HChunk buf( bsize > 0 ? bsize + 1 : GETPW_R_SIZE );
	passwd* any;
	if ( ! getpwuid_r( uid, &accountInfo, buf.get<char>(), static_cast<int>( bsize ), &any ) )
		_loginName = accountInfo.pw_name;
	else
		_loginName = uid;
	M_ENSURE( ::gethostname( _hostName.get<char>(), HOSTNAME_SIZE - 1 ) == 0 );
	return;
	M_EPILOG
	}

HLog::~HLog( void )
	{
	M_PROLOG
	if ( _logMask & LOG_TYPE::NOTICE )
		{
		if ( _newLine )
			timestamp();
		_file::ref() << "Process exited normally.\n";
		}
	return;
	M_EPILOG
	}

void HLog::do_rehash( void* src_, char const* const processName_ )
	{
	M_PROLOG
#ifndef HAVE_GETLINE
	char* ptr = NULL;
	int len = 0;
#endif /* not HAVE_GETLINE */
	_realMode = true;
	if ( processName_ )
		_processName = ::basename( processName_ );
	FILE* src( static_cast<FILE*>( src_ ) );
	if ( src )
		{
		::fseek( src, 0, SEEK_SET );
		char* buf = _buffer.get<char>();
#ifdef HAVE_GETLINE
    while ( ::getline( &buf, &_bufferSize, src ) > 0 )
#else /* HAVE_GETLINE */
    while ( ( len = static_cast<int>( ::fread( buf, sizeof ( char ), _bufferSize, src ) ) ) )
#endif /* not HAVE_GETLINE */
      {
#ifndef HAVE_GETLINE
      ptr = static_cast<char*>( ::memchr( buf, '\n', len ) );
      if ( ! ptr )
        {
       	_file::ref() << buf;
        continue;
        }
      * ++ ptr = 0;
      ::fseek( src, static_cast<int long>( ptr - buf ) - len, SEEK_CUR );
#endif /* not HAVE_GETLINE */
			_type = ::strtol( buf, NULL, 0x10 );
			if ( ! ( _type && _realMode ) || ( _type & _logMask ) )
				{
				timestamp();
				_file::ref() << buf + 10;
				}
			}
		if ( buf[ ::strlen( buf ) - 1 ] == '\n' )
			_type = 0;
		::fclose( src );
		}
	do_flush();
	return;
	M_EPILOG
	}

void HLog::rehash( void* stream_,
		char const* const processName_ )
	{
	M_PROLOG
	HLock l( _mutex );
	if ( ! stream_ )
		M_THROW( "file parameter is", reinterpret_cast<int long>( stream_ ) );
	void* src( _file::ref().release() );
	_file::ref().open( stream_ );
	do_rehash( src, processName_ );
	return;
	M_EPILOG
	}

void HLog::rehash( HString const& logFileName_,
		char const* const processName_ )
	{
	M_PROLOG
	HLock l( _mutex );
	if ( logFileName_.is_empty() )
		M_THROW( "new file name argument is", logFileName_.get_length() );
	void* src( _file::ref().release() );
	_file::ref().open( logFileName_, HFile::open_t( HFile::OPEN::WRITING ) | HFile::OPEN::APPEND );
	do_rehash( src, processName_ );
	return;
	M_EPILOG
	}

void HLog::timestamp( void )
	{
	M_PROLOG
	char buffer[ TIMESTAMP_SIZE ];
	if ( ! _realMode )
		{
		if ( !! _file::ref() )
			{
			::snprintf( buffer, TIMESTAMP_SIZE - 1, "%-10lx", _type );
			_file::ref() << buffer;
			}
		return;
		}
	time_t currentTime = ::time( NULL );
	tm* brokenTime = ::localtime( &currentTime );
	::memset( buffer, 0, TIMESTAMP_SIZE );
	/* ISO C++ does not support the `%e' strftime format */
	/* `%e': The day of the month like with `%d', but padded with blank */
	/* (range ` 1' through `31'). */
	/* This format was first standardized by POSIX.2-1992 and by ISO C99.*/
	/* I will have to wait with using `%e'. */
	int long size = static_cast<int long>( ::strftime( buffer, TIMESTAMP_SIZE, "%b %d %H:%M:%S",
			brokenTime ) );
	if ( size > TIMESTAMP_SIZE )
		M_THROW( _( "strftime returned more than TIMESTAMP_SIZE" ), size );
	if ( _processName )
		_file::ref() << buffer << " " << _loginName << "@" << _hostName.get<char>() << "->" << _processName << ": ";
	else
		_file::ref() << buffer << " " << _loginName << "@" << _hostName.get<char>() << ": ";
	return;
	M_EPILOG
	}

int HLog::operator() ( char const* const format_, va_list ap_ )
	{
	M_PROLOG
	int err = 0;
	if ( _newLine )
		timestamp();
	char* buf = _buffer.get<char>();
	::memset( buf, 0, _bufferSize );
	err = ::vsnprintf( buf, _bufferSize, format_, ap_ );
	_file::ref() << buf;
	if ( buf[ ::strlen( buf ) - 1 ] != '\n' )
		_newLine = false;
	else
		{
		_type = 0;
		_newLine = true;
		_file::ref().flush();
		}
	return ( err );
	M_EPILOG
	}

int HLog::operator() ( char const * const format_, ... )
	{
	M_PROLOG
	int err = 0;
	va_list ap;
	if ( ! ( _type && _realMode ) || ( _type & _logMask ) )
		{
		va_start( ap, format_ );
		err = ( *this )( format_, ap );
		va_end( ap );
		}
	return ( err );
	M_EPILOG
	}

int HLog::operator() ( int long const type_,
		char const* const format_, ... )
	{
	M_PROLOG
	int err = 0;
	va_list ap;
	_type = type_;
	if ( ! ( _type && _realMode ) || ( _type & _logMask ) )
		{
		va_start( ap, format_ );
		err = ( *this )( format_, ap );
		va_end( ap );
		}
	return ( err );
	M_EPILOG
	}

HLog& HLog::operator() ( int long const& type_ )
	{
	M_PROLOG
	_type = type_;
	return ( *this );
	M_EPILOG
	}

HLog& HLog::filter( int long const& type_ )
	{
	M_PROLOG
	_type = type_;
	return ( *this );
	M_EPILOG
	}

int long HLog::do_write( void const* const string_, int long const& size_ )
	{
	M_PROLOG
	if ( ! string_ )
		return ( 0 );
	int len = 0;
	char const* const str = static_cast<char const* const>( string_ );
	if ( ! ( _type && _realMode ) || ( _type & _logMask ) )
		{
		if ( _newLine )
			timestamp();
		len = static_cast<int>( _file::ref().write( str, size_ ) );
		if ( str[ size_ - 1 ] != '\n' )
			_newLine = false;
		else
			{
			_newLine = true;
			_type = 0;
			_file::ref().flush();
			}
		}
	return ( len );
	M_EPILOG
	}

void HLog::do_flush( void ) const
	{
	M_PROLOG
	_file::ref().flush();
	return;
	M_EPILOG
	}

int long HLog::do_read( void* const, int long const& )
	{
	return ( 0 );
	}

bool HLog::do_is_valid( void ) const
	{
	M_PROLOG
	return ( ! _file::ref() );
	M_EPILOG
	}

}

}

