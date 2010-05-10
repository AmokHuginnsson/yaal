/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hopenssl.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
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

#if ! defined(OPENSSL_THREADS)
#error Thread support in OpenSSL library is missing.
#endif /* not OPENSSL_THREADS */

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hopenssl.hxx"
#include "hlog.hxx"

#include "hcore.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief OpenSSL libraary wrapper helpers.
 */
namespace openssl_helper
{

/*! \brief Format OpenSSL related error.
 *
 * \param a_oBuffer - buffer for error message.
 * \param err - error code to translate to message.
 * \return Error message.
 */
HString& format_error_message( HString& a_oBuffer, int err = 0 )
	{
	int long l_iCode = 0;
	a_oBuffer = err ? ERR_error_string( err, NULL ) : "";
	while ( ( l_iCode = ERR_get_error() ) )
		a_oBuffer.append( ( a_oBuffer.is_empty() ? "" : "\n" ) ).append( ERR_error_string( l_iCode, NULL ) );
	return ( a_oBuffer );
	}

}

yaal::hcore::HString HOpenSSL::f_oSSLKey;
yaal::hcore::HString HOpenSSL::f_oSSLCert;

int HOpenSSL::OSSLContext::_instances = 0;
HMutex HOpenSSL::OSSLContext::_mutex( HMutex::TYPE::RECURSIVE );
HOpenSSL::OSSLContext::mutexes_t HOpenSSL::OSSLContext::_sslLibMutexes;

HOpenSSL::OSSLContext::OSSLContext( void ) : _context( NULL ), _users( 0 )
	{
	}

int long unsigned get_thread_id( void )
	{
	return ( HThread::get_id() );
	}

void HOpenSSL::OSSLContext::init( void )
	{
	M_PROLOG
	HLock l_oLock( _mutex );
	if ( _instances == 0 )
		{
		SSL_load_error_strings();
		int numLocks( CRYPTO_num_locks() );
		M_ENSURE( numLocks > 0 );
		_sslLibMutexes.resize( numLocks );
		for ( mutexes_t::iterator it( _sslLibMutexes.begin() ), endIt( _sslLibMutexes.end() ); it != endIt; ++ it )
			{
			it->first = mutex_ptr_t( new HMutex( HMutex::TYPE::RECURSIVE ) );
			it->second = 0;
			}
		CRYPTO_set_locking_callback( &HOpenSSL::OSSLContext::libssl_rule_mutex );
		CRYPTO_set_id_callback( &get_thread_id );
		SSL_library_init();
		}
	SSL_METHOD* l_pxMethod = static_cast<SSL_METHOD*>( method() );
	SSL_CTX* ctx = NULL;
	HString l_oBuffer;
	ERR_clear_error();
	_context = ctx = SSL_CTX_new( l_pxMethod );
	if ( ! _context )
		throw HOpenSSLFatalException( openssl_helper::format_error_message( l_oBuffer ) );
	++ _instances;
	if ( SSL_CTX_use_PrivateKey_file( ctx, f_oSSLKey.raw(), SSL_FILETYPE_PEM ) <= 0 )
		throw HOpenSSLFatalException( openssl_helper::format_error_message( l_oBuffer ) );
	if ( SSL_CTX_use_certificate_file( ctx, f_oSSLCert.raw(), SSL_FILETYPE_PEM ) <= 0 )
		throw HOpenSSLFatalException( openssl_helper::format_error_message( l_oBuffer ) );
	if ( ! SSL_CTX_check_private_key( ctx ) )
		throw HOpenSSLFatalException( openssl_helper::format_error_message( l_oBuffer ) );
	return;
	M_EPILOG
	}

void* HOpenSSL::OSSLContext::method( void ) const
	{
	return ( do_method() );
	}

HOpenSSL::OSSLContext::~OSSLContext( void )
	{
	M_PROLOG
	HLock l_oLock( _mutex );
	M_ENSURE( ! _users );
	if ( _context )
		SSL_CTX_free( static_cast<SSL_CTX*>( _context ) );
	_context = NULL;
	-- _instances;
	if ( _instances == 0 )
		{
		ERR_remove_state( getpid() );
		ERR_remove_state( 0 );
		ENGINE_cleanup();
		CONF_modules_unload( 1 );
		ERR_free_strings();
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		CRYPTO_set_locking_callback( NULL );
		for ( int i( 0 ), SIZE( static_cast<int>( _sslLibMutexes.size() ) ); i < SIZE; ++ i )
			{
			mutex_info_t& m( _sslLibMutexes[ i ] );
			if ( m.second > 0 )
				{
				log_trace << "A ssl lock " << CRYPTO_get_lock_name( i ) << " still holds " << m.second << "locks." <<endl;
				while ( m.second -- )
					m.first->unlock();
				}
			}
		_sslLibMutexes.clear();
		}
	return;
	M_EPILOG
	}

void* HOpenSSL::OSSLContext::create_ssl( void )
	{
	M_PROLOG
	HLock l_oLock( _mutex );
	M_ASSERT( _context );
	SSL* ssl( SSL_new( static_cast<SSL_CTX*>( _context ) ) );
	HString l_oBuffer;
	if ( ! ssl )
		throw HOpenSSLException( openssl_helper::format_error_message( l_oBuffer ) );
	++ _users;
	return ( ssl );
	M_EPILOG
	}

void HOpenSSL::OSSLContext::consume_ssl( void* ssl_ )
	{
	M_PROLOG
	HLock l_oLock( _mutex );
	M_ASSERT( ssl_ );
	SSL_free( static_cast<SSL*>( ssl_ ) );
	-- _users;
	M_EPILOG
	}

void HOpenSSL::OSSLContext::libssl_rule_mutex( int mode, int nth, char const* file_, int line_ )
	{
	M_PROLOG
	HLock l_oLock( _mutex );
	mutex_info_t& m( _sslLibMutexes[ nth ] );
	if ( mode & CRYPTO_LOCK )
		{
		m.first->lock();
		++ m.second;
		}
	else
		{
		M_ASSERT( m.second > 0 );
		m.first->unlock();
		-- m.second;
		}
	file_ = NULL;
	line_ = 0;
	return;
	M_EPILOG
	}

HOpenSSL::OSSLContextServer::OSSLContextServer( void )
	{
	M_PROLOG
	init();
	M_EPILOG
	}

void* HOpenSSL::OSSLContextServer::do_method( void ) const
	{
	M_PROLOG
	SSL_METHOD* m( SSLv23_server_method() );
	M_ENSURE( m );
	return ( m );
	M_EPILOG
	}

HOpenSSL::OSSLContextClient::OSSLContextClient( void )
	{
	M_PROLOG
	init();
	M_EPILOG
	}

void* HOpenSSL::OSSLContextClient::do_method( void ) const
	{
	M_PROLOG
	SSL_METHOD* m( SSLv23_client_method() );
	M_ENSURE( m );
	return ( m );
	M_EPILOG
	}

HOpenSSL::HOpenSSL( int a_iFileDescriptor, TYPE::ssl_context_type_t a_eType )
	: _pendingOperation( false ), _ssl( NULL ),
	_ctx( ( a_eType == TYPE::SERVER )
			? static_cast<OSSLContext*>( &OSSLContextServerInstance::get_instance() )
			: static_cast<OSSLContext*>( &OSSLContextClientInstance::get_instance() ) ),
	do_accept_or_connect( ( a_eType == TYPE::SERVER ) ? &HOpenSSL::accept : &HOpenSSL::connect )
	{
	M_PROLOG
	SSL* ssl( static_cast<SSL*>( _ctx->create_ssl() ) );
	_ssl = ssl;
	SSL_set_fd( ssl, a_iFileDescriptor );
	accept_or_connect();
	return;
	M_EPILOG
	}

HOpenSSL::~HOpenSSL( void )
	{
	M_PROLOG
	_ctx->consume_ssl( _ssl );
	return;
	M_EPILOG
	}

void HOpenSSL::accept_or_connect( void )
	{
	M_PROLOG
	int ret = (this->*do_accept_or_connect)();
	if ( ret == -1 )
		{
		check_err( ret );
		_pendingOperation = true;
		}
	else
		_pendingOperation = false;
	return;
	M_EPILOG
	}

int HOpenSSL::accept( void )
	{
	M_PROLOG
	return ( SSL_accept( static_cast<SSL*>( _ssl ) ) );
	M_EPILOG
	}

int HOpenSSL::connect( void )
	{
	M_PROLOG
	return ( SSL_connect( static_cast<SSL*>( _ssl ) ) );
	M_EPILOG
	}

void HOpenSSL::check_err( int code ) const
	{
	M_PROLOG
	int err = SSL_get_error( static_cast<SSL*>( _ssl ), code );
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
	M_ASSERT( _ssl );
	if ( _pendingOperation )
		accept_or_connect();
	int nRead = -1;
	if ( ! _pendingOperation )
		{
		nRead = SSL_read( static_cast<SSL*>( _ssl ), a_pvBuffer, static_cast<int>( a_lSize ) );
		if ( nRead <= 0 )
			check_err( nRead );
		}
	return ( nRead );
	M_EPILOG
	}

int long HOpenSSL::write( void const* const a_pvBuffer, int long const& a_lSize )
	{
	M_PROLOG
	M_ASSERT( _ssl );
	if ( _pendingOperation )
		accept_or_connect();
	int nWritten = 0;
	if ( ! _pendingOperation )
		{
		nWritten = SSL_write( static_cast<SSL*>( _ssl ), a_pvBuffer, static_cast<int>( a_lSize ) );
		if ( nWritten <= 0 )
			check_err( nWritten );
		}
	return ( nWritten );
	M_EPILOG
	}

void HOpenSSL::shutdown( void )
	{
	M_PROLOG
	M_ASSERT( _ssl );
	SSL_shutdown( static_cast<SSL*>( _ssl ) );
	M_EPILOG
	}

}

}

