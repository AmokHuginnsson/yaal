/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <pthread.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "htls.hxx"

namespace yaal {

namespace hcore {

namespace tls {

static_assert( sizeof ( key_t ) >= sizeof ( pthread_key_t ), "native key size exceeds yaal's implementation key size" );

namespace {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename to_t, typename from_t>
inline to_t old_style_cast( from_t val_ ) {
	return ( (to_t)val_ );
}
#pragma GCC diagnostic pop
}

key_t create( destruct_t destruct_ ) {
	M_PROLOG
#ifdef __HOST_OS_TYPE_CYGWIN__
	pthread_key_t key( nullptr );
#else
	pthread_key_t key( 0 );
#endif
	M_ENSURE( pthread_key_create( &key, destruct_ ) == 0 );
	return ( old_style_cast<key_t>( key ) );
	M_EPILOG
}

void set( key_t key_, void const* value_ ) {
	M_PROLOG
	M_ENSURE( ::pthread_setspecific( old_style_cast<pthread_key_t>( key_ ), value_ ) == 0 );
	return;
	M_EPILOG
}

void* get( key_t key_ ) {
	M_PROLOG
	return ( ::pthread_getspecific( old_style_cast<pthread_key_t>( key_ ) ) );
	M_EPILOG
}

void free( key_t key_ ) {
	M_PROLOG
	M_ENSURE( ::pthread_key_delete( old_style_cast<pthread_key_t>( key_ ) ) == 0 );
	return;
	M_EPILOG
}

}

}

}

