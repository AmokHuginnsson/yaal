/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  filesystem.cxx - this file is integral part of `yaal' project.

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
#include "exception.hxx"
#include "hcore/hfile.hxx"
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
	struct OPERATIONS {
		static int const READING = 1;
		static int const WRITING = 2;
	};
	HHuginn::class_t _streamClass;
	HHuginn::class_t _directoryScanClass;
	HHuginn::class_t _timeClass;
	HHuginn::class_t _fileStatClass;
	HHuginn::class_t _exceptionClass;
public:
	HFileSystem( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _streamClass( HStream::get_class( class_->runtime() ) )
		, _directoryScanClass( HDirectoryScan::get_class( class_->runtime() ) )
		, _timeClass( huginn::HTime::get_class( class_->runtime() ) )
		, _fileStatClass( HFileStat::get_class( class_->runtime() ) )
		, _exceptionClass(
			exception::create_class(
				HHuginn::ACCESS::PACKAGE,
				class_->runtime(),
				"FileSystemException",
				"The `FileSystemException` exception type for `FileSystem` package."
			)
		) {
		return;
	}
	static HHuginn::value_t open( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HFileSystem*>( object_->raw() )->do_open( thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t reading( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "FileSystem.reading", values_, 0, 0, thread_, position_ );
		return ( thread_->object_factory().create_integer( OPERATIONS::READING + 0 ) );
		M_EPILOG
	}
	static HHuginn::value_t writing( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "FileSystem.writing", values_, 0, 0, thread_, position_ );
		return ( thread_->object_factory().create_integer( OPERATIONS::WRITING + 0 ) );
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
		return ( thread_->object_factory().create<HFileStat>( fsc->_fileStatClass.raw(), fsc->_exceptionClass.raw(), fsc->_timeClass.raw(), get_string( values_[0] ) ) );
		M_EPILOG
	}
private:
	HHuginn::value_t do_open( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "FileSystem.open";
		verify_signature( name, values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::INTEGER }, thread_, position_ );
		int operation( static_cast<int>( get_integer( values_[1] ) ) );
		if ( ( operation != OPERATIONS::READING ) && ( operation != OPERATIONS::WRITING ) ) {
			throw HHuginn::HHuginnRuntimeException(
				HString( name ).append( "(): second argument must be either FileSystem.reading() or FileSystem.writing()" ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		HStreamInterface::ptr_t stream(
			make_pointer<HFile>(
				get_string( values_[0] ),
				operation == OPERATIONS::READING ? HFile::OPEN::READING : HFile::OPEN::WRITING
			)
		);
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
			nullptr,
			HHuginn::field_definitions_t{
				{ "open",                      runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::open, _1, _2, _3, _4 ) ),    "( *path*, *mode* ) - open file under given *path* in the attached file system, using specified (i/o) *mode*" },
				{ "reading",                   runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::reading, _1, _2, _3, _4 ) ), "a mode for *.open()* method, used to open files for reading" },
				{ "writing",                   runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::writing, _1, _2, _3, _4 ) ), "a mode for *.open()* method, used to open files for writing" },
				{ "rename",                    runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::rename, _1, _2, _3, _4 ) ),  "( *oldPath*, *newPath* ) - rename or move file from *oldPath* to *newPath* in attached file system", },
				{ "remove",                    runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::remove, _1, _2, _3, _4 ) ),  "( *path* ) - remove file with given *path* from attached file system" },
				{ "readlink",                  runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::path_transform, "FileSystem.readlink", &filesystem::readlink, _1, _2, _3, _4 ) ), "( *path* ) - get resolved symbolic links or canonical file name for given *path*" },
				{ "basename",                  runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::path_transform, "FileSystem.basename", &filesystem::basename, _1, _2, _3, _4 ) ), "( *path* ) - strip directory from filename for given *path*" },
				{ "dirname",                   runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::path_transform, "FileSystem.dirname", &filesystem::dirname, _1, _2, _3, _4 ) ),   "( *path* ) - strip last component from file name for given *path*" },
				{ "chmod",                     runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::chmod, _1, _2, _3, _4 ) ),   "( *path*, *mode* ) - change file mode bits for file *path* to new mode *mode*" },
				{ "dir",                       runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::dir, _1, _2, _3, _4 ) ),     "( *path* ) - list content of the directory given by *path*" },
				{ "stat",                      runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::stat, _1, _2, _3, _4 ) ),    "( *path* ) - get metadata information for file given by *path*" },
				{ "current_working_directory", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::current_working_directory, _1, _2, _3, _4 ) ), "get current working directory path" }
			},
			"The `FileSystem` package provides interface to various file system queries and operations."
		)
	);
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

