/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "packagefactory.hxx"
M_VCSID( "$Id: " __ID__ " $" )
#include "runtime.hxx"
#include "objectfactory.hxx"
#include "compiler.hxx"
#include "source.hxx"
#include "hcore/hcore.hxx"
#include "hcore/hfile.hxx"
#include "tools/filesystem.hxx"
#include "tools/hplugin.hxx"
#include "tools/util.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

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
	, _binaries()
	, _visitedImports()
	, _mutex( HMutex::TYPE::RECURSIVE ) {
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

namespace {
inline void remove_visited( HPackageFactory::visited_imports_t* vi_, HPackageFactory::visited_imports_t::iterator const* it_ ) {
	vi_->erase( *it_ );
}
}

HHuginn::value_t HPackageFactory::create_package( HRuntime* runtime_, yaal::hcore::HString name_, int position_ ) {
	M_PROLOG
	HLock l( _mutex );
	HHuginn::value_t package;
	creators_t::iterator it = _creators.find( name_ );
	if ( it != _creators.end() ) {
		package = it->second._instantiator->new_instance( runtime_ );
	}
	visited_imports_t::insert_result ir( _visitedImports.insert( name_ ) );
	if ( ! ir.second ) {
		throw HHuginn::HHuginnRuntimeException(
			"Package `"_ys.append( name_ ).append( "' is already being imported." ),
			runtime_->file_id(),
			position_
		);
	}
	HScopeExitCall::call_t bc( call( &remove_visited, &_visitedImports, &ir.first ) );
	HScopeExitCall sec( yaal::move( bc ) );
	HHuginn::paths_t paths( runtime_->module_paths() );
	paths.insert( paths.end(), HHuginn::MODULE_PATHS.begin(), HHuginn::MODULE_PATHS.end() );
	if ( ! package ) {
		package = load_binary( runtime_, paths, name_, position_ );
	}
	if ( ! package ) {
		package = load_module( runtime_, paths, name_, position_ );
	}
	if ( ! package ) {
		throw HHuginn::HHuginnRuntimeException(
			"Package `"_ys.append( name_ ).append( "' does not exist." ),
			runtime_->file_id(),
			position_
		);
	}
	return ( package );
	M_EPILOG
}

HHuginn::value_t HPackageFactory::load_binary( HRuntime* runtime_, HHuginn::paths_t const& paths_, yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	static HRegex const re( "([^.])[.]([^.])" );
	HString name( re.replace( re.replace( name_, "$1/$2" ), "$1/$2" ) );
	plugin_t plugin( make_pointer<HPlugin>() );
	bool isSubDir( name.find( filesystem::path::SEPARATOR ) != HString::npos );
	HString pluginName( isSubDir ? filesystem::dirname( name ).append( filesystem::path::SEPARATOR ) : "" );
	pluginName
		.append( LIB_PREFIX )
		.append( "huginn_" )
		.append( isSubDir ? filesystem::basename( name ) : name )
		.append( LIB_INFIX )
		.append( "." )
		.append( LIB_EXT );
	pluginName.lower();
	auto load = [&plugin]( yaal::hcore::HString path ) {
		try {
			substitute_environment( path, ENV_SUBST_MODE::RECURSIVE );
			plugin->load( path );
		} catch ( HPluginException const& ) {
		}
		return ( plugin->is_loaded() );
	};
	if ( ! load( pluginName ) && filesystem::is_relative( name ) ) {
		HString test;
		for ( HString const& p : paths_ ) {
			test.assign( p ).append( filesystem::path::SEPARATOR ).append( pluginName );
			if ( load( test ) ) {
				break;
			}
		}
	}
	HHuginn::value_t package;
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
			throw HHuginn::HHuginnRuntimeException( e.what(), runtime_->file_id(), position_ );
		}
	}
	return ( package );
	M_EPILOG
}

HHuginn::value_t HPackageFactory::load_module( HRuntime* runtime_, HHuginn::paths_t const& paths_, yaal::hcore::HString const& name_, int position_ ) {
	M_PROLOG
	static HRegex const re( "([^.])[.]([^.])" );
	HString name( re.replace( re.replace( name_, "$1/$2" ), "$1/$2" ) );
	name.append( ".hgn" );
	HString path;
	auto acquire_path = []( yaal::hcore::HString module ) {
		substitute_environment( module, ENV_SUBST_MODE::RECURSIVE );
		try {
			if ( filesystem::is_regular_file( module ) ) {
				return ( module );
			}
		} catch ( filesystem::HFileSystemException const& ) {
		}
		return ( hcore::HString() );
	};
	if ( filesystem::is_relative( name ) ) {
		HString test;
		for ( HString const& p : paths_ ) {
			if ( p.is_empty() ) {
				continue;
			}
			test.assign( p ).append( filesystem::path::SEPARATOR ).append( name );
			if ( ! ( path = acquire_path( test ) ).is_empty() ) {
				break;
			}
		}
	} else {
		path = acquire_path( name );
	}
	return (
		! path.is_empty()
			? compile_module( runtime_, paths_, path, name_, position_ )
			: HHuginn::value_t()
	);
	M_EPILOG
}

HHuginn::value_t HPackageFactory::compile_module(
	HRuntime* contextRuntime_,
	HHuginn::paths_t const& paths_,
	yaal::hcore::HString const& path_,
	yaal::hcore::HString const& name_,
	int position_
) {
	M_PROLOG
	HHuginn& contextHuginn( *contextRuntime_->huginn() );
	HHuginn loader( contextRuntime_ );
	HRuntime& loaderRuntime( const_cast<HRuntime&>( loader.runtime() ) );
	HFile src( path_, HFile::OPEN::READING );
	loaderRuntime.copy_text( *contextRuntime_ );
	loader._compiler->_isModule = true;
	loader.load( src );
	loader.preprocess();
	M_ASSERT( static_cast<int>( contextHuginn._sources.get_size() - 1 ) == contextHuginn._compiler->_fileId );
	loader._compiler->_fileId = contextHuginn._compiler->_fileId;
	loader._sources.insert( loader._sources.begin(), contextHuginn._sources.begin(), contextHuginn._sources.end() );
	if ( ! ( loader.parse() && loader.compile( paths_, contextRuntime_->compiler_setup() ) ) ) {
		contextRuntime_->fix_references();
		throw HHuginn::HHuginnRuntimeException( loader.error_message(), contextHuginn._compiler->_fileId, position_ );
	}
	contextHuginn._compiler->_fileId = loader._compiler->_fileId;
	loader._state = HHuginn::STATE::PARSED;
	HHuginn::class_t c( loaderRuntime.make_package( name_, *contextRuntime_ ) );
	contextRuntime_->copy_text( loaderRuntime );
	contextHuginn.register_class( c );
	contextHuginn._sources = yaal::move( loader._sources );
	huginn::HThread t( contextRuntime_, hcore::HThread::get_current_thread_id() );
	return ( contextRuntime_->object_factory()->create_object( c.raw(), c->get_defaults( &t, position_ ) ) );
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

HPackage::HPackage( HHuginn::HClass* class_ )
	: HHuginn::HValue( class_ )
	, _exceptionClass( class_exception( class_ ) ) {
	return;
}

HHuginn::HClass const* HPackage::exception_class( void ) const {
	return ( _exceptionClass.raw() );
}

}

}

}

