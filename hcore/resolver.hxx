/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	resolver.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_RESOLVER_HXX_INCLUDED
#define YAAL_HCORE_RESOLVER_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/hstring.hxx"
#include "hcore/htls.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

class HIP {
private:
	u32_t _data;
public:
	explicit HIP( u32_t data_ = 0 )
		: _data( data_ )
		{}
	HIP( u8_t, u8_t, u8_t, u8_t );
	u32_t raw( void ) const
		{ return ( _data ); }
	bool operator == ( HIP const& ip_ ) const
		{ return ( _data == ip_._data ); }
	bool operator != ( HIP const& ip_ ) const
		{ return ( _data != ip_._data ); }
};

typedef HIP ip_t;

struct resolver {
	typedef resolver this_type;
	typedef HTLS<HChunk> cache_on_tls_t;
	static cache_on_tls_t _cache;
	static ip_t get_ip( HString const& );
	static HString ip_to_string( ip_t );
	static HString get_name( ip_t );
	static char const* error_message( int );
};

typedef HExceptionT<resolver> HResolverException;

}

}

#endif /* #ifndef YAAL_HCORE_RESOLVER_HXX_INCLUDED */

