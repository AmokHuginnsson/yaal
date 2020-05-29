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
#include "hcore/system.hxx"
#include "hcore/hcore.hxx"

using namespace yaal::hcore::system;

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

HFile::HFile( yaal::hcore::HString const& path_, open_t open_ )
	: HStreamInterface()
	, _handle( nullptr )
	, _path()
	, _error()
	, _ownership( OWNERSHIP::ACQUIRED ) {
	M_PROLOG
	try {
		open( path_, open_ );
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

int HFile::open( HString const& path_, open_t open_ ) {
	M_PROLOG
	return ( do_open( path_, open_ ) );
	M_EPILOG
}

int HFile::do_open( HString const& path_, open_t open_ ) {
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
	bytes_t bytes( string_to_bytes( path_ ) );
	_handle = ::std::fopen( bytes.data(), mode );
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
	reset();
	return ( error );
	M_EPILOG
}

void HFile::do_clear( void ) {
	M_PROLOG
	if ( _handle ) {
		clearerr( static_cast<FILE*>( _handle ) );
	}
	HStreamInterface::do_clear();
	return;
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

HFile& HFile::read_line( HString& line_, READ read_, int maximumLength_ ) {
	M_PROLOG
	M_ASSERT( _handle );
	READ readMode( read_ );
	if ( readMode == READ::DEFAULTS ) {
		readMode = READ::BUFFERED_READS;
	}
	M_ENSURE( _handle, _error_ );
	int length( -1 );
	if ( readMode == READ::BUFFERED_READS ) {
		int bufferSize( max( maximumLength_, system::get_page_size() ) );
		char* nl( nullptr );
		char* ptr( _cache.get<char>() );
		if ( _cachedBytes > 0 ) {
			nl = static_cast<char*>( memchr( ptr, '\n', static_cast<size_t>( _cachedBytes ) ) );
		}
		while ( ! nl ) {
			ptr = static_cast<char*>( _cache.realloc( bufferSize ) );
			int nRead( static_cast<int>( do_read( ptr + _cachedBytes, bufferSize - _cachedBytes ) ) );
			if ( nRead < 0 ) {
				_valid = false;
				M_THROW( "read error", errno );
			} else if ( nRead == 0 ) {
				_valid = false;
				nl = ptr + _cachedBytes;
				break;
			}
			_cachedBytes += nRead;
			nl = static_cast<char*>( memchr( ptr, '\n', static_cast<size_t>( _cachedBytes ) ) );
			bufferSize *= 2;
		}
		length = static_cast<int>( nl - ptr );
		line_.assign( ptr, length );
		if ( _cachedBytes > length ) {
			::memmove( ptr, nl + 1, static_cast<size_t>( _cachedBytes - ( length + 1 ) ) );
			_cachedBytes -= ( length + 1 );
		} else {
			_cachedBytes = 0;
		}
	} else { /* UNBUFFERED_READS */
		length = static_cast<int>( read_until( line_, "\n", true ) );
	}
	if ( maximumLength_ && ( length > maximumLength_ ) ) {
		_fail = true;
		M_THROW( _( "line too long" ), length );
	}
	line_.trim_right( "\r\n" );
	return ( *this );
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

int long HFile::do_read_some( void* buffer_, int long size_ ) {
	M_PROLOG
	M_ENSURE( _handle, _error_ );
	system::file_descriptor_t fd( static_cast<system::file_descriptor_t>( get_file_descriptor() ) );
	int statusFlags( ::fcntl( fd, F_GETFL, 0 ) );
	bool wasNonBlocking( ( statusFlags & O_NONBLOCK ) != 0 );
	if ( ! wasNonBlocking ) {
		::fcntl( fd, F_SETFL, statusFlags | O_NONBLOCK );
	}
	wait_for_io( fd, IO_EVENT_TYPE::READ, INFINITY );
	int long len( static_cast<int long>( ::std::fread( buffer_, sizeof ( char ), static_cast<size_t>( size_ ), static_cast<FILE*>( _handle ) ) ) );
	if ( ! wasNonBlocking ) {
		::fcntl( fd, F_SETFL, statusFlags );
	}
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

