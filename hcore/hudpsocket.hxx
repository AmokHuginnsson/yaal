/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hudpsocket.hxx - this file is integral part of `yaal' project.

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
		: _ip(), _port( 0 ), _data( size_ ), _size( size_ )
		{}
};

class HUDPSocket : public trait::HNonCopyable {
public:
	typedef HUDPSocket this_type;
	typedef HPointer<HUDPSocket> ptr_t;
	/*! \brief Socket types.
	 */
	struct MODE {
		/*! \brief Socket types.
		 */
		typedef enum {
			DEFAULT,
			BLOCKING,
			NONBLOCKING
		} socket_mode_t;
	};
private:
	int _fileDescriptor;
public:
	HUDPSocket( MODE::socket_mode_t = MODE::DEFAULT );
	HUDPSocket( int, ip_t = ip_t(), MODE::socket_mode_t = MODE::DEFAULT );
	virtual ~HUDPSocket( void );
	int get_file_descriptor( void ) const;
	void bind( int, ip_t = ip_t() );
	void connect( ip_t, int );
	void send_to( ip_t, int, void const*, int long );
	void send( void const*, int long );
	int long receive( ODatagram& );
private:
	void init( MODE::socket_mode_t );
	void do_bind( int, ip_t );
};

typedef HExceptionT<HUDPSocket> HUDPSocketException;

}

}

#endif /* #ifndef YAAL_HCORE_HUDPSOCKET_HXX_INCLUDED */

