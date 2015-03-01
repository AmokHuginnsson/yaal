/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.cxx - this file is integral part of `yaal' project.

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

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdata/hdataprocess.hxx"
#include "hcore/memory.hxx"
#include "hcore/hlog.hxx"
#include "hcore/hfile.hxx"
#include "tools/hplugin.hxx"
#include "hconsole/hmainwindow.hxx"
#include "dbwrapper/db_driver_loader.hxx"
#include "hdatawindow.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataProcess::HDataProcess( void )
	: HTUIProcess(), _dataBase( HDataBase::get_connector() ) {
	M_PROLOG
	return;
	M_EPILOG
}

HDataProcess::~HDataProcess( void ) {
	M_PROLOG
#ifdef __DEBUGGER_BABUNI__
	log_trace << "destruction success" << endl;
#endif /* __DEBUGGER_BABUNI__ */
	return;
	M_DESTRUCTOR_EPILOG
}

void HDataProcess::init_xrc(
		yaal::hcore::HString const& processName_,
		yaal::hcore::HString const& resource_ ) {
	M_PROLOG
	HTUIProcess::init_xrc( processName_, resource_ );
	if ( dbwrapper::_dBDrivers_.is_empty() )
		M_THROW( "no database driver loaded", errno );
	M_ASSERT( ( _foregroundWindow != HTUIProcess::cyclic_iterator() ) && ( !! (*_foregroundWindow) ) );
	HMainWindow* mainWindow( dynamic_cast<HMainWindow*>( &*(*_foregroundWindow) ) );
	M_ASSERT( mainWindow );
	mainWindow->paint();
	return;
	M_EPILOG
}

database_ptr_t HDataProcess::data_base( void ) {
	return ( _dataBase );
}

namespace {
void dummy( void ) {}
}

void HDataProcess::do_quit( void ) {
	M_PROLOG
	HDataWindow* window = NULL;
	if ( _windows->size() ) {
		model_t::iterator it = _windows->begin();
		if ( it != _windows->end() )
			++ it;
		for ( ; it != _windows->end(); ++ it ) {
			window = dynamic_cast<HDataWindow*>( static_cast<HWindow*>( &*(*it) ) );
			if ( window && window->is_modified() ) {
				select( window );
				handler_refresh( HKeyPressEvent( 'r' ) );
				window->status_bar()->confirm( "exit program", call( &HDataProcess::force_quit, this ), call( &dummy ) );
				return;
			}
		}
	}
	HTUIProcess::do_quit();
	return;
	M_EPILOG
}

void HDataProcess::force_quit( void ) {
	M_PROLOG
	HTUIProcess::do_quit();
	return;
	M_EPILOG
}

void HDataProcess::do_close_window( void ) {
	M_PROLOG
	HDataWindow* window = NULL;
	if ( !! (*_foregroundWindow) ) {
		window = dynamic_cast<HDataWindow*>( &*(*_foregroundWindow) );
		if ( window && window->is_modified() ) {
			window->status_bar()->confirm( "close window", call( &HDataProcess::force_close_window, this ), call( &dummy ) );
			return;
		}
	}
	HTUIProcess::do_close_window();
	return;
	M_EPILOG
}

void HDataProcess::force_close_window( void ) {
	M_PROLOG
	HTUIProcess::do_close_window();
	return;
	M_EPILOG
}

}

}

