/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
#include "dbwrapper/dbwrapper.hxx"
#include "hdatawindow.hxx"

using namespace yaal::hcore;
using namespace yaal::hconsole;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataProcess::HDataProcess( void )
	: HTUIProcess()
	, _dataBase() {
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

void HDataProcess::do_init_xrc(
		yaal::hcore::HString const& processName_,
		yaal::hcore::HString const& resource_ ) {
	M_PROLOG
	init_xrc_db( processName_, resource_, HString() );
	return;
	M_EPILOG
}

void HDataProcess::init_xrc_db(
		yaal::hcore::HString const& processName_,
		yaal::hcore::HString const& resource_,
		yaal::hcore::HString const& dsn_ ) {
	M_PROLOG
	do_init_xrc_db( processName_, resource_, dsn_ );
	return;
	M_EPILOG
}

void HDataProcess::do_init_xrc_db(
		yaal::hcore::HString const& processName_,
		yaal::hcore::HString const& resource_,
		yaal::hcore::HString const& dsn_ ) {
	M_PROLOG
	HTUIProcess::do_init_xrc( processName_, resource_ );
	xml::value_t dsn( _resource->get_value( "/resource/dsn" ) );
	if ( ! dsn_.is_empty() ) {
		_dataBase = dbwrapper::util::connect( dsn_ );
	} else if ( !! dsn ) {
		_dataBase = dbwrapper::util::connect( *dsn );
	} else {
		_dataBase = HDataBase::get_connector();
	}
	if ( dbwrapper::_dBDrivers_.is_empty() ) {
		M_THROW( "no database driver loaded", errno );
	}
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
	HDataWindow* window = nullptr;
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
	HDataWindow* window = nullptr;
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

