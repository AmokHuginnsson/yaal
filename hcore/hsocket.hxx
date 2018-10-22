/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
 * network connectivity and data transfer operations.
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
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const SSL;
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const SERVER;
		static M_YAAL_HCORE_PUBLIC_API socket_type_t const CLIENT;
	};
protected:
	bool _needShutdown;
	socket_type_t _type;
	int _maximumNumberOfClients;
	int _addressSize;
	void* _address;
	HString _hostName;
public:
	HSocket( socket_type_t const& = TYPE::DEFAULT, int = 0 );
	virtual ~HSocket( void );
	void listen( yaal::hcore::HString const&, int = 0 );
	ptr_t accept( void );
	void connect( yaal::hcore::HString const&, int = 0 );
	int get_port( void ) const;
	void shutdown( void );
	HString const& get_host_name( void );
protected:
	virtual int do_close( void ) override;
	void make_address( yaal::hcore::HString const&, int );
	void cleanup( void );
private:
	HSocket( HSocket const& );
	HSocket& operator = ( HSocket const& );
};

typedef HExceptionT<HSocket, HRawFileException> HSocketException;

}

}

#endif /* #ifndef YAAL_HCORE_HSOCKET_HXX_INCLUDED */

