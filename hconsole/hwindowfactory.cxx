/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hwindowfactory.hxx"
#include "hconsole/hwindow.hxx"
#include "hcore/hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

void HWindowFactory::register_window_creator( HString const& name_, HWindowCreatorInterface::ptr_t creator_ ) {
	M_PROLOG
	creators_t::iterator it( _creators.find( name_ ) );
	if ( it != _creators.end() ) {
		M_THROW( _( "Window already registered" ), errno );
	}
	_creators[ name_ ] = creator_;
	hcore::log << "window creator for: `" << name_ << "' registered" << endl;
	return;
	M_EPILOG
}

HWindowCreatorInterface::window_ptr_t HWindowFactory::create_window( HTUIProcess* tuiProcess_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HWindow::ptr_t window;
	creators_t::iterator it( _creators.find( node_.get_name() ) );
	if ( it != _creators.end() ) {
		window = it->second->new_instance( tuiProcess_, node_ );
		it->second->apply_resources( tuiProcess_, window, node_ );
	} else {
		M_THROW( "no window creator for: " + node_.get_name() + ", at: ", node_.get_line() );
	}
	return window;
	M_EPILOG
}

bool HWindowFactory::is_type_valid( yaal::hcore::HString const& type_ ) {
	M_PROLOG
	return ( _creators.find( type_ ) != _creators.end() );
	M_EPILOG
}

HWindowFactory::creators_t::iterator HWindowFactory::begin( void ) {
	return ( _creators.begin() );
}

HWindowFactory::creators_t::iterator HWindowFactory::end( void ) {
	return ( _creators.end() );
}

int HWindowFactory::life_time( int lifeTime_ ) {
	return lifeTime_;
}

void HWindowFactory::initialize_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it( _creators.begin() ), e( _creators.end() ); it != e; ++ it ) {
		it->second->initialize_globals();
	}
	return;
	M_EPILOG
}

void HWindowFactory::cleanup_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it( _creators.begin() ), e( _creators.end() ); it != e; ++ it ) {
		it->second->cleanup_globals();
	}
	return;
	M_EPILOG
}

void HWindowCreatorInterface::initialize_globals( void ) {
	M_PROLOG
	do_initialize_globals();
	return;
	M_EPILOG
}

void HWindowCreatorInterface::cleanup_globals( void ) {
	M_PROLOG
	do_cleanup_globals();
	return;
	M_EPILOG
}

HWindowCreatorInterface::window_ptr_t HWindowCreatorInterface::new_instance( HTUIProcess* tuiProcess_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	return ( do_new_instance( tuiProcess_, node_ ) );
	M_EPILOG
}

bool HWindowCreatorInterface::apply_resources( HTUIProcess* tui_, HWindow::ptr_t window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	return ( do_apply_resources( tui_, window_, node_ ) );
	M_EPILOG
}

}

}

