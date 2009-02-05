/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawdestructor.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hrawdestructor.hxx"

namespace yaal
{

namespace hcore
{

HRawDestructor::HRawDestructor( void )
	: f_pvResourcePointer( NULL ), DESTRUCTOR()
	{
	return;
	}

HRawDestructor::HRawDestructor( void* a_pvResourcePointer, destructor_t A_DESTRUCTOR )
	: f_pvResourcePointer( a_pvResourcePointer ), DESTRUCTOR( A_DESTRUCTOR )
	{
	return;
	}

HRawDestructor::~HRawDestructor( void )
	{
	if ( f_pvResourcePointer )
		DESTRUCTOR( f_pvResourcePointer );
	return;
	}

void HRawDestructor::set_destruction_scheme( void* a_pvResourcePointer, destructor_t A_DESTRUCTOR )
	{
	f_pvResourcePointer = a_pvResourcePointer;
	DESTRUCTOR = A_DESTRUCTOR;
	return;
	}

}

}

