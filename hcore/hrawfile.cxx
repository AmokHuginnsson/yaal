/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <libintl.h>
#include <sys/time.h> /* timeval */

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hrawfile.hxx"
#include "hcore/memory.hxx"
#include "system.hxx"
#include "hclock.hxx"
#include "hlog.hxx"

#undef INFINITY

using namespace yaal::hcore::system;

namespace yaal {

namespace hcore {

static char const* const _error_ = _( "file is not opened" );
int long _writeTimeout_ = 4 * 1000; /* 4 seconds */

HRawFile::HRawFile( TYPE::raw_file_type_t type_ )
	: _type( type_ )
	, _fileDescriptor( -1 )
	, _timeout( _writeTimeout_ )
	, _ssl()
	, reader(
		( ( type_ & TYPE::SSL_SERVER ) || ( type_ & TYPE::SSL_CLIENT ) )
			? &HRawFile::read_ssl_loader
			: &HRawFile::read_plain
		)
	, writer(
		( ( type_ & TYPE::SSL_SERVER ) || ( type_ & TYPE::SSL_CLIENT ) )
			? &HRawFile::write_ssl_loader
			: &HRawFile::write_plain
		)
	, closer( &HRawFile::close_plain )
	, some_reader(
		( ( type_ & TYPE::SSL_SERVER ) || ( type_ & TYPE::SSL_CLIENT ) )
			? &HRawFile::read_some_ssl_loader
			: &HRawFile::read_some_plain
		)
	, _ownership( OWNERSHIP::ACQUIRED ) {
	M_PROLOG
	M_ASSERT( ! ( ( type_ == TYPE::DEFAULT ) && ( type_ & TYPE::SSL_SERVER ) ) );
	M_ASSERT( ! ( ( type_ == TYPE::DEFAULT ) && ( type_ & TYPE::SSL_CLIENT ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::PLAIN ) && ( type_ & TYPE::SSL_SERVER ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::PLAIN ) && ( type_ & TYPE::SSL_CLIENT ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::SSL_CLIENT ) && ( type_ & TYPE::SSL_SERVER ) ) );
	return;
	M_EPILOG
}

HRawFile::HRawFile( file_descriptor_t fd_, OWNERSHIP ownership_ )
	: _type( TYPE::DEFAULT )
	, _fileDescriptor( fd_ )
	, _timeout( _writeTimeout_ )
	, _ssl()
	, reader( &HRawFile::read_plain )
	, writer( &HRawFile::write_plain )
	, closer( &HRawFile::close_plain )
	, some_reader( &HRawFile::read_some_plain )
	, _ownership( fd_ >= 0 ? ownership_ : OWNERSHIP::NONE ) {
	M_PROLOG
	return;
	M_EPILOG
}

HRawFile::~HRawFile( void ) {
	M_PROLOG
	if ( ( _ownership == OWNERSHIP::ACQUIRED ) && ( _fileDescriptor >= 0 ) ) {
		HRawFile::close();
	}
	M_DESTRUCTOR_EPILOG
}

int HRawFile::close( void ) {
	M_PROLOG
	M_ENSURE( ( _fileDescriptor >= 0 ) && ( _ownership == OWNERSHIP::ACQUIRED ) );
	return ( (this->*closer)() );
	M_EPILOG
}

int HRawFile::close_plain( void ) {
	M_PROLOG
	return ( do_close() );
	M_EPILOG
}

int HRawFile::close_ssl( void ) {
	M_PROLOG
	flush();
	_ssl->shutdown();
	return ( do_close() );
	M_EPILOG
}

int HRawFile::do_close( void ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( "file is not opened", errno );
	}
	flush();
	int error( static_cast<int>( M_TEMP_FAILURE_RETRY( system::close( _fileDescriptor ) ) ) );
	_fileDescriptor = -1;
	return ( error );
	M_EPILOG
}

file_descriptor_t HRawFile::get_file_descriptor( void ) const {
	M_PROLOG
	return ( _fileDescriptor );
	M_EPILOG
}

int long HRawFile::read_plain( void* buffer_, int long size_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _error_, errno );
	}
	return ( ::read( _fileDescriptor, buffer_, static_cast<size_t>( size_ ) ) );
	M_EPILOG
}

int long HRawFile::read_ssl( void* buffer_, int long size_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _error_, errno );
	}
	return ( _ssl->read( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::read_ssl_loader( void* buffer_, int long size_ ) {
	M_PROLOG
	_ssl = HOpenSSL::ptr_t( make_pointer<HOpenSSL>( _fileDescriptor, _type & TYPE::SSL_SERVER ? HOpenSSL::TYPE::SERVER : HOpenSSL::TYPE::CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	some_reader = &HRawFile::read_some_ssl;
	return ( read_ssl( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::read_some_ssl_loader( void* buffer_, int long size_ ) {
	M_PROLOG
	_ssl = HOpenSSL::ptr_t( make_pointer<HOpenSSL>( _fileDescriptor, _type & TYPE::SSL_SERVER ? HOpenSSL::TYPE::SERVER : HOpenSSL::TYPE::CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	some_reader = &HRawFile::read_some_ssl;
	return ( read_some_ssl( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::do_write( void const* buffer_, int long size_ ) {
	M_PROLOG
	return ( (this->*writer)( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::write_plain( void const* buffer_, int long size_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _error_, errno );
	}
	int long totalWritten( 0 );
	do {
		IO_EVENT_TYPE ioEventType( wait_for_io( _fileDescriptor, IO_EVENT_TYPE::WRITE, static_cast<int>( _timeout ) ) );
		if ( ioEventType != IO_EVENT_TYPE::WRITE ) {
			throw HStreamInterfaceException(
				ioEventType == IO_EVENT_TYPE::TIMEOUT
					? _( "timeout on write" )
					: _( "wait on write failed (interrupted)" )
			);
		}
		int long nWrittenChunk(
			M_TEMP_FAILURE_RETRY(
				::write(
					_fileDescriptor,
					static_cast<char const*>( buffer_ ) + totalWritten,
					static_cast<size_t>( size_ - totalWritten )
				)
			)
		);
		if ( nWrittenChunk > 0 ) {
			totalWritten += nWrittenChunk;
		} else if ( totalWritten > 0 ) {
			break;
		} else {
			totalWritten = nWrittenChunk;
			break;
		}
	} while ( totalWritten < size_ );
	return ( totalWritten );
	M_EPILOG
}

int long HRawFile::write_ssl( void const* buffer_, int long size_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _error_, errno );
	}
	HClock clk;
	int long nWritten = 0;
	do {
		IO_EVENT_TYPE ioEventType(
			wait_for_io( _fileDescriptor, IO_EVENT_TYPE::READ | IO_EVENT_TYPE::WRITE, static_cast<int>( _timeout ) )
		);
		if ( ! ( ioEventType & ( IO_EVENT_TYPE::READ | IO_EVENT_TYPE::WRITE ) ) ) {
			throw HStreamInterfaceException(
				ioEventType == IO_EVENT_TYPE::TIMEOUT
					? _( "timeout on SSL write" )
					: _( "wait on SSL write failed (interrupted)" )
			);
		}
		int long ret = _ssl->write( static_cast<char const*>( buffer_ ) + nWritten, size_ );
		if ( ! ret ) {
			nWritten = ret;
			break;
		} else if ( ret > 0 ) {
			nWritten += ret;
		}
	} while ( nWritten < size_ );
	return ( nWritten );
	M_EPILOG
}

int long HRawFile::write_ssl_loader( void const* buffer_, int long size_ ) {
	M_PROLOG
	_ssl = HOpenSSL::ptr_t( make_pointer<HOpenSSL>( _fileDescriptor, _type & TYPE::SSL_SERVER ? HOpenSSL::TYPE::SERVER : HOpenSSL::TYPE::CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	some_reader = &HRawFile::read_some_ssl;
	return ( write_ssl( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::do_read( void* buffer_, int long size_ ) {
	M_PROLOG
	return ( (this->*reader)( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::read_some_plain( void* buffer_, int long size_ ) {
	M_PROLOG
	int statusFlags( ::fcntl( _fileDescriptor, F_GETFL, 0 ) );
	bool wasNonBlocking( ( statusFlags & O_NONBLOCK ) != 0 );
	if ( ! wasNonBlocking ) {
		::fcntl( _fileDescriptor, F_SETFL, statusFlags | O_NONBLOCK );
	}
	wait_for_io( _fileDescriptor, IO_EVENT_TYPE::READ, INFINITY );
	int long len( (this->*reader)( buffer_, size_ ) );
	if ( ! wasNonBlocking ) {
		::fcntl( _fileDescriptor, F_SETFL, statusFlags );
	}
	return ( len );
	M_EPILOG
}

int long HRawFile::read_some_ssl( void* buffer_, int long size_ ) {
	M_PROLOG
	int statusFlags( ::fcntl( _fileDescriptor, F_GETFL, 0 ) );
	M_ENSURE( statusFlags >= 0 );
	bool wasNonBlocking( ( statusFlags & O_NONBLOCK ) != 0 );
	if ( ! wasNonBlocking ) {
		_ssl->set_nonblocking( true );
	}
	wait_for_io( _fileDescriptor, IO_EVENT_TYPE::READ, INFINITY );
	int long len( (this->*reader)( buffer_, size_ ) );
	if ( ! wasNonBlocking ) {
		_ssl->set_nonblocking( false );
	}
	return ( len );
	M_EPILOG
}

int long HRawFile::do_read_some( void* buffer_, int long size_ ) {
	M_PROLOG
	return ( (this->*some_reader)( buffer_, size_ ) );
	M_EPILOG
}

void HRawFile::do_flush( void ) {
}

void HRawFile::set_timeout( int long timeout_ ) {
	_timeout = timeout_;
	if ( _timeout < LOW_TIMEOUT_WARNING ) {
		log( LOG_LEVEL::WARNING ) << "Low timeout for write operations!" << endl;
	}
	return;
}

bool HRawFile::do_is_valid( void ) const {
	M_PROLOG
	return ( _fileDescriptor >= 0 );
	M_EPILOG
}

HStreamInterface::POLL_TYPE HRawFile::do_poll_type( void ) const {
	M_PROLOG
	return ( is_valid() ? POLL_TYPE::NATIVE : POLL_TYPE::INVALID );
	M_EPILOG
}

void const* HRawFile::do_data( void ) const {
	M_PROLOG
	return ( is_valid() ? reinterpret_cast<void const*>( static_cast<int_native_t>( _fileDescriptor ) ) : memory::INVALID_HANDLE );
	M_EPILOG
}

}

}

