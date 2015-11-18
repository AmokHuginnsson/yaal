/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hdatawidget.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdatawidget.hxx"
#include "hdatawindow.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::dbwrapper;
using namespace yaal::hconsole;

namespace yaal {

namespace hdata {

HDataWidget::HDataWidget( void )
	: HWidget( NULL, 0, 0, 0, 0, hcore::HString() ),
	_crud(),
	_viewQuery() {
	M_PROLOG
	return;
	M_EPILOG
}

HDataWidget::~HDataWidget( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HDataWidget::set_crud_descriptor( HCRUDDescriptor::ptr_t crud_ ) {
	M_PROLOG
	_crud = crud_;
	M_EPILOG
}

void HDataWidget::set_view_query( yaal::hcore::HString const& viewQuery_ ) {
	M_PROLOG
	_viewQuery = viewQuery_;
	return;
	M_EPILOG
}

void HDataWidget::load( int long id_ ) {
	M_PROLOG
	do_load( id_ );
	return;
	M_EPILOG
}

void HDataWidget::save( int long id_ ) {
	M_PROLOG
	do_save( id_ );
	return;
	M_EPILOG
}

int long HDataWidget::get_current_id( void ) {
	M_PROLOG
	return ( do_get_current_id() );
	M_EPILOG
}

int long HDataWidget::do_get_current_id( void ) {
	M_PROLOG
	return ( -1 );
	M_EPILOG
}

void HDataWidget::add_new( void ) {
	M_PROLOG
	do_add_new();
	return;
	M_EPILOG
}

void HDataWidget::cancel_new( void ) {
	M_PROLOG
	do_cancel_new();
	return;
	M_EPILOG
}

void HDataWidget::do_load( int long ) {
	return;
}

void HDataWidget::do_save( int long ) {
	return;
}

void HDataWidget::do_add_new( void ) {
	return;
}

void HDataWidget::do_cancel_new( void ) {
	return;
}

void sync( HRecordSet::value_t& to_, HInfo const& from_, TYPE type_ ) {
	M_PROLOG
	switch ( type_ ) {
		case ( TYPE::HSTRING ): {
			to_ = from_.get_string();
		} break;
		case ( TYPE::INT_LONG_LONG ): {
			to_ = to_string( from_.get_integer() );
		} break;
		case ( TYPE::DOUBLE_LONG ): {
			to_ = to_string( from_.get_real() );
		} break;
		case ( TYPE::HTIME ): {
			to_ = from_.get_time().string();
		} break;
		default: {
			M_ASSERT( !"bad field type"[0] );
		}
	}
	return;
	M_EPILOG
}

void apply_role( HDataWindow* window_, HDataWidget* widget_, yaal::tools::HXml::HConstNodeProxy node_ ) {
	M_PROLOG
	HString xmlRole( xml::attr_val( node_, "role" ) );
	HDataWidget::ROLE role( HDataWidget::ROLE::INVALID );
	if ( xmlRole == "main" )
		role = HDataWidget::ROLE::MAIN;
	else if ( xmlRole == "data" )
		role = HDataWidget::ROLE::DATA;
	else
		M_THROW( _( "unknown role" ), node_.get_line() );
	window_->set_widget_role( widget_, role );
	return;
	M_EPILOG
}

}

}

