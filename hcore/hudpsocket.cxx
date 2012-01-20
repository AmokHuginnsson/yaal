/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hudpsocket.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hudpsocket.hxx"

namespace yaal {

namespace hcore {

HUDPSocket::HUDPSocket( socket_type_t type_ )
	: _fileDescriptor( -1 ) {
	M_PROLOG
	init( type_ );
	return;
	M_EPILOG
}

HUDPSocket::HUDPSocket( int port_, ip_t ip_, socket_type_t type_ )
	: _fileDescriptor( -1 ) {
	M_PROLOG
	init( type_ );
	bind( port_, ip_ );
	return;
	M_EPILOG
}

HUDPSocket::~HUDPSocket( void ) {
	M_PROLOG
	M_ASSERT( _fileDescriptor >= 0 );
	::close( _fileDescriptor );
	_fileDescriptor = -1;
	return;
	M_DESTRUCTOR_EPILOG
}

void HUDPSocket::init( socket_type_t type_ ) {
	M_PROLOG
	if ( type_ == TYPE::DEFAULT )
		type_ = socket_type_t( TYPE::BLOCKING ) | TYPE::PLAIN;
	if ( ( !!( type_ & TYPE::BLOCKING ) ) && ( !!( type_ & TYPE::NONBLOCKING ) ) )
		M_THROW( "bad socket blocking option", type_.value() );
	if ( ( !!( type_ & TYPE::SSL ) ) && ( !!( type_ & TYPE::PLAIN ) ) )
		M_THROW( "bad socket ssl option", type_.value() );
	if ( ! ( type_ & ( socket_type_t( TYPE::BLOCKING ) | TYPE::NONBLOCKING ) ) )
		type_ |= TYPE::BLOCKING;
	if ( ! ( type_ & ( socket_type_t( TYPE::SSL ) | TYPE::PLAIN ) ) )
		type_ |= TYPE::PLAIN;
	M_ENSURE( ( _fileDescriptor = ::socket(
					PF_INET,
					static_cast<int>( SOCK_DGRAM ),
					0 /* info libc "Creating a Socket"
							 says that "zero is usually right for PROTOCOL" */ ) ) >= 0 );
	if ( !!( type_ & TYPE::NONBLOCKING ) ) {
		int flags( ::fcntl( _fileDescriptor, F_GETFL, 0 ) );
		M_ENSURE( flags >= 0 );
		M_ENSURE( ::fcntl( _fileDescriptor, F_SETFL, flags | O_NONBLOCK ) == 0 );
	}
	return;
	M_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename T>
inline T fwd_htons( T arg_ ) {
	return ( htons( arg_ ) );
}
template<typename T>
inline T fwd_ntohs( T arg_ ) {
	return ( ntohs( arg_ ) );
}
#pragma GCC diagnostic error "-Wold-style-cast"
}

void HUDPSocket::bind( int port_, ip_t ip_ ) {
	M_PROLOG
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = fwd_htons( static_cast<int short unsigned>( port_ ) );
	address.sin_addr.s_addr = ip_.raw();

	M_ENSURE_EX( ( ::bind( _fileDescriptor, reinterpret_cast<sockaddr*>( &address ), sizeof ( address ) ) == 0 ), resolver::ip_to_string( ip_ ) + ":" + port_ );
	return;
	M_EPILOG
}

void HUDPSocket::send_to( ip_t dest_, int port_, void const* data_, int long size_ ) {
	M_PROLOG
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = fwd_htons( static_cast<int short unsigned>( port_ ) );
	address.sin_addr.s_addr = dest_.raw();
	M_ENSURE( ::sendto( _fileDescriptor, data_, size_, 0, reinterpret_cast<sockaddr*>( &address ), sizeof ( address ) ) == size_ );
	return;
	M_EPILOG
}

int long HUDPSocket::receive( ODatagram& datagram_ ) {
	M_PROLOG
	sockaddr_in address;
	socklen_t len( sizeof ( address ) );
	int long nRead( ::recvfrom( _fileDescriptor, datagram_._data.raw(), datagram_._size, 0, reinterpret_cast<sockaddr*>( &address ), &len ) );
	datagram_._ip = ip_t( address.sin_addr.s_addr );
	datagram_._port = fwd_ntohs( address.sin_port );
	return ( nRead );
	M_EPILOG
}

int HUDPSocket::get_file_descriptor( void ) const
	{ return ( _fileDescriptor ); }

}

}

