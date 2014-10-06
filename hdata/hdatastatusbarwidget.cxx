/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatastatusbarwidget.cxx - this file is integral part of `yaal' project.

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
#include "hconsole/console.hxx"
#include "hdatastatusbarwidget.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;

namespace yaal {

namespace hdata {

HDataStatusBarWidget::HDataStatusBarWidget( HDataWindow* parent_,
		yaal::hcore::HString const& label_, int statusBarAttribute_ )
	: HWidget( parent_, - 2, 0, 2, - 1, label_ ),
	HStatusBarWidget( NULL, HString(), statusBarAttribute_ ) {
	M_PROLOG
	_labelLength += 3; /* for "[+]" */
	return;
	M_EPILOG
}

void HDataStatusBarWidget::do_draw_label ( void ) {
	M_PROLOG
	HStatusBarWidget::do_draw_label();
	HDataWindow* win( dynamic_cast<HDataWindow*>( _window ) );
	M_ASSERT ( win );
	if ( win->is_modified() ) {
		set_attr_shortcut();
		HConsole& cons = HConsole::get_instance();
		cons.mvprintf ( cons.get_height() - 2, _labelLength - 4, "[+]" );
		set_attr_data();
	}
	return;
	M_EPILOG
}

}

}

