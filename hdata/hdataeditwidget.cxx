/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdataeditwidget.hxx"
#include "hconsole/console.hxx"
#include "hcore/hlog.hxx"
#include "tools/keycode.hxx"

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
		case ( KEY_CODE::LEFT ):
		case ( KEY_CODE::RIGHT ):
		case ( KEY < 'a' >::ctrl ):
		case ( KEY_CODE::HOME ):
		case ( KEY < 'e' >::ctrl ):
		case ( KEY_CODE::END ):
		case ( KEY_CODE::INSERT ):
		case ( KEY < 'f' >::meta ):
		case ( KEY < 'b' >::meta ):
			noChange = true;
		break;
		case ( KEY_CODE::PAGE_UP ):
		case ( KEY_CODE::PAGE_DOWN ):
		case ( KEY_CODE::UP ):
		case ( KEY_CODE::DOWN ):
			if ( _multiLine )
				noChange = true;
		break;
		case ( KEY_CODE::BACKSPACE ):
			if ( ! ( _widgetOffset + _cursorPosition ) )
				noChange = true;
		break;
		case ( KEY_CODE::DELETE ):
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
			_attributeFocused._data = static_cast<COLOR::color_t>( COLOR::FG_BROWN | COLOR::BG_LIGHTGRAY );
		}
	}
	return code_;
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
	HDataEditWidget* edit( create_widget<HDataEditWidget>( window, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( edit->get_pointer(), node_ );
	apply_role( window, edit, node_ );
	return ( edit->get_pointer() );
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
	return ok;
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

