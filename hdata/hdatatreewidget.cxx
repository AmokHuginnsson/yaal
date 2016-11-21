/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hdatatreewidget.cxx - this file is integral part of `yaal' project.

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

