/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdatatreewidget.hxx"
#include "hdatawindow.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataTreeWidget::HDataTreeWidget(
		HDataWindow* window_, int row_, int column_, int height_,
		int width_, yaal::hcore::HString const& title_,
		hconsole::HWidgetAttributesInterface const& attrs_ )
	: HWidget( window_, row_, column_, height_,
			width_, title_, attrs_ ),
		HTreeWidget( nullptr, 0, 0, 0, 0, hcore::HString(), attrs_ ),
		HDataWidget() {
	M_PROLOG
	return;
	M_EPILOG
}

HDataTreeWidget::~HDataTreeWidget( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HDataTreeWidget::do_load( int long /*id_*/ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HWidget::ptr_t HDataTreeWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HTreeWidgetAttributes attrs;
	HDataWindow* window( dynamic_cast<HDataWindow*>( window_ ) );
	M_ENSURE( window );
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	HDataTreeWidget* tree( create_widget<HDataTreeWidget>( window, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( tree->get_pointer(), node_ );
	apply_role( window, tree, node_ );
	return ( tree->get_pointer() );
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "datatree",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HDataTreeWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}

