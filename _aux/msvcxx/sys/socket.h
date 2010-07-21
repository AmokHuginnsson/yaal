#ifndef YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED
#define YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED 1

#define setsockopt unix_setsockopt
#define htons unix_htons
#define ntohs unix_ntohs
#define inet_ntop unix_inet_ntop
#define socket unix_off
#define accept accept_off
#define connect connect_off
#define listen listen_off
#define shutdown shutdown_off
#define bind bind_off
#include <glibc/sys/socket.h>
#undef shutdown
#undef listen
#undef connect
#undef accept
#undef socket
#undef bind

namespace msvcxx
{

int unix_socket( int, int, int );
int unix_listen( int const&, int const& );
int unix_accept( int, struct sockaddr*, socklen_t* );
int unix_connect( int, struct sockaddr*, socklen_t );
int unix_shutdown( int, int );
int unix_bind( int&, const struct sockaddr*, socklen_t );

}

inline int socket( int af_, int type_, int protocol_ )
	{ return ( msvcxx::unix_socket( af_, type_, protocol_ ) ); }

inline int bind( int& s_, const struct sockaddr* name_, socklen_t namelen_ )
	{ return ( msvcxx::unix_bind( s_, name_, namelen_ ) ); }

inline int listen( int s_, int backlog_ )
	{ return ( msvcxx::unix_listen( s_, backlog_ ) ); }

inline int accept( int fd_, struct sockaddr* addr_, socklen_t* len_ )
	{ return ( msvcxx::unix_accept( fd_, addr_, len_ ) ); }

inline int connect( int fd_, struct sockaddr* addr_, socklen_t len_ )
	{ return ( msvcxx::unix_connect( fd_, addr_, len_ ) ); }

inline int shutdown( int fd_, int how_ )
	{ return ( msvcxx::unix_shutdown( fd_, how_ ) ); }

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

