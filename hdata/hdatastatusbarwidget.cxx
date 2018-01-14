/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
		yaal::hcore::HString const& label_, OAttribute const& statusBarAttribute_ )
	: HWidget( parent_, - 2, 0, 2, - 1, label_ ),
	HStatusBarWidget( nullptr, HString(), statusBarAttribute_ ) {
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

