/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hsingleton.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hsingleton.hxx"

namespace yaal {

namespace hcore {

HLifeTimeTracker::HLifeTimeTracker( void )
	: _mutex(), _destructors() {
}

HLifeTimeTracker::~HLifeTimeTracker( void ) {
	M_PROLOG
	M_ASSERT( _destructors.size() == 0 );
	M_DESTRUCTOR_EPILOG
}

HLifeTimeTracker& HLifeTimeTracker::get_instance( void ) {
	static HLifeTimeTracker lifeTimeTracker;
	return ( lifeTimeTracker );
}

int HSingletonInterface::life_time( int lifeTime_ ) {
	return ( lifeTime_ );
}

void HLifeTimeTracker::do_destruct( void ) {
	M_PROLOG
	HLock lock( _mutex );
	map_stack_t::iterator it = _destructors.begin();
	M_ASSERT( it != _destructors.end() );
	destructor_ptr_t destructor = (*it).second;
	M_ASSERT( !! destructor );
	_destructors.erase( it );
	destructor->destruct();
	return;
	M_EPILOG
}

#if ! defined( __HOST_OS_TYPE_FREEBSD__ )
void HLifeTimeTracker::destruct( void ) {
	M_PROLOG
	HLifeTimeTracker& lt = get_instance();
	lt.do_destruct();
	M_EPILOG
}
#else /* not __HOST_OS_TYPE_FREEBSD__ */
void HLifeTimeTracker::destruct( void* ) {
	M_PROLOG
	HLifeTimeTracker& lt = get_instance();
	lt.do_destruct();
	M_EPILOG
}

typedef void ( *cxa_handle_t )( void* );
extern "C" int __cxa_atexit( cxa_handle_t, void*, void* );
extern "C" void* __dso_handle;

inline int safe_atexit( cxa_handle_t cxa_handle ) {
	return ( __cxa_atexit( cxa_handle, 0, __dso_handle ) );
}
#endif /* __HOST_OS_TYPE_FREEBSD__ */

void HLifeTimeTracker::register_destructor( destructor_ptr_t destructor_, int lifeTime_ ) {
	M_PROLOG
	HLock lock( _mutex );
	_destructors.push_back( make_pair( lifeTime_, destructor_ ) );
#if ! defined( __HOST_OS_TYPE_FREEBSD__ )
	M_ENSURE( atexit( HLifeTimeTracker::destruct ) == 0 );
#else /* not __HOST_OS_TYPE_FREEBSD__ */
	M_ENSURE( safe_atexit( HLifeTimeTracker::destruct ) == 0 );
#endif /* __HOST_OS_TYPE_FREEBSD__ */
	M_EPILOG
}

}

}

