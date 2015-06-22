/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hdatadatewidget.cxx - this file is integral part of `yaal' project.

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
	attrs.label_position( r._labelPosition ).label_decoration( r._labelDecoration );
	HDataDateWidget* list( new ( memory::yaal ) HDataDateWidget( window, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( list->get_pointer(), node_ );
	apply_role( window, list, node_ );
	return ( list->get_pointer() );
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

