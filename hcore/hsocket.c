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
#include <fcntl.h>
#include <libintl.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hsocket.h"
#include "xalloc.h"
#include "hstring.h"

namespace stdhapi
{

namespace hcore
{

char const * const n_pcError = _ ( "socket not initialized" );

HSocket::HSocket ( socket_type_t const a_eSocketType,
		int const a_iMaximumNumberOfClients )
	: HRawFile ( ), f_eType ( D_DEFAULTS ),
	f_iMaximumNumberOfClients ( a_iMaximumNumberOfClients ),
	f_iAddressSize ( 0 ), f_pvAddress ( NULL ), f_poClients ( NULL )
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
	int l_iKey = 0;
	HSocket * l_poSocket = NULL;
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
		}
	if ( f_pvAddress )
		xfree ( f_pvAddress );
	return;
	M_EPILOG
	}

void HSocket::listen ( char const * const a_pcAddress, int const a_iPort )
	{
	M_PROLOG
	int l_iReuseAddr = 1;
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, f_iFileDescriptor );
	make_address ( a_pcAddress, a_iPort );
	M_ENSURE ( setsockopt ( f_iFileDescriptor, SOL_SOCKET, SO_REUSEADDR,
				& l_iReuseAddr, sizeof ( int ) ) == 0 );
	M_ENSURE ( ::bind ( f_iFileDescriptor,
				static_cast < sockaddr * > ( f_pvAddress ), f_iAddressSize ) == 0 );
	M_ENSURE ( ::listen ( f_iFileDescriptor, f_iMaximumNumberOfClients ) == 0 );
	f_poClients = new clients_t ( f_iMaximumNumberOfClients );
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
		M_THROW ( n_pcError, f_iFileDescriptor );
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
		M_THROW ( n_pcError, f_iFileDescriptor );
	make_address ( a_pcAddress, a_iPort );
	M_ENSURE ( ::connect ( f_iFileDescriptor,
				static_cast < sockaddr * > ( f_pvAddress ), f_iAddressSize ) == 0 );
	return;
	M_EPILOG
	}

void HSocket::make_address ( char const * const a_pcAddress, int const a_iPort )
	{
	M_PROLOG
#define D_GETHOST_BY_NAME_R_WORK_BUFFER_SIZE 1024;
	int l_iError = 0;
	HString l_oWorkBuffer;
	sockaddr_in * l_psAddressNetwork = NULL;
	sockaddr_un * l_psAddressFile = NULL;
	hostent l_sHostName;
	hostent * l_psHostName = NULL;
	if ( f_eType & D_NETWORK )
		{
		l_psAddressNetwork = static_cast < sockaddr_in * > ( f_pvAddress );
		l_psAddressNetwork->sin_family = AF_INET;
		l_psAddressNetwork->sin_port = htons (
				static_cast < int short unsigned > ( a_iPort ) );
		f_iAddressSize = D_GETHOST_BY_NAME_R_WORK_BUFFER_SIZE;
		l_oWorkBuffer.hs_realloc ( f_iAddressSize );
		while ( gethostbyname_r ( a_pcAddress, & l_sHostName,
					static_cast < char * > ( l_oWorkBuffer ), f_iAddressSize,
					& l_psHostName, & l_iError ) == ERANGE )
			l_oWorkBuffer.hs_realloc ( f_iAddressSize <<= 1 );
		g_iErrNo = l_iError;
		M_ENSURE ( l_psHostName );
		l_psAddressNetwork->sin_addr.s_addr = reinterpret_cast < in_addr * > (
				l_sHostName.h_addr_list [ 0 ] )->s_addr;
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
	return;
	M_EPILOG
	}

int const HSocket::get_port ( void ) const
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, f_iFileDescriptor );
	sockaddr_in * l_psAddressNetwork = NULL;
	if ( ! ( f_eType & D_NETWORK ) )
		M_THROW ( _ ( "unix socket has not a port attribute" ),
				static_cast < int > ( f_eType ) );
	l_psAddressNetwork = static_cast < sockaddr_in * > ( f_pvAddress );
	return ( ntohs ( l_psAddressNetwork->sin_port ) );
	M_EPILOG
	}

}

}

