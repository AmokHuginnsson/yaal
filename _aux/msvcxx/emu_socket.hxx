#ifndef YAAL_MSVCXX_EMU_SOCKET_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_SOCKET_HXX_INCLUDED 1

namespace msvcxx
{

int socket( int, int, int );
int listen( int, int );
int accept( int, struct sockaddr*, socklen_t* );
int connect( int, struct sockaddr*, socklen_t );
int shutdown( int, int );
int bind( int, struct sockaddr const*, socklen_t );
int setsockopt( int, int, int, void const*, socklen_t );
int getsockopt( int, int, int, void*, socklen_t* );

} /* namespace msvcxx */

#endif /* #ifndef YAAL_MSVCXX_EMU_SOCKET_HXX_INCLUDED */
