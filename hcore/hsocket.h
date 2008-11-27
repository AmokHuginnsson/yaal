/*
---             `yaal' (c) 1978 by Marcin 'Amok' Konarski              ---

	hsocket.h - this file is integral part of `yaal' project.

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
/*! \file hcore/hsocket.h
 * \brief Declaration of HSocket class.
 */

#ifndef __YAAL_HCORE_HSOCKET_H
#define __YAAL_HCORE_HSOCKET_H

#include "hcore/hpointer.h"
#include "hcore/hrawfile.h"
#include "hcore/hhashmap.h"
#include "hcore/hexception.h"

namespace yaal
{

namespace hcore
{

/*! \brief Network socket primitive.
 *
 * HSocket implements both server (listening) and client (connecting)
 * network connectivity and data tranfer operations.
 */
class HSocket : public HRawFile
	{
public:
	typedef HSocket self_t;
	typedef HRawFile hier_t;
	typedef HPointer<HSocket> ptr_t;
	static bool f_bResolveHostnames;
	/*! \brief Socket types.
	 */
	struct TYPE
		{
		/*! \brief Socket types.
		 */
		typedef enum
			{
			D_DEFAULT			= 0x00,
			D_FILE				= 0x01,
			D_NETWORK			= 0x02,
			D_BLOCKING		= 0x04,
			D_NONBLOCKING	= 0x08,
			D_SSL_SERVER	= 0x10,
			D_SSL_CLIENT	= 0x20
			} socket_type_t;
		};
protected:
	typedef HHashMap<int, ptr_t> clients_t;
	bool f_bNeedShutdown;
	TYPE::socket_type_t f_eType;
	int f_iMaximumNumberOfClients;
	int f_iAddressSize;
	void* f_pvAddress;
	clients_t* f_poClients;
	HString f_oHostName;
public:
	HSocket( TYPE::socket_type_t const = TYPE::D_DEFAULT, int const = 0 );
	virtual ~HSocket( void );
	void listen( yaal::hcore::HString const&, int const = 0 );
	ptr_t accept( void );
	void connect( yaal::hcore::HString const&, int const = 0 );
	int get_port( void ) const;
	void shutdown( void );
	void shutdown_client( int );
	ptr_t get_client( int ) const;
	clients_t::const_iterator begin( void ) const;
	clients_t::const_iterator end( void ) const;
	int long write_until_eos( HString const& );
	int get_client_count( void ) const;
	HString const& get_host_name( void );
protected:
	virtual int do_close( void );
	void make_address( yaal::hcore::HString const&, int const );
private:
	HSocket( HSocket const& );
	HSocket& operator = ( HSocket const& );
	};

typedef HExceptionT<HSocket, HRawFileException> HSocketException;

}

}

#endif /* not __YAAL_HCORE_HSOCKET_H */

