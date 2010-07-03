#ifndef YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED
#define YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED 1

#define socket unix_socket
#define bind unix_bind
#define setsockopt unix_setsockopt
#define htons unix_htons
#define ntohs unix_ntohs
#define inet_ntop unix_inet_ntop
#define accept accept_off
#define connect connect_off
#define listen listen_off
#define shutdown shutdown_off
#include <glibc/sys/socket.h>
#undef shutdown
#undef listen
#undef connect
#undef accept

namespace msvcxx
{

int unix_listen( int const&, int const& );
int unix_accept( int, struct sockaddr*, socklen_t* );
int unix_connect( int, struct sockaddr*, socklen_t );
int unix_shutdown( int, int );

}

inline int listen( int s_, int backlog_ )
	{ return msvcxx::unix_listen( s_, backlog_ ); }

inline int accept( int fd_, struct sockaddr* addr_, socklen_t* len_ )
	{ return ( msvcxx::unix_accept( fd_, addr_, len_ ) ); }

inline int connect( int fd_, struct sockaddr* addr_, socklen_t len_ )
	{ return ( msvcxx::unix_connect( fd_, addr_, len_ ) ); }

inline int shutdown( int fd_, int how_ )
	{ return ( msvcxx::unix_shutdown( fd_, how_ ) ); }

#endif /* not YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED */

