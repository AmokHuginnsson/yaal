/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	omenuitem.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "omenuitem.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

OMenuItem::OMenuItem( void )
	: _subMenu( NULL ), HANDLER( NULL ), _param( NULL ), _label() {
	M_PROLOG
	return;
	M_EPILOG
}

OMenuItem::OMenuItem( OMenuItem* const menuItem_, HANDLER_t const handler,
		void* param_, HString const& label_ )
	: _subMenu( menuItem_ ), HANDLER( handler ),
	_param( param_ ), _label ( label_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

OMenuItem::OMenuItem( OMenuItem const& menuItem_ )
	: _subMenu( menuItem_._subMenu ), HANDLER( menuItem_.HANDLER ),
	_param( menuItem_._param ), _label( menuItem_._label ) {
	M_PROLOG
	return;
	M_EPILOG
}

OMenuItem& OMenuItem::operator = ( OMenuItem const& menuItem_ ) {
	M_PROLOG
	if ( &menuItem_ != this ) {
		OMenuItem item( menuItem_ );
		swap( item );
	}
	return ( *this );
	M_EPILOG
}

void OMenuItem::reset( void ) {
	M_PROLOG
	_subMenu = NULL;
	HANDLER = NULL;
	_param = NULL;
	_label = "";
	return;
	M_EPILOG
}

void OMenuItem::call( HTUIProcess* proc ) {
	M_PROLOG
	static_cast<void>( ( proc->*( HANDLER ) )( _param ) );
	return;
	M_EPILOG
}

void OMenuItem::swap( OMenuItem& other ) {
	M_PROLOG
	if ( &other != this ) {
		using yaal::swap;
		swap( _subMenu, other._subMenu );
		swap( HANDLER, other.HANDLER );
		swap( _param, other._param );
		swap( _label, other._label );
	}
	return;
	M_EPILOG
}

}

}

