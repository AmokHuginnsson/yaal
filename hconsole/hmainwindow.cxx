/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hmainwindow.hxx"
#include "hcore/memory.hxx"
#include "tools/keycode.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::hconsole::list_widget_helper;

namespace yaal {

namespace hconsole {

HMainWindow::HMainWindow( yaal::hcore::HString const& title_, HTUIProcess::model_ptr_t windows_,
		HTUIProcess::cyclic_iterator& foregroundWindow_ )
		: HWindow( title_ ), _menu( nullptr ), _windowList( windows_ ),
		_foregroundWindow( foregroundWindow_ ) {
	M_PROLOG
	register_postprocess_handler( KEY<'q'>::command, nullptr, call( &HMainWindow::handler_close, this, _1 ) );
	return;
	M_EPILOG
}

HMainWindow::~HMainWindow( void ) {
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log_trace << "destruction success" << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_DESTRUCTOR_EPILOG
}

void HMainWindow::do_init( void ) {
	M_PROLOG
	HConsole& cons = HConsole::get_instance();
	if ( ( _focusedChild != HWidgetList::cyclic_iterator() ) && ( !! (*_focusedChild) ) ) {
		return;
	}
	HWindow::do_init();
	_menu = create_widget<HMenuWidget>( this, 1, 1, -2, - cons.get_width() / 2 - 1, "&Menu" );
	_menu->enable( true );
	_menu->set_focus();
	HWindowListWidget* windowList(
		create_widget<HWindowListWidget>(
			this, 1, - cons.get_width() / 2 + 1, - 2, - 1, "&Opened window list",
			make_pointer<HAsIsValueListModel<HWindow::ptr_t> >( _windowList ),
			_foregroundWindow
		)
	);
	windowList->add_column( -1, make_resource<HListWidget::HColumnInfo>( "&Okno", 1 ) );
	windowList->enable( true );
	_menu->init( _tuiProcess );
	_widgets.select( _menu );
	return;
	M_EPILOG
}

bool HMainWindow::handler_close( HEvent const& ) {
	M_PROLOG
	_tuiProcess->schedule_call( call( &HTUIProcess::quit, _tuiProcess ) );
	return ( true );
	M_EPILOG
}

void HMainWindow::set_menu( HMenuWidget::data_ptr_t menu_ ) {
	M_PROLOG
	return ( _menu->set_data( menu_ ) );
	M_EPILOG
}

}

}

