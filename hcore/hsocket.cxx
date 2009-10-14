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

char const* const n_ppcErrMsgHSocket[ 4 ] =
	{
	_( "ok" ),
	_( "socket not initialized" ),
	_( "socket is not a server" ),
	_( "already listening" )
	};

bool HSocket::f_bResolveHostnames = true;

HSocket::HSocket( socket_type_t const a_eSocketType,
		int const a_iMaximumNumberOfClients )
	: HRawFile( !!( a_eSocketType & TYPE::SSL_SERVER )
			? HRawFile::TYPE::SSL_SERVER
			: ( ( ( a_eSocketType == TYPE::DEFAULT ) || ( !!( a_eSocketType & TYPE::SSL_CLIENT ) ) )
				? HRawFile::TYPE::SSL_CLIENT : HRawFile::TYPE::DEFAULT ) ),
	f_bNeedShutdown( false ), f_eType( a_eSocketType ),
	f_iMaximumNumberOfClients( a_iMaximumNumberOfClients ),
	f_iAddressSize( 0 ), f_pvAddress( NULL ), f_poClients( NULL ),
	f_oHostName()
	{
	M_PROLOG
	if ( f_eType == TYPE::DEFAULT )
		f_eType |= TYPE::SSL_CLIENT;
	if ( ( !!( a_eSocketType & TYPE::FILE ) ) && ( !!( a_eSocketType & TYPE::NETWORK ) ) )
		M_THROW( _( "bad socket namespace setting" ), a_eSocketType.value() );
	if ( ! ( a_eSocketType & ( socket_type_t( TYPE::FILE ) | TYPE::NETWORK ) ) )
		f_eType |= TYPE::NETWORK;
	if ( ( !!( a_eSocketType & TYPE::BLOCKING ) ) && ( !!( a_eSocketType & TYPE::NONBLOCKING ) ) )
		M_THROW( _( "bad socket option" ), a_eSocketType.value() );
	if ( ! ( a_eSocketType & ( socket_type_t( TYPE::BLOCKING ) | TYPE::NONBLOCKING ) ) )
		f_eType |= TYPE::BLOCKING;
	if ( f_iMaximumNumberOfClients >= 0 )
		{
		M_ENSURE( ( f_iFileDescriptor = ::socket(
						!!( f_eType & TYPE::NETWORK ) ? PF_INET : PF_UNIX,
						static_cast<int>( SOCK_STREAM ),
						0 /* info libc "Creating a Socket"
								 says that "zero is usually right for PROTOCOL" */ ) ) >= 0 );
		if ( !!( f_eType & TYPE::NONBLOCKING ) )
			M_ENSURE( ::fcntl( f_iFileDescriptor, F_SETFL, O_NONBLOCK ) == 0 );
		}
	if ( !!( f_eType & TYPE::NETWORK ) )
		f_pvAddress = xcalloc<sockaddr_in>( 1 );
	else
		f_pvAddress = xcalloc<sockaddr_un>( 1 );
	return;
	M_EPILOG
	}

HSocket::~HSocket( void )
	{
	M_PROLOG
	shutdown();
	/* There will be no memory leakage if shutdown() throws,
	 * because application has to terminate. */
	if ( f_pvAddress )
		xfree ( f_pvAddress );
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
	sockaddr_un* l_psAddressFile = NULL;
	if ( f_poClients )
		{
		delete f_poClients;
		f_poClients = NULL;
		if ( ( !!( f_eType & TYPE::FILE ) ) && ( f_pvAddress ) )
			{
			l_psAddressFile = static_cast<sockaddr_un*>( f_pvAddress );
			if ( l_psAddressFile->sun_path [ 0 ] )
				M_ENSURE( ::unlink( l_psAddressFile->sun_path ) == 0 );
			}
		}
	if ( f_bNeedShutdown && ( f_iFileDescriptor >= 0 ) )
		{
		if ( is_write_ready( f_iFileDescriptor ) )
			M_ENSURE( ( ::shutdown( f_iFileDescriptor, SHUT_RDWR ) == 0 ) || ( errno == ENOTCONN ) || ( errno == ECONNRESET ) );
		HRawFile::do_close();
		f_bNeedShutdown = false;
		}
	return( 0 );
	M_EPILOG
	}

void HSocket::shutdown_client( int a_iFileDescriptor )
	{
	M_PROLOG
	ptr_t l_oClient;
	if ( ! f_poClients )
		M_THROW( n_ppcErrMsgHSocket[ NOT_A_SERVER ], f_iFileDescriptor );
	if ( f_poClients->get( a_iFileDescriptor, l_oClient ) )
		M_THROW( _( "no such client" ), a_iFileDescriptor );
	M_ASSERT( !! l_oClient );
	f_poClients->remove( a_iFileDescriptor );
	return;
	M_EPILOG
	}

void HSocket::listen( yaal::hcore::HString const& a_oAddress, int const a_iPort )
	{
	M_PROLOG
	int l_iReuseAddr = 1;
	if ( f_iFileDescriptor < 0 )
		M_THROW( n_ppcErrMsgHSocket[ NOT_INITIALIZED ], f_iFileDescriptor );
	if ( f_poClients )
		M_THROW( n_ppcErrMsgHSocket[ ALREADY_LISTENING ], f_iFileDescriptor );
	if ( f_iMaximumNumberOfClients < 1 )
		M_THROW( _( "bad maximum number of clients" ), f_iMaximumNumberOfClients );
	make_address ( a_oAddress, a_iPort );
	M_ENSURE( ::setsockopt( f_iFileDescriptor, SOL_SOCKET, SO_REUSEADDR,
				reinterpret_cast<char*>( &l_iReuseAddr ), sizeof ( l_iReuseAddr ) ) == 0 );
	M_ENSURE( ::bind( f_iFileDescriptor,
				static_cast<sockaddr*>( f_pvAddress ), f_iAddressSize ) == 0 );
	M_ENSURE( ::listen( f_iFileDescriptor, f_iMaximumNumberOfClients ) == 0 );
	f_poClients = new clients_t( f_iMaximumNumberOfClients );
	f_bNeedShutdown = true;
	return;
	M_EPILOG
	}

HSocket::ptr_t HSocket::accept( void )
	{
	M_PROLOG
	int l_iFileDescriptor = -1;
	socklen_t l_iAddressSize = 0;
	sockaddr_in l_sAddressNetwork;
	sockaddr_un l_sAddressFile;
	sockaddr* l_psAddress;
	if ( f_iFileDescriptor < 0 )
		M_THROW( n_ppcErrMsgHSocket[ NOT_INITIALIZED ], f_iFileDescriptor );
	if ( ! f_poClients )
		M_THROW( n_ppcErrMsgHSocket[ NOT_A_SERVER ], f_iFileDescriptor );
	if ( !!( f_eType & TYPE::NETWORK ) )
		{
		l_psAddress = static_cast<sockaddr*>(
				static_cast<void*>( &l_sAddressNetwork ) );
		l_iAddressSize = sizeof ( l_sAddressNetwork );
		}
	else
		{
		l_psAddress = static_cast<sockaddr*>(
				static_cast<void*>( &l_sAddressFile ) );
		l_iAddressSize = sizeof ( l_sAddressFile );
		}
	M_ENSURE( ( l_iFileDescriptor = ::accept( f_iFileDescriptor,
					l_psAddress, &l_iAddressSize ) ) >= 0 );
	if ( !!( f_eType & TYPE::NONBLOCKING ) )
		M_ENSURE( ::fcntl( l_iFileDescriptor, F_SETFL, O_NONBLOCK ) == 0 );
	/* - 1 means that constructor shall not create socket */
	socket_type_t l_eType = f_eType;
	if ( !!( f_eType & ( socket_type_t( TYPE::SSL_SERVER  ) | TYPE::SSL_CLIENT ) ) )
		l_eType &= ~socket_type_t( TYPE::SSL_CLIENT ), l_eType |= TYPE::SSL_SERVER;
	ptr_t l_oSocket = ptr_t( new HSocket( l_eType, -1 ) );
	M_ASSERT( ! l_oSocket->f_oSSL );
	l_oSocket->f_iFileDescriptor = l_iFileDescriptor;
	l_oSocket->f_iAddressSize = l_iAddressSize;
	l_oSocket->f_bNeedShutdown = true;
	l_oSocket->set_timeout( f_iTimeOut );
	::memcpy( l_oSocket->f_pvAddress, l_psAddress, l_iAddressSize );
	if ( f_poClients->has_key( l_iFileDescriptor ) )
		M_THROW( _( "inconsitent client list state" ), l_iFileDescriptor );
	f_poClients->operator[]( l_iFileDescriptor ) = l_oSocket;
	return ( l_oSocket );
	M_EPILOG
	}

void HSocket::connect( yaal::hcore::HString const& a_oAddress, int const a_iPort )
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW( n_ppcErrMsgHSocket[ NOT_INITIALIZED ], f_iFileDescriptor );
	make_address( a_oAddress, a_iPort );
	M_ENSURE( ::connect( f_iFileDescriptor,
				static_cast<sockaddr*>( f_pvAddress ), f_iAddressSize ) == 0 );
	f_bNeedShutdown = true;
	return;
	M_EPILOG
	}

void HSocket::make_address( yaal::hcore::HString const& a_oAddress, int const a_iPort )
	{
	M_PROLOG
	int l_iError = 0;
	sockaddr_in* l_psAddressNetwork = NULL;
	sockaddr_un* l_psAddressFile = NULL;
#ifdef HAVE_GETHOSTBYNAME_R
	static int const GETHOST_BY_NAME_R_WORK_BUFFER_SIZE = 1024;
	hostent l_sHostName;
	hostent* l_psHostName = NULL;
#else /* HAVE_GETHOSTBYNAME_R */
	addrinfo* l_psAddrInfo = NULL;
#endif /* ! HAVE_GETHOSTBYNAME_R */
	if ( !!( f_eType & TYPE::NETWORK ) )
		{
		l_psAddressNetwork = static_cast < sockaddr_in * > ( f_pvAddress );
		l_psAddressNetwork->sin_family = AF_INET;
		l_psAddressNetwork->sin_port = htons(
				static_cast<int short unsigned>( a_iPort ) );
#ifdef HAVE_GETHOSTBYNAME_R
		f_iAddressSize = GETHOST_BY_NAME_R_WORK_BUFFER_SIZE;
		f_oCache.pool_realloc( f_iAddressSize );
		while ( ::gethostbyname_r( a_oAddress.raw(), &l_sHostName,
					f_oCache.raw(), f_iAddressSize,
					&l_psHostName, &l_iError ) == ERANGE )
			f_oCache.pool_realloc ( f_iAddressSize <<= 1 );
		errno = l_iError;
		M_ENSURE( l_psHostName );
		l_psAddressNetwork->sin_addr.s_addr = reinterpret_cast<in_addr*>(
				l_sHostName.h_addr_list [ 0 ] )->s_addr;
#else /* HAVE_GETHOSTBYNAME_R */
		l_iError = ::getaddrinfo( a_oAddress.raw(), NULL, NULL, &l_psAddrInfo );
		M_ENSURE( ! l_iError && l_psAddrInfo );
		l_psAddressNetwork->sin_addr.s_addr = reinterpret_cast<in_addr*>( l_psAddrInfo->ai_addr )->s_addr;
		::freeaddrinfo( l_psAddrInfo );
#endif /* not HAVE_GETHOSTBYNAME_R */
		f_iAddressSize = sizeof ( sockaddr_in );
		}
	else /* f_eType & TYPE::FILE */
		{
		l_psAddressFile = static_cast<sockaddr_un*>( f_pvAddress );
		l_psAddressFile->sun_family = AF_UNIX;
		::strncpy( l_psAddressFile->sun_path, a_oAddress.raw(),
				sizeof ( l_psAddressFile->sun_path ) );
		l_psAddressFile->sun_path[ sizeof ( l_psAddressFile->sun_path ) - 1 ] = 0;
		f_iAddressSize = static_cast<int>( SUN_LEN( l_psAddressFile ) );
		}
	f_oHostName = a_oAddress;
	return;
	M_EPILOG
	}

int HSocket::get_port( void ) const
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW( n_ppcErrMsgHSocket[ NOT_INITIALIZED ], f_iFileDescriptor );
	sockaddr_in* l_psAddressNetwork = NULL;
	if ( ! ( f_eType & TYPE::NETWORK ) )
		M_THROW( _( "unix socket has not a port attribute" ), f_eType.value() );
	l_psAddressNetwork = static_cast<sockaddr_in*>( f_pvAddress );
	return ( ntohs( l_psAddressNetwork->sin_port ) );
	M_EPILOG
	}

HSocket::ptr_t HSocket::get_client( int const a_iFileDescriptor ) const
	{
	M_PROLOG
	ptr_t l_oClient;
	if ( ! f_poClients )
		M_THROW( n_ppcErrMsgHSocket[ NOT_A_SERVER ], f_iFileDescriptor );
	f_poClients->get( a_iFileDescriptor, l_oClient );
	return ( l_oClient );
	M_EPILOG
	}

HSocket::clients_t::const_iterator HSocket::begin( void ) const
	{
	M_PROLOG
	if ( ! f_poClients )
		M_THROW( n_ppcErrMsgHSocket[ NOT_A_SERVER ], f_iFileDescriptor );
	clients_t const* c = f_poClients;
	return ( c->begin() );
	M_EPILOG
	}

HSocket::clients_t::const_iterator HSocket::end( void ) const
	{
	M_PROLOG
	if ( ! f_poClients )
		M_THROW( n_ppcErrMsgHSocket[ NOT_A_SERVER ], f_iFileDescriptor );
	clients_t const* c = f_poClients;
	return ( c->end() );
	M_EPILOG
	}

int long HSocket::write_until_eos( HString const& a_roMessage )
	{
	M_PROLOG
	int long nSize = 0;
	nSize = a_roMessage.get_length();
	if ( nSize > 0 )
		nSize = write( a_roMessage.raw(), nSize );
	return ( nSize );
	M_EPILOG
	}

int HSocket::get_client_count( void ) const
	{
	M_PROLOG
	if ( ! f_poClients )
		M_THROW( n_ppcErrMsgHSocket[ NOT_A_SERVER ], f_iFileDescriptor );
	return ( f_poClients->size() );
	M_EPILOG
	}

HString const& HSocket::get_host_name( void )
	{
	static int const GETHOST_BY_NAME_R_WORK_BUFFER_SIZE = 1024;
	M_PROLOG
	int l_iError = 0;
	int l_iSize = GETHOST_BY_NAME_R_WORK_BUFFER_SIZE;
	sockaddr_in* l_psAddressNetwork = NULL;
	sockaddr_un* l_psAddressFile = NULL;
#ifdef HAVE_GETHOSTBYNAME_R
	int l_iCode = 0;
	hostent l_sHostName;
	hostent* l_psHostName = NULL;
#else /* HAVE_GETHOSTBYNAME_R */
	l_iSize = NI_MAXHOST;
#endif /* ! HAVE_GETHOSTBYNAME_R */
	if ( f_iFileDescriptor < 0 )
		M_THROW( n_ppcErrMsgHSocket[ NOT_INITIALIZED ], f_iFileDescriptor );
	if ( f_oHostName.is_empty() )
		{
		if ( !!( f_eType & TYPE::NETWORK ) )
			{
			char const* name = NULL;
			f_oCache.pool_realloc( l_iSize );
			l_psAddressNetwork = reinterpret_cast<sockaddr_in*>( f_pvAddress );
			if ( f_bResolveHostnames )
				{
#ifdef HAVE_GETHOSTBYNAME_R
				::memset( &l_sHostName, 0, sizeof ( hostent ) );
				while ( ( l_iError = ::gethostbyaddr_r( &l_psAddressNetwork->sin_addr, f_iAddressSize,
							AF_INET, &l_sHostName,
							f_oCache.raw(),
							l_iSize, &l_psHostName, &l_iCode ) ) == ERANGE )
					f_oCache.pool_realloc( l_iSize <<= 1 );
				if ( l_iCode )
					log_trace << error_message( l_iCode ) << endl;
				errno = l_iError;
				M_ENSURE( l_iError == 0 );
				if ( l_psHostName )
					name = l_sHostName.h_name;
#else /* HAVE_GETHOSTBYNAME_R */
				l_iError = getnameinfo(
								reinterpret_cast<sockaddr*>( l_psAddressNetwork ), f_iAddressSize,
								f_oCache.raw(), l_iSize, NULL, 0, NI_NOFQDN );
				M_ENSURE( l_iError == 0 );
				name = f_oCache.raw();
#endif /* ! HAVE_GETHOSTBYNAME_R */
				}
			if ( ! name )
				name = inet_ntop( AF_INET, &l_psAddressNetwork->sin_addr,
						f_oCache.raw(), l_iSize );
			f_oHostName = name;
			}
		else
			{
			l_psAddressFile = static_cast<sockaddr_un*>( f_pvAddress );
			if ( l_psAddressFile->sun_path[ 0 ] )
				f_oHostName = l_psAddressFile->sun_path[ 0 ];
			}
		}
	return ( f_oHostName );
	M_EPILOG
	}

}

}

