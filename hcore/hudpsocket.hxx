/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hudpsocket.hxx
 * \brief Declaration of HUDPSocket class.
 */

#ifndef YAAL_HCORE_HUDPSOCKET_HXX_INCLUDED
#define YAAL_HCORE_HUDPSOCKET_HXX_INCLUDED 1

#include "hcore/trait.hxx"
#include "hcore/hexception.hxx"
#include "hcore/resolver.hxx"
#include "hcore/hpointer.hxx"

namespace yaal {

namespace hcore {

struct ODatagram {
	ip_t _ip;
	int _port;
	HChunk _data;
	int long _size;
	ODatagram( int size_ )
		: _ip()
		, _port( 0 )
		, _data( size_ )
		, _size( size_ ) {
	}
};

class HUDPSocket : public trait::HNonCopyable {
public:
	typedef HUDPSocket this_type;
	typedef HPointer<HUDPSocket> ptr_t;
	/*! \brief Socket types.
	 */
	enum class MODE {
		DEFAULT,
		BLOCKING,
		NONBLOCKING
	};
private:
	int _fileDescriptor;
public:
	HUDPSocket( MODE = MODE::DEFAULT );
	HUDPSocket( int, ip_t = ip_t(), MODE = MODE::DEFAULT );
	virtual ~HUDPSocket( void );
	int get_file_descriptor( void ) const;
	void bind( int, ip_t = ip_t() );
	void connect( ip_t, int );
	void send_to( ip_t, int, void const*, int long );
	void send( void const*, int long );
	int long receive( ODatagram& );
private:
	void init( MODE );
	void do_bind( int, ip_t );
};

typedef HExceptionT<HUDPSocket> HUDPSocketException;

}

}

#endif /* #ifndef YAAL_HCORE_HUDPSOCKET_HXX_INCLUDED */

