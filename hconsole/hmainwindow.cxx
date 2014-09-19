/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmainwindow.cxx - this file is integral part of `yaal' project.

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
#include "hmainwindow.hxx"
#include "hcore/memory.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole::list_widget_helper;

namespace yaal {

namespace hconsole {

HMainWindow::HMainWindow( yaal::hcore::HString const& title_, HTUIProcess::model_ptr_t windows_,
		HTUIProcess::model_t::cyclic_iterator& foregroundWindow_ )
		: HWindow( title_ ), _menu( NULL ), _windowList( windows_ ),
		_foregroundWindow( foregroundWindow_ ) {
	M_PROLOG
	register_postprocess_handler( KEY<'q'>::command, NULL, call( &HMainWindow::handler_close, this, _1 ) );
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
	if ( ( _focusedChild != HWidgetList::model_t::cyclic_iterator() ) && ( !! (*_focusedChild) ) ) {
		return;
	}
	HWindow::do_init();
	_menu = new ( memory::yaal ) HMenuWidget( this, 1, 1, -2, - cons.get_width() / 2 - 1, "&Menu" );
	_menu->enable( true );
	_menu->set_focus();
	HWindowListWidget* windowList = new ( memory::yaal ) HWindowListWidget( this, 1,
			- cons.get_width() / 2 + 1, - 2, - 1, "&Opened window list",
			make_pointer<HAsIsValueListModel<HWindow::ptr_t> >( _windowList ),
			_foregroundWindow );
	windowList->add_column( -1, "&Okno", 1 );
	windowList->enable( true );
	_widgets.select( _menu );
	return;
	M_EPILOG
}

void HMainWindow::init_menu( HTUIProcess* process_, OMenuItem* menu_ ) {
	M_PROLOG
	_menu->init( process_, menu_ );
	return;
	M_EPILOG
}

bool HMainWindow::handler_close( HEvent const& ) {
	M_PROLOG
	_tuiProcess->schedule_call( call( &HTUIProcess::quit, _tuiProcess ) );
	return ( true );
	M_EPILOG
}

}

}

