/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hopenssl.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hopenssl.h"

#include "hcore.h"

namespace yaal
{

namespace hcore
{

HOpenSSL::OSSLContext::OSSLContext( void ) : f_pvContext( NULL )
	{
	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
	SSL_METHOD* l_pxMethod = static_cast<SSL_METHOD*>( get_method() );
	SSL_CTX* ctx = NULL;
	f_pvContext = ctx = SSL_CTX_new( l_pxMethod );
	if ( SSL_CTX_use_certificate_file( ctx, n_oSSLCert, SSL_FILETYPE_PEM ) <= 0 )
		{
		ERR_print_errors_fp(stderr);
		exit(3);
		}
	if ( SSL_CTX_use_PrivateKey_file( ctx, n_oSSLKey, SSL_FILETYPE_PEM ) <= 0 )
		{
		ERR_print_errors_fp(stderr);
		exit(4);
		}
	if ( ! SSL_CTX_check_private_key( ctx ) )
		{
		fprintf(stderr,"Private key does not match the certificate public key\n");
		exit(5);
		}
	return;
	}

HOpenSSL::OSSLContext::~OSSLContext( void )
	{
	if ( f_pvContext )
		SSL_CTX_free( static_cast<SSL_CTX*>( f_pvContext ) );
	f_pvContext = NULL;
	return;
	}

void* HOpenSSL::OSSLContextServer::get_method( void )
	{
	return ( SSLv23_server_method() );
	}

void* HOpenSSL::OSSLContextClient::get_method( void )
	{
	return ( SSLv23_client_method() );
	}

}

}

