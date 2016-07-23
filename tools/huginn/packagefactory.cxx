/*
---           `gameground' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  packagefactory.cxx - this file is integral part of `gameground' project.

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

#include "packagefactory.hxx"
M_VCSID( "$Id: " __ID__ " $" )

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace tools {

namespace huginn {

void HPackageCreatorInterface::initialize_globals( void ) {
	M_PROLOG
	do_initialize_globals();
	return;
	M_EPILOG
}

void HPackageCreatorInterface::cleanup_globals( void ) {
	M_PROLOG
	do_cleanup_globals();
	return;
	M_EPILOG
}

HHuginn::value_t HPackageCreatorInterface::new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	return ( do_new_instance( runtime_ ) );
	M_EPILOG
}

HPackageFactory::HPackageFactory( void )
	: _creators() {
	return;
}

HPackageFactory::~HPackageFactory( void ) {
	return;
}

void HPackageFactory::register_package_creator( HString const& name_, HPackageCreatorInterface* instatiator_ ) {
	M_PROLOG
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() )
		M_THROW( _( "Package already registered" ), errno );
	OCreator creator;
	creator._instantiator = instatiator_;
	_creators[ name_ ] = creator;
	return;
	M_EPILOG
}

HHuginn::value_t HPackageFactory::create_package( HRuntime* runtime_, yaal::hcore::HString const& name_ ) {
	M_PROLOG
	HHuginn::value_t package;
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() ) {
		package = it->second._instantiator->new_instance( runtime_ );
	}
	return ( package );
	M_EPILOG
}

bool HPackageFactory::is_type_valid( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	bool valid = false;
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() ) {
		valid = true;
	}
	return ( valid );
	M_EPILOG
}

HPackageFactory::creators_t::iterator HPackageFactory::begin( void ) {
	return ( _creators.begin() );
}

HPackageFactory::creators_t::iterator HPackageFactory::end( void ) {
	return ( _creators.end() );
}

int HPackageFactory::life_time( int lifeTime_ ) {
	return ( lifeTime_ );
}

void HPackageFactory::initialize_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(); it != _creators.end(); ++ it ) {
		it->second._instantiator->initialize_globals();
	}
	return;
	M_EPILOG
}

void HPackageFactory::cleanup_globals( void ) {
	M_PROLOG
	for ( creators_t::iterator it = _creators.begin(); it != _creators.end(); ++ it ) {
		it->second._instantiator->cleanup_globals();
	}
	return;
	M_EPILOG
}

}

}

}

