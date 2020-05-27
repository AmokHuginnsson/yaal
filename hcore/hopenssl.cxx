/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <unistd.h> /* read(), write() for BIO */
#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#if ! defined( OPENSSL_THREADS )
#error Thread support in OpenSSL library is missing.
#endif /* not OPENSSL_THREADS */

#include "config.hxx"

#ifdef HAVE_VALGRIND_MEMCHECK_H
#include <valgrind/memcheck.h>
#endif /* #ifdef HAVE_VALGRIND_MEMCHECK_H */

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hopenssl.hxx"
#include "hcore/memory.hxx"
#include "system.hxx"
#include "hlog.hxx"

#if OPENSSL_VERSION_NUMBER < 0x10100000L
namespace {
BIO_METHOD* BIO_meth_new( int type_, char const* name_ ) {
	static BIO_METHOD method = {
		type_,
		name_,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};
	return ( &method );
}
void BIO_meth_free( BIO_METHOD* ) {
}
void BIO_meth_set_write( BIO_METHOD* method_, int (*write_)( BIO*, char const*, int ) ) {
	method_->bwrite = write_;
}
void BIO_meth_set_read( BIO_METHOD* method_, int (*read_)( BIO*, char*, int ) ) {
	method_->bread = read_;
}
void BIO_meth_set_ctrl( BIO_METHOD* method_, int long (*ctrl_)( BIO*, int, int long, void* ) ) {
	method_->ctrl = ctrl_;
}
void BIO_meth_set_create( BIO_METHOD* method_, int (*create_)( BIO* ) ) {
	method_->create = create_;
}
void BIO_meth_set_destroy( BIO_METHOD* method_, int (*destroy_)( BIO* ) ) {
	method_->destroy = destroy_;
}
void BIO_set_init( BIO* bio_, int init_ ) {
	bio_->init = init_;
}
void BIO_set_data( BIO* bio_, void* data_ ) {
	bio_->ptr = data_;
}
void* BIO_get_data( BIO* bio_ ) {
	return ( bio_->ptr );
}
}
#endif

namespace yaal {

namespace hcore {

/*! \brief OpenSSL library wrapper helpers.
 */
namespace openssl_helper {

/*! \brief Format OpenSSL related error.
 *
 * \param buffer_ - buffer for error message.
 * \param err - error code to translate to message.
 * \return Error message.
 */
inline HString& format_error_message( HString& buffer_, int err = 0 ) {
	int long unsigned code( 0 );
	buffer_ = err ? ERR_error_string( static_cast<unsigned int>( err ), nullptr ) : "";
	while ( ( code = ERR_get_error() ) ) {
		buffer_.append( ( buffer_.is_empty() ? "" : "\n" ) ).append( ERR_error_string( code, nullptr ) );
	}
	return ( buffer_ );
}

}

int HOpenSSL::OSSLContext::_instances = 0;
HMutex HOpenSSL::OSSLContext::_mutex( HMutex::TYPE::RECURSIVE );
HOpenSSL::OSSLContext::mutexes_t HOpenSSL::OSSLContext::_sslLibMutexes;

HOpenSSL::OSSLContext::OSSLContext( void )
	: _context( nullptr )
	, _users( 0 ) {
}


inline int long unsigned get_thread_id( void ) {
	return ( static_cast<int long unsigned>( HThread::get_current_thread_id() ) );
}

namespace {

int bio_read( BIO* bio_, char* buf_, int size_ ) {
	M_PROLOG
	int nRead( static_cast<int>( ::read( static_cast<int>( reinterpret_cast<int long>( BIO_get_data( bio_ ) ) ), buf_, static_cast<size_t>( size_ ) ) ) );
	if ( ( nRead < 0 ) && ( errno == EAGAIN ) ) {
		BIO_set_retry_read( bio_ );
	} else {
		BIO_clear_retry_flags( bio_ );
	}
	return ( nRead );
	M_EPILOG
}

int bio_write( BIO* bio_, char const* buf_, int size_ ) {
	M_PROLOG
#ifdef HAVE_VALGRIND_MEMCHECK_H
#pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wold-style-cast"
	VALGRIND_MAKE_MEM_DEFINED( buf_, size_ );
#pragma GCC diagnostic pop
#endif /* #ifdef HAVE_VALGRIND_MEMCHECK_H */
	int nWritten( static_cast<int>( ::write( static_cast<int>( reinterpret_cast<int long>( BIO_get_data( bio_ ) ) ), buf_, static_cast<size_t>( size_ ) ) ) );
	if ( ( nWritten < 0 ) && ( errno == EAGAIN ) ) {
		BIO_set_retry_write( bio_ );
	} else {
		BIO_clear_retry_flags( bio_ );
	}
	return ( nWritten );
	M_EPILOG
}

int long bio_ctrl( BIO*, int cmd_, int long, void* ) {
	M_PROLOG
	int code( 0 );
	switch ( cmd_ ) {
		case ( BIO_CTRL_EOF ): break;
		case ( BIO_CTRL_RESET ): break;
		case ( BIO_C_FILE_SEEK ): break;
		case ( BIO_C_FILE_TELL ): break;
		case ( BIO_CTRL_INFO ): break;
		case ( BIO_CTRL_PENDING ): break;
		case ( BIO_CTRL_WPENDING ): break;
		case ( BIO_CTRL_DUP ): break;
		case ( BIO_CTRL_FLUSH ): code = 1; break;
		default: break;
	}
	return ( code );
	M_EPILOG
}

int bio_create( BIO* bio_ ) {
	M_PROLOG
	BIO_set_init( bio_, 1 );
	return ( 1 );
	M_EPILOG
}

int bio_destroy( BIO* ) {
	M_PROLOG
	return ( 1 );
	M_EPILOG
}

}

typedef HResource<BIO_METHOD, void (*) ( BIO_METHOD* )> bio_method_t;
bio_method_t _fdMethod_;

void HOpenSSL::OSSLContext::init( void ) {
	M_PROLOG
	HLock lock( _mutex );
	if ( _instances == 0 ) {
		SSL_load_error_strings();
		int numLocks( CRYPTO_num_locks() );
		M_ENSURE( numLocks > 0 );
		_sslLibMutexes.resize( numLocks );
		for ( mutexes_t::iterator it( _sslLibMutexes.begin() ), endIt( _sslLibMutexes.end() ); it != endIt; ++ it ) {
			it->first = mutex_ptr_t( make_pointer<HMutex>( HMutex::TYPE::RECURSIVE ) );
			it->second = 0;
		}
		CRYPTO_set_locking_callback( &HOpenSSL::OSSLContext::libssl_rule_mutex );
		CRYPTO_set_id_callback( &get_thread_id );
		SSL_library_init();
		_fdMethod_ = bio_method_t( BIO_meth_new( BIO_TYPE_SOURCE_SINK, "yaal-stream" ), &BIO_meth_free );
		BIO_meth_set_write( _fdMethod_.get(), &bio_write );
		BIO_meth_set_read( _fdMethod_.get(), &bio_read );
		BIO_meth_set_ctrl( _fdMethod_.get(), &bio_ctrl );
		BIO_meth_set_create( _fdMethod_.get(), &bio_create );
		BIO_meth_set_destroy( _fdMethod_.get(), &bio_destroy );
	}
	SSL_METHOD const* method( static_cast<SSL_METHOD const*>( select_method() ) );
	SSL_CTX* ctx( nullptr );
	ERR_clear_error();
	_context = ctx = SSL_CTX_new( const_cast<SSL_METHOD*>( method ) );
	if ( ! _context ) {
		HString buffer;
		throw HOpenSSLFatalException( openssl_helper::format_error_message( buffer ) );
	}
	++ _instances;
	do_init();
	return;
	M_EPILOG
}

void const* HOpenSSL::OSSLContext::select_method( void ) const {
	return ( do_method() );
}

HOpenSSL::OSSLContext::~OSSLContext( void ) {
	M_PROLOG
#ifndef __HOST_OS_TYPE_SOLARIS__
	HLock lock( _mutex );
#endif /* #ifndef __HOST_OS_TYPE_SOLARIS__ */
	M_ENSURE( ! _users );
	if ( _context ) {
		SSL_CTX_free( static_cast<SSL_CTX*>( _context ) );
	}
	_context = nullptr;
	-- _instances;
	if ( _instances == 0 ) {
		_fdMethod_.reset();
#if OPENSSL_VERSION_NUMBER < 0x10100000L
		ERR_remove_state( static_cast<int unsigned>( system::getpid() ) );
		ERR_remove_state( 0 );
#endif
		ENGINE_cleanup();
		CONF_modules_finish();
		CONF_modules_free();
		CONF_modules_unload( 1 );
		ERR_free_strings();
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		CRYPTO_set_locking_callback( nullptr );
#ifndef __HOST_OS_TYPE_SOLARIS__
		for ( int i( 0 ), SIZE( static_cast<int>( _sslLibMutexes.size() ) ); i < SIZE; ++ i ) {
			mutex_info_t& m( _sslLibMutexes[ i ] );
			if ( m.second > 0 ) {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
				log_trace << "A ssl lock " << CRYPTO_get_lock_name( i ) << " still holds " << m.second << "locks." <<endl;
#endif
				while ( m.second -- ) {
					m.first->unlock();
				}
			}
		}
		_sslLibMutexes.clear();
#endif /* #ifndef __HOST_OS_TYPE_SOLARIS__ */
	}
	return;
	M_DESTRUCTOR_EPILOG
}

void* HOpenSSL::OSSLContext::create_ssl( void ) {
	M_PROLOG
	HLock lock( _mutex );
	M_ASSERT( _context );
	SSL* ssl( SSL_new( static_cast<SSL_CTX*>( _context ) ) );
	HString buffer;
	if ( ! ssl ) {
		throw HOpenSSLException( openssl_helper::format_error_message( buffer ) );
	}
	++ _users;
	return ( ssl );
	M_EPILOG
}

void HOpenSSL::OSSLContext::consume_ssl( void* ssl_ ) {
	M_PROLOG
	HLock lock( _mutex );
	M_ASSERT( ssl_ );
	SSL_free( static_cast<SSL*>( ssl_ ) );
	-- _users;
	M_EPILOG
}

void HOpenSSL::OSSLContext::libssl_rule_mutex( int mode, int nth, char const* /* file_ */, int /* line_ */ ) {
	M_PROLOG
	mutex_info_t& m( _sslLibMutexes[ nth ] );
	if ( mode & CRYPTO_LOCK ) {
		m.first->lock();
		++ m.second;
	} else {
		M_ASSERT( m.second > 0 );
		-- m.second;
		m.first->unlock();
	}
	return;
	M_EPILOG
}

HOpenSSL::OSSLContextServer::OSSLContextServer( void ) {
	M_PROLOG
	init();
	return;
	M_EPILOG
}

void HOpenSSL::OSSLContextServer::do_init( void ) {
	M_PROLOG
	SSL_CTX* ctx( static_cast<SSL_CTX*>( data() ) );
	HString buffer;
	HUTF8String utf8( _sslKey );
	if ( SSL_CTX_use_PrivateKey_file( ctx, utf8.c_str(), SSL_FILETYPE_PEM ) <= 0 ) {
		throw HOpenSSLFatalException( openssl_helper::format_error_message( buffer ) + ", key: `" + _sslKey + "'" );
	}
	utf8 = _sslCert;
	if ( SSL_CTX_use_certificate_file( ctx, utf8.c_str(), SSL_FILETYPE_PEM ) <= 0 ) {
		throw HOpenSSLFatalException( openssl_helper::format_error_message( buffer ) + ": cert: `" + _sslCert + "'" );
	}
	if ( ! SSL_CTX_check_private_key( ctx ) ) {
		throw HOpenSSLFatalException( openssl_helper::format_error_message( buffer ) );
	}
	return;
	M_EPILOG
}

void const* HOpenSSL::OSSLContextServer::do_method( void ) const {
	M_PROLOG
	SSL_METHOD const* m( SSLv23_server_method() );
	M_ENSURE( m );
	return ( m );
	M_EPILOG
}

HOpenSSL::OSSLContextClient::OSSLContextClient( void ) {
	M_PROLOG
	init();
	M_EPILOG
}

void HOpenSSL::OSSLContextClient::do_init( void ) {
	return;
}

void const* HOpenSSL::OSSLContextClient::do_method( void ) const {
	M_PROLOG
	SSL_METHOD const* m( SSLv23_client_method() );
	M_ENSURE( m );
	return ( m );
	M_EPILOG
}

HOpenSSL::HOpenSSL( int fileDescriptor_, TYPE type_ )
	: _pendingOperation( false )
	, _ssl( nullptr )
	, _ctx(
		( type_ == TYPE::SERVER )
			? static_cast<OSSLContext*>( &OSSLContextServer::get_instance() )
			: static_cast<OSSLContext*>( &OSSLContextClient::get_instance() )
	)
	, do_accept_or_connect( ( type_ == TYPE::SERVER ) ? &HOpenSSL::accept : &HOpenSSL::connect ) {
	M_PROLOG
	try {
		SSL* ssl( static_cast<SSL*>( _ctx->create_ssl() ) );
		_ssl = ssl;
		BIO* bio( BIO_new( _fdMethod_.get() ) );
		M_ENSURE( bio );
		BIO_set_data( bio, reinterpret_cast<void*>( fileDescriptor_ ) );
		SSL_set_bio( ssl, bio, bio );
		accept_or_connect();
	} catch ( HOpenSSLException const& ) {
		if ( _ssl ) {
			_ctx->consume_ssl( _ssl );
		}
		_ssl = nullptr;
		throw;
	}
	return;
	M_EPILOG
}

HOpenSSL::~HOpenSSL( void ) {
	M_PROLOG
	_ctx->consume_ssl( _ssl );
	return;
	M_DESTRUCTOR_EPILOG
}

void HOpenSSL::accept_or_connect( void ) {
	M_PROLOG
	int ret( (this->*do_accept_or_connect)() );
	if ( ret == -1 ) {
		check_err( ret );
		_pendingOperation = true;
	} else {
		_pendingOperation = false;
	}
	return;
	M_EPILOG
}

int HOpenSSL::accept( void ) {
	M_PROLOG
	ERR_clear_error();
	return ( SSL_accept( static_cast<SSL*>( _ssl ) ) );
	M_EPILOG
}

int HOpenSSL::connect( void ) {
	M_PROLOG
	ERR_clear_error();
	return ( SSL_connect( static_cast<SSL*>( _ssl ) ) );
	M_EPILOG
}

int HOpenSSL::check_err( int code ) const {
	M_PROLOG
	int err( SSL_get_error( static_cast<SSL const*>( _ssl ), code ) );
	if ( ( err != SSL_ERROR_ZERO_RETURN ) && ( err != SSL_ERROR_WANT_READ ) && ( err != SSL_ERROR_WANT_WRITE ) ) {
		int errTop( static_cast<int>( ERR_get_error() ) );
		HString buffer;
		if ( ! errTop && ( err == SSL_ERROR_SYSCALL ) ) {
			if ( code == 0 ) {
				buffer = "EOF that violates protocol was observed.";
			} else if ( code == -1 ) {
				buffer = strerror( errno );
			} else {
				buffer = "unknown problem occurred!";
			}
		} else {
			openssl_helper::format_error_message( buffer, errTop );
		}
		throw HOpenSSLException( buffer );
	}
	return ( ( err == SSL_ERROR_WANT_READ ) || ( err == SSL_ERROR_WANT_WRITE ) ? -1 : 0 );
	M_EPILOG
}

int long HOpenSSL::read( void* buffer_, int long size_ ) {
	M_PROLOG
	M_ASSERT( _ssl );
	if ( _pendingOperation ) {
		accept_or_connect();
	}
	int nRead( -1 );
	if ( ! _pendingOperation ) {
		nRead = SSL_read( static_cast<SSL*>( _ssl ), buffer_, static_cast<int>( size_ ) );
#ifdef HAVE_VALGRIND_MEMCHECK_H
		if ( nRead > 0 ) {
#pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wold-style-cast"
			VALGRIND_MAKE_MEM_DEFINED( buffer_, nRead );
#pragma GCC diagnostic pop
		}
#endif /* #ifdef HAVE_VALGRIND_MEMCHECK_H */
		if ( nRead <= 0 ) {
			nRead = check_err( nRead );
		}
	}
	return ( nRead );
	M_EPILOG
}

int long HOpenSSL::write( void const* buffer_, int long size_ ) {
	M_PROLOG
	M_ASSERT( _ssl );
	if ( _pendingOperation ) {
		accept_or_connect();
	}
	int nWritten( -1 );
	if ( ! _pendingOperation ) {
		if ( size_ > 0 ) {
			nWritten = SSL_write( static_cast<SSL*>( _ssl ), buffer_, static_cast<int>( size_ ) );
			if ( nWritten <= 0 ) {
				check_err( nWritten );
			}
		} else {
			nWritten = 0;
		}
	}
	return ( nWritten );
	M_EPILOG
}

void HOpenSSL::shutdown( void ) {
	M_PROLOG
	M_ASSERT( _ssl );
	SSL_shutdown( static_cast<SSL*>( _ssl ) );
	return;
	M_EPILOG
}

void HOpenSSL::set_nonblocking( bool nonBlocking_ ) {
	M_PROLOG
	M_ASSERT( _ssl );
	BIO* bio( SSL_get_rbio( static_cast<SSL*>( _ssl ) ) );
	BIO_set_nbio( bio, nonBlocking_ ? 1 : 0 );
	return;
	M_EPILOG
}

}

}

