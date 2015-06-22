/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hwindowfactory.cxx - this file is integral part of `yaal' project.

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
	log << "window creator for: `" << name_ << "' registered" << endl;
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
	return ( window );
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
	return ( lifeTime_ );
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

