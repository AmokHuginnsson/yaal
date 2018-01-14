/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/resolver.hxx
 * \brief All name resolution related classes and free function reside here.
 *
 * HIP and resolver belongs here.
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

/*! \brief IP address representation.
 */
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

