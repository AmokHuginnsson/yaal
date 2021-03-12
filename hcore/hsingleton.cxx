/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	return lifeTimeTracker;
}

int HSingletonInterface::life_time( int lifeTime_ ) {
	return lifeTime_;
}

void HLifeTimeTracker::do_destruct( void ) {
	M_PROLOG
	HLock lock( _mutex );
	_isKilled_ = true;
	map_stack_t::iterator it( _destructors.begin() );
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
	return ( __cxa_atexit( cxa_handle, nullptr, __dso_handle ) );
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

