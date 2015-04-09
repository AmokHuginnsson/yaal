/*
---             `yaal' (c) 1978 by Marcin 'Amok' Konarski              ---

  hsocket.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/hsocket.hxx
 * \brief Declaration of HSocket class.
 */

#ifndef YAAL_HCORE_HSOCKET_HXX_INCLUDED
#define YAAL_HCORE_HSOCKET_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hrawfile.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hexception.hxx"
#include "hcore/hbitflag.hxx"

namespace yaal {

namespace hcore {

/*! \brief Network socket primitive.
 *
 * HSocket implements both server (listening) and client (connecting)
 * network connectivity and data tranfer operations.
 */
class HSocket : public HRawFile {
public:
	typedef HSocket this_type;
	typedef HRawFile base_type;
	typedef HPointer<HSocket> ptr_t;
	M_YAAL_HCORE_PUBLIC_API static bool _resolveHostnames;
	/*! \brief Socket types.
	 */
	struct TYPE;
	/*! \brief Socket types.
	 */
	typedef HBitFlag<TYPE> socket_type_t;
	struct TYPE {
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const DEFAULT;
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const FILE;
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const NETWORK;
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const BLOCKING;
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const NONBLOCKING;
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const SSL_SERVER;
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const SSL_CLIENT;
	};
protected:
	typedef HHashMap<int, ptr_t> clients_t;
	typedef HResource<clients_t> clients_res_t;
public:
	typedef clients_t::const_iterator iterator;
protected:
	bool _needShutdown;
	socket_type_t _type;
	int _maximumNumberOfClients;
	int _addressSize;
	void* _address;
	clients_res_t _clients;
	HString _hostName;
public:
	HSocket( socket_type_t const& = TYPE::DEFAULT, int = 0 );
	virtual ~HSocket( void );
	void listen( yaal::hcore::HString const&, int = 0 );
	ptr_t accept( void );
	void connect( yaal::hcore::HString const&, int = 0 );
	int get_port( void ) const;
	void shutdown( void );
	void shutdown_client( int );
	ptr_t get_client( int ) const;
	iterator begin( void ) const;
	iterator end( void ) const;
	iterator find( int ) const;
	int long write_until_eos( HString const& );
	int get_client_count( void ) const;
	HString const& get_host_name( void );
protected:
	virtual int do_close( void ) override;
	void make_address( yaal::hcore::HString const&, int );
private:
	HSocket( HSocket const& );
	HSocket& operator = ( HSocket const& );
};

typedef HExceptionT<HSocket, HRawFileException> HSocketException;

}

}

#endif /* #ifndef YAAL_HCORE_HSOCKET_HXX_INCLUDED */

