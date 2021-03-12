/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <unistd.h>
#include <sys/socket.h>

#include "config.hxx"

#ifdef HAVE_NETDB_H
#	include <netdb.h>
#endif /* #ifdef HAVE_NETDB_H */
#ifdef HAVE_ARPA_INET_H
#	include <arpa/inet.h>
#endif /* #ifdef HAVE_ARPA_INET_H */
#ifdef HAVE_NETINET_IN_H
#	include <netinet/in.h>
#endif /* #ifdef HAVE_NETINET_IN_H */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
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
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	HUTF8String utf8( hostName_ );
#if defined( HAVE_GETADDRINFO ) && ( HAVE_GETADDRINFO != 0 )
	addrinfo* addrInfo( nullptr );
	errno = ::getaddrinfo( utf8.c_str(), nullptr, nullptr, &addrInfo );
	M_ENSURE( ! errno && addrInfo, hostName_ );
	ip = ip_t( static_cast<sockaddr_in*>( static_cast<void*>( addrInfo->ai_addr ) )->sin_addr.s_addr );
	::freeaddrinfo( addrInfo );
#else /* #if defined( HAVE_GETADDRINFO ) && ( HAVE_GETADDRINFO != 0 ) */
	hostent hostName;
	hostent* hostNameStatus( nullptr );
	int size( static_cast<int>( _cache->get_size() ) );
	_cache->realloc( size );
	int error( 0 );
	while ( ::gethostbyname_r( utf8.c_str(), &hostName,
				_cache->raw(), size,
				&hostNameStatus, &error ) == ERANGE )
		_cache->realloc( size <<= 1 );
	errno = error;
	M_ENSURE( hostNameStatus, hostName_ );
	ip = ip_t( reinterpret_cast<in_addr*>( hostName.h_addr_list[ 0 ] )->s_addr );
#endif /* #else #if defined( HAVE_GETADDRINFO ) && ( HAVE_GETADDRINFO != 0 ) */
	return ip;
	M_EPILOG
}

HString resolver::ip_to_string( ip_t ip_ ) {
	M_PROLOG
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip_.raw();
	char const* name( nullptr );
	while ( ! name ) {
		name = inet_ntop( AF_INET, &addr.sin_addr, _cache->get<char>(), static_cast<socklen_t>( _cache->get_size() ) );
		if ( ! name ) {
			M_ENSURE( errno == ENOSPC );
			_cache->realloc( _cache->size() * 2 );
		}
	}
	return name;
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
		reinterpret_cast<sockaddr*>( &addr ), static_cast<socklen_t>( sizeof ( addr ) ),
		_cache->get<char>(), static_cast<socklen_t>( size ), nullptr, 0, NI_NOFQDN
	);
	HScopedValueReplacement<int> saveErrno( errno, error );
	M_ENSURE( error == 0 );
	name = _cache->get<char>();
#else /* #if defined ( HAVE_GETNAMEINFO ) && ( HAVE_GETNAMEINFO != 0 ) */
	int size( RESOLVER_INITIAL_CACHE_SIZE );
	int code( 0 );
	hostent hostName;
	hostent* hostNameStatus( nullptr );
	::memset( &hostName, 0, sizeof ( hostent ) );
	while ( ( error = ::gethostbyaddr_r( &addr.sin_addr, sizeof ( addr.sin_addr ),
				AF_INET, &hostName,
				_cache->raw(),
				size, &hostNameStatus, &code ) ) == ERANGE ) {
		_cache->realloc( size <<= 1 );
	}
	if ( code ) {
		log_trace << error_message( code ) << endl;
	}
	HScopedValueReplacement<int> saveErrno( errno, error );
	M_ENSURE( error == 0 );
	if ( hostNameStatus ) {
		name = hostName.h_name;
	}
#endif /* #else #if defined ( HAVE_GETNAMEINFO ) && ( HAVE_GETNAMEINFO != 0 ) */
	if ( ! name ) {
		name = ip_to_string( ip_ );
	}
	return name;
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

