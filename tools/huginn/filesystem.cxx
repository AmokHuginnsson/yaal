/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "stream.hxx"
#include "time.hxx"
#include "filestat.hxx"
#include "objectfactory.hxx"
#include "directoryscan.hxx"
#include "helper.hxx"
#include "enumeration.hxx"
#include "exception.hxx"
#include "hcore/hfile.hxx"
#include "hcore/safe_int.hxx"
#include "packagefactory.hxx"
#include "tools/filesystem.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::filesystem;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

typedef yaal::hcore::HString (*str_transform_func_t)( yaal::hcore::HString const& );

class HFileSystem : public HPackage {
	HHuginn::class_t _directoryScanClass;
	HHuginn::class_t _fileStatClass;
	enumeration::HEnumerationClass::ptr_t _openModeClass;
	enumeration::HEnumerationClass::ptr_t _fileTypeClass;
public:
	HFileSystem( huginn::HClass* class_ )
		: HPackage( class_ )
		, _directoryScanClass(
			add_class_as_type_reference(
				class_,
				HDirectoryScan::get_class( class_->runtime(), class_ )
			)
		)
		, _fileStatClass(
			add_class_as_type_reference(
				class_,
				HFileStat::get_class( class_->runtime(), class_ )
			)
		)
		, _openModeClass(
			add_enumeration_as_member(
				class_,
				enumeration::create_class(
					class_->runtime(),
					"OPEN_MODE",
					enumeration::descriptions_t{
						{ "READ", "Open file for reading only.", safe_int::cast<int>( HFile::OPEN::READING.value() ) },
						{ "WRITE", "Open file for writing while truncating existing content.", safe_int::cast<int>( HFile::OPEN::WRITING.value() ) },
						{ "APPEND", "Open file for appending, writing begins at end of file.", safe_int::cast<int>( HFile::OPEN::APPEND.value() ) },
					},
					"The `OPEN_MODE` is set of possible modes used for opening the files.",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"set of possible modes used for opening the files."
			)
		)
		, _fileTypeClass(
			add_enumeration_as_member(
				class_,
				enumeration::create_class(
					class_->runtime(),
					"FILE_TYPE",
					enumeration::descriptions_t{
						{ "REGULAR",          "A regular file.", static_cast<int>( FILE_TYPE::REGULAR ) },
						{ "DIRECTORY",        "A directory.", static_cast<int>( FILE_TYPE::DIRECTORY ) },
						{ "SYMBOLIC_LINK",    "A symbolic link", static_cast<int>( FILE_TYPE::SYMBOLIC_LINK ) },
						{ "SOCKET",           "A UNIX socket file.", static_cast<int>( FILE_TYPE::SOCKET ) },
						{ "FIFO",             "A FIFO.", static_cast<int>( FILE_TYPE::FIFO ) },
						{ "CHARACTER_DEVICE", "A character device.", static_cast<int>( FILE_TYPE::CHARACTER_DEVICE ) },
						{ "BLOCK_DEVICE",     "A block device.", static_cast<int>( FILE_TYPE::BLOCK_DEVICE ) }
					},
					"The `FILE_TYPE` is set of possible file types.",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"set of all possible file types."
			)
		) {
		return;
	}
	static HHuginn::value_t open( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HFileSystem*>( object_->raw() )->do_open( thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t current_working_directory( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "FileSystem.current_working_directory", values_, 0, 0, thread_, position_ );
		HObjectFactory& of( thread_->object_factory() );
		HHuginn::value_t v( of.none_value() );
		try {
			hcore::HString cwd( filesystem::current_working_directory() );
			v = thread_->object_factory().create_string( yaal::move( cwd ) );
		} catch ( HFileSystemException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t set_working_directory( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.set_working_directory", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		try {
			filesystem::chdir( get_string( values_[0] ) );
		} catch ( HFSItemException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t rename( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.rename", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		try {
			filesystem::rename( get_string( values_[0] ), get_string( values_[1] ) );
		} catch ( HFileSystemException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t remove( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.remove", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		try {
			filesystem::remove( get_string( values_[0] ) );
		} catch ( HFileSystemException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t path_transform( char const* name_, str_transform_func_t pathTransformFunc_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( name_, values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			v = thread_->object_factory().create_string( pathTransformFunc_( get_string( values_[0] ) ) );
		} catch ( HFileSystemException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t chmod( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.chmod", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::INTEGER }, thread_, position_ );
		try {
			int long long mode( get_integer( values_[1] ) );
			if ( ( mode < 0 ) || ( mode > 07777 ) ) {
				throw HHuginn::HHuginnRuntimeException( "Bad mode: "_ys.append( mode ), thread_->current_frame()->file_id(), position_ );
			}
			filesystem::chmod( get_string( values_[0] ), static_cast<u32_t>( mode ) );
		} catch ( HFileSystemException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t list_directory( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.list_directory", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
		try {
			v = thread_->object_factory().create<HDirectoryScan>( fsc->_directoryScanClass.raw(), get_string( values_[0] ) );
		} catch ( HFSItemException const& e ) {
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t stat( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.stat", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
		return (
			thread_->object_factory().create<HFileStat>(
				fsc->_fileStatClass.raw(),
				fsc->_fileTypeClass.raw(),
				fsc->exception_class(),
				get_string( values_[0] )
			)
		);
		M_EPILOG
	}
	static HHuginn::value_t exists( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.exists", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		return ( thread_->runtime().boolean_value( filesystem::exists( get_string( values_[0] ) ) ) );
		M_EPILOG
	}
	static HHuginn::value_t update_times( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HObjectFactory& of( thread_->object_factory() );
		verify_signature_by_class( "FileSystem.update_times", values_, { of.string_class(), of.time_class(), of.time_class() }, thread_, position_ );
		try {
			filesystem::update_times(
				get_string( values_[0] ),
				static_cast<HTime*>( values_[1].raw() )->value(),
				static_cast<HTime*>( values_[2].raw() )->value()
			);
		} catch ( hcore::HException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t disk_usage( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.disk_usage", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		system::HResourceInfo ri;
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			ri = system::get_disk_space_info( get_string( values_[0] ) );
			HHuginn::values_t data;
			HObjectFactory& of( *thread_->runtime().object_factory() );
			data.emplace_back( of.create_integer( ri.total() ) );
			data.emplace_back( of.create_integer( ri.free() ) );
			data.emplace_back( of.create_integer( ri.available() ) );
			v = of.create_tuple( yaal::move( data ) );
		} catch ( hcore::HException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t glob( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.glob", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		filesystem::paths_t paths( filesystem::glob( get_string( values_[0] ) ) );
		HHuginn::values_t data;
		HObjectFactory& of( *thread_->runtime().object_factory() );
		for ( filesystem::path_t& path : paths ) {
			data.emplace_back( of.create_string( yaal::move( path ) ) );
		}
		return ( of.create_list( yaal::move( data ) ) );
		M_EPILOG
	}
private:
	HHuginn::value_t do_open( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "FileSystem.open";
		HObjectFactory& of( thread_->object_factory() );
		verify_signature_by_class( name, values_, { of.string_class(), _openModeClass->enumeral_class() }, thread_, position_ );
		HEnumeral::value_type val( get_enumeral( values_[1] ) );
		HFile::open_t openMode( HFile::OPEN::READING );
		if ( val == safe_int::cast<int>( HFile::OPEN::WRITING.value() ) ) {
			openMode = HFile::OPEN::WRITING;
		} else if ( val == safe_int::cast<int>( HFile::OPEN::APPEND.value() ) ) {
			openMode = HFile::OPEN::APPEND | HFile::OPEN::WRITING;
		}
		HStreamInterface::ptr_t stream( make_pointer<HFile>( get_string( values_[0] ), openMode ) );
		HFile* f( static_cast<HFile*>( stream.raw() ) );
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( f->is_opened() ) {
			v = of.create<HStream>( of.stream_class(), stream );
		} else {
			thread_->raise( exception_class(), f->get_error(), position_ );
		}
		return ( v );
		M_EPILOG
	}
};

namespace package_factory {

class HFileSystemCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} filesystemCreator;

HPackageCreatorInterface::HInstance HFileSystemCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"FileSystem",
			"The `FileSystem` package provides interface to various file system queries and operations.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "open",                      runtime_->create_method( &HFileSystem::open ),    "( *path*, *mode* ) - open file under given *path* in the attached file system, using specified (i/o) *mode*" },
		{ "rename",                    runtime_->create_method( &HFileSystem::rename ),  "( *oldPath*, *newPath* ) - rename or move file from *oldPath* to *newPath* in attached file system", },
		{ "remove",                    runtime_->create_method( &HFileSystem::remove ),  "( *path* ) - remove file with given *path* from attached file system" },
		{ "readlink",                  runtime_->create_method( &HFileSystem::path_transform, "FileSystem.readlink", &filesystem::readlink ), "( *path* ) - get resolved symbolic links or canonical file name for given *path*" },
		{ "basename",                  runtime_->create_method( &HFileSystem::path_transform, "FileSystem.basename", &filesystem::basename ), "( *path* ) - strip directory from filename for given *path*" },
		{ "dirname",                   runtime_->create_method( &HFileSystem::path_transform, "FileSystem.dirname",  &filesystem::dirname ),  "( *path* ) - strip last component from file name for given *path*" },
		{ "chmod",                     runtime_->create_method( &HFileSystem::chmod ),   "( *path*, *mode* ) - change file mode bits for file *path* to new mode *mode*" },
		{ "stat",                      runtime_->create_method( &HFileSystem::stat ),    "( *path* ) - get metadata information for file given by *path*" },
		{ "exists",                    runtime_->create_method( &HFileSystem::exists ),  "( *path* ) - tell if given *path* exists, return false for broken links" },
		{ "glob",                      runtime_->create_method( &HFileSystem::glob ),    "( *pattern* ) - find pathnames matching a *pattern*" },
		{ "disk_usage",                runtime_->create_method( &HFileSystem::disk_usage ),   "( *mountPoint* ) - get disk usage statistics for the file system mounted at *mountPoint*" },
		{ "update_times",              runtime_->create_method( &HFileSystem::update_times ), "( *path*, *modTime*, *accessTime* ) - update modification (*modTime*) and access (*accessTime*) times of the file given by *path*" },
		{ "list_directory",            runtime_->create_method( &HFileSystem::list_directory ),        "( *path* ) - list content of the directory given by *path*" },
		{ "set_working_directory",     runtime_->create_method( &HFileSystem::set_working_directory ), "( *path* ) - change working directory to given *path*" },
		{ "current_working_directory", runtime_->create_method( &HFileSystem::current_working_directory ), "get current working directory path" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HFileSystem>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "FileSystem", &filesystemCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

