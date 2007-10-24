/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hopenssl.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HOPENSSL_H
#define __YAAL_HCORE_HOPENSSL_H

#include "hcore/hpointer.h"
#include "hcore/hsingleton.h"
#include "hcore/hexception.h"
#include "hcore/hthread.h"

namespace yaal
{

namespace hcore
{

class HOpenSSL
	{
public:
	struct TYPE
		{
		typedef enum
			{
			D_SERVER,
			D_CLIENT
			} ssl_context_type_t;
		};
private:
	struct OSSLContext
		{
		static HMutex f_oMutex;
		void* f_pvMethod;
		void* f_pvContext;
		OSSLContext( void );
		virtual ~OSSLContext( void );
		void init( void );
		static int life_time( int );
	private:
		OSSLContext( OSSLContext const& );
		OSSLContext& operator=( OSSLContext const& );
		};
	struct OSSLContextServer : public OSSLContext
		{
		OSSLContextServer( void );
		};
	struct OSSLContextClient : public OSSLContext
		{
		OSSLContextClient( void );
		};
	typedef yaal::hcore::HSingleton<OSSLContextServer> OSSLContextServerInstance;
	typedef yaal::hcore::HSingleton<OSSLContextClient> OSSLContextClientInstance;
	void* f_pvSSL;
public:
	typedef yaal::hcore::HPointer<HOpenSSL, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> ptr_t;
	HOpenSSL( int, TYPE::ssl_context_type_t );
	~HOpenSSL( void );
	int read( void* const, int const );
	int write( void const* const, int const );
	void shutdown( void );
private:
	HOpenSSL( HOpenSSL const& );
	HOpenSSL& operator=( HOpenSSL const& );
	};

typedef HExceptionT<HOpenSSL> HOpenSSLException;

}

}

#endif /* not __YAAL_HCORE_HOPENSSL_H */

