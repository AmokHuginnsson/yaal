#ifndef MSVCXX_VISUALSTUDIO_OPENSSL_RSA_H_INCLUDED
#define MSVCXX_VISUALSTUDIO_OPENSSL_RSA_H_INCLUDED

#define _EXCPT_H_ 1
#define __STRALIGN_H_ 1

#include <../include/openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#undef X509_NAME 
#include <openssl/engine.h>
#include <openssl/err.h>
#undef ERROR
#undef min
#undef max

#endif /* not MSVCXX_VISUALSTUDIO_OPENSSL_RSA_H_INCLUDED */
