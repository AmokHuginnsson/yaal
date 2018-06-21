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

class HFileSystem : public HHuginn::HValue {
	HHuginn::class_t _streamClass;
	HHuginn::class_t _directoryScanClass;
	HHuginn::class_t _timeClass;
	HHuginn::class_t _fileStatClass;
	enumeration::HEnumerationClass::ptr_t _openModeClass;
	enumeration::HEnumerationClass::ptr_t _fileTypeClass;
	HHuginn::class_t _exceptionClass;
public:
	HFileSystem( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _streamClass( HStream::get_class( class_->runtime() ) )
		, _directoryScanClass( HDirectoryScan::get_class( class_->runtime(), class_ ) )
		, _timeClass( huginn::HTime::get_class( class_->runtime() ) )
		, _fileStatClass( HFileStat::get_class( class_->runtime(), class_ ) )
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
					HHuginn::VISIBILITY::PACKAGE
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
					HHuginn::VISIBILITY::PACKAGE
				),
				"set of all possible file types."
			)
		)
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t open( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HFileSystem*>( object_->raw() )->do_open( thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t current_working_directory( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "FileSystem.current_working_directory", values_, 0, 0, thread_, position_ );
		return ( thread_->object_factory().create_string( filesystem::current_working_directory() ) );
		M_EPILOG
	}
	static HHuginn::value_t rename( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.rename", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		try {
			filesystem::rename( get_string( values_[0] ), get_string( values_[1] ) );
		} catch ( HFileSystemException const& e ) {
			HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
			thread_->raise( fsc->_exceptionClass.raw(), e.what(), position_ );
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
			thread_->raise( fsc->_exceptionClass.raw(), e.what(), position_ );
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
			thread_->raise( fsc->_exceptionClass.raw(), e.what(), position_ );
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
			thread_->raise( fsc->_exceptionClass.raw(), e.what(), position_ );
		}
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t dir( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.dir", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
		try {
			v = thread_->object_factory().create<HDirectoryScan>( fsc->_directoryScanClass.raw(), get_string( values_[0] ) );
		} catch ( HFSItemException const& e ) {
			thread_->raise( fsc->_exceptionClass.raw(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t stat( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "FileSystem.stat", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		HFileSystem* fsc( static_cast<HFileSystem*>( object_->raw() ) );
		return (
			thread_->object_factory().create<HFileStat>(
				fsc->_fileStatClass.raw(),
				fsc->_fileTypeClass.raw(),
				fsc->_exceptionClass.raw(),
				fsc->_timeClass.raw(),
				get_string( values_[0] )
			)
		);
		M_EPILOG
	}
private:
	HHuginn::value_t do_open( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "FileSystem.open";
		verify_signature_by_class( name, values_, { thread_->object_factory().string_class(), _openModeClass->enumeral_class() }, thread_, position_ );
		HHuginn::HEnumeral::value_type val( get_enumeral( values_[1] ) );
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
			v = thread_->object_factory().create<HStream>( _streamClass.raw(), stream );
		} else {
			thread_->raise( _exceptionClass.raw(), f->get_error(), position_ );
		}
		return ( v );
		M_EPILOG
	}
};

namespace package_factory {

class HFileSystemCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} filesystemCreator;

HHuginn::value_t HFileSystemCreator::do_new_instance( HRuntime* runtime_ ) {
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
		{ "dirname",                   runtime_->create_method( &HFileSystem::path_transform, "FileSystem.dirname", &filesystem::dirname ),   "( *path* ) - strip last component from file name for given *path*" },
		{ "chmod",                     runtime_->create_method( &HFileSystem::chmod ),   "( *path*, *mode* ) - change file mode bits for file *path* to new mode *mode*" },
		{ "dir",                       runtime_->create_method( &HFileSystem::dir ),     "( *path* ) - list content of the directory given by *path*" },
		{ "stat",                      runtime_->create_method( &HFileSystem::stat ),    "( *path* ) - get metadata information for file given by *path*" },
		{ "current_working_directory", runtime_->create_method( &HFileSystem::current_working_directory ), "get current working directory path" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HFileSystem>( c.raw() ) );
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

