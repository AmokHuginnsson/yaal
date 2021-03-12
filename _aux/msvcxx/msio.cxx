#include "_aux/msvcxx/sys/time.h"
#include <io.h>
#include <sstream>

#include "sys/socket.h"
#include "unistd.h"
#include "fcntl.h"
#include "hcore/base.hxx"
#include "hcore/memory.hxx"
#include "hcore/hexception.hxx"

#include "msio.hxx"
#include "emu_unistd.hxx"

using namespace std;

namespace msvcxx {

static int const IO_BUFFER_SIZE( 256 );

DWORD fd_to_handle( int fd_ ) {
	DWORD h( static_cast<DWORD>( fd_ ) );
	switch ( fd_ ) {
		case ( STDIN_FILENO ):  h = STD_INPUT_HANDLE;  break;
		case ( STDOUT_FILENO ): h = STD_OUTPUT_HANDLE; break;
		case ( STDERR_FILENO ): h = STD_ERROR_HANDLE;  break;
	}
	return h;
}

IO::IO( TYPE::type_t t_, HANDLE h_, HANDLE e_, std::string const& p_ )
	: _type( t_ )
	, _handle( h_ )
	, _overlapped()
	, _inBuffer( 0 )
	, _buffer( IO_BUFFER_SIZE )
	, _connected( ( t_ == TYPE::TERMINAL ) || ( t_ == TYPE::PIPE ) )
	, _scheduled( false )
	, _ready( false )
	, _nonBlocking( false )
	, _path( p_ ) {
	_overlapped.hEvent = e_ ? e_ : ::CreateEvent( nullptr, true, false, nullptr );
}

IO::~IO( void ) {
	M_PROLOG
	if ( _overlapped.hEvent ) {
		::CloseHandle( _overlapped.hEvent );
	}
	M_DESTRUCTOR_EPILOG
}

bool IO::schedule_read( int size_ ) {
	if ( _type == TYPE::SOCKET_DGRAM ) {
		return ( true );
	}
	M_ASSERT( _inBuffer == 0 );
	if ( ! _connected || _scheduled ) {
		return ( _scheduled = true );
	}
	if ( size_ > _buffer.get_size() ) {
		_buffer.realloc( size_ );
	}
	DWORD nRead( 0 );
	BOOL status( ::ReadFile( _handle, _buffer.raw(), size_, &nRead, &_overlapped ) );
	DWORD errCode( ::GetLastError() );
	bool valid( true );
	if ( status ) {
		_ready = true;
		_inBuffer = nRead;
	} else if ( ( errCode == ERROR_BROKEN_PIPE ) || ( errCode == ERROR_HANDLE_EOF ) ) {
		valid = false;
	}
	_scheduled = ! _ready && valid;
	return valid;
}

bool IO::sync( bool block_ ) {
	if ( _type == TYPE::SOCKET_DGRAM ) {
		return ( true );
	}
	if ( ! _scheduled ) {
		return ( _connected );
	}
	M_ASSERT( _inBuffer == 0 );
	DWORD iTransferred( 0 );
	BOOL status( ::GetOverlappedResult( _handle, &_overlapped, &iTransferred, block_ ) );
	DWORD errCode( ::GetLastError() );
	if ( ! status ) {
		_connected = ( errCode != ERROR_BROKEN_PIPE ) && ( errCode != ERROR_HANDLE_EOF );
		return ( _connected );
	}
	if ( _connected ) {
		_inBuffer = static_cast<int>( iTransferred );
	} else {
		_connected = true;
	}
	_ready = true;
	_scheduled = false;
	return ( _connected );
}

int IO::consume( char* dst_, int long& size_ ) {
	int nRead( 0 );
	if ( _inBuffer == 0 ) {
		_ready = false;
		return nRead;
	}
	nRead = std::min<int>( _inBuffer, size_ );
	char* src( static_cast<char*>( _buffer.raw() ) );
	::memcpy( dst_, src, nRead );
	_inBuffer -= nRead;
	size_ -= nRead;
	if ( _inBuffer == 0 ) {
		_ready = false;
		return nRead;
	}
	M_ASSERT( size_ == 0 );
	::memmove( src, src + nRead, _inBuffer );
	return nRead;
}

int long IO::read( void* buf_, int long size_ ) {
	M_ASSERT( size_ >= 0 );
	if ( ! sync() ) {
		return ( 0 );
	}
	M_ASSERT( _connected );
	char* dst( static_cast<char*>( buf_ ) );
	int nRead( consume( dst, size_ ) );
	if ( size_ == 0 ) {
		return nRead;
	}
	M_ASSERT( _inBuffer == 0 );
	if ( ! schedule_read( size_ ) ) {
		return nRead;
	}
	if ( ! sync( ! _nonBlocking ) ) {
		return nRead;
	}
	if ( _nonBlocking && ( _inBuffer == 0 ) && ( nRead == 0 ) ) {
		get_socket_errno() = EAGAIN;
		return ( -1 );
	}
	nRead += consume( dst + nRead, size_ );
	return nRead;
}

int long IO::write( void const* buf_, int long size_ ) {
	M_ASSERT( _connected );
	DWORD iWritten( 0 );
	if ( ::WriteFile( _handle, buf_, size_, &iWritten, &_overlapped ) == TRUE ) {
		return iWritten;
	}
	if ( ::GetLastError() != ERROR_IO_PENDING ) {
		log_windows_error( "GetOverlappedResult(WriteFile)" );
		return ( -1 );
	}
	if ( ::GetOverlappedResult( _handle, &_overlapped, &iWritten, true ) != TRUE ) {
		log_windows_error( "GetOverlappedResult(write)" );
		return ( -1 );
	}
	return iWritten;
}

int IO::close( void ) {
	int ret( 0 );
	_connected = false;
	if ( reinterpret_cast<SOCKET>( _handle ) != -1 ) {
		if ( ( _type == IO::TYPE::PIPE ) || ( _type == IO::TYPE::NAMED_PIPE ) ) {
			ret = ::CloseHandle( _handle ) ? 0 : -1;
		} else if ( ( _type == IO::TYPE::SOCKET ) || ( _type == IO::TYPE::SOCKET_DGRAM ) ) {
			ret = ::closesocket( reinterpret_cast<SOCKET>( _handle ) );
		} else {
			M_ASSERT( ! "invalid HANDLE type" );
		}
	}
	return ret;
}

int IO::fcntl( int cmd_, int arg_ ) {
	int ret( 0 );
	if ( cmd_ == F_SETFL ) {
		_nonBlocking = ( ( arg_ & O_NONBLOCK ) ? true : false );
	} else if ( cmd_ == F_GETFL ) {
		ret = _nonBlocking ? O_NONBLOCK : 0;
	}
	return ret;
}

HANDLE IO::event( void ) const {
	return ( _overlapped.hEvent ? _overlapped.hEvent : _handle );
}

HANDLE IO::handle( void ) const {
	return ( _handle );
}

void IO::set_handle( HANDLE handle_ ) {
	_handle = handle_;
}

IO::TYPE::type_t IO::type( void ) const {
	return ( _type );
}

OVERLAPPED* IO::overlapped( void ) {
	return ( &_overlapped );
}

std::string IO::path( void ) const {
	return ( _path );
}

void IO::set_path( std::string const& path_ ) {
	_path = path_;
	return;
}

void IO::swap( IO& io_ ) {
	using std::swap;
	swap( _overlapped, io_._overlapped );
	swap( _buffer, io_._buffer );
	swap( _inBuffer, io_._inBuffer );
	swap( _connected, io_._connected );
	swap( _scheduled, io_._scheduled );
	swap( _ready, io_._ready );
	swap( _handle, io_._handle );
	swap( _nonBlocking, io_._nonBlocking );
	swap( _path, io_._path );
	swap( _type, io_._type );
	return;
}

void IO::reset( void ) {
	if ( ! _ready ) {
		_scheduled = false;
	}
}

bool IO::ready( void ) const {
	return ( _ready || ( _inBuffer > 0 ) );
}

bool IO::is_connected( void ) const {
	return ( _connected );
}

int IO::connect( void ) {
	M_ASSERT( ! _connected );
	if ( ! _nonBlocking ) {
		DWORD iTransferred( 0 );
		if ( ! ::GetOverlappedResult( _handle, &_overlapped, &iTransferred, true ) ) {
			log_windows_error( "GetOverlappedResult(connect)" );
		}
		_connected = true;
	} else if ( _type == TYPE::SOCKET ) {
		get_socket_errno() = WSAEINPROGRESS;
	} else {
		_connected = true;
	}
	_scheduled = false;
	_ready = false;
	return ( _connected ? 0 : -1 );
}

void IO::accept( void ) {
	M_ASSERT( ! _connected );
	_connected = true;
	_ready = false;
	_scheduled = false;
}

int IO::sendto( char const* buf_, int size_, int flags_, struct sockaddr const* to_, int toLen_ ) {
	return ( ::sendto( reinterpret_cast<SOCKET>( _handle ), buf_, size_, flags_, to_, toLen_ ) );
}

int IO::recvfrom( char* buf_, int size_, int flags_, struct sockaddr* from_, int* fromLen_ ) {
	return ( ::recvfrom( reinterpret_cast<SOCKET>( _handle ), buf_, size_, flags_, from_, fromLen_ )  );
}

SystemIO::SystemIO( void )
	: _ioTable()
	, _mutex()
	, _idPool( MANAGED_IO ) {
	WSADATA wsaData;
	WORD wVersionRequested( MAKEWORD( 2, 2 ) );
	int err( WSAStartup( wVersionRequested, &wsaData ) );
	setenv( "TZ", "", 1 );
	_tzset();
	static UINT const UNICODE_UTF_8_CODE_PAGE = 65001;
	SetConsoleCP( UNICODE_UTF_8_CODE_PAGE );
	SetConsoleOutputCP( UNICODE_UTF_8_CODE_PAGE );
}

SystemIO& SystemIO::get_instance( void ) {
	static SystemIO instance;
	return instance;
}

SystemIO::io_t& SystemIO::create_io( IO::TYPE::type_t type_, HANDLE h_, HANDLE e_, std::string const& p_ ) {
	CLock l( _mutex );
	return ( *( _ioTable.insert( std::make_pair( _idPool ++, io_ptr_t( new IO( type_, h_, e_, p_ ) ) ) ) ).first );
}

SystemIO::io_t& SystemIO::get_io( int id_ ) {
	CLock l( _mutex );
	io_table_t::iterator i( _ioTable.find( id_ ) );
	if ( i != _ioTable.end() ) {
		return ( *i );
	}
	M_ASSERT( id_ < MANAGED_IO );
	HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( id_ ) ) );
	return ( *( _ioTable.insert( std::make_pair( id_, io_ptr_t( new IO( IO::TYPE::TERMINAL, h, h ) ) ) ) ).first );
}

int SystemIO::close_io( int id_ ) {
	CLock l( _mutex );
	io_table_t::iterator i( _ioTable.find( id_ ) );
	int ret( 0 );
	if ( i != _ioTable.end() ) {
		ret = i->second->close();
		_ioTable.erase( i );
	} else {
		ret = -1;
		errno = EBADF;
	}
	return ret;
}

int SystemIO::dup_io( int id_ ) {
	M_ASSERT( id_ >= MANAGED_IO );
	CLock l( _mutex );
	int fd( id_ < MANAGED_IO ? id_ : _open_osfhandle( reinterpret_cast<intptr_t>( get_io( id_ ).second->_handle ), 0 ) );
	fd = _dup( fd );
/*	HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( fd ) ) );
	 _ioTable.insert( std::make_pair( fd, make_shared<IO>( IO::TYPE::TERMINAL, h, h ) ) ) */
	return fd;
}

int SystemIO::dup2_io( int id1_, int id2_ ) {
	M_ASSERT( ( id1_ >= MANAGED_IO ) || ( id2_ >= MANAGED_IO ) );
	CLock l( _mutex );
	int fd1( id1_ < MANAGED_IO ? id1_ : _open_osfhandle( reinterpret_cast<intptr_t>( get_io( id1_ ).second->_handle ), 0 ) );
	int fd2( id2_ < MANAGED_IO ? id2_ : _open_osfhandle( reinterpret_cast<intptr_t>( get_io( id2_ ).second->_handle ), 0 ) );
	int fd( _dup2( fd1, fd2 ) );
/*	HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( fd ) ) );
	 _ioTable.insert( std::make_pair( fd, make_shared<IO>( IO::TYPE::TERMINAL, h, h ) ) ) */
	return fd;
}

} /* namespace msvcxx */

