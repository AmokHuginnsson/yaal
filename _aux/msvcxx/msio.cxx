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

IO::IO( TYPE::type_t t_, HANDLE h_, HANDLE e_, std::string const& p_ )
	: _type( t_ )
	, _handle( h_ )
	, _overlapped()
	, _readRequest( 0 )
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

void IO::schedule_read( void ) {
	if ( _type != TYPE::SOCKET_DGRAM ) {
		DWORD nRead( 0 );
		if ( _connected && ! _scheduled && ( _readRequest == 0 ) ) {
			BOOL status( ::ReadFile( _handle, _buffer.raw(), _readRequest = 1, &nRead, &_overlapped ) );
			if ( status && ( nRead == 1 ) ) {
				_ready = true;
			}
		}
		_scheduled = ! _ready;
	}
	return;
}

void IO::sync( void ) {
	if ( _type != TYPE::SOCKET_DGRAM ) {
		DWORD iTransferred( 0 );
		if ( ! ::GetOverlappedResult( _handle, &_overlapped, &iTransferred, true ) ) {
			log_windows_error( "GetOverlappedResult(sync)" );
		} else {
			if ( _connected ) {
				if ( ( static_cast<int>( iTransferred ) < _readRequest ) && ( ::GetLastError() != ERROR_HANDLE_EOF ) ) {
					stringstream ss;
					ss << "iTransferred: " << iTransferred << ", _readRequest: " << _readRequest;
					log_windows_error( ( ss.str() + "GetOverlappedResult(bad read)" ).c_str() );
				} else {
					_ready = true;
				}
			} else {
				_connected = _ready = true;
			}
			_scheduled = false;
		}
	}
	return;
}

int long IO::read( void* buf_, int long size_ ) {
	M_ASSERT( _connected );
	M_ASSERT( size_ >= 0 );
	int long nRead( 0 );
	if ( size_ > 0 ) {
		if ( _inBuffer > 0 ) {
			nRead = std::min<int>( _inBuffer, static_cast<int>( size_ ) );
			::memcpy( buf_, _buffer.raw(), nRead );
			_inBuffer -= nRead;
			if ( _inBuffer > 0 ) {
				::memmove( _buffer.raw(), _buffer.get<char>() + nRead, _inBuffer );
			}
		} else {
			int off( 0 );
			if ( _scheduled && ! _ready ) {
				sync();
			}
			bool ok( false );
			if ( _ready ) {
				_ready = false;
				off = std::min<int>( size_, _readRequest );
				::memcpy( buf_, _buffer.raw(), off );
				size_ -= off;
				_readRequest -= off;
				ok = true;
			}
			if ( size_ > 0 ) {
				DWORD iRead( 0 );
				DWORD errCode( 0 );
				if ( _readRequest == 0 ) { /* No pending read operation. */
					if ( _buffer.get_size() < ( size_ + off ) ) {
						_buffer.realloc( size_ + off );
					}
					ok = ::ReadFile( _handle, _buffer.get<char>() + off, _readRequest = size_, &iRead, &_overlapped ) ? true : false;
					errCode = ::GetLastError();
				}
				if (
						( ! ok && ( ( errCode == ERROR_IO_PENDING ) || ( errCode == ERROR_IO_INCOMPLETE ) ) )
						|| ( ! ok && ( errCode == 0 ) )
						|| ( ok && ( ( _readRequest > 0 ) || ( ! _nonBlocking )	) )
				) { /* Previous read operation still pending. */
					ok = ::GetOverlappedResult( _handle, &_overlapped, &iRead, ! _nonBlocking ) ? true : false;
					errCode = ::GetLastError();
				}
				if ( ok ) { /* We got all data from requested read. */
					iRead = std::min( _readRequest, std::min<int>( iRead, static_cast<int>( size_ ) ) );
					::memcpy( static_cast<char*>( buf_ ) + off, _buffer.get<char>() + off, iRead );
					_inBuffer = _readRequest - iRead;
					if ( _inBuffer > 0 ) {
						::memmove( _buffer.raw(), _buffer.get<char>() + iRead, _inBuffer );
						_readRequest = 0;
					} else {
						_readRequest -= iRead;
					}
				} else {
					/* Read still pending, handle errors. */
					if ( ( errCode == ERROR_IO_PENDING ) || ( errCode == ERROR_IO_INCOMPLETE ) ) {
						if ( off > 0 ) {
							ok = true;
						} else {
							get_socket_errno() = EAGAIN;
						}
					} else if ( errCode == ERROR_HANDLE_EOF ) {
						ok = true;
						iRead = 0;
					} else {
						log_windows_error( "GetOverlappedResult(ReadFile)" );
					}
				}
				nRead = ok ? iRead + off : -1;
			} else {
				nRead = ok ? off : -1;
			}
		}
	}
	return ( nRead );
}

int long IO::write( void const* buf_, int long size_ ) {
	M_ASSERT( _connected );
	DWORD iWritten( 0 );
	bool ok( ::WriteFile( _handle, buf_, size_, &iWritten, &_overlapped ) ? true : false );
	if ( ! ok ) {
		if ( ::GetLastError() == ERROR_IO_PENDING ) {
			ok = ::GetOverlappedResult( _handle, &_overlapped, &iWritten, true ) ? true : false;
			if ( ! ok ) {
				log_windows_error( "GetOverlappedResult(write)" );
			}
		} else {
			log_windows_error( "GetOverlappedResult(WriteFile)" );
		}
	}
	return ( ok ? iWritten : -1 );
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
	return ( ret );
}

int IO::fcntl( int cmd_, int arg_ ) {
	int ret( 0 );
	if ( cmd_ == F_SETFL ) {
		_nonBlocking = ( ( arg_ & O_NONBLOCK ) ? true : false );
		if ( ( _nonBlocking ) && ( ( _type == IO::TYPE::NAMED_PIPE ) || ( _type == IO::TYPE::PIPE ) ) ) {
			schedule_read();
		}
	} else if ( cmd_ == F_GETFL ) {
		ret = _nonBlocking ? O_NONBLOCK : 0;
	}
	return ( ret );
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
	swap( _readRequest, io_._readRequest );
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
	if ( ! _ready )
		_scheduled = false;
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
}

SystemIO& SystemIO::get_instance( void ) {
	static SystemIO instance;
	return ( instance );
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
	return ( ret );
}

int SystemIO::dup_io( int id_ ) {
	M_ASSERT( id_ >= MANAGED_IO );
	CLock l( _mutex );
	int fd( id_ < MANAGED_IO ? id_ : _open_osfhandle( reinterpret_cast<intptr_t>( get_io( id_ ).second->_handle ), 0 ) );
	fd = _dup( fd );
/*	HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( fd ) ) );
	 _ioTable.insert( std::make_pair( fd, make_shared<IO>( IO::TYPE::TERMINAL, h, h ) ) ) */
	return ( fd );
}

int SystemIO::dup2_io( int id1_, int id2_ ) {
	M_ASSERT( ( id1_ >= MANAGED_IO ) || ( id2_ >= MANAGED_IO ) );
	CLock l( _mutex );
	int fd1( id1_ < MANAGED_IO ? id1_ : _open_osfhandle( reinterpret_cast<intptr_t>( get_io( id1_ ).second->_handle ), 0 ) );
	int fd2( id2_ < MANAGED_IO ? id2_ : _open_osfhandle( reinterpret_cast<intptr_t>( get_io( id2_ ).second->_handle ), 0 ) );
	int fd( _dup2( fd1, fd2 ) );
/*	HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( fd ) ) );
	 _ioTable.insert( std::make_pair( fd, make_shared<IO>( IO::TYPE::TERMINAL, h, h ) ) ) */
	return ( fd );
}

} /* namespace msvcxx */

