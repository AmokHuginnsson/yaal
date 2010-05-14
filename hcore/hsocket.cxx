/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hsocket.cxx - this file is integral part of `yaal' project.

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

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <libintl.h>

#include "config.hxx"

#if ! defined( HAVE_GETHOSTBYNAME_R ) || ( HAVE_GETHOSTBYNAME_R == 0 )
#if ! defined( HAVE_DECL_GETADDRINFO ) || ( HAVE_DECL_GETADDRINFO == 0 )
#	include <getaddrinfo.h>
#endif /* not HAVE_DECL_GETADDRINFO */
#endif /* not HAVE_GETHOSTBYNAME_R */

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hsocket.hxx"
#include "xalloc.hxx"
#include "hstring.hxx"
#include "hlog.hxx"

namespace yaal
{

namespace hcore
{

enum
	{
	OK = 0,
	NOT_INITIALIZED,
	NOT_A_SERVER,
	ALREADY_LISTENING
	};

char const* const _errMsgHSocket_[ 4 ] =
	{
	_( "ok" ),
	_( "socket not initialized" ),
	_( "socket is not a server" ),
	_( "already listening" )
	};

bool HSocket::_resolveHostnames = true;

HSocket::HSocket( socket_type_t const socketType_,
		int const maximumNumberOfClients_ )
	: HRawFile( !!( socketType_ & TYPE::SSL_SERVER )
			? HRawFile::TYPE::SSL_SERVER
			: ( ( ( socketType_ == TYPE::DEFAULT ) || ( !!( socketType_ & TYPE::SSL_CLIENT ) ) )
				? HRawFile::TYPE::SSL_CLIENT : HRawFile::TYPE::DEFAULT ) ),
	_needShutdown( false ), _type( socketType_ ),
	_maximumNumberOfClients( maximumNumberOfClients_ ),
	_addressSize( 0 ), _address( NULL ), _clients( NULL ),
	_hostName()
	{
	M_PROLOG
	if ( _type == TYPE::DEFAULT )
		_type |= TYPE::SSL_CLIENT;
	if ( ( !!( socketType_ & TYPE::FILE ) ) && ( !!( socketType_ & TYPE::NETWORK ) ) )
		M_THROW( _( "bad socket namespace setting" ), socketType_.value() );
	if ( ! ( socketType_ & ( socket_type_t( TYPE::FILE ) | TYPE::NETWORK ) ) )
		_type |= TYPE::NETWORK;
	if ( ( !!( socketType_ & TYPE::BLOCKING ) ) && ( !!( socketType_ & TYPE::NONBLOCKING ) ) )
		M_THROW( _( "bad socket option" ), socketType_.value() );
	if ( ! ( socketType_ & ( socket_type_t( TYPE::BLOCKING ) | TYPE::NONBLOCKING ) ) )
		_type |= TYPE::BLOCKING;
	if ( _maximumNumberOfClients >= 0 )
		{
		M_ENSURE( ( _fileDescriptor = ::socket(
						!!( _type & TYPE::NETWORK ) ? PF_INET : PF_UNIX,
						static_cast<int>( SOCK_STREAM ),
						0 /* info libc "Creating a Socket"
								 says that "zero is usually right for PROTOCOL" */ ) ) >= 0 );
		if ( !!( _type & TYPE::NONBLOCKING ) )
			M_ENSURE( ::fcntl( _fileDescriptor, F_SETFL, O_NONBLOCK ) == 0 );
		}
	if ( !!( _type & TYPE::NETWORK ) )
		_address = xcalloc<sockaddr_in>( 1 );
	else
		_address = xcalloc<sockaddr_un>( 1 );
	return;
	M_EPILOG
	}

HSocket::~HSocket( void )
	{
	M_PROLOG
	shutdown();
	/* There will be no memory leakage if shutdown() throws,
	 * because application has to terminate. */
	if ( _address )
		xfree ( _address );
	return;
	M_EPILOG
	}

void HSocket::shutdown( void )
	{
	M_PROLOG
	close();
	M_EPILOG
	}

int HSocket::do_close( void )
	{
	M_PROLOG
	sockaddr_un* addressFile = NULL;
	if ( _clients )
		{
		delete _clients;
		_clients = NULL;
		if ( ( !!( _type & TYPE::FILE ) ) && ( _address ) )
			{
			addressFile = static_cast<sockaddr_un*>( _address );
			if ( addressFile->sun_path [ 0 ] )
				M_ENSURE_EX( ( ::unlink( addressFile->sun_path ) == 0 ), addressFile->sun_path );
			}
		}
	if ( _needShutdown && ( _fileDescriptor >= 0 ) )
		{
		if ( is_write_ready( _fileDescriptor ) )
			M_ENSURE( ( ::shutdown( _fileDescriptor, SHUT_RDWR ) == 0 ) || ( errno == ENOTCONN ) || ( errno == ECONNRESET ) );
		HRawFile::do_close();
		_needShutdown = false;
		}
	return( 0 );
	M_EPILOG
	}

void HSocket::shutdown_client( int fileDescriptor_ )
	{
	M_PROLOG
	ptr_t client;
	if ( ! _clients )
		M_THROW( _errMsgHSocket_[ NOT_A_SERVER ], _fileDescriptor );
	clients_t::iterator it( _clients->find( fileDescriptor_ ) );
	if ( it == _clients->end() )
		M_THROW( _( "no such client" ), fileDescriptor_ );
	M_ASSERT( !! it->second );
	_clients->erase( fileDescriptor_ );
	return;
	M_EPILOG
	}

void HSocket::listen( yaal::hcore::HString const& address_, int const port_ )
	{
	M_PROLOG
	int reuseAddr = 1;
	if ( _fileDescriptor < 0 )
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	if ( _clients )
		M_THROW( _errMsgHSocket_[ ALREADY_LISTENING ], _fileDescriptor );
	if ( _maximumNumberOfClients < 1 )
		M_THROW( _( "bad maximum number of clients" ), _maximumNumberOfClients );
	make_address( address_, port_ );
	M_ENSURE( ::setsockopt( _fileDescriptor, SOL_SOCKET, SO_REUSEADDR,
				reinterpret_cast<char*>( &reuseAddr ), sizeof ( reuseAddr ) ) == 0 );
	M_ENSURE_EX( ( ::bind( _fileDescriptor,
				static_cast<sockaddr*>( _address ), _addressSize ) == 0 ), address_ );
	M_ENSURE_EX( ( ::listen( _fileDescriptor, _maximumNumberOfClients ) == 0 ), address_ );
	_clients = new clients_t( _maximumNumberOfClients );
	_needShutdown = true;
	return;
	M_EPILOG
	}

HSocket::ptr_t HSocket::accept( void )
	{
	M_PROLOG
	int fileDescriptor = -1;
	socklen_t addressSize = 0;
	sockaddr_in addressNetwork;
	sockaddr_un addressFile;
	sockaddr* address;
	if ( _fileDescriptor < 0 )
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	if ( ! _clients )
		M_THROW( _errMsgHSocket_[ NOT_A_SERVER ], _fileDescriptor );
	if ( !!( _type & TYPE::NETWORK ) )
		{
		address = static_cast<sockaddr*>(
				static_cast<void*>( &addressNetwork ) );
		addressSize = sizeof ( addressNetwork );
		}
	else
		{
		address = static_cast<sockaddr*>(
				static_cast<void*>( &addressFile ) );
		addressSize = sizeof ( addressFile );
		}
	M_ENSURE( ( fileDescriptor = ::accept( _fileDescriptor,
					address, &addressSize ) ) >= 0 );
	if ( !!( _type & TYPE::NONBLOCKING ) )
		M_ENSURE( ::fcntl( fileDescriptor, F_SETFL, O_NONBLOCK ) == 0 );
	/* - 1 means that constructor shall not create socket */
	socket_type_t type = _type;
	if ( !!( _type & ( socket_type_t( TYPE::SSL_SERVER  ) | TYPE::SSL_CLIENT ) ) )
		type &= ~socket_type_t( TYPE::SSL_CLIENT ), type |= TYPE::SSL_SERVER;
	ptr_t socket = ptr_t( new HSocket( type, -1 ) );
	M_ASSERT( ! socket->_sSL );
	socket->_fileDescriptor = fileDescriptor;
	socket->_addressSize = addressSize;
	socket->_needShutdown = true;
	socket->set_timeout( _timeOut );
	::memcpy( socket->_address, address, addressSize );
	if ( _clients->find( fileDescriptor ) != _clients->end() )
		M_THROW( _( "inconsitent client list state" ), fileDescriptor );
	_clients->operator[]( fileDescriptor ) = socket;
	return ( socket );
	M_EPILOG
	}

void HSocket::connect( yaal::hcore::HString const& address_, int const port_ )
	{
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	make_address( address_, port_ );
	M_ENSURE_EX( ( ::connect( _fileDescriptor,
				static_cast<sockaddr*>( _address ), _addressSize ) == 0 ), address_ );
	_needShutdown = true;
	return;
	M_EPILOG
	}

void HSocket::make_address( yaal::hcore::HString const& address_, int const port_ )
	{
	M_PROLOG
	if ( !!( _type & TYPE::NETWORK ) )
		{
		int error( 0 );
		sockaddr_in* addressNetwork( static_cast<sockaddr_in*>( _address ) );
		addressNetwork->sin_family = AF_INET;
		addressNetwork->sin_port = htons(
				static_cast<int short unsigned>( port_ ) );
#ifdef HAVE_GETHOSTBYNAME_R
		static int const GETHOST_BY_NAME_R_WORK_BUFFER_SIZE = 1024;
		hostent hostName;
		hostent* hostNameStatus( NULL );
		_addressSize = GETHOST_BY_NAME_R_WORK_BUFFER_SIZE;
		_cache.realloc( _addressSize );
		while ( ::gethostbyname_r( address_.raw(), &hostName,
					_cache.raw(), _addressSize,
					&hostNameStatus, &error ) == ERANGE )
			_cache.realloc( _addressSize <<= 1 );
		errno = error;
		M_ENSURE( hostNameStatus );
		addressNetwork->sin_addr.s_addr = reinterpret_cast<in_addr*>(
				hostName.h_addr_list[ 0 ] )->s_addr;
#else /* HAVE_GETHOSTBYNAME_R */
		addrinfo* addrInfo( NULL );
		error = ::getaddrinfo( address_.raw(), NULL, NULL, &addrInfo );
		M_ENSURE( ! error && addrInfo );
		addressNetwork->sin_addr.s_addr = reinterpret_cast<sockaddr_in*>( addrInfo->ai_addr )->sin_addr.s_addr;
		::freeaddrinfo( addrInfo );
#endif /* not HAVE_GETHOSTBYNAME_R */
		_addressSize = sizeof ( sockaddr_in );
		}
	else /* _type & TYPE::FILE */
		{
		sockaddr_un* addressFile( static_cast<sockaddr_un*>( _address ) );
		addressFile->sun_family = AF_UNIX;
		::strncpy( addressFile->sun_path, address_.raw(),
				sizeof ( addressFile->sun_path ) );
		addressFile->sun_path[ sizeof ( addressFile->sun_path ) - 1 ] = 0;
		_addressSize = static_cast<int>( SUN_LEN( addressFile ) );
		}
	_hostName = address_;
	return;
	M_EPILOG
	}

int HSocket::get_port( void ) const
	{
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	sockaddr_in* addressNetwork = NULL;
	if ( ! ( _type & TYPE::NETWORK ) )
		M_THROW( _( "unix socket has not a port attribute" ), _type.value() );
	addressNetwork = static_cast<sockaddr_in*>( _address );
	return ( ntohs( addressNetwork->sin_port ) );
	M_EPILOG
	}

HSocket::ptr_t HSocket::get_client( int const fileDescriptor_ ) const
	{
	M_PROLOG
	ptr_t client;
	if ( ! _clients )
		M_THROW( _errMsgHSocket_[ NOT_A_SERVER ], _fileDescriptor );
	clients_t::iterator it( _clients->find( fileDescriptor_ ) );
	if ( it == _clients->end() )
		M_THROW( _( "no such client" ), fileDescriptor_ );
	return ( it->second );
	M_EPILOG
	}

HSocket::clients_t::const_iterator HSocket::begin( void ) const
	{
	M_PROLOG
	if ( ! _clients )
		M_THROW( _errMsgHSocket_[ NOT_A_SERVER ], _fileDescriptor );
	clients_t const* c = _clients;
	return ( c->begin() );
	M_EPILOG
	}

HSocket::clients_t::const_iterator HSocket::end( void ) const
	{
	M_PROLOG
	if ( ! _clients )
		M_THROW( _errMsgHSocket_[ NOT_A_SERVER ], _fileDescriptor );
	clients_t const* c = _clients;
	return ( c->end() );
	M_EPILOG
	}

int long HSocket::write_until_eos( HString const& message_ )
	{
	M_PROLOG
	int long nSize = 0;
	nSize = message_.get_length();
	if ( nSize > 0 )
		nSize = write( message_.raw(), nSize );
	return ( nSize );
	M_EPILOG
	}

int HSocket::get_client_count( void ) const
	{
	M_PROLOG
	if ( ! _clients )
		M_THROW( _errMsgHSocket_[ NOT_A_SERVER ], _fileDescriptor );
	return ( _clients->size() );
	M_EPILOG
	}

HString const& HSocket::get_host_name( void )
	{
	static int const GETHOST_BY_NAME_R_WORK_BUFFER_SIZE = 1024;
	M_PROLOG
	int error = 0;
	int size = GETHOST_BY_NAME_R_WORK_BUFFER_SIZE;
	sockaddr_in* addressNetwork = NULL;
	sockaddr_un* addressFile = NULL;
#ifdef HAVE_GETHOSTBYNAME_R
	int code = 0;
	hostent hostName;
	hostent* hostNameStatus = NULL;
#else /* HAVE_GETHOSTBYNAME_R */
	size = NI_MAXHOST;
#endif /* ! HAVE_GETHOSTBYNAME_R */
	if ( _fileDescriptor < 0 )
		M_THROW( _errMsgHSocket_[ NOT_INITIALIZED ], _fileDescriptor );
	if ( _hostName.is_empty() )
		{
		if ( !!( _type & TYPE::NETWORK ) )
			{
			char const* name = NULL;
			_cache.realloc( size );
			addressNetwork = reinterpret_cast<sockaddr_in*>( _address );
			if ( _resolveHostnames )
				{
#ifdef HAVE_GETHOSTBYNAME_R
				::memset( &hostName, 0, sizeof ( hostent ) );
				while ( ( error = ::gethostbyaddr_r( &addressNetwork->sin_addr, _addressSize,
							AF_INET, &hostName,
							_cache.raw(),
							size, &hostNameStatus, &code ) ) == ERANGE )
					_cache.realloc( size <<= 1 );
				if ( code )
					log_trace << error_message( code ) << endl;
				errno = error;
				M_ENSURE( error == 0 );
				if ( hostNameStatus )
					name = hostName.h_name;
#else /* HAVE_GETHOSTBYNAME_R */
				error = getnameinfo(
								reinterpret_cast<sockaddr*>( addressNetwork ), _addressSize,
								_cache.raw(), size, NULL, 0, NI_NOFQDN );
				M_ENSURE( error == 0 );
				name = _cache.raw();
#endif /* ! HAVE_GETHOSTBYNAME_R */
				}
			if ( ! name )
				name = inet_ntop( AF_INET, &addressNetwork->sin_addr,
						_cache.raw(), size );
			_hostName = name;
			}
		else
			{
			addressFile = static_cast<sockaddr_un*>( _address );
			if ( addressFile->sun_path[ 0 ] )
				_hostName = addressFile->sun_path[ 0 ];
			}
		}
	return ( _hostName );
	M_EPILOG
	}

}

}

