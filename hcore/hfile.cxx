/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hfile.hxx"
#include "hcore/memory.hxx"

namespace yaal {

namespace hcore {

namespace {
static char const* const _error_ = _( "file is not opened" );
HFile::open_t const NONE = HFile::open_t::new_flag();
}

HFile::open_t const HFile::OPEN::READING = HFile::open_t::new_flag();
HFile::open_t const HFile::OPEN::WRITING = HFile::open_t::new_flag();
HFile::open_t const HFile::OPEN::APPEND = HFile::open_t::new_flag();
HFile::open_t const HFile::OPEN::TRUNCATE = HFile::open_t::new_flag();

HFile::HFile( void )
	: HStreamInterface()
	, _handle( nullptr )
	, _path()
	, _error()
	, _ownership( OWNERSHIP::NONE ) {
	M_PROLOG
	return;
	M_EPILOG
}

HFile::HFile( void* handle_, OWNERSHIP ownership_ )
	: HStreamInterface()
	, _handle( handle_ )
	, _path()
	, _error()
	, _ownership( handle_ ? ownership_ : OWNERSHIP::NONE ) {
	M_PROLOG
	return;
	M_EPILOG
}

HFile::HFile( yaal::hcore::HString const& path, open_t const& open_ )
	: HStreamInterface()
	, _handle( nullptr )
	, _path()
	, _error()
	, _ownership( OWNERSHIP::ACQUIRED ) {
	M_PROLOG
	try {
		open( path, open_ );
	} catch ( ... ) {
		if ( _handle ) {
			::fclose( static_cast<FILE*>( _handle ) );
			_handle = nullptr;
			_ownership = OWNERSHIP::NONE;
		}
		throw;
	}
	return;
	M_EPILOG
}

HFile::~HFile( void ) {
	M_PROLOG
	if ( _handle && ( _ownership == OWNERSHIP::ACQUIRED ) ) {
		close();
	}
	return;
	M_DESTRUCTOR_EPILOG
}

int HFile::open( HString const& path_, open_t const& open_ ) {
	M_PROLOG
	return ( do_open( path_, open_ ) );
	M_EPILOG
}

int HFile::do_open( HString const& path_, open_t const& open_ ) {
	M_PROLOG
	M_ENSURE( ! _handle, "stream already opened" );
	char const* mode( nullptr );
	if ( open_ == OPEN::READING ) {
		mode = "rb";
	} else if ( ( open_ == OPEN::WRITING ) || ( open_ == ( OPEN::WRITING | OPEN::TRUNCATE ) ) ) {
		mode = "wb";
	} else if ( open_ == ( OPEN::WRITING | OPEN::APPEND ) ) {
		mode = "ab";
	} else if ( open_ == ( OPEN::READING | OPEN::WRITING ) ) {
		mode = "r+b";
	} else if ( open_ == ( OPEN::READING | OPEN::WRITING | OPEN::TRUNCATE ) ) {
		mode = "w+b";
	} else if ( open_ == ( OPEN::READING | OPEN::WRITING | OPEN::APPEND ) ) {
		mode = "a+b";
	} else {
		M_THROW( "unexpected mode setting", open_.value() );
	}
	int saveErrno( errno );
	_path = path_;
	HUTF8String utf8( path_ );
	_handle = ::std::fopen( utf8.c_str(), mode );
	int error( 0 );
	if ( ! _handle ) {
		saveErrno = error = errno;
		_error = error_message( error );
		_error += ": " + path_;
	} else {
		int fd( ::fileno( static_cast<FILE*>( _handle ) ) );
		M_ENSURE( fd >= 0 );
		system::set_close_on_exec( fd );
		_ownership = OWNERSHIP::ACQUIRED;
	}
	errno = saveErrno;
	return ( error );
	M_EPILOG
}

int HFile::open( void* handle_, OWNERSHIP ownership_ ) {
	M_PROLOG
	M_ENSURE( ! _handle, "stream already opened" );
	M_ENSURE( handle_ || ( ownership_ == OWNERSHIP::NONE ) );
	_handle = handle_;
	_ownership = ownership_;
	_path.clear();
	return ( 0 );
	M_EPILOG
}

int HFile::close( void ) {
	M_PROLOG
	return ( do_close() );
	M_EPILOG
}

int HFile::do_close( void ) {
	M_PROLOG
	M_ENSURE( _handle && ( _ownership == OWNERSHIP::ACQUIRED ) );
	int error( ::std::fclose( static_cast<FILE*>( _handle ) ) );
	if ( error ) {
		_error = error_message( error );
	} else {
		_handle = nullptr;
		_ownership = OWNERSHIP::NONE;
	}
	return ( error );
	M_EPILOG
}

void* HFile::release( void ) {
	M_PROLOG
	M_ENSURE( _handle );
	void* handle( nullptr );
	using yaal::swap;
	swap( _handle, handle );
	_ownership = OWNERSHIP::NONE;
	return ( handle );
	M_EPILOG
}

int long HFile::tell( void ) const {
	M_PROLOG
	M_ASSERT( _handle );
	int long pos( 0 );
	M_ENSURE( ( pos = ::std::ftell( static_cast<FILE*>( _handle ) ) ) >= 0 );
	return ( pos );
	M_EPILOG
}

void HFile::do_seek( int long pos, SEEK seek_ ) {
	M_PROLOG
	M_ASSERT( _handle );
	int s( 0 );
	switch ( seek_ ) {
		case ( SEEK::BEGIN ): {
			s = SEEK_SET;
		} break;
		case ( SEEK::CURRENT ): {
			s = SEEK_CUR;
		} break;
		case ( SEEK::END ): {
			s = SEEK_END;
		} break;
		default: {
			M_ASSERT( 0 && "bad seek type" );
		} break;
	}
	M_ENSURE( ::std::fseek( static_cast<FILE*>( _handle ), pos, s ) == 0 );
	return;
	M_EPILOG
}

int long HFile::read_line( HString& line_, READ read_,
		int const maximumLength_ ) {
	M_PROLOG
	M_ASSERT( _handle );
	READ readMode( read_ );
	if ( readMode == READ::DEFAULTS ) {
		readMode = READ::BUFFERED_READS;
	}
	M_ENSURE( _handle, _error_ );
	int long length( -1 );
	if ( readMode == READ::BUFFERED_READS ) {
		length = get_line_length();
		if ( length ) {
			if ( maximumLength_ && ( length > maximumLength_ ) ) {
				M_THROW( _( "line too long" ), length );
			}
			char* ptr( static_cast<char*>( _cache.realloc( length ) ) );
			M_ENSURE( read( ptr, length ) == length );
			ptr[ length - 1 ] = 0;
			line_ = ptr;
		}
	} else /* UNBUFFERED_READS */ {
		length = read_until( line_, "\n", true );
		if ( maximumLength_ && ( length > maximumLength_ ) ) {
			M_THROW( _( "line too long" ), length );
		}
	}
	if ( length > 0 ) {
		if ( ( length > 1 ) && ( line_[ length - 2 ] == '\r' ) ) {
			line_.set_at( length - 2, 0_ycp );
		}
	} else {
		length = -1;
	}
	return ( length );
	M_EPILOG
}

int long HFile::get_line_length( void ) {
	M_PROLOG
	M_ASSERT( _handle );
	static int const SCAN_BUFFER_SIZE( 8 );
	char buffer[ SCAN_BUFFER_SIZE ];
	char const* ptr( nullptr );
	int long length( 0 );
	int long size( 0 );
	do {
		size = static_cast<int long>( ::std::fread( buffer, sizeof ( char ),
			SCAN_BUFFER_SIZE, static_cast<FILE*>( _handle ) ) );
		length += size;
		ptr = static_cast<char*>( ::std::memchr( buffer, '\n', static_cast<size_t>( size ) ) );
		if ( ::std::memchr( buffer, 0, static_cast<size_t>( size ) ) != nullptr ) {
			throw HFileException( "binary data in file" );
		}
	} while ( ! ptr && ( size == SCAN_BUFFER_SIZE ) );
	M_ENSURE( ::std::fseek( static_cast<FILE*>( _handle ), -length, SEEK_CUR ) == 0 );
	if ( ptr ) {
		length -= ( size - static_cast<int long>( ptr + 1 - buffer ) ); /* + 1 for \n */
	}
	return ( length );
	M_EPILOG
}

HString const& HFile::get_path( void ) const {
	M_PROLOG
	return ( _path );
	M_EPILOG
}

HString const& HFile::get_error( void ) const {
	M_PROLOG
	return ( _error );
	M_EPILOG
}

void HFile::do_flush( void ) {
	M_PROLOG
	M_ENSURE( _handle, _error_ );
	M_ENSURE( ::std::fflush( static_cast<FILE*>( _handle ) ) == 0 );
	return;
	M_EPILOG
}

bool HFile::is_opened( void ) const {
	M_PROLOG
	return ( _handle != nullptr );
	M_EPILOG
}

HFile::operator bool ( void ) const {
	M_PROLOG
	return ( is_opened() );
	M_EPILOG
}

int HFile::get_file_descriptor( void ) const {
	M_PROLOG
	int fd( -1 );
	if ( is_opened() ) {
		M_ENSURE( ( fd = fileno( static_cast<FILE*>( _handle ) ) ) >= 0 );
	}
	return ( fd );
	M_EPILOG
}

int long HFile::do_read( void* buffer_, int long size_ ) {
	M_PROLOG
	M_ENSURE( _handle, _error_ );
	int long len( static_cast<int long>( ::std::fread( buffer_, sizeof ( char ), static_cast<size_t>( size_ ), static_cast<FILE*>( _handle ) ) ) );
	return ( len ? len : ( ::std::ferror( static_cast<FILE*>( _handle ) ) ? -1 : len ) );
	M_EPILOG
}

int long HFile::do_write( void const* string_, int long size_ ) {
	M_PROLOG
	M_ENSURE( _handle, _error_ );
	int long len( static_cast<int long>( ::std::fwrite( string_, sizeof ( char ), static_cast<size_t>( size_ ), static_cast<FILE*>( _handle ) ) ) );
	return ( len ? len : ( ::std::ferror( static_cast<FILE*>( _handle ) ) ? -1 : len ) );
	M_EPILOG
}

bool HFile::do_is_valid( void ) const {
	M_PROLOG
	return ( _handle != nullptr );
	M_EPILOG
}

HStreamInterface::POLL_TYPE HFile::do_poll_type( void ) const {
	M_PROLOG
	return ( is_valid() ? POLL_TYPE::NATIVE : POLL_TYPE::INVALID );
	M_EPILOG
}

void const* HFile::do_data( void ) const {
	M_PROLOG
	int_native_t fd( get_file_descriptor() );
	return ( is_valid() ? reinterpret_cast<void const*>( fd ) : memory::INVALID_HANDLE );
	M_EPILOG
}

HFile cinInstance( stdin, HFile::OWNERSHIP::ACQUIRED );
HSynchronizedStream cin( cinInstance );
HSynchronizedStream cout( make_pointer<HFile>( stdout, HFile::OWNERSHIP::ACQUIRED ) );
HFile cerrInstance( stderr, HFile::OWNERSHIP::ACQUIRED );
HSynchronizedStream cerr( cerrInstance );
HSynchronizedStream clog;

}

}

