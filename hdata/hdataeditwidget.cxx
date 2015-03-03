/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataeditwidget.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdataeditwidget.hxx"
#include "hconsole/console.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataEditWidget::HDataEditWidget( HDataWindow * parent_,
		int row_, int column_, int height_, int width_,
		yaal::hcore::HString const& label_, HWidgetAttributesInterface const& attr_ )
	:	HWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HEditWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HDataWidget(),
		_valid(),
		_attributeCache( _attributeFocused ) {
	M_PROLOG
	attr_.apply( *this );
	return;
	M_EPILOG
}

void HDataEditWidget::set_validator( yaal::hcore::HString const& valid_ ) {
	M_PROLOG
	if ( ! _valid.compile( valid_ ) ) {
		M_THROW( "Bad regex as validator: `"_ys.append( valid_ ).append( "', " ).append( _valid.error() ), _valid.error_code() );
	}
	_attributeCache = _attributeFocused;
	return;
	M_EPILOG
}

bool HDataEditWidget::is_valid( void ) const {
	M_PROLOG
	return ( ! _valid.is_valid() || _valid.matches( _string ) );
	M_EPILOG
}

int HDataEditWidget::do_process_input( int code_ ) {
	M_PROLOG
	bool noChange = false;
	switch ( code_ ) {
		case ( '\t' ):
		case ( '\r' ):
		case ( KEY_CODES::LEFT ):
		case ( KEY_CODES::RIGHT ):
		case ( KEY < 'a' >::ctrl ):
		case ( KEY_CODES::HOME ):
		case ( KEY < 'e' >::ctrl ):
		case ( KEY_CODES::END ):
		case ( KEY_CODES::INSERT ):
		case ( KEY < 'f' >::meta ):
		case ( KEY < 'b' >::meta ):
			noChange = true;
		break;
		case ( KEY_CODES::PAGE_UP ):
		case ( KEY_CODES::PAGE_DOWN ):
		case ( KEY_CODES::UP ):
		case ( KEY_CODES::DOWN ):
			if ( _multiLine )
				noChange = true;
		break;
		case ( KEY_CODES::BACKSPACE ):
			if ( ! ( _widgetOffset + _cursorPosition ) )
				noChange = true;
		break;
		case ( KEY_CODES::DELETE ):
			if ( _string.is_empty() )
				noChange = true;
		break;
		default:
		break;
	}
	code_ = HEditWidget::do_process_input( code_ );
	if ( ! ( code_ || noChange ) ) {
		HDataWindow* win( dynamic_cast<HDataWindow*>( _window ) );
		M_ASSERT( win );
		win->set_modified();
		if ( is_valid() ) {
			_attributeFocused = _attributeCache;
		} else {
			_attributeFocused._data = COLORS::FG_BROWN | COLORS::BG_LIGHTGRAY;
		}
	}
	return ( code_ );
	M_EPILOG
}

HDataEditWidgetAttributes::HDataEditWidgetAttributes( void )
	: HEditWidgetAttributes(),
	_valid( _maskLoose_ ),
	_validSet( false ) {
	return;
}

void HDataEditWidgetAttributes::do_apply( HWidget& widget_ ) const {
	M_PROLOG
	HDataEditWidget* widget( dynamic_cast<HDataEditWidget*>( &widget_ ) );
	if ( widget ) {
		if ( _validSet ) {
			widget->set_validator( _valid );
		}
	}
	HEditWidgetAttributes::do_apply( widget_ );
	return;
	M_EPILOG
}

HDataEditWidgetAttributes& HDataEditWidgetAttributes::valid( yaal::hcore::HString const& valid_ ) {
	M_PROLOG
	_valid = valid_;
	_validSet = true;
	return ( *this );
	M_EPILOG
}

HWidget::ptr_t HDataEditWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HDataEditWidgetAttributes attrs;
	HDataWindow* window( dynamic_cast<HDataWindow*>( window_ ) );
	M_ENSURE( window );
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	attrs.label_position( r._labelPosition ).label_decoration( r._labelDecoration );
	HDataEditWidget* list( new ( memory::yaal ) HDataEditWidget( window, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( list->get_pointer(), node_ );
	apply_role( window, list, node_ );
	return ( list->get_pointer() );
	M_EPILOG
}

bool HDataEditWidgetCreator::do_prepare_attributes( HWidgetAttributesInterface& attributes_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HDataEditWidgetAttributes& attrs( dynamic_cast<HDataEditWidgetAttributes&>( attributes_ ) );
	HString const& name( node_.get_name() );
	bool ok( HEditWidgetCreator::do_prepare_attributes( attributes_, node_ ) );
	if ( name == "valid" ) {
		attrs.valid( xml::node_val( node_ ) );
	} else {
		ok = false;
	}
	return ( ok );
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "dataedit",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HDataEditWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}

