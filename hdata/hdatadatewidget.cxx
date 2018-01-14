/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdatadatewidget.hxx"
#include "hconsole/console.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataDateWidget::HDataDateWidget( HDataWindow * parent_,
		int row_, int column_, int height_, int width_,
		yaal::hcore::HString const& label_, HWidgetAttributesInterface const& attr_ )
	:	HWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HDateWidget( parent_, row_, column_, label_, attr_ ),
		HDataWidget() {
	M_PROLOG
	return;
	M_EPILOG
}

HWidget::ptr_t HDataDateWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HEditWidgetAttributes attrs;
	HDataWindow* window( dynamic_cast<HDataWindow*>( window_ ) );
	M_ENSURE( window );
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	HDataDateWidget* date( create_widget<HDataDateWidget>( window, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( date->get_pointer(), node_ );
	apply_role( window, date, node_ );
	return ( date->get_pointer() );
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "datadate",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HDataDateWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}

