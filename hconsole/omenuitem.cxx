/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	omenuitem.cxx - this file is integral part of `yaal' project.

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
#include "omenuitem.h"
M_VCSTID ( D_VCSID_OMENUITEM_H )

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

OMenuItem::OMenuItem( void )
	: f_psSubMenu( NULL ), HANDLER( NULL ), f_pvParam( NULL ), f_oLabel()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

OMenuItem::OMenuItem( OMenuItem* const a_psMenuItem, HANDLER_t const handler,
		void* a_pvParam, HString const& a_roLabel )
	: f_psSubMenu( a_psMenuItem ), HANDLER( handler ),
	f_pvParam( a_pvParam ), f_oLabel ( a_roLabel )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

OMenuItem::OMenuItem( OMenuItem const& a_roMenuItem )
	: f_psSubMenu( a_roMenuItem.f_psSubMenu ), HANDLER( a_roMenuItem.HANDLER ),
	f_pvParam( a_roMenuItem.f_pvParam ), f_oLabel( a_roMenuItem.f_oLabel )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

OMenuItem& OMenuItem::operator = ( OMenuItem const& a_roMenuItem )
	{
	M_PROLOG
	if ( &a_roMenuItem != this )
		{
		OMenuItem item( a_roMenuItem );
		swap( item );
		}
	return ( *this );
	M_EPILOG
	}

void OMenuItem::reset( void )
	{
	M_PROLOG
	f_psSubMenu = NULL;
	HANDLER = NULL;
	f_pvParam = NULL;
	f_oLabel = "";
	return;
	M_EPILOG
	}

void OMenuItem::call( HTUIProcess* proc )
	{
	M_PROLOG
	static_cast<void>( ( proc->*( HANDLER ) )( f_pvParam ) );
	return;
	M_EPILOG
	}

void OMenuItem::swap( OMenuItem& other )
	{
	M_PROLOG
	if ( &other != this )
		{
		using yaal::swap;
		swap( f_psSubMenu, other.f_psSubMenu );
		swap( HANDLER, other.HANDLER );
		swap( f_pvParam, other.f_pvParam );
		swap( f_oLabel, other.f_oLabel );
		}
	return;
	M_EPILOG
	}

}

}

