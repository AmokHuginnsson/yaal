/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "config.hxx"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#ifdef HAVE_NETINET_IN_H
#	include <netinet/in.h>
#endif /* #ifdef HAVE_NETINET_IN_H */
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hudpsocket.hxx"
#include "system.hxx"

namespace yaal {

namespace hcore {

HUDPSocket::HUDPSocket( MODE type_ )
	: _fileDescriptor( -1 ) {
	M_PROLOG
	try {
		init( type_ );
	} catch ( ... ) {
		if ( _fileDescriptor >= 0 ) {
			M_TEMP_FAILURE_RETRY( hcore::system::close( _fileDescriptor ) );
			_fileDescriptor = -1;
		}
		throw;
	}
	return;
	M_EPILOG
}

HUDPSocket::HUDPSocket( int port_, ip_t ip_, MODE type_ )
	: _fileDescriptor( -1 ) {
	M_PROLOG
	try {
		init( type_ );
		bind( port_, ip_ );
	} catch ( ... ) {
		if ( _fileDescriptor >= 0 ) {
			M_TEMP_FAILURE_RETRY( hcore::system::close( _fileDescriptor ) );
			_fileDescriptor = -1;
		}
		throw;
	}
	return;
	M_EPILOG
}

HUDPSocket::~HUDPSocket( void ) {
	M_PROLOG
	M_ASSERT( _fileDescriptor >= 0 );
	M_TEMP_FAILURE_RETRY( hcore::system::close( _fileDescriptor ) );
	_fileDescriptor = -1;
	return;
	M_DESTRUCTOR_EPILOG
}

void HUDPSocket::init( MODE type_ ) {
	M_PROLOG
	if ( type_ == MODE::DEFAULT ) {
		type_ = MODE::BLOCKING;
	}
	M_ENSURE( ( _fileDescriptor = ::socket(
					PF_INET,
					static_cast<int>( SOCK_DGRAM ),
					0 /* info libc "Creating a Socket"
							 says that "zero is usually right for PROTOCOL" */ ) ) >= 0 );
		if ( type_ == MODE::NONBLOCKING ) {
			int statusFlags( ::fcntl( _fileDescriptor, F_GETFL, 0 ) );
			M_ENSURE( statusFlags >= 0 );
			M_ENSURE( ::fcntl( _fileDescriptor, F_SETFL, statusFlags | O_NONBLOCK ) == 0 );
		}
	system::set_close_on_exec( _fileDescriptor, true );
	return;
	M_EPILOG
}

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
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

void HUDPSocket::bind( int port_, ip_t ip_ ) {
	M_PROLOG
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = fwd_htons( static_cast<int short unsigned>( port_ ) );
	address.sin_addr.s_addr = ip_.raw();
	M_ENSURE(
		( ::bind( _fileDescriptor, reinterpret_cast<sockaddr*>( &address ), static_cast<socklen_t>( sizeof ( address ) ) ) == 0 ),
		resolver::ip_to_string( ip_ ) + ":" + port_
	);
	return;
	M_EPILOG
}

void HUDPSocket::connect( ip_t ip_, int port_ ) {
	M_PROLOG
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = fwd_htons( static_cast<int short unsigned>( port_ ) );
	address.sin_addr.s_addr = ip_.raw();
	M_ENSURE(
		( ::connect( _fileDescriptor, reinterpret_cast<sockaddr*>( &address ), static_cast<socklen_t>( sizeof ( address ) ) ) == 0 ),
		resolver::ip_to_string( ip_ ) + ":" + port_
	);
	return;
	M_EPILOG
}

void HUDPSocket::send_to( ip_t dest_, int port_, void const* data_, int long size_ ) {
	M_PROLOG
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = fwd_htons( static_cast<int short unsigned>( port_ ) );
	address.sin_addr.s_addr = dest_.raw();
	M_ENSURE(
		::sendto( _fileDescriptor, data_, static_cast<size_t>( size_ ),
				0, reinterpret_cast<sockaddr*>( &address ),
				static_cast<socklen_t>( sizeof ( address ) )
		) == size_
	);
	return;
	M_EPILOG
}

void HUDPSocket::send( void const* data_, int long size_ ) {
	M_PROLOG
	M_ENSURE( ::write( _fileDescriptor, data_, static_cast<size_t>( size_ ) ) == size_ );
	return;
	M_EPILOG
}

int long HUDPSocket::receive( ODatagram& datagram_ ) {
	M_PROLOG
	sockaddr_in address;
	socklen_t len( static_cast<socklen_t>( sizeof ( address ) ) );
	int long nRead( ::recvfrom( _fileDescriptor, datagram_._data.raw(),
				static_cast<size_t>( datagram_._size ), 0, reinterpret_cast<sockaddr*>( &address ), &len ) );
	datagram_._ip = ip_t( address.sin_addr.s_addr );
	datagram_._port = fwd_ntohs( address.sin_port );
	return ( nRead );
	M_EPILOG
}

int HUDPSocket::get_file_descriptor( void ) const
	{ return ( _fileDescriptor ); }

}

}

