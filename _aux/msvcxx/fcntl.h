#ifndef YAAL_MSVCXX_FCNTL_H_INCLUDED
#define YAAL_MSVCXX_FCNTL_H_INCLUDED 1

#include <glibc/fcntl.h>

#define socket unix_socket
#define bind unix_bind
#define listen unix_listen
#define accept unix_accept
#define connect unix_connect
#define shutdown unix_shutdown
#define setsockopt unix_setsockopt
#define htons unix_htons
#define ntohs unix_ntohs
#define inet_ntop unix_inet_ntop

#endif /* not YAAL_MSVCXX_FCNTL_H_INCLUDED */
