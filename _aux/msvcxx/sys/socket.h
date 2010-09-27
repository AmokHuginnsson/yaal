#ifndef YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED
#define YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED 1

#include <sys/time.h>

#define ENOTCONN WSAENOTCONN
#define ECONNRESET WSAECONNRESET
#define EINPROGRESS WSAEINPROGRESS
static int const SHUT_RDWR = SD_RECEIVE | SD_SEND;

struct sockaddr_un
	{
	int sun_family;
	char sun_path[MAX_PATH];		/* Path name.  */
	};
#define SUN_LEN(ptr) ((size_t) (((struct sockaddr_un *) 0)->sun_path) + strlen ((ptr)->sun_path))
namespace msvcxx
{

int unix_socket( int, int, int );
int unix_listen( int, int );
int unix_accept( int, struct sockaddr*, socklen_t* );
int unix_connect( int, struct sockaddr*, socklen_t );
int unix_shutdown( int, int );
int unix_bind( int, struct sockaddr const*, socklen_t );
int unix_setsockopt( int, int, int, void const*, socklen_t );
int unix_getsockopt( int, int, int, void*, socklen_t* );

}

inline int ms_socket( int af_, int type_, int protocol_ )
	{ return ( msvcxx::unix_socket( af_, type_, protocol_ ) ); }
#define socket ms_socket

inline int bind( int s_, struct sockaddr const* name_, socklen_t namelen_ )
	{ return ( msvcxx::unix_bind( s_, name_, namelen_ ) ); }

inline int listen( int s_, int backlog_ )
	{ return ( msvcxx::unix_listen( s_, backlog_ ) ); }

inline int accept( int fd_, struct sockaddr* addr_, socklen_t* len_ )
	{ return ( msvcxx::unix_accept( fd_, addr_, len_ ) ); }

inline int connect( int fd_, struct sockaddr* addr_, socklen_t len_ )
	{ return ( msvcxx::unix_connect( fd_, addr_, len_ ) ); }

inline int shutdown( int fd_, int how_ )
	{ return ( msvcxx::unix_shutdown( fd_, how_ ) ); }

inline int setsockopt( int fd_, int level_, int optname_, char* optval_, int unsigned optlen_ )
	{	return ( msvcxx::unix_setsockopt( fd_, level_, optname_, optval_, optlen_ ) ); }

inline int getsockopt( int fd_, int level_, int optname_, void* optval_, socklen_t* optlen_ )
	{	return ( msvcxx::unix_getsockopt( fd_, level_, optname_, optval_, optlen_ ) ); }

inline char const* inet_ntop( int af_, void const* src_, char* dst_, socklen_t size_ )
	{
	char* name = inet_ntoa( *static_cast<in_addr const*>( src_ ) );
	if ( name )
		::strncpy_s( dst_, size_, name, size_ );
	return ( name ? dst_ : NULL );
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

inline SocketErrno& get_socket_errno( void )
	{
	static SocketErrno socketErrno;
	return ( socketErrno );
	}

#undef errno
#define errno get_socket_errno()

#endif /* not YAAL_MSVCXX_SYS_SOCKET_H_INCLUDED */

