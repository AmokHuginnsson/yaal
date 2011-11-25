/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	resolver.cxx - this file is integral part of `yaal' project.

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

#include <netdb.h>
#include <arpa/inet.h>

#include "config.hxx"

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "resolver.hxx"
#include "hlog.hxx"

namespace yaal {

namespace hcore {

HIP::HIP( u8_t oct0_, u8_t oct1_, u8_t oct2_, u8_t oct3_ )
	: _data( 0 ) {
	u8_t* ip( reinterpret_cast<u8_t*>( &_data ) );
	ip[0] = oct0_;
	ip[1] = oct1_;
	ip[2] = oct2_;
	ip[3] = oct3_;
}

namespace {
static int const RESOLVER_INITIAL_CACHE_SIZE( 256 );
}

resolver::cache_on_tls_t resolver::_cache( RESOLVER_INITIAL_CACHE_SIZE );

ip_t resolver::get_ip( HString const& hostName_ ) {
	M_PROLOG
	ip_t ip;
	int error( 0 );
#if defined( HAVE_GETADDRINFO ) && ( HAVE_GETADDRINFO != 0 )
	addrinfo* addrInfo( NULL );
	error = ::getaddrinfo( hostName_.raw(), NULL, NULL, &addrInfo );
	M_ENSURE_EX( ! error && addrInfo, hostName_ );
	ip = ip_t( reinterpret_cast<sockaddr_in*>( addrInfo->ai_addr )->sin_addr.s_addr );
	::freeaddrinfo( addrInfo );
#else /* #if defined( HAVE_GETADDRINFO ) && ( HAVE_GETADDRINFO != 0 ) */
	hostent hostName;
	hostent* hostNameStatus( NULL );
	int size( static_cast<int>( _cache->get_size() ) );
	_cache->realloc( size );
	while ( ::gethostbyname_r( hostName_.raw(), &hostName,
				_cache->raw(), size,
				&hostNameStatus, &error ) == ERANGE )
		_cache->realloc( size <<= 1 );
	errno = error;
	M_ENSURE_EX( hostNameStatus, hostName_ );
	ip = ip_t( reinterpret_cast<in_addr*>( hostName.h_addr_list[ 0 ] )->s_addr );
#endif /* #else #if defined( HAVE_GETADDRINFO ) && ( HAVE_GETADDRINFO != 0 ) */
	return ( ip );
	M_EPILOG
}

HString resolver::ip_to_string( ip_t ip_ ) {
	M_PROLOG
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip_.raw();
	char const* name( NULL );
	while ( ! name ) {
		name = inet_ntop( AF_INET, &addr.sin_addr, _cache->raw(), static_cast<socklen_t>( _cache->get_size() ) );
		if ( ! name ) {
			M_ENSURE( errno == ENOSPC );
			_cache->realloc( _cache->size() * 2 );
		}
	}
	return ( name );
	M_EPILOG
}

HString resolver::get_name( ip_t ip_ ) {
	M_PROLOG
	HString name;
	int error( 0 );
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip_.raw();
#if defined ( HAVE_GETNAMEINFO ) && ( HAVE_GETNAMEINFO != 0 )
	int size( NI_MAXHOST );
	_cache->realloc( size );
	error = getnameinfo(
					reinterpret_cast<sockaddr*>( &addr ), sizeof ( addr ),
					_cache->raw(), size, NULL, 0, NI_NOFQDN );
	HScopedValueReplacement<int> saveErrno( errno, error );
	M_ENSURE( error == 0 );
	name = _cache->raw();
#else /* #if defined ( HAVE_GETNAMEINFO ) && ( HAVE_GETNAMEINFO != 0 ) */
	int size( RESOLVER_INITIAL_CACHE_SIZE );
	int code( 0 );
	hostent hostName;
	hostent* hostNameStatus( NULL );
	::memset( &hostName, 0, sizeof ( hostent ) );
	while ( ( error = ::gethostbyaddr_r( &addr.sin_addr, sizeof ( addr.sin_addr ),
				AF_INET, &hostName,
				_cache->raw(),
				size, &hostNameStatus, &code ) ) == ERANGE )
		_cache->realloc( size <<= 1 );
	if ( code )
		log_trace << error_message( code ) << endl;
	HScopedValueReplacement<int> saveErrno( errno, error );
	M_ENSURE( error == 0 );
	if ( hostNameStatus )
		name = hostName.h_name;
#endif /* #else #if defined ( HAVE_GETNAMEINFO ) && ( HAVE_GETNAMEINFO != 0 ) */
	if ( ! name )
		name = ip_to_string( ip_ );
	return ( name );
	M_EPILOG
}

char const* resolver::error_message( int code_ ) {
#if defined ( HAVE_GETNAMEINFO ) && ( HAVE_GETNAMEINFO != 0 )
	return ( gai_strerror( code_ ) );
#else /* #if defined ( HAVE_GETNAMEINFO ) && ( HAVE_GETNAMEINFO != 0 ) */
	return ( hstrerror( code_ ) );
#endif /* #else #if defined ( HAVE_GETNAMEINFO ) && ( HAVE_GETNAMEINFO != 0 ) */
}

}

}

