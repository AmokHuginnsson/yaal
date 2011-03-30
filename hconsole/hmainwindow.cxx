/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmainwindow.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hmainwindow.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole::list_control_helper;

namespace yaal
{

namespace hconsole
{

HMainWindow::HMainWindow( char const* title_, HTUIProcess::model_ptr_t windows_,
		HTUIProcess::model_t::cyclic_iterator& foregroundWindow_ )
		: HWindow( title_ ), _menu( NULL ), _windowList( windows_ ),
		_foregroundWindow( foregroundWindow_ )
	{
	M_PROLOG
	register_postprocess_handler ( KEY<'q'>::command, NULL,
				& HMainWindow::handler_close );
	return;
	M_EPILOG
	}

HMainWindow::~HMainWindow( void )
	{
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log_trace << "destruction success" << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_DESTRUCTOR_EPILOG
	}

int HMainWindow::init( void )
	{
	M_PROLOG
	int error = 0;
	HConsole& cons = HConsole::get_instance();
	if ( ( _focusedChild != HControlList::model_t::cyclic_iterator() ) && ( !! (*_focusedChild) ) )
		return ( 0 );
	error = HWindow::init();
	_menu = new HMenuControl( this, 1, 1, - 2,	- cons.get_width() / 2 - 1,
			" &Menu \n" );
	_menu->enable( true );
	_menu->set_focus();
	HWindowListControl* windowList = new HWindowListControl( this, 1,
			- cons.get_width() / 2 + 1, - 2, - 1, " &Opened window list: \n",
			HListControler<HWindow::ptr_t>::ptr_t( new HListControler<HWindow::ptr_t>( _windowList ) ),
			_foregroundWindow );
	windowList->add_column( -1, "&Okno", 1 );
	windowList->enable( true );
	_controls.select( _menu );
	return ( error );
	M_EPILOG
	}

void HMainWindow::init_menu( HTUIProcess* process_, OMenuItem* menu_ )
	{
	M_PROLOG
	_menu->init( process_,	menu_ );
	return;
	M_EPILOG
	}

int HMainWindow::handler_close( int code_, void const* )
	{
	M_PROLOG
	code_ = KEY<'x'>::command;
	return ( code_ );
	M_EPILOG
	}

}

}

