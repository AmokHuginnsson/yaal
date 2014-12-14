/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hlog.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <libintl.h>

#include "config.hxx"

#ifndef HAVE_BASENAME_IN_CSTRING
#	include <libgen.h>
#endif /* not HAVE_BASENAME_IN_CSTRING */

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hlog.hxx"
#include "system.hxx"
#include "hcore.hxx"

namespace yaal {

namespace hcore {

namespace {

static int const BUFFER_SIZE		= 1024;
static int const TIMESTAMP_SIZE	= 16;

}

bool HLog::_autoRehash = true;
int long HLog::_logMask = 0;

void* DEFAULT_LOG_STREAM( stderr );

HLog::HLog( void ) : HField<HFile>( tmpfile(), HFile::OWNERSHIP::ACQUIRED ), HSynchronizedStream( _file::ref() ), _realMode( false ), _newLine( true ),
	_type( 0 ), _bufferSize( BUFFER_SIZE ),
	_processName( NULL ),
	_loginName(), _hostName( system::get_host_name() ),
	_buffer( static_cast<int>( _bufferSize ) ) {
	M_PROLOG
	if ( ! _file::ref() )
		M_THROW( "tmpfile() failed", errno );
	HString intro;
	intro.format( "%-10xProcess started (%ld).\n",
			LOG_TYPE::NOTICE, static_cast<int long>( system::getpid() ) );
	_file::ref() << intro;
	return;
	M_EPILOG
}

HLog::~HLog( void ) {
	M_PROLOG
	if ( _logMask & LOG_TYPE::NOTICE ) {
		if ( _newLine )
			timestamp();
		_file::ref() << "Process exited normally.\n";
	}
	if ( ! _realMode ) {
		if ( _autoRehash ) {
			rehash_stream( stderr, PACKAGE_NAME );
		}
	}
	return;
	M_DESTRUCTOR_EPILOG
}

void HLog::disable_auto_rehash( void ) {
	_autoRehash = false;
	return;
}

void HLog::do_rehash( void* src_, char const* const processName_ ) {
	M_PROLOG
	M_ASSERT( ! _realMode );
#ifndef HAVE_GETLINE
	char* ptr = NULL;
	int len = 0;
#endif /* not HAVE_GETLINE */
	_realMode = true;
	if ( processName_ )
		_processName = ::basename( processName_ );
	FILE* src( static_cast<FILE*>( src_ ) );
	if ( src ) {
		_loginName = system::get_user_name( static_cast<int>( getuid() ) );
		::fseek( src, 0, SEEK_SET );
		char* buf = _buffer.get<char>();
#ifdef HAVE_GETLINE
		while ( ::getline( &buf, &_bufferSize, src ) > 0 )
#else /* HAVE_GETLINE */
		while ( ( len = static_cast<int>( ::fread( buf, sizeof ( char ), _bufferSize, src ) ) ) )
#endif /* not HAVE_GETLINE */
		/**/ {
#ifndef HAVE_GETLINE
			ptr = static_cast<char*>( ::memchr( buf, '\n', len ) );
			if ( ! ptr ) {
				_file::ref() << buf;
				continue;
			}
			* ++ ptr = 0;
			::fseek( src, static_cast<int long>( ptr - buf ) - len, SEEK_CUR );
#endif /* not HAVE_GETLINE */
			_type = ::strtol( buf, NULL, 0x10 );
			if ( ! _type || ( _type & _logMask ) ) {
				timestamp();
				static int const TIMESTAMP_LENGTH( 10 );
				_file::ref() << buf + TIMESTAMP_LENGTH;
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

void HLog::rehash_stream( void* stream_,
		char const* const processName_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( ! stream_ )
		M_THROW( "file parameter is", reinterpret_cast<int long>( stream_ ) );
	void* src( _file::ref().release() );
	_file::ref().open( stream_, HFile::OWNERSHIP::ACQUIRED );
	do_rehash( src, processName_ );
	return;
	M_EPILOG
}

void HLog::rehash( HString const& logFileName_,
		char const* const processName_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( logFileName_.is_empty() )
		M_THROW( "new file name argument is", logFileName_.get_length() );
	void* src( _file::ref().release() );
	_file::ref().open( logFileName_, HFile::open_t( HFile::OPEN::WRITING ) | HFile::OPEN::APPEND );
	if ( ! _file::ref() ) {
		_file::ref().open( src, HFile::OWNERSHIP::ACQUIRED );
		HException::disable_logging();
		throw HLogException( _file::ref().get_error() );
	}
	do_rehash( src, processName_ );
	return;
	M_EPILOG
}

void HLog::timestamp( void ) {
	M_PROLOG
	char buffer[ TIMESTAMP_SIZE ];
	if ( ! _realMode ) {
		if ( !! _file::ref() ) {
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
		_file::ref() << buffer << " " << _loginName << "@" << _hostName << "->" << _processName << ": ";
	else
		_file::ref() << buffer << " " << _loginName << "@" << _hostName << ": ";
	if ( _type & LOG_TYPE::WARNING )
		_file::ref() << "(WARNING) ";
	else if ( _type & LOG_TYPE::ERROR )
		_file::ref() << "(ERROR) ";
	return;
	M_EPILOG
}

void HLog::eol_reset( char const* const buf_, int long len_ ) {
	M_PROLOG
	if ( ( len_ < 1 ) || ( buf_[ len_ - 1 ] != '\n' ) )
		_newLine = false;
	else {
		_type = 0;
		_newLine = true;
		_file::ref().flush();
	}
	return;
	M_EPILOG
}

int HLog::vformat( char const* const format_, va_list ap_ ) {
	M_PROLOG
	int err = 0;
	if ( _newLine )
		timestamp();
	char* buf = _buffer.get<char>();
	::memset( buf, 0, _bufferSize );
	err = ::vsnprintf( buf, _bufferSize, format_, ap_ );
	if ( ! ( _type && _realMode ) || ( _type & _logMask ) )
		_file::ref() << buf;
	eol_reset( buf, static_cast<int>( ::strlen( buf ) ) );
	return ( err );
	M_EPILOG
}

int HLog::operator() ( char const * const format_, ... ) {
	M_PROLOG
	int err = 0;
	va_list ap;
	va_start( ap, format_ );
	err = vformat( format_, ap );
	va_end( ap );
	return ( err );
	M_EPILOG
}

int HLog::operator() ( int long const type_,
		char const* const format_, ... ) {
	M_PROLOG
	int err = 0;
	va_list ap;
	_type = type_;
	va_start( ap, format_ );
	err = vformat( format_, ap );
	va_end( ap );
	return ( err );
	M_EPILOG
}

HLog& HLog::operator() ( int long type_ ) {
	M_PROLOG
	_type = type_;
	return ( *this );
	M_EPILOG
}

HLog& HLog::filter( int long type_ ) {
	M_PROLOG
	_type = type_;
	return ( *this );
	M_EPILOG
}

int long HLog::do_write( void const* const string_, int long size_ ) {
	M_PROLOG
	if ( ! string_ )
		return ( 0 );
	int len = 0;
	char const* const str = static_cast<char const* const>( string_ );
	if ( ! ( _type && _realMode ) || ( _type & _logMask ) ) {
		if ( _newLine )
			timestamp();
		len = static_cast<int>( _file::ref().write( str, size_ ) );
	}
	eol_reset( str, size_ );
	return ( len );
	M_EPILOG
}

void HLog::do_flush( void ) {
	M_PROLOG
	_file::ref().flush();
	return;
	M_EPILOG
}

int long HLog::do_read( void* const, int long ) {
	return ( 0 );
}

bool HLog::do_is_valid( void ) const {
	M_PROLOG
	return ( ! _file::ref() );
	M_EPILOG
}

yaal::hcore::HStreamInterface& trace( char const* packageName_, char const* file_, int line_, char const* function_ ) {
	M_PROLOG
	char const* p( strstr( file_, packageName_ ) );
	if ( p ) {
		p += strlen( packageName_ );
		if ( *p == '/' ) {
			++ p;
		}
	} else {
		p = file_;
	}
	return ( log << "trace: " << p << ":" << line_ << ": " << function_ << ": " );
	M_EPILOG
}

}

}

