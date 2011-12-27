/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	htls.cxx - this file is integral part of `yaal' project.

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

#include <pthread.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "htls.hxx"

namespace yaal {

namespace hcore {

namespace tls {

STATIC_ASSERT( sizeof ( pthread_key_t ) == sizeof ( key_t ) );

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename to_t, typename from_t>
inline to_t old_style_cast( from_t val_ ) {
	return ( (to_t)val_ );
}
#pragma GCC diagnostic error "-Wold-style-cast"
}

key_t create( destruct_t destruct_ ) {
	M_PROLOG
	pthread_key_t key( 0 );
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

