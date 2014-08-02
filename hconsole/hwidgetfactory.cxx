/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hwidgetfactory.cxx - this file is integral part of `yaal' project.

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
#include "hwidgetfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

void HWidgetFactory::register_widget_creator( HString const& name_, HWidgetCreatorInterface::ptr_t creator_ ) {
	M_PROLOG
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() ) {
		M_THROW( _( "Widget already registered" ), errno );
	}
	_creators[ name_ ] = creator_;
	return;
	M_EPILOG
}

HWidget::ptr_t HWidgetFactory::create_widget( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HWidget::ptr_t widget;
	creators_t::iterator it = _creators.find( node_.get_name() );
	if ( it != _creators.end() ) {
		widget = it->second->new_instance( window_, node_ );
	}
	return ( widget );
	M_EPILOG
}

bool HWidgetFactory::is_type_valid( yaal::hcore::HString const& type_ ) {
	M_PROLOG
	return ( _creators.find( type_ ) != _creators.end() );
	M_EPILOG
}

HWidgetFactory::creators_t::iterator HWidgetFactory::begin( void ) {
	return ( _creators.begin() );
}

HWidgetFactory::creators_t::iterator HWidgetFactory::end( void ) {
	return ( _creators.end() );
}

int HWidgetFactory::life_time( int lifeTime_ ) {
	return ( lifeTime_ );
}

void HWidgetFactory::initialize_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(); it != _creators.end(); ++ it ) {
		it->second->initialize_globals();
	}
	return;
	M_EPILOG
}

void HWidgetFactory::cleanup_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(); it != _creators.end(); ++ it ) {
		it->second->cleanup_globals();
	}
	return;
	M_EPILOG
}

HWidgetCreatorInterface::OResource::OResource( void )
	: _row( 0 ), _column( 0 ), _height( 0 ), _width( 0 ),
	_label( "" ),
	_labelPosition( HWidget::LABEL::POSITION::SIDE_BY_SIDE ),
	_labelDecoration( HWidget::LABEL::DECORATION::AUTO ) {
}

void HWidgetCreatorInterface::initialize_globals( void ) {
	M_PROLOG
	do_initialize_globals();
	return;
	M_EPILOG
}

void HWidgetCreatorInterface::cleanup_globals( void ) {
	M_PROLOG
	do_cleanup_globals();
	return;
	M_EPILOG
}

HWidget::ptr_t HWidgetCreatorInterface::new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	return ( do_new_instance( window_, node_ ) );
	M_EPILOG
}

void HWidgetCreatorInterface::apply_resources( HWidget::ptr_t widget_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	do_apply_resources( widget_, node_ );
	return;
	M_EPILOG
}

void HWidgetCreatorInterface::prepare_attributes( HWidgetAttributesInterface& attrs_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	do_prepare_attributes( attrs_, node_ );
	return;
	M_EPILOG
}

}

}

