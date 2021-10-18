/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "config.hxx"

#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_UN_H
#	include <sys/un.h>
#endif /* #ifdef HAVE_SYS_UN_H */
#ifdef HAVE_NETINET_IN_H
#	include <netinet/in.h>
#endif /* #ifdef HAVE_NETINET_IN_H */
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hsocket.hxx"
#include "memory.hxx"
#include "hstring.hxx"
#include "resolver.hxx"
#include "system.hxx"

using namespace yaal::hcore::system;

namespace yaal {

namespace hcore {

enum {
	OK = 0,
	NOT_INITIALIZED,
	NOT_A_SERVER,
	ALREADY_LISTENING,
	ALREADY_CONNECTED
};

char const* const _errMsgHSocket_[ 5 ] = {
	_( "ok" ),
	_( "socket not initialized" ),
	_( "socket is not a server" ),
	_( "already listening" ),
	_( "already connected" )
};

HSocket::socket_type_t const HSocket::TYPE::DEFAULT = HSocket::socket_type_t::new_flag();
HSocket::socket_type_t const HSocket::TYPE::FILE = HSocket::socket_type_t::new_flag();
HSocket::socket_type_t const HSocket::TYPE::NETWORK = HSocket::socket_type_t::new_flag();
HSocket::socket_type_t const HSocket::TYPE::BLOCKING = HSocket::socket_type_t::new_flag();
HSocket::socket_type_t const HSocket::TYPE::NONBLOCKING = HSocket::socket_type_t::new_flag();
HSocket::socket_type_t const HSocket::TYPE::SSL = HSocket::socket_type_t::new_flag();
HSocket::socket_type_t const HSocket::TYPE::SERVER = HSocket::socket_type_t::new_flag();
HSocket::socket_type_t const HSocket::TYPE::CLIENT = HSocket::socket_type_t::new_flag();

bool HSocket::_resolveHostnames = true;

HSocket::HSocket( socket_type_t const& socketType_,
		int maximumNumberOfClients_ )
	: HRawFile(
			( ( socketType_ & TYPE::SSL ) || ( socketType_ == TYPE::DEFAULT ) )
				? (
					( socketType_ & TYPE::SERVER )
						? HRawFile::TYPE::SSL_SERVER
						: HRawFile::TYPE::SSL_CLIENT
				) : HRawFile::TYPE::PLAIN
		)
	, _needShutdown( false )
	, _type( socketType_ )
	, _maximumNumberOfClients( maximumNumberOfClients_ )
	, _addressSize( 0 )
	, _address( nullptr )
	, _hostName() {
	M_PROLOG
	if ( _type == TYPE::DEFAULT ) {
		_type |= TYPE::SSL;
	}
	if ( ( _type & TYPE::SERVER ) && ( _type & TYPE::CLIENT ) ) {
		M_THROW( _( "bad socket mode setting" ), socketType_.value() );
	}
	_type &= ~TYPE::SERVER;
	_type &= ~TYPE::CLIENT;
	if ( ( !!( socketType_ & TYPE::FILE ) ) && ( !!( socketType_ & TYPE::NETWORK ) ) ) {
		M_THROW( _( "bad socket namespace setting" ), socketType_.value() );
	}
	if ( ! ( socketType_ & ( TYPE::FILE | TYPE::NETWORK ) ) ) {
		_type |= TYPE::NETWORK;
	}
	if ( ( !!( socketType_ & TYPE::BLOCKING ) ) && ( !!( socketType_ & TYPE::NONBLOCKING ) ) ) {
		M_THROW( _( "bad socket option" ), socketType_.value() );
	}
	if ( ! ( socketType_ & ( TYPE::BLOCKING | TYPE::NONBLOCKING ) ) ) {
		_type |= TYPE::BLOCKING;
	}
	if ( _maximumNumberOfClients >= 0 ) {
		M_ENSURE( ( _fileDescriptor = ::socket(
						!!( _type & TYPE::NETWORK ) ? PF_INET : PF_UNIX,
						static_cast<int>( SOCK_STREAM ),
						0 /* info libc "Creating a Socket"
								 says that "zero is usually right for PROTOCOL" */ ) ) >= 0 );
		try {
			if ( !!( _type & TYPE::NONBLOCKING ) ) {
				int statusFlags( ::fcntl( _fileDescriptor, F_GETFL, 0 ) );
				M_ENSURE( statusFlags >= 0 );
				M_ENSURE( ::fcntl( _fileDescriptor, F_SETFL, statusFlags | O_NONBLOCK ) == 0 );
			}
			system::set_close_on_exec( _fileDescriptor, true );
		} catch ( ... ) {
			M_TEMP_FAILURE_RETRY( hcore::system::close( _fileDescriptor ) );
			_fileDescriptor = -1;
			throw;
		}
	}
	if ( !!( _type & TYPE::NETWORK ) ) {
		_address = memory::calloc<sockaddr_in>( 1 );
	} else {
		_address = memory::calloc<sockaddr_un>( 1 );
	}
	return;
	M_EPILOG
}

HSocket::~HSocket( void ) {
	M_PROLOG
	if ( _needShutdown ) {
		try {
			shutdown();
		} catch ( HOpenSSLException const& ) {
		}
	}
	/*
	 * There will be no memory leakage if shutdown() throws,
	 * because application has to terminate.
	 */
	if ( _address ) {
		memory::free( _address );
	}
	return;
	M_DESTRUCTOR_EPILOG
}

void HSocket::shutdown( void ) {
	M_PROLOG
	close();
	return;
	M_EPILOG
}

void HSocket::cleanup( void ) {
	M_PROLOG
	if ( ( !!( _type & TYPE::FILE ) ) && ( _address ) ) {
		sockaddr_un* addressFile( static_cast<sockaddr_un*>( _address ) );
		if ( addressFile->sun_path[ 0 ] ) {
			M_ENSURE( ( ::unlink( addressFile->sun_path ) == 0 ), addressFile->sun_path );
		}
	}
	return;
	M_EPILOG
}

int HSocket::do_close( void ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	flush();
	if ( _type & TYPE::SERVER ) {
		cleanup();
	}
	int res( 0 );
	_type &= ~TYPE::SERVER;
	_type &= ~TYPE::CLIENT;
	if ( _needShutdown && ( _fileDescriptor >= 0 ) ) {
		M_ENSURE( ( ::shutdown( _fileDescriptor, SHUT_RDWR ) == 0 ) || ( errno == ENOTCONN ) || ( errno == ECONNRESET ) );
		res = HRawFile::do_close();
		_needShutdown = false;
	}
	return( res );
	M_EPILOG
}

void HSocket::listen( yaal::hcore::HString const& address_, int port_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	if ( _fileDescriptor < 0 ) {
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	}
	if ( _type & TYPE::CLIENT ) {
		M_THROW( _errMsgHSocket_[ ALREADY_CONNECTED ], _fileDescriptor );
	}
	if ( _type & TYPE::SERVER ) {
		M_THROW( _errMsgHSocket_[ ALREADY_LISTENING ], _fileDescriptor );
	}
	if ( _maximumNumberOfClients < 1 ) {
		M_THROW( _( "bad maximum number of clients" ), _maximumNumberOfClients );
	}
	make_address( address_, port_ );
	int reuseAddr( 1 );
	M_ENSURE(
		::setsockopt(
			_fileDescriptor, SOL_SOCKET, SO_REUSEADDR,
			reinterpret_cast<char*>( &reuseAddr ), static_cast<socklen_t>( sizeof ( reuseAddr ) )
		) == 0
	);
	M_ENSURE(
		( ::bind( _fileDescriptor, static_cast<sockaddr*>( _address ), static_cast<socklen_t>( _addressSize ) ) == 0 ),
		!!( _type & TYPE::NETWORK ) ? address_ + ":" + port_ : address_
	);
	if ( ::listen( _fileDescriptor, _maximumNumberOfClients ) != 0 ) {
		cleanup();
		throw HSocketException( !!( _type & TYPE::NETWORK ) ? address_ + ":" + port_ : address_ );
	}
	_type |= TYPE::SERVER;
	_needShutdown = true;
	return;
	M_EPILOG
}

HSocket::ptr_t HSocket::accept( void ) {
	M_PROLOG
	int fileDescriptor = -1;
	socklen_t addressSize = 0;
	sockaddr_in addressNetwork;
	sockaddr_un addressFile;
	sockaddr* address;
	if ( _fileDescriptor < 0 ) {
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	}
	if ( ! ( _type & TYPE::SERVER ) ) {
		M_THROW( _errMsgHSocket_[ NOT_A_SERVER ], _fileDescriptor );
	}
	if ( !!( _type & TYPE::NETWORK ) ) {
		address = static_cast<sockaddr*>(
				static_cast<void*>( &addressNetwork ) );
		addressSize = static_cast<socklen_t>( sizeof ( addressNetwork ) );
	} else {
		address = static_cast<sockaddr*>(
				static_cast<void*>( &addressFile ) );
		addressSize = static_cast<socklen_t>( sizeof ( addressFile ) );
	}
	M_ENSURE( ( fileDescriptor = ::accept( _fileDescriptor,
					address, &addressSize ) ) >= 0 );
	/* - 1 means that constructor shall not create socket */
	ptr_t socket( make_pointer<HSocket>( _type, -1 ) );
	M_ASSERT( ! socket->_ssl );
	socket->_fileDescriptor = fileDescriptor;
	if ( !!( _type & TYPE::NONBLOCKING ) ) {
		int statusFlags( ::fcntl( socket->_fileDescriptor, F_GETFL, 0 ) );
		M_ENSURE( statusFlags >= 0 );
		M_ENSURE( ::fcntl( socket->_fileDescriptor, F_SETFL, statusFlags | O_NONBLOCK ) == 0 );
	}
	system::set_close_on_exec( socket->_fileDescriptor, true );
	socket->_addressSize = static_cast<int>( addressSize );
	socket->_needShutdown = true;
	socket->set_timeout( _timeout );
	::memcpy( socket->_address, address, static_cast<size_t>( addressSize ) );
	return socket;
	M_EPILOG
}

void HSocket::connect( yaal::hcore::HString const& address_, int port_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	}
	if ( _type & TYPE::CLIENT ) {
		M_THROW( _errMsgHSocket_[ ALREADY_CONNECTED ], _fileDescriptor );
	}
	if ( _type & TYPE::SERVER ) {
		M_THROW( _errMsgHSocket_[ ALREADY_LISTENING ], _fileDescriptor );
	}
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	make_address( address_, port_ );
	int error( ::connect( _fileDescriptor, static_cast<sockaddr*>( _address ), static_cast<socklen_t>( _addressSize ) ) );
	if ( ( !!( _type & TYPE::NONBLOCKING ) ) && error && ( errno == EINPROGRESS ) ) {
		IO_EVENT_TYPE waitResult( hcore::system::wait_for_io( _fileDescriptor, IO_EVENT_TYPE::WRITE, static_cast<int>( _timeout ) ) );
		if ( waitResult == IO_EVENT_TYPE::WRITE ) {
			socklen_t optLen( static_cast<socklen_t>( sizeof ( error ) ) );
			M_ENSURE( ::getsockopt( _fileDescriptor, SOL_SOCKET, SO_ERROR, &error, &optLen ) == 0 );
		} else if ( waitResult == IO_EVENT_TYPE::TIMEOUT ) {
			int curErrno( errno );
			M_ENSURE( ! "connection timedout"[0], !!( _type & TYPE::NETWORK ) ? address_ + ":" + port_ : address_, curErrno );
		}
	}
	int curErrno( errno );
	M_ENSURE( error == 0, !!( _type & TYPE::NETWORK ) ? address_ + ":" + port_ : address_, curErrno );
	_type |= TYPE::CLIENT;
	_needShutdown = true;
	return;
	M_EPILOG
}

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
template<typename T>
inline int FWD_SUN_LEN( T arg ) {
	return ( static_cast<int>( SUN_LEN( arg ) ) );
}
template<typename T>
inline T fwd_htons( T arg_ ) {
	return ( htons( arg_ ) );
}
template<typename T>
inline T fwd_ntohs( T arg_ ) {
	return ( ntohs( arg_ ) );
}
#pragma GCC diagnostic pop
}

void HSocket::make_address( yaal::hcore::HString const& address_, int port_ ) {
	M_PROLOG
	if ( !!( _type & TYPE::NETWORK ) ) {
		sockaddr_in* addressNetwork( static_cast<sockaddr_in*>( _address ) );
		addressNetwork->sin_family = AF_INET;
		addressNetwork->sin_port = fwd_htons(
				static_cast<int short unsigned>( port_ ) );
		addressNetwork->sin_addr.s_addr = resolver::get_ip( address_ ).raw();
		_addressSize = static_cast<int>( sizeof ( sockaddr_in ) );
	} else /* _type & TYPE::FILE */ {
		sockaddr_un* addressFile( static_cast<sockaddr_un*>( _address ) );
		addressFile->sun_family = AF_UNIX;
		HUTF8String utf8( address_ );
		::strncpy( addressFile->sun_path, utf8.c_str(),
				sizeof ( addressFile->sun_path ) );
		addressFile->sun_path[ sizeof ( addressFile->sun_path ) - 1 ] = 0;
		_addressSize = static_cast<int>( FWD_SUN_LEN( addressFile ) );
	}
	_hostName = address_;
	return;
	M_EPILOG
}

int HSocket::get_port( void ) const {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	}
	sockaddr_in* addressNetwork = nullptr;
	if ( ! ( _type & TYPE::NETWORK ) ) {
		M_THROW( _( "unix socket has not a port attribute" ), _type.value() );
	}
	addressNetwork = static_cast<sockaddr_in*>( _address );
	return ( fwd_ntohs( addressNetwork->sin_port ) );
	M_EPILOG
}

HString const& HSocket::get_host_name( void ) {
	M_PROLOG
	if ( _fileDescriptor < 0 ) {
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	}
	if ( _hostName.is_empty() ) {
		if ( !!( _type & TYPE::NETWORK ) ) {
			ip_t ip( reinterpret_cast<sockaddr_in*>( _address )->sin_addr.s_addr );
			if ( _resolveHostnames ) {
				_hostName = resolver::get_name( ip );
			} else {
				_hostName = resolver::ip_to_string( ip );
			}
		} else {
			sockaddr_un* addressFile( static_cast<sockaddr_un*>( _address ) );
			if ( addressFile->sun_path[ 0 ] ) {
				_hostName = addressFile->sun_path[ 0 ];
			}
		}
	}
	return ( _hostName );
	M_EPILOG
}

}

}

