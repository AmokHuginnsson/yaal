/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hsocket.c - this file is integral part of `stdhapi' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
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

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hsocket.h"
#include "xalloc.h"
#include "hstring.h"
#include "hlog.h"

namespace stdhapi
{

namespace hcore
{

#define E_HCORE_HSOCKET_NOT_INITIALIZED		1
#define E_HCORE_HSOCKET_NOT_A_SERVER			2
#define E_HCORE_HSOCKET_ALREADY_LISTENING	3

char const * const n_ppcErrMsgHSocket [ 4 ] =
	{
	_ ( "ok" ),
	_ ( "socket not initialized" ),
	_ ( "socket is not a server" ),
	_ ( "already listening" )
	};

HSocket::HSocket ( socket_type_t const a_eSocketType,
		int const a_iMaximumNumberOfClients )
	: HRawFile ( ), f_bNeedShutdown ( false ), f_eType ( D_DEFAULTS ),
	f_iMaximumNumberOfClients ( a_iMaximumNumberOfClients ),
	f_iAddressSize ( 0 ), f_pvAddress ( NULL ), f_poClients ( NULL ),
	f_oHostName ( ), f_oVarTmpBuffer ( )
	{
	M_PROLOG
	f_eType = a_eSocketType;
	if ( ( a_eSocketType & D_FILE ) && ( a_eSocketType & D_NETWORK ) )
		M_THROW ( _ ( "bad socket namespace setting" ), static_cast < int > ( a_eSocketType ) );
	if ( ! ( a_eSocketType & ( D_FILE | D_NETWORK ) ) )
		f_eType |= D_NETWORK;
	if ( ( a_eSocketType & D_BLOCKING ) && ( a_eSocketType & D_NONBLOCKING ) )
		M_THROW ( _ ( "bad socket option" ), static_cast < int > ( a_eSocketType ) );
	if ( ! ( a_eSocketType & ( D_BLOCKING | D_NONBLOCKING ) ) )
		f_eType |= D_BLOCKING;
	if ( f_iMaximumNumberOfClients >= 0 )
		{
		M_ENSURE ( ( f_iFileDescriptor = socket (
						( f_eType & D_NETWORK ) ? PF_INET : PF_LOCAL,
						static_cast < int > ( SOCK_STREAM ),
						0 /* info libc "Creating a Socket"
								 says that "zero is usually right for PROTOCOL" */ ) ) >= 0 );
		if ( f_eType & D_NONBLOCKING )
			M_ENSURE ( fcntl ( f_iFileDescriptor, F_SETFL, O_NONBLOCK ) == 0 );
		}
	if ( f_eType & D_NETWORK )
		f_pvAddress = xcalloc ( 1, sockaddr_in );
	else
		f_pvAddress = xcalloc ( 1, sockaddr_un );
	return;
	M_EPILOG
	}

HSocket::~HSocket ( void )
	{
	M_PROLOG
	shutdown ( );
	/* There will be no memory leakage if shutdown() throws,
	 * because application has to terminate. */
	if ( f_pvAddress )
		xfree ( f_pvAddress );
	return;
	M_EPILOG
	}

void HSocket::shutdown ( void )
	{
	M_PROLOG
	int l_iKey = 0;
	HSocket * l_poSocket = NULL;
	sockaddr_un * l_psAddressFile = NULL;
	if ( f_poClients )
		{
		f_poClients->rewind ( );
		while ( f_poClients->iterate ( l_iKey, l_poSocket ) )
			{
			if ( l_poSocket )
				delete l_poSocket;
			f_poClients->operator [ ] ( l_iKey ) = NULL;
			}
		delete f_poClients;
		f_poClients = NULL;
		if ( ( f_eType & D_FILE ) && ( f_pvAddress ) )
			{
			l_psAddressFile = static_cast < sockaddr_un * > ( f_pvAddress );
			if ( l_psAddressFile->sun_path [ 0 ] )
				M_ENSURE ( unlink ( l_psAddressFile->sun_path ) == 0 );
			}
		}
	if ( f_bNeedShutdown && ( f_iFileDescriptor >= 0 ) )
		{
		M_ENSURE ( ::shutdown ( f_iFileDescriptor, 2 ) == 0 );
		f_iFileDescriptor = - 1;
		f_bNeedShutdown = false;
		}
	return;
	M_EPILOG
	}

void HSocket::shutdown_client ( int a_iFileDescriptor )
	{
	M_PROLOG
	HSocket * l_poClient = NULL;
	if ( ! f_poClients )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_A_SERVER ], f_iFileDescriptor );
	if ( ! f_poClients->get ( a_iFileDescriptor, l_poClient ) )
		M_THROW ( _ ( "no such client" ), a_iFileDescriptor );
	M_ASSERT ( l_poClient );
	delete l_poClient;
	f_poClients->remove ( a_iFileDescriptor );
	return;
	M_EPILOG
	}

void HSocket::listen ( char const * const a_pcAddress, int const a_iPort )
	{
	M_PROLOG
	int l_iReuseAddr = 1;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_INITIALIZED ], f_iFileDescriptor );
	if ( f_poClients )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_ALREADY_LISTENING ], f_iFileDescriptor );
	if ( f_iMaximumNumberOfClients < 1 )
		M_THROW ( _ ( "bad maximum number of clients" ), f_iMaximumNumberOfClients );
	make_address ( a_pcAddress, a_iPort );
	M_ENSURE ( setsockopt ( f_iFileDescriptor, SOL_SOCKET, SO_REUSEADDR,
				& l_iReuseAddr, sizeof ( int ) ) == 0 );
	M_ENSURE ( ::bind ( f_iFileDescriptor,
				static_cast < sockaddr * > ( f_pvAddress ), f_iAddressSize ) == 0 );
	M_ENSURE ( ::listen ( f_iFileDescriptor, f_iMaximumNumberOfClients ) == 0 );
	f_poClients = new clients_t ( f_iMaximumNumberOfClients );
	f_bNeedShutdown = true;
	return;
	M_EPILOG
	}

HSocket * HSocket::accept ( void )
	{
	M_PROLOG
	int l_iFileDescriptor = - 1;
	socklen_t l_iAddressSize = 0;
	HSocket * l_poSocket = NULL;
	sockaddr_in l_sAddressNetwork;
	sockaddr_un l_sAddressFile;
	sockaddr * l_psAddress;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_INITIALIZED ], f_iFileDescriptor );
	if ( ! f_poClients )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_A_SERVER ], f_iFileDescriptor );
	if ( f_eType & D_NETWORK )
		{
		l_psAddress = static_cast < sockaddr * > (
				static_cast < void * > ( & l_sAddressNetwork ) );
		l_iAddressSize = sizeof ( l_sAddressNetwork );
		}
	else
		{
		l_psAddress = static_cast < sockaddr * > (
				static_cast < void * > ( & l_sAddressFile ) );
		l_iAddressSize = sizeof ( l_sAddressFile );
		}
	M_ENSURE ( ( l_iFileDescriptor = ::accept ( f_iFileDescriptor,
					l_psAddress, & l_iAddressSize ) ) >= 0 );
	if ( f_eType & D_NONBLOCKING )
		M_ENSURE ( fcntl ( l_iFileDescriptor, F_SETFL, O_NONBLOCK ) == 0 );
	/* - 1 means that constructor shall not create socket */
	l_poSocket = new HSocket ( f_eType, - 1 );
	l_poSocket->f_iFileDescriptor = l_iFileDescriptor;
	l_poSocket->f_iAddressSize = l_iAddressSize;
	l_poSocket->f_bNeedShutdown = true;
	memcpy ( l_poSocket->f_pvAddress, l_psAddress, l_iAddressSize );
	if ( f_poClients->has_key ( l_iFileDescriptor ) )
		M_THROW ( _ ( "inconsitient client list state" ), l_iFileDescriptor );
	f_poClients->operator [ ] ( l_iFileDescriptor ) = l_poSocket;
	return ( l_poSocket );
	M_EPILOG
	}

void HSocket::connect ( char const * const a_pcAddress, int const a_iPort )
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_INITIALIZED ], f_iFileDescriptor );
	make_address ( a_pcAddress, a_iPort );
	M_ENSURE ( ::connect ( f_iFileDescriptor,
				static_cast < sockaddr * > ( f_pvAddress ), f_iAddressSize ) == 0 );
	f_bNeedShutdown = true;
	return;
	M_EPILOG
	}

void HSocket::make_address ( char const * const a_pcAddress, int const a_iPort )
	{
#define D_GETHOST_BY_NAME_R_WORK_BUFFER_SIZE 1024;
	M_PROLOG
	int l_iError = 0;
	sockaddr_in * l_psAddressNetwork = NULL;
	sockaddr_un * l_psAddressFile = NULL;
#ifdef HAVE_GETHOSTBYNAME_R
	hostent l_sHostName;
	hostent * l_psHostName = NULL;
#else /* HAVE_GETHOSTBYNAME_R */
	addrinfo * l_psAddrInfo = NULL;
#endif /* ! HAVE_GETHOSTBYNAME_R */
	if ( f_eType & D_NETWORK )
		{
		l_psAddressNetwork = static_cast < sockaddr_in * > ( f_pvAddress );
		l_psAddressNetwork->sin_family = AF_INET;
		l_psAddressNetwork->sin_port = htons (
				static_cast < int short unsigned > ( a_iPort ) );
#ifdef HAVE_GETHOSTBYNAME_R
		f_iAddressSize = D_GETHOST_BY_NAME_R_WORK_BUFFER_SIZE;
		f_oVarTmpBuffer.hs_realloc ( f_iAddressSize );
		while ( gethostbyname_r ( a_pcAddress, & l_sHostName,
					f_oVarTmpBuffer.raw ( ), f_iAddressSize,
					& l_psHostName, & l_iError ) == ERANGE )
			f_oVarTmpBuffer.hs_realloc ( f_iAddressSize <<= 1 );
		g_iErrNo = l_iError;
		M_ENSURE ( l_psHostName );
		l_psAddressNetwork->sin_addr.s_addr = reinterpret_cast < in_addr * > (
				l_sHostName.h_addr_list [ 0 ] )->s_addr;
#else /* HAVE_GETHOSTBYNAME_R */
		l_iError = getaddrinfo ( a_pcAddress, NULL, NULL, & l_psAddrInfo );
		M_ENSURE ( ! l_iError && l_psAddrInfo );
		l_psAddressNetwork->sin_addr.s_addr = reinterpret_cast < in_addr * > ( l_psAddrInfo->ai_addr )->s_addr;
		freeaddrinfo ( l_psAddrInfo );
#endif /* not HAVE_GETHOSTBYNAME_R */
		f_iAddressSize = sizeof ( sockaddr_in );
		}
	else /* f_eType & D_FILE */
		{
		l_psAddressFile = static_cast < sockaddr_un * > ( f_pvAddress );
		l_psAddressFile->sun_family = AF_LOCAL;
		strncpy ( l_psAddressFile->sun_path, a_pcAddress,
				sizeof ( l_psAddressFile->sun_path ) );
		l_psAddressFile->sun_path [ sizeof ( l_psAddressFile->sun_path ) - 1 ] = 0;
		f_iAddressSize = SUN_LEN ( l_psAddressFile );
		}
	f_oHostName = a_pcAddress;
	return;
	M_EPILOG
#undef D_GETHOST_BY_NAME_R_WORK_BUFFER_SIZE
	}

int const HSocket::get_port ( void ) const
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_INITIALIZED ], f_iFileDescriptor );
	sockaddr_in * l_psAddressNetwork = NULL;
	if ( ! ( f_eType & D_NETWORK ) )
		M_THROW ( _ ( "unix socket has not a port attribute" ),
				static_cast < int > ( f_eType ) );
	l_psAddressNetwork = static_cast < sockaddr_in * > ( f_pvAddress );
	return ( ntohs ( l_psAddressNetwork->sin_port ) );
	M_EPILOG
	}

HSocket * HSocket::get_client ( int const a_iFileDescriptor ) const
	{
	M_PROLOG
	HSocket * l_poClient = NULL;
	if ( ! f_poClients )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_A_SERVER ], f_iFileDescriptor );
	f_poClients->get ( a_iFileDescriptor, l_poClient );
	return ( l_poClient );
	M_EPILOG
	}

bool HSocket::get_client_next ( int & a_riFileDescriptor, HSocket * & a_rpoClient ) const
	{
	M_PROLOG
	if ( ! f_poClients )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_A_SERVER ], f_iFileDescriptor );
	return ( f_poClients->iterate ( a_riFileDescriptor, a_rpoClient ) );
	M_EPILOG
	}

void HSocket::rewind_client_list ( void ) const
	{
	M_PROLOG
	if ( ! f_poClients )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_A_SERVER ], f_iFileDescriptor );
	f_poClients->rewind ( );
	return;
	M_EPILOG
	}

int HSocket::read_until ( HString & a_roMessage, char const * const a_pcStopSet )
	{
	M_PROLOG
	int l_iCtr = 0;
	char * l_pcPtr = NULL;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_INITIALIZED ], f_iFileDescriptor );
	a_roMessage = "";
	do
		{
		f_oVarTmpBuffer.hs_realloc ( l_iCtr + 1 );
		l_pcPtr = f_oVarTmpBuffer.raw ( );
		if ( read ( l_pcPtr + l_iCtr, sizeof ( char ) * 1 ) <= 0 )
			break;
		}
	while ( ! strchr ( a_pcStopSet, l_pcPtr [ l_iCtr ++ ] ) );
	l_iCtr --; /* go back one char for stripping terminator */
	if ( l_iCtr > 0 )
		{
		l_pcPtr [ l_iCtr ] = 0;
		if ( l_iCtr > 0 )
			{
			a_roMessage.hs_realloc ( l_iCtr );
			memcpy ( a_roMessage.raw ( ), l_pcPtr, l_iCtr + 1 );
			}
		}
	return ( l_iCtr );
	M_EPILOG
	}

int HSocket::write_until_eos ( HString const & a_roMessage )
	{
	M_PROLOG
	int l_iSize = 0;
	l_iSize = a_roMessage.get_length ( );
	if ( l_iSize > 0 )
		l_iSize = write ( a_roMessage, l_iSize );
	return ( l_iSize );
	M_EPILOG
	}

int HSocket::get_client_count ( void ) const
	{
	M_PROLOG
	if ( ! f_poClients )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_A_SERVER ], f_iFileDescriptor );
	return ( f_poClients->quantity ( ) );
	M_EPILOG
	}

HString const & HSocket::get_host_name ( void )
	{
#define D_GETHOST_BY_NAME_R_WORK_BUFFER_SIZE 1024
	M_PROLOG
	int l_iError = 0;
	int l_iSize = D_GETHOST_BY_NAME_R_WORK_BUFFER_SIZE;
	sockaddr_in * l_psAddressNetwork = NULL;
	sockaddr_un * l_psAddressFile = NULL;
#ifdef HAVE_GETHOSTBYNAME_R
	int l_iCode = 0;
	hostent l_sHostName;
	hostent * l_psHostName = NULL;
#else /* HAVE_GETHOSTBYNAME_R */
	l_iSize = NI_MAXHOST;
#endif /* ! HAVE_GETHOSTBYNAME_R */
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_ppcErrMsgHSocket [ E_HCORE_HSOCKET_NOT_INITIALIZED ], f_iFileDescriptor );
	if ( f_oHostName.is_empty ( ) )
		{
		if ( f_eType & D_NETWORK )
			{
			l_psAddressNetwork = reinterpret_cast < sockaddr_in * > ( f_pvAddress );
			f_oVarTmpBuffer.hs_realloc ( l_iSize );
#ifdef HAVE_GETHOSTBYNAME_R
			memset ( & l_sHostName, 0, sizeof ( hostent ) );
			while ( ( l_iError = gethostbyaddr_r ( &l_psAddressNetwork->sin_addr, f_iAddressSize,
						AF_INET, & l_sHostName,
						f_oVarTmpBuffer.raw ( ),
						l_iSize, & l_psHostName, & l_iCode ) ) == ERANGE )
				f_oVarTmpBuffer.hs_realloc ( l_iSize <<= 1 );
			if ( l_iCode )
				M_LOG ( hstrerror ( l_iCode ) );
			g_iErrNo = l_iError;
			M_ENSURE ( l_iError == 0 );
			if ( l_psHostName )
				f_oHostName = l_sHostName.h_name;
			else
				{
				f_oHostName = inet_ntop ( AF_INET, & l_psAddressNetwork->sin_addr,
						f_oVarTmpBuffer.raw ( ), l_iSize );
				}
#else /* HAVE_GETHOSTBYNAME_R */
			l_iError = getnameinfo (
							reinterpret_cast < sockaddr * > ( l_psAddressNetwork ), f_iAddressSize,
							f_oVarTmpBuffer.raw ( ), l_iSize, NULL, 0, NI_NOFQDN );
			M_ENSURE ( l_iError == 0 );
			f_oHostName = f_oVarTmpBuffer;
#endif /* ! HAVE_GETHOSTBYNAME_R */
			}
		else
			{
			l_psAddressFile = static_cast < sockaddr_un * > ( f_pvAddress );
			if ( l_psAddressFile->sun_path [ 0 ] )
				f_oHostName = l_psAddressFile->sun_path [ 0 ];
			}
		}
	return ( f_oHostName );
	M_EPILOG
#undef D_GETHOST_BY_NAME_R_WORK_BUFFER_SIZE
	}

}

}

