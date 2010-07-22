#ifndef YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED
#define YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED 1

#define setsockopt setsockopt_off
#define socket socket_off
#define accept accept_off
#define connect connect_off
#define listen listen_off
#define shutdown shutdown_off
#define bind bind_off
#define getaddrinfo getaddrinfo_off
#define getnameinfo getnameinfo_off
#define freeaddrinfo freeaddrinfo_off
#define fd_set fd_set_off
#define timeval timeval_off
#define _SYS_UN_H 1
#define _NETINET_IN_H 1
#define _NETDB_H 1
#define _ARPA_INET_H 1
#undef gethostname
#define gethostname gethostname_off
#define select select_off
#define __inline inline
#include <ws2tcpip.h>
#include "cleanup.hxx"
#undef select
#undef gethostname
#undef fd_set
#undef timeval
#undef getaddrinfo
#undef getnameinfo
#undef freeaddrinfo
#undef setsockopt
#undef shutdown
#undef listen
#undef connect
#undef accept
#undef socket
#undef bind
#define ENOTCONN WSAENOTCONN
#define ECONNRESET WSAECONNRESET
static int const SHUT_RDWR = 1 | 2;

struct sockaddr_un
	{
	int sun_family;
	char sun_path[MAX_PATH];		/* Path name.  */
	};
#define SUN_LEN(ptr) ((size_t) (((struct sockaddr_un *) 0)->sun_path) + strlen ((ptr)->sun_path))
namespace msvcxx
{

int unix_socket( int, int, int );
int unix_listen( int const&, int const& );
int unix_accept( int, struct sockaddr*, socklen_t* );
int unix_connect( int&, struct sockaddr*, socklen_t );
int unix_shutdown( int, int );
int unix_bind( int&, struct sockaddr const*, socklen_t );
int unix_getaddrinfo( char const*, char const*, struct addrinfo const*, struct addrinfo** );
void unix_freeaddrinfo( struct addrinfo* );
int unix_getnameinfo( struct sockaddr const*, socklen_t, char*, size_t, char*, size_t, int );
int unix_setsockopt( int, int, int, void const*, socklen_t );

}

inline int socket( int af_, int type_, int protocol_ )
	{ return ( msvcxx::unix_socket( af_, type_, protocol_ ) ); }

inline int bind( int& s_, struct sockaddr const* name_, socklen_t namelen_ )
	{ return ( msvcxx::unix_bind( s_, name_, namelen_ ) ); }

inline int listen( int s_, int backlog_ )
	{ return ( msvcxx::unix_listen( s_, backlog_ ) ); }

inline int accept( int fd_, struct sockaddr* addr_, socklen_t* len_ )
	{ return ( msvcxx::unix_accept( fd_, addr_, len_ ) ); }

inline int connect( int& fd_, struct sockaddr* addr_, socklen_t len_ )
	{ return ( msvcxx::unix_connect( fd_, addr_, len_ ) ); }

inline int shutdown( int fd_, int how_ )
	{ return ( msvcxx::unix_shutdown( fd_, how_ ) ); }

inline int setsockopt( int fd_, int level_, int optname_, void const* optval_, socklen_t optlen_ )
	{	return ( msvcxx::unix_setsockopt( fd_, level_, optname_, optval_, optlen_ ) ); }

inline int getaddrinfo( char const* node_,
								char const* service_,
								struct addrinfo const* hints_,
								struct addrinfo** res_ )
	{
	return ( msvcxx::unix_getaddrinfo( node_, service_, hints_, res_ ) );
	}

inline void freeaddrinfo( struct addrinfo* res_ )
	{
	msvcxx::unix_freeaddrinfo( res_ );
	}

inline int getnameinfo( struct sockaddr const* sa_,
								 socklen_t salen_,
								 char* host_, size_t hostlen_,
                 char* serv_, size_t servlen_,
                 int flags_ )
	{
	return ( msvcxx::unix_getnameinfo( sa_, salen_, host_, hostlen_, serv_, servlen_, flags_ ) );
	}

int get_socket_error( void );
void set_socket_error( int );

class SocketErrno
	{
public:
	SocketErrno& operator = ( int errno_ )
		{
		set_socket_error( errno_ );
		return ( *this );
		}
	operator int ( void ) const
		{ return ( get_socket_error() ); }
	};

#undef errno
#define errno SocketErrno()

#endif /* not YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED */

