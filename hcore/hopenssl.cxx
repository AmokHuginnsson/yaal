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
#include <openssl/conf.h>
#include <openssl/engine.h>
#include <unistd.h>

#include <iostream>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hopenssl.h"
#include "hlog.h"

#include "hcore.h"

namespace yaal
{

namespace hcore
{

namespace openssl_helper
{

HString& format_error_message( HString& a_oBuffer, int err = 0 )
	{
	int long l_iCode = 0;
	a_oBuffer = err ? ERR_error_string( err, NULL ) : "";
	while ( ( l_iCode = ERR_get_error() ) )
		( a_oBuffer += ( a_oBuffer.is_empty() ? "" : "\n" ) ) += ERR_error_string( l_iCode, NULL );
	return ( a_oBuffer );
	}

}

yaal::hcore::HString HOpenSSL::f_oSSLKey;
yaal::hcore::HString HOpenSSL::f_oSSLCert;

int HOpenSSL::OSSLContext::f_iUsers = 0;
HMutex HOpenSSL::OSSLContext::f_oMutex;

HOpenSSL::OSSLContext::OSSLContext( void ) : f_pvMethod( NULL ), f_pvContext( NULL )
	{
	}

void HOpenSSL::OSSLContext::init( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	if ( f_iUsers == 0 )
		{
		SSL_load_error_strings();
		SSLeay_add_ssl_algorithms();
		}
	++ f_iUsers;
	SSL_METHOD* l_pxMethod = static_cast<SSL_METHOD*>( f_pvMethod );
	SSL_CTX* ctx = NULL;
	HString l_oBuffer;
	ERR_clear_error();
	f_pvContext = ctx = SSL_CTX_new( l_pxMethod );
	if ( ! f_pvContext )
		throw HOpenSSLFatalException( openssl_helper::format_error_message( l_oBuffer ) );
	if ( SSL_CTX_use_PrivateKey_file( ctx, f_oSSLKey, SSL_FILETYPE_PEM ) <= 0 )
		throw HOpenSSLFatalException( openssl_helper::format_error_message( l_oBuffer ) );
	if ( SSL_CTX_use_certificate_file( ctx, f_oSSLCert, SSL_FILETYPE_PEM ) <= 0 )
		throw HOpenSSLFatalException( openssl_helper::format_error_message( l_oBuffer ) );
	if ( ! SSL_CTX_check_private_key( ctx ) )
		throw HOpenSSLFatalException( openssl_helper::format_error_message( l_oBuffer ) );
	return;
	M_EPILOG
	}

HOpenSSL::OSSLContext::~OSSLContext( void )
	{
	HLock l_oLock( f_oMutex );
	if ( f_pvContext )
		SSL_CTX_free( static_cast<SSL_CTX*>( f_pvContext ) );
	f_pvContext = NULL;
	-- f_iUsers;
	if ( f_iUsers == 0 )
		{
		ERR_remove_state( getpid() );
		ERR_remove_state( 0 );
		ENGINE_cleanup();
		CONF_modules_unload( 1 );
		ERR_free_strings();
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		}
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

HOpenSSL::HOpenSSL( int a_iFileDescriptor, TYPE::ssl_context_type_t a_eType )
	: f_bPendingOperation( false ), f_pvSSL( NULL ),
	do_accept_or_connect( ( a_eType == TYPE::D_SERVER ) ? &HOpenSSL::accept : &HOpenSSL::connect )
	{
	M_PROLOG
	HString l_oBuffer;
	OSSLContext* context = ( ( a_eType == TYPE::D_SERVER ) ? static_cast<OSSLContext*>( &OSSLContextServerInstance::get_instance() ) : static_cast<OSSLContext*>( &OSSLContextClientInstance::get_instance() ) );
	SSL* ssl = NULL;
	f_pvSSL = ssl = SSL_new( static_cast<SSL_CTX*>( context->f_pvContext ) );
	if ( ! f_pvSSL )
		throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
	SSL_set_fd( ssl, a_iFileDescriptor );
	accept_or_connect();
	return;
	M_EPILOG
	}

HOpenSSL::~HOpenSSL( void )
	{
	if ( f_pvSSL )
		SSL_free( static_cast<SSL*>( f_pvSSL ) );
	return;
	}

void HOpenSSL::accept_or_connect( void )
	{
	M_PROLOG
	int ret = (this->*do_accept_or_connect)();
	if ( ret == -1 )
		{
		check_err( ret );
		f_bPendingOperation = true;
		}
	else
		f_bPendingOperation = false;
	return;
	M_EPILOG
	}

int HOpenSSL::accept( void )
	{
	M_PROLOG
	return ( SSL_accept( static_cast<SSL*>( f_pvSSL ) ) );
	M_EPILOG
	}

int HOpenSSL::connect( void )
	{
	M_PROLOG
	return ( SSL_connect( static_cast<SSL*>( f_pvSSL ) ) );
	M_EPILOG
	}

void HOpenSSL::check_err( int code ) const
	{
	M_PROLOG
	int err = SSL_get_error( static_cast<SSL*>( f_pvSSL ), code );
	if ( ( err != SSL_ERROR_WANT_READ ) && ( err != SSL_ERROR_WANT_WRITE ) )
		{
		HString l_oBuffer;
		throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
		}
	return;
	M_EPILOG
	}

int long HOpenSSL::read( void* const a_pvBuffer, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( f_pvSSL );
	if ( f_bPendingOperation )
		accept_or_connect();
	int nRead = -1;
	if ( ! f_bPendingOperation )
		{
		nRead = SSL_read( static_cast<SSL*>( f_pvSSL ), a_pvBuffer, static_cast<int>( a_lSize ) );
		if ( nRead <= 0 )
			check_err( nRead );
		}
	return ( nRead );
	M_EPILOG
	}

int long HOpenSSL::write( void const* const a_pvBuffer, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( f_pvSSL );
	if ( f_bPendingOperation )
		accept_or_connect();
	int nWritten = 0;
	if ( ! f_bPendingOperation )
		{
		nWritten = SSL_write( static_cast<SSL*>( f_pvSSL ), a_pvBuffer, static_cast<int>( a_lSize ) );
		if ( nWritten <= 0 )
			check_err( nWritten );
		}
	return ( nWritten );
	M_EPILOG
	}

void HOpenSSL::shutdown( void )
	{
	M_PROLOG
	M_ASSERT( f_pvSSL );
	SSL_shutdown( static_cast<SSL*>( f_pvSSL ) );
	M_EPILOG
	}

}

}

