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

HLifeTimeTracker::map_stack_t HLifeTimeTracker::f_oDestructors;

void HLifeTimeTracker::register_destructor( destructor_ptr_t a_oDestructor, int a_iLifeTime )
	{
	if ( f_oDestructors.find( a_iLifeTime ) == f_oDestructors.end() )
		f_oDestructors[ a_iLifeTime ] = destructor_list_ptr_t( new destructor_list_t() );
	f_oDestructors[ a_iLifeTime ]->push_back( a_oDestructor );
	M_ENSURE( atexit( HLifeTimeTracker::destruct ) == 0 );
	}

void HLifeTimeTracker::destruct( void )
	{
	map_stack_t::HIterator it = f_oDestructors.begin();
	M_ASSERT( it != f_oDestructors.end() );
	destructor_list_ptr_t i = it->second;
	destructor_list_t::iterator top = i->rbegin();
	M_ASSERT( top != i->rend() );
	destructor_ptr_t destructor = *top;
	M_ASSERT( !! destructor );
	i->remove_tail();
	destructor->destruct();
	if ( i->empty() )
		f_oDestructors.remove( it->first );
	return;
	}

}

}

