/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatacontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.h"
M_VCSID( "$Id: "__ID__" $" )
#include "hdatacontrol.h"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal
{

namespace hdata
{

HDataControl::HDataControl( void )
						: HControl( NULL, 0, 0, 0, 0, NULL ), f_psResource( NULL ), f_oSQL()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataControl::~HDataControl( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HDataControl::set_resource( OResource const* a_psResource )
	{
	M_PROLOG
	f_psResource = a_psResource;
	M_EPILOG
	}

void HDataControl::set_dbd( HSQLDescriptor::ptr_t dbd )
	{
	M_PROLOG
	f_oSQL = dbd;
	M_EPILOG
	}

void HDataControl::load( int long )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int long HDataControl::get_current_id( void )
	{
	M_PROLOG
	return ( -1 );
	M_EPILOG
	}

void HDataControl::add_new( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HDataControl::cancel_new( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

}

}

