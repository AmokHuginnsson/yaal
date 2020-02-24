/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hopenssl.hxx
 * \brief Declaration of HOpenSSL class.
 */

#ifndef YAAL_HCORE_HOPENSSL_HXX_INCLUDED
#define YAAL_HCORE_HOPENSSL_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hsingleton.hxx"
#include "hcore/hexception.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hstring.hxx"
#include "hcore/harray.hxx"

namespace yaal {

namespace hcore {

/*! \brief Secure Socket Layer utility class.
 *
 * One can create both SSL server and SSL client
 * IO wrappers.
 */
class HOpenSSL {
public:
	typedef HOpenSSL this_type;
	static yaal::hcore::HString _sslKey;
	static yaal::hcore::HString _sslCert;
	/*! \brief SSL object types.
	 */
	enum class TYPE {
		SERVER, /*!< Server side of SSL connection. */
		CLIENT  /*!< Client side of SSL connection. */
	};
private:
	class OSSLContext {
		typedef HPointer<HMutex> mutex_ptr_t;
		typedef HPair<mutex_ptr_t, int> mutex_info_t;
		typedef HArray<mutex_info_t> mutexes_t;
		static int _instances;
		static HMutex _mutex;
		static mutexes_t _sslLibMutexes;
		void* _context;
		int _users;
	protected:
		OSSLContext( void );
		virtual ~OSSLContext( void );
		void init( void );
		virtual void const* do_method( void ) const = 0;
		virtual void do_init( void ) = 0;
		static void libssl_rule_mutex( int, int, char const*, int );
		void* data( void ) {
			return ( _context );
		}
	public:
		void* create_ssl( void );
		void consume_ssl( void* );
		void const* select_method( void ) const;
	private:
		OSSLContext( OSSLContext const& );
		OSSLContext& operator=( OSSLContext const& );
	};
	class M_YAAL_HCORE_PUBLIC_API OSSLContextServer : public OSSLContext, public HSingleton<OSSLContextServer> {
		OSSLContextServer( void );
		virtual void const* do_method( void ) const override;
		virtual void do_init( void ) override;
		friend class HSingleton<OSSLContextServer>;
		friend class HDestructor<OSSLContextServer>;
	};
	class M_YAAL_HCORE_PUBLIC_API OSSLContextClient : public OSSLContext, public HSingleton<OSSLContextClient> {
		OSSLContextClient( void );
		virtual void const* do_method( void ) const override;
		virtual void do_init( void ) override;
		friend class HSingleton<OSSLContextClient>;
		friend class HDestructor<OSSLContextClient>;
	};
	bool _pendingOperation;
	void* _ssl;
	OSSLContext* _ctx;
public:
	typedef yaal::hcore::HPointer<HOpenSSL> ptr_t;
	HOpenSSL( int, TYPE );
	~HOpenSSL( void );
	int long read( void*, int long );
	int long write( void const*, int long );
	void shutdown( void );
private:
	void accept_or_connect( void );
	int accept( void );
	int connect( void );
	int check_err( int ) const;
	typedef int ( HOpenSSL::* operation_t )( void );
	operation_t do_accept_or_connect;
	HOpenSSL( HOpenSSL const& );
	HOpenSSL& operator=( HOpenSSL const& );
};

typedef HExceptionT<HOpenSSL> HOpenSSLException;
typedef HExceptionT<HOpenSSL, HOpenSSLException> HOpenSSLFatalException;

}

}

#endif /* #ifndef YAAL_HCORE_HOPENSSL_HXX_INCLUDED */

