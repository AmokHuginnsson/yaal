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

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hsingleton.h"

namespace yaal
{

namespace hcore
{

HMutex HLifeTimeTracker::f_oMutex;
HLifeTimeTracker::map_stack_t HLifeTimeTracker::f_oDestructors;

int HSingletonInterface::life_time( int a_iLifeTime )
	{
	return ( a_iLifeTime );
	}

void HLifeTimeTracker::register_destructor( destructor_ptr_t a_oDestructor, int const& a_iLifeTime )
	{
	HLock l_oLock( f_oMutex );
	f_oDestructors.push_back( a_iLifeTime, a_oDestructor );
	M_ENSURE( atexit( HLifeTimeTracker::destruct ) == 0 );
	}

void HLifeTimeTracker::destruct( void )
	{
	HLock l_oLock( f_oMutex );
	map_stack_t::HIterator it = f_oDestructors.begin();
	M_ASSERT( it != f_oDestructors.end() );
	destructor_ptr_t destructor = (*it).second;
	M_ASSERT( !! destructor );
	f_oDestructors.erase( it );
	destructor->destruct();
	return;
	}

}

}

