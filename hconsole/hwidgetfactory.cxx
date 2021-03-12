/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hwidgetfactory.hxx"
#include "hconsole.hxx"
#include "hcore/hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

void HWidgetFactory::register_widget_creator( HString const& name_, HWidgetCreatorInterface::ptr_t creator_ ) {
	M_PROLOG
	creators_t::iterator it( _creators.find( name_ ) );
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
	creators_t::iterator it( _creators.find( node_.get_name() ) );
	if ( it != _creators.end() ) {
		widget = it->second->new_instance( window_, node_ );
	}
	return widget;
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
	return lifeTime_;
}

void HWidgetFactory::initialize_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(), e( _creators.end() ); it != e; ++ it ) {
		it->second->initialize_globals();
	}
	return;
	M_EPILOG
}

void HWidgetFactory::cleanup_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(), e( _creators.end() ); it != e; ++ it ) {
		it->second->cleanup_globals();
	}
	return;
	M_EPILOG
}

HWidgetCreatorInterface::OResource::OResource( void )
	: _row( 0 )
	, _column( 0 )
	, _height( 0 )
	, _width( 0 )
	, _label() {
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

char const POSITION_NAME[] = "position";
char const LABEL_NAME[] = "label";
char const ATTRIBUTE_NAME[] = "attribute";

void HWidgetCreatorInterface::apply_resources( HWidget::ptr_t widget_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	for ( HXml::HConstNodeProxy const& n : node_ ) {
		if ( ! do_apply_resources( widget_, n ) ) {
			HString const& name( n.get_name() );
			if ( ( name != POSITION_NAME ) && ( name != LABEL_NAME ) ) {
				hcore::log << "unknown " << node_.get_name() << " attribute name: " << name << ", at: " << n.get_line() << endl;
			}
		}
	}
	return;
	M_EPILOG
}

namespace {

HWidget::OAttribute make_attr( yaal::hcore::HString const& type_, yaal::tools::HXml::HConstNodeProxy const& node_, HWidget::OAttribute const& init_ ) {
	M_PROLOG
	HWidget::OAttribute attr( init_ );
	xml::value_t a( xml::try_attr_val( node_, "foreground_label_" + type_ ) );
	if ( !! a ) {
		int v( attr._label & 0xf0 );
		attr._label = static_cast<COLOR::color_t>( v | COLOR::from_string( *a ) );
	}
	a = xml::try_attr_val( node_, "background_label_" + type_ );
	if ( !! a ) {
		int v( attr._label & 0x0f );
		attr._label = static_cast<COLOR::color_t>( v | COLOR::fg_to_bg( COLOR::from_string( *a ) ) );
	}
	a = xml::try_attr_val( node_, "foreground_data_" + type_ );
	if ( !! a ) {
		int v( attr._data & 0xf0 );
		attr._data = static_cast<COLOR::color_t>( v | COLOR::from_string( *a ) );
	}
	a = xml::try_attr_val( node_, "background_data_" + type_ );
	if ( !! a ) {
		int v( attr._data & 0x0f );
		attr._data = static_cast<COLOR::color_t>( v | COLOR::fg_to_bg( COLOR::from_string( *a ) ) );
	}
	return attr;
	M_EPILOG
}

}

void HWidgetCreatorInterface::prepare_attributes( HWidgetAttributesInterface& attrs_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	xml::value_t id( xml::try_attr_val( node_, "id" ) );
	HWidgetAttributes& wa( static_cast<HWidgetAttributes&>( attrs_ ) );
	if ( !! id ) {
		wa.id( *id );
	}
	for ( HXml::HConstNodeProxy const& n : node_ ) {
		HString const& name( n.get_name() );
		if ( name == LABEL_NAME ) {
			xml::value_t position( xml::try_attr_val( n, "position" ) );
			if ( position ) {
				HWidget::LABEL::POSITION labelPosition;
				if ( *position == "stacked" ) {
					labelPosition = HWidget::LABEL::POSITION::STACKED;
				} else if ( *position == "side_by_side" ) {
					labelPosition = HWidget::LABEL::POSITION::SIDE_BY_SIDE;
				} else {
					throw HWidgetFactoryException( "Bad label position type `"_ys.append( *position ).append( ", at: " ).append( n.get_line() ) );
				}
				wa.label_position( labelPosition );
			}
			xml::value_t decoration( xml::try_attr_val( n, "decoration" ) );
			if ( decoration ) {
				HWidget::LABEL::DECORATION labelDecoration;
				if ( *decoration == "auto" ) {
					labelDecoration = HWidget::LABEL::DECORATION::AUTO;
				} else if ( *decoration == "explicit" ) {
					labelDecoration = HWidget::LABEL::DECORATION::EXPLICIT;
				} else {
					throw HWidgetFactoryException( "Bad label decoration type `"_ys.append( *decoration ).append( ", at: " ).append( n.get_line() ) );
				}
				wa.label_decoration( labelDecoration );
			}
		} else if ( name == ATTRIBUTE_NAME ) {
			wa.attribute_focused( make_attr( "focused", n, _attributeFocused_ ) );
			wa.attribute_enabled( make_attr( "enabled", n, _attributeEnabled_ ) );
			wa.attribute_disabled( make_attr( "disabled", n, _attributeDisabled_ ) );
		} else if ( ! do_prepare_attributes( attrs_, n ) ) {
			if ( ( name != POSITION_NAME ) && ( name != LABEL_NAME ) ) {
				hcore::log << "unknown " << node_.get_name() << " attribute name: " << name << ", at: " << n.get_line() << endl;
			}
		}
	}
	return;
	M_EPILOG
}

HWidgetCreatorInterface::OResource HWidgetCreatorInterface::get_resource( yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	OResource r;
	for ( HXml::HConstNodeProxy const& n : node_ ) {
		HString const& name( n.get_name() );
		if ( name == POSITION_NAME ) {
			r._row = lexical_cast<int>( xml::attr_val( n, "row" ) );
			r._column = lexical_cast<int>( xml::attr_val( n, "column" ) );
			xml::value_t height( xml::try_attr_val( n, "height" ) );
			r._height = !! height ? lexical_cast<int>( *height ) : 0;
			xml::value_t width( xml::try_attr_val( n, "width" ) );
			r._width = !! width ? lexical_cast<int>( *width ) : 0;
		} else if ( name == LABEL_NAME ) {
			r._label = xml::node_val( n );
		}
	}
	return r;
	M_EPILOG
}

}

}

