#ifndef YAAL_MSVCXX_OPENSSL_RSA_H_INCLUDED
#define YAAL_MSVCXX_OPENSSL_RSA_H_INCLUDED 1

#define _EXCPT_H_ 1
#define __STRALIGN_H_ 1

#define SSL_set_fd SSL_set_fd_off

#include <../include/openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#undef X509_NAME 
#include <openssl/engine.h>
#include <openssl/err.h>
#undef ERROR
#undef min
#undef max
#undef SSL_set_fd

#include "hcore/macro.hxx"

namespace msvcxx
{

M_YAAL_HCORE_PUBLIC_API int SSL_set_fd( SSL* s, int fd );

}

inline int SSL_set_fd( SSL* ssl_, int fd_ )
	{
	return ( msvcxx::SSL_set_fd( ssl_, fd_ ) );
	}

#endif /* not YAAL_MSVCXX_OPENSSL_RSA_H_INCLUDED */
