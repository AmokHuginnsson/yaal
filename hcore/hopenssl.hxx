/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hopenssl.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HOPENSSL_HXX_INCLUDED
#define YAAL_HCORE_HOPENSSL_HXX_INCLUDED

#include "hcore/hpointer.hxx"
#include "hcore/hsingleton.hxx"
#include "hcore/hexception.hxx"
#include "hcore/hthread.hxx"
#include "hcore/hstring.hxx"
#include "hcore/harray.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Secure Socket Layer utility class.
 *
 * One can create both SSL server and SSL client
 * IO wrappers.
 */
class HOpenSSL
	{
public:
	typedef HOpenSSL this_type;
	static yaal::hcore::HString _sSLKey;
	static yaal::hcore::HString _sSLCert;
	/*! \brief SSL object types.
	 */
	struct TYPE
		{
		/*! \brief Types.
		 */
		typedef enum
			{
			SERVER, /*!< Server side of SSL connection. */
			CLIENT  /*!< Client side of SSL connection. */
			} ssl_context_type_t;
		};
private:
	class OSSLContext
		{
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
		virtual void* do_method( void ) const = 0;
		static void libssl_rule_mutex( int, int, char const*, int );
	public:
		void* create_ssl( void );
		void consume_ssl( void* );
		void* select_method( void ) const;
	private:
		OSSLContext( OSSLContext const& );
		OSSLContext& operator=( OSSLContext const& );
		};
	class OSSLContextServer : public OSSLContext, public HSingleton<OSSLContextServer>
		{
		OSSLContextServer( void );
		virtual void* do_method( void ) const;
		friend class HSingleton<OSSLContextServer>;
		friend class HDestructor<OSSLContextServer>;
		};
	class OSSLContextClient : public OSSLContext, public HSingleton<OSSLContextClient>
		{
		OSSLContextClient( void );
		virtual void* do_method( void ) const;
		friend class HSingleton<OSSLContextClient>;
		friend class HDestructor<OSSLContextClient>;
		};
	bool _pendingOperation;
	void* _ssl;
	OSSLContext* _ctx;
public:
	typedef yaal::hcore::HPointer<HOpenSSL> ptr_t;
	HOpenSSL( int, TYPE::ssl_context_type_t );
	~HOpenSSL( void );
	int long read( void* const, int long const& );
	int long write( void const* const, int long const& );
	void shutdown( void );
private:
	void accept_or_connect( void );
	int accept( void );
	int connect( void );
	void check_err( int ) const;
	typedef int ( HOpenSSL::* operation_t )( void );
	operation_t do_accept_or_connect;
	HOpenSSL( HOpenSSL const& );
	HOpenSSL& operator=( HOpenSSL const& );
	};

typedef HExceptionT<HOpenSSL> HOpenSSLException;
typedef HExceptionT<HOpenSSL, HOpenSSLException> HOpenSSLFatalException;

}

}

#endif /* not YAAL_HCORE_HOPENSSL_HXX_INCLUDED */

