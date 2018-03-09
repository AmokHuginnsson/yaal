/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "packagefactory.hxx"
M_VCSID( "$Id: " __ID__ " $" )
#include "runtime.hxx"
#include "objectfactory.hxx"
#include "compiler.hxx"
#include "source.hxx"
#include "exception.hxx"
#include "hcore/hcore.hxx"
#include "hcore/hfile.hxx"
#include "tools/filesystem.hxx"
#include "tools/hplugin.hxx"

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
	: _creators()
	, _binaries() {
	return;
}

HPackageFactory::~HPackageFactory( void ) {
	return;
}

void HPackageFactory::register_package_creator( HString const& name_, HPackageCreatorInterface* instantiator_ ) {
	M_PROLOG
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() ) {
		M_THROW( _( "Package already registered" ), errno );
	}
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
	}
	HHuginn::paths_t paths( paths_ );
	paths.insert( paths.end(), HHuginn::MODULE_PATHS.begin(), HHuginn::MODULE_PATHS.end() );
	if ( ! package ) {
		package = load_binary( runtime_, paths, name_, position_ );
	}
	if ( ! package ) {
		package = load_module( runtime_, paths, compilerSetup_, name_, position_ );
	}
	if ( ! package ) {
		throw HHuginn::HHuginnRuntimeException( "Package `"_ys.append( name_ ).append( "' does not exist." ), MAIN_FILE_ID, position_ );
	}
	return ( package );
	M_EPILOG
}

HHuginn::value_t HPackageFactory::load_binary( HRuntime* runtime_, HHuginn::paths_t const& paths_, yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	plugin_t plugin( make_pointer<HPlugin>() );
	HString pluginName;
	pluginName
		.assign( LIB_PREFIX )
		.append( "huginn_" )
		.append( name_ )
		.append( LIB_INFIX )
		.append( "." )
		.append( LIB_EXT );
	pluginName.lower();
	HHuginn::value_t package;
	try {
		plugin->load( pluginName );
	} catch ( HPluginException const& ) {
	}
	if ( ! plugin->is_loaded() ) {
		HString test;
		for ( HString const& p : paths_ ) {
			test.assign( p ).append( "/" ).append( pluginName );
			substitute_environment( test, ENV_SUBST_MODE::RECURSIVE );
			try {
				plugin->load( test );
				break;
			} catch ( HPluginException const& ) {
			}
		}
	}
	if ( plugin->is_loaded() ) {
		try {
			typedef HPackageCreatorInterface* (*instantiator_getter_t)( void );
			instantiator_getter_t instantiator_getter( nullptr );
			plugin->resolve( "get_package_creator", instantiator_getter );
			HPackageCreatorInterface* instantiator( instantiator_getter() );
			register_package_creator( name_, instantiator );
			package = instantiator->new_instance( runtime_ );
			_binaries.emplace_back( plugin );
		} catch ( HPluginException const& e ) {
			throw HHuginn::HHuginnRuntimeException( e.what(), MAIN_FILE_ID, position_ );
		}
	}
	return ( package );
	M_EPILOG
}

HHuginn::value_t HPackageFactory::load_module( HRuntime* runtime_, HHuginn::paths_t const& paths_, HHuginn::compiler_setup_t compilerSetup_, yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	HString path;
	HString test;
	for ( HString const& p : paths_ ) {
		test.assign( p ).append( "/" ).append( name_ ).append( ".hgn" );
		substitute_environment( test, ENV_SUBST_MODE::RECURSIVE );
		try {
			if ( ! p.is_empty() && filesystem::is_regular_file( test ) ) {
				path = test;
				break;
			}
		} catch ( filesystem::HFileSystemException const& ) {
		}
	}
	return (
		! path.is_empty()
			? compile_module( runtime_, paths_, compilerSetup_, path, name_, position_ )
			: HHuginn::value_t()
	);
	M_EPILOG
}

HHuginn::value_t HPackageFactory::compile_module( HRuntime* runtime_, HHuginn::paths_t const& paths_, HHuginn::compiler_setup_t compilerSetup_, yaal::hcore::HString const& path_, yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	HFile src( path_, HFile::OPEN::READING );
	HHuginn loader( runtime_ );
	HRuntime& rt( const_cast<HRuntime&>( loader.runtime() ) );
	rt.copy_text( *runtime_ );
	loader.load( src );
	loader.preprocess();
	loader._compiler->_isModule = true;
	HHuginn& h( *runtime_->huginn() );
	int fileId( h._compiler->_fileId == MAIN_FILE_ID ? static_cast<int>( h._sources.get_size() - 1 ) : h._compiler->_fileId );
	loader._compiler->_fileId = fileId;
	if ( ! ( loader.parse() && loader.compile( paths_, compilerSetup_ ) ) ) {
		throw HHuginn::HHuginnRuntimeException( loader.error_message(), fileId, position_ );
	}
	h._compiler->_fileId = loader._compiler->_fileId;
	loader._state = HHuginn::STATE::PARSED;
	HHuginn::class_t c( rt.make_package( name_, *runtime_ ) );
	runtime_->copy_text( rt );
	h.register_class( c );
	h._sources.insert( h._sources.end(), make_move_iterator( loader._sources.begin() ), make_move_iterator( loader._sources.end() ) );
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

HHuginn::class_t add_to_package( HHuginn::HClass* package_, HHuginn::class_t const& class_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	HHuginn::value_t member(
		package_->runtime()->object_factory()->create_method_raw( class_->constructor_function( HHuginn::ACCESS::PUBLIC ) )
	);
	package_->add_member(
		HHuginn::HFieldDefinition(
			class_->name(), member, doc_
		)
	);
	return ( class_ );
	M_EPILOG
}

HHuginn::class_t package_exception( HHuginn::HClass* package_ ) {
	HString name( package_->name() );
	HString exName( name );
	exName.append( "Exception" );
	return (
		add_to_package(
			package_,
			exception::create_class(
				package_->runtime(),
				exName,
				"The `"_ys.append( exName ).append( "` exception type for `" ).append( name ).append( "` package." )
			),
			"( *message* ) - create instance of "_ys.append( exName ).append( " with given *message*" )
		)
	);
}

}

}

}

