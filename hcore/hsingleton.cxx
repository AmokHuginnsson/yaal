/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hsingleton.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib>

#include "base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hsingleton.h"

namespace yaal
{

namespace hcore
{

HLifeTimeTracker::HLifeTimeTracker( void )
	: f_oMutex(), f_oDestructors()
	{
	}

HLifeTimeTracker::~HLifeTimeTracker( void )
	{
	M_ASSERT( f_oDestructors.size() == 0 );
	}

HLifeTimeTracker& HLifeTimeTracker::get_instance( void )
	{
	static HLifeTimeTracker lifeTimeTracker;
	return ( lifeTimeTracker );
	}

int HSingletonInterface::life_time( int a_iLifeTime )
	{
	return ( a_iLifeTime );
	}

void HLifeTimeTracker::do_destruct( void )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	map_stack_t::iterator it = f_oDestructors.begin();
	M_ASSERT( it != f_oDestructors.end() );
	destructor_ptr_t destructor = (*it).second;
	M_ASSERT( !! destructor );
	f_oDestructors.erase( it );
	destructor->destruct();
	return;
	M_EPILOG
	}

#if ! defined( __HOST_OS_TYPE_FREEBSD__ )
void HLifeTimeTracker::destruct( void )
	{
	M_PROLOG
	HLifeTimeTracker& lt = get_instance();
	lt.do_destruct();
	M_EPILOG
	}
#else /* not __HOST_OS_TYPE_FREEBSD__ */
void HLifeTimeTracker::destruct( void* )
	{
	M_PROLOG
	HLifeTimeTracker& lt = get_instance();
	lt.do_destruct();
	M_EPILOG
	}

typedef void ( *cxa_handle_t )( void* );
extern "C" int __cxa_atexit( cxa_handle_t, void*, void* );
extern "C" void* __dso_handle;		

inline int safe_atexit( cxa_handle_t cxa_handle )
	{
	return ( __cxa_atexit( cxa_handle, 0, __dso_handle ) );
	}
#endif /* __HOST_OS_TYPE_FREEBSD__ */

void HLifeTimeTracker::register_destructor( destructor_ptr_t a_oDestructor, int const& a_iLifeTime )
	{
	M_PROLOG
	HLock l_oLock( f_oMutex );
	f_oDestructors.push_back( a_iLifeTime, a_oDestructor );
#if ! defined( __HOST_OS_TYPE_FREEBSD__ )
	M_ENSURE( atexit( HLifeTimeTracker::destruct ) == 0 );
#else /* not __HOST_OS_TYPE_FREEBSD__ */
	M_ENSURE( safe_atexit( HLifeTimeTracker::destruct ) == 0 );
#endif /* __HOST_OS_TYPE_FREEBSD__ */
	M_EPILOG
	}

}

}

