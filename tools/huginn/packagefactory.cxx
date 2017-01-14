/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  packagefactory.cxx - this file is integral part of `yaal' project.

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
#include "runtime.hxx"
#include "objectfactory.hxx"
#include "compiler.hxx"
#include "hcore/hfile.hxx"
#include "tools/filesystem.hxx"

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

void HPackageFactory::register_package_creator( HString const& name_, HPackageCreatorInterface* instantiator_ ) {
	M_PROLOG
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() )
		M_THROW( _( "Package already registered" ), errno );
	OCreator creator;
	creator._instantiator = instantiator_;
	_creators[ name_ ] = creator;
	return;
	M_EPILOG
}

HHuginn::value_t HPackageFactory::create_package( HRuntime* runtime_, HHuginn::paths_t const& paths_, HHuginn::compiler_setup_t compilerSetup_, yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	HHuginn::value_t package;
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() ) {
		package = it->second._instantiator->new_instance( runtime_ );
	} else {
		HHuginn::paths_t paths( paths_ );
		paths.insert( paths.end(), HHuginn::MODULE_PATHS.begin(), HHuginn::MODULE_PATHS.end() );
		HString path;
		HString test;
		for ( HString const& p : paths ) {
			test.assign( p ).append( "/" ).append( name_ ).append( ".hgn" );
			try {
				if ( ! p.is_empty() && filesystem::is_regular_file( test ) ) {
					path = test;
					break;
				}
			} catch ( filesystem::HFileSystemException const& ) {
			}
		}
		if ( ! path.is_empty() ) {
			package = load_module( runtime_, paths_, compilerSetup_, path, name_, position_ );
		} else {
			throw HHuginn::HHuginnRuntimeException( "Package `"_ys.append( name_ ).append( "' does not exist." ), position_ );
		}
	}
	return ( package );
	M_EPILOG
}

HHuginn::value_t HPackageFactory::load_module( HRuntime* runtime_, HHuginn::paths_t const& paths_, HHuginn::compiler_setup_t compilerSetup_, yaal::hcore::HString const& path_, yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	HFile src( path_, HFile::OPEN::READING );
	HHuginn loader( runtime_ );
	HRuntime& rt( const_cast<HRuntime&>( loader.runtime() ) );
	rt.copy_text( *runtime_ );
	loader.load( src );
	loader.preprocess();
	loader._compiler->_isModule = true;
	if ( ! ( loader.parse() && loader.compile( paths_, compilerSetup_ ) ) ) {
		throw HHuginn::HHuginnRuntimeException( loader.error_message(), position_ );
	}
	loader._state = HHuginn::STATE::PARSED;
	HHuginn::class_t c( rt.make_package( name_, *runtime_ ) );
	runtime_->copy_text( rt );
	runtime_->huginn()->register_class( c );
	huginn::HThread t( runtime_, hcore::HThread::get_current_thread_id() );
	return ( runtime_->object_factory()->create_object( c.raw(), c->get_defaults( &t, position_ ) ) );
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

void initialize_packages( void ) {
	M_PROLOG
	HPackageFactory::get_instance().initialize_globals();
	return;
	M_EPILOG
}

void cleanup_packages( void ) {
	M_PROLOG
	HPackageFactory::get_instance().cleanup_globals();
	return;
	M_EPILOG
}

}

}

}

