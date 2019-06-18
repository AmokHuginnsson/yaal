/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <libintl.h>

#include "config.hxx"

#ifndef HAVE_BASENAME_IN_CSTRING
#	ifdef HAVE_LIBGEN_H
#		include <libgen.h>
#	endif /* #ifdef HAVE_LIBGEN_H */
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

char const* LOG_LEVEL::name( priority_t level_ ) {
	char const* n( "?????" );
	switch ( level_ ) {
		case ( LOG_LEVEL::DEBUG ):     n = "DEBUG";     break;
		case ( LOG_LEVEL::INFO ):      n = "INFO";      break;
		case ( LOG_LEVEL::NOTICE ):    n = "NOTICE";    break;
		case ( LOG_LEVEL::WARNING ):   n = "WARNING";   break;
		case ( LOG_LEVEL::ERROR ):     n = "ERROR";     break;
		case ( LOG_LEVEL::CRITICAL ):  n = "CRITICAL";  break;
		case ( LOG_LEVEL::ALERT ):     n = "ALERT";     break;
		case ( LOG_LEVEL::EMERGENCY ): n = "EMERGENCY"; break;
	}
	return ( n );
}

bool HLog::_autoRehash = true;
LOG_LEVEL::priority_t HLog::_logLevel = LOG_LEVEL::NOTICE;

void* DEFAULT_LOG_STREAM( stderr );

HLog::HLog( void )
	: HField<HFile>( tmpfile(), HFile::OWNERSHIP::ACQUIRED )
	, HSynchronizedStream( _file::ref() )
	, _realMode( false )
	, _newLine( true )
	, _type( LOG_LEVEL::DEBUG )
	, _bufferSize( BUFFER_SIZE )
	, _tag()
	, _buffer( static_cast<int>( _bufferSize )
) {
	M_PROLOG
	if ( ! _file::ref() ) {
		M_THROW( "tmpfile() failed", errno );
	}
	operator()( LOG_LEVEL::NOTICE ) << "Process started." << endl;
	operator()( LOG_LEVEL::NOTICE ) << "yaal version = " << yaal_version( true ) << endl;
	return;
	M_EPILOG
}

HLog::~HLog( void ) {
	M_PROLOG
	if ( _logLevel >= LOG_LEVEL::NOTICE ) {
		if ( _newLine ) {
			timestamp();
		}
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

void HLog::do_rehash( void* src_, char const* processName_ ) {
	M_PROLOG
	M_ASSERT( ! _realMode );
	M_ASSERT( processName_ );
#ifndef HAVE_GETLINE
	char* ptr = nullptr;
#endif /* not HAVE_GETLINE */
	int len( 0 );
	_realMode = true;
	FILE* src( static_cast<FILE*>( src_ ) );
	if ( src ) {
		_tag.assign( " " )
			.append( system::get_user_name( system::get_user_id() ) )
			.append( "@" )
			.append( system::get_host_name() )
			.append( "->" )
			.append( ::basename( processName_ ) )
			.append( "[" )
			.append( system::getpid() )
			.append( "]: " );
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
			_type = static_cast<LOG_LEVEL::priority_t>( ::strtol( buf, nullptr, 0x10 ) );
			if ( _type <= _logLevel ) {
				timestamp();
				static int const TIMESTAMP_LENGTH( 10 );
				_file::ref() << buf + TIMESTAMP_LENGTH;
			}
		}
		len = static_cast<int>( ::strlen( buf ) );
		if ( ( len > 0 ) && ( buf[ len - 1 ] == '\n' ) ) {
			_type = _logLevel;
		}
		::fclose( src );
	}
	do_flush();
	return;
	M_EPILOG
}

void HLog::rehash_stream( void* stream_, char const* processName_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( ! stream_ ) {
		M_THROW( "file parameter is", reinterpret_cast<int long>( stream_ ) );
	}
	void* src( _file::ref().release() );
	_file::ref().open( stream_, HFile::OWNERSHIP::ACQUIRED );
	do_rehash( src, processName_ );
	return;
	M_EPILOG
}

void HLog::rehash( HString const& logFileName_, char const* processName_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( logFileName_.is_empty() ) {
		M_THROW( "new file name argument is empty", logFileName_.get_length() );
	}
	void* src( _file::ref().release() );
	_file::ref().open( logFileName_, HFile::OPEN::WRITING | HFile::OPEN::APPEND );
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
			::snprintf( buffer, TIMESTAMP_SIZE - 1, "%-10x", _type );
			_file::ref() << buffer;
		}
		return;
	}
	time_t currentTime = ::time( nullptr );
	tm* brokenTime = ::localtime( &currentTime );
	::memset( buffer, 0, TIMESTAMP_SIZE );
	/* ISO C++ does not support the `%e' strftime format */
	/* `%e': The day of the month like with `%d', but padded with blank */
	/* (range ` 1' through `31'). */
	/* This format was first standardized by POSIX.2-1992 and by ISO C99.*/
	/* I will have to wait with using `%e'. */
	int long size = static_cast<int long>(
		::strftime( buffer, TIMESTAMP_SIZE, "%b %d %H:%M:%S", brokenTime )
	);
	if ( size > TIMESTAMP_SIZE ) {
		M_THROW( _( "strftime returned more than TIMESTAMP_SIZE" ), size );
	}
	_file::ref() << buffer << _tag;
	if ( _type <= LOG_LEVEL::WARNING ) {
		_file::ref() << "(" << LOG_LEVEL::name( _type ) << ") ";
	}
	return;
	M_EPILOG
}

void HLog::eol_reset( char const* buf_, int long len_ ) {
	M_PROLOG
	if ( ( len_ < 1 ) || ( buf_[ len_ - 1 ] != '\n' ) ) {
		_newLine = false;
	} else {
		_type = _logLevel;
		_newLine = true;
		_file::ref().flush();
	}
	return;
	M_EPILOG
}

int HLog::vformat( char const* format_, va_list ap_ ) {
	M_PROLOG
	int err = 0;
	if ( _newLine )
		timestamp();
	char* buf = _buffer.get<char>();
	::memset( buf, 0, _bufferSize );
	err = ::vsnprintf( buf, _bufferSize, format_, ap_ );
	if ( ! _realMode || ( _type <= _logLevel ) ) {
		_file::ref() << buf;
	}
	eol_reset( buf, static_cast<int>( ::strlen( buf ) ) );
	return ( err );
	M_EPILOG
}

int HLog::operator() ( char const * format_, ... ) {
	M_PROLOG
	int err = 0;
	va_list ap;
	va_start( ap, format_ );
	err = vformat( format_, ap );
	va_end( ap );
	return ( err );
	M_EPILOG
}

int HLog::operator() ( LOG_LEVEL::priority_t type_, char const* format_, ... ) {
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

HLog& HLog::operator() ( LOG_LEVEL::priority_t type_ ) {
	M_PROLOG
	_type = type_;
	return ( *this );
	M_EPILOG
}

HLog& HLog::filter( LOG_LEVEL::priority_t type_ ) {
	M_PROLOG
	_type = type_;
	return ( *this );
	M_EPILOG
}

int long HLog::do_write( void const* string_, int long size_ ) {
	M_PROLOG
	if ( ! string_ )
		return ( 0 );
	int len = 0;
	char const* str = static_cast<char const*>( string_ );
	if ( ! _realMode || ( _type <= _logLevel ) ) {
		if ( _newLine ) {
			timestamp();
		}
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

int long HLog::do_read( void*, int long ) {
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

