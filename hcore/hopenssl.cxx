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

namespace openssl_helper
{

HString& format_error_message( HString& a_oBuffer )
	{
	int l_iCode = 0;
	a_oBuffer = "";
	while ( ( l_iCode = ERR_get_error() ) )
		( a_oBuffer += ( a_oBuffer.is_empty() ? "" : "\n" ) ) += ERR_error_string( l_iCode, NULL );
	return ( a_oBuffer );
	}

}

HOpenSSL::OSSLContext::OSSLContext( void ) : f_pvMethod( NULL ), f_pvContext( NULL )
	{
	}

void HOpenSSL::OSSLContext::init( void )
	{
	M_PROLOG
	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
	SSL_METHOD* l_pxMethod = static_cast<SSL_METHOD*>( f_pvMethod );
	SSL_CTX* ctx = NULL;
	HString l_oBuffer;
	ERR_clear_error();
	f_pvContext = ctx = SSL_CTX_new( l_pxMethod );
	if ( ! f_pvContext )
		throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
	if ( SSL_CTX_use_PrivateKey_file( ctx, n_oSSLKey, SSL_FILETYPE_PEM ) <= 0 )
		throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
	if ( SSL_CTX_use_certificate_file( ctx, n_oSSLCert, SSL_FILETYPE_PEM ) <= 0 )
		throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
	if ( ! SSL_CTX_check_private_key( ctx ) )
		throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
	return;
	M_EPILOG
	}

HOpenSSL::OSSLContext::~OSSLContext( void )
	{
	if ( f_pvContext )
		SSL_CTX_free( static_cast<SSL_CTX*>( f_pvContext ) );
	f_pvContext = NULL;
	return;
	}

HOpenSSL::OSSLContextServer::OSSLContextServer( void )
	{
	M_PROLOG
	f_pvMethod = SSLv23_server_method();
	init();
	M_EPILOG
	}

HOpenSSL::OSSLContextClient::OSSLContextClient( void )
	{
	M_PROLOG
	f_pvMethod = SSLv23_client_method();
	init();
	M_EPILOG
	}

HOpenSSL::HOpenSSL( int a_iFileDescriptor, TYPE::ssl_context_type_t a_eType ) : f_pvSSL( NULL )
	{
	M_PROLOG
	HString l_oBuffer;
	OSSLContext* context = ( ( a_eType == TYPE::D_SERVER ) ? static_cast<OSSLContext*>( &OSSLContextServerInstance::get_instance() ) : static_cast<OSSLContext*>( &OSSLContextClientInstance::get_instance() ) );
	SSL* ssl = NULL;
	f_pvSSL = ssl = SSL_new( static_cast<SSL_CTX*>( context->f_pvContext ) );
	if ( ! f_pvSSL )
		throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
	SSL_set_fd( ssl, a_iFileDescriptor );
	if ( a_eType == TYPE::D_SERVER )
		{
		if ( SSL_accept( ssl ) == -1 )
			throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
		}
	else
		{
		M_ASSERT( a_eType == TYPE::D_CLIENT );
		if ( SSL_connect( ssl ) == -1 )
			throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
		}
	return;
	M_EPILOG
	}

HOpenSSL::~HOpenSSL( void )
	{
	if ( f_pvSSL )
		SSL_free( static_cast<SSL*>( f_pvSSL ) );
	return;
	}

int HOpenSSL::read( void* const a_pvBuffer, int const a_iSize )
	{
	M_PROLOG
	M_ASSERT( f_pvSSL );
	return ( SSL_read( static_cast<SSL*>( f_pvSSL ), a_pvBuffer, a_iSize ) );
	M_EPILOG
	}

int HOpenSSL::write( void const* const a_pvBuffer, int const a_iSize )
	{
	M_PROLOG
	M_ASSERT( f_pvSSL );
	return ( SSL_write( static_cast<SSL*>( f_pvSSL ), a_pvBuffer, a_iSize ) );
	M_EPILOG
	}

void HOpenSSL::shutdown( void )
	{
	M_PROLOG
	M_ASSERT( f_pvSSL );
	while ( ! SSL_shutdown( static_cast<SSL*>( f_pvSSL ) ) )
		;
	M_EPILOG
	}

}

}

