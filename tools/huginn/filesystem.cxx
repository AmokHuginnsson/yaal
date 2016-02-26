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
#include "thread.hxx"
#include "stream.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "hcore/hfile.hxx"
#include "packagefactory.hxx"
#include "tools/filesystem.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HFileSystem : public HHuginn::HObject {
	struct OPERATIONS {
		static int const READING = 1;
		static int const WRITTING = 2;
	};
	HHuginn::class_t _streamClass;
	HHuginn::class_t _exceptionClass;
public:
	HFileSystem( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _streamClass( HStream::get_class( class_->huginn() ) )
		, _exceptionClass( exception::create_class( class_->huginn(), "FileSystemException" ) ) {
		return;
	}
	static HHuginn::value_t open( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HFileSystem*>( object_->raw() )->do_open( thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t reading( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "FileSystem.reading", values_, 0, 0, position_ );
		return ( thread_->object_factory().create_integer( OPERATIONS::READING + 0 ) );
		M_EPILOG
	}
	static HHuginn::value_t writting( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "FileSystem.writting", values_, 0, 0, position_ );
		return ( thread_->object_factory().create_integer( OPERATIONS::WRITTING + 0 ) );
		M_EPILOG
	}
	static HHuginn::value_t current_working_directory( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "FileSystem.current_working_directory", values_, 0, 0, position_ );
		return ( thread_->object_factory().create_string( filesystem::current_working_directory() ) );
		M_EPILOG
	}
	static HHuginn::value_t rename( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "FileSystem.rename";
		verify_arg_count( name, values_, 2, 2, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, false, position_ );
		verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, false, position_ );
		filesystem::rename( get_string( values_[0] ), get_string( values_[1] ) );
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t remove( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "FileSystem.remove";
		verify_arg_count( name, values_, 1, 1, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
		filesystem::remove( get_string( values_[0] ) );
		return ( *object_ );
		M_EPILOG
	}
private:
	HHuginn::value_t do_open( huginn::HThread* thread_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "FileSystem.open";
		verify_arg_count( name, values_, 2, 2, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, false, position_ );
		verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
		int operation( static_cast<int>( get_integer( values_[1] ) ) );
		if ( ( operation != OPERATIONS::READING ) && ( operation != OPERATIONS::WRITTING ) ) {
			throw HHuginn::HHuginnRuntimeException(
				HString( name ).append( "(): second argument must be either FileSystem.reading() or FileSystem.writting()" ),
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
		HHuginn::value_t v( thread_->huginn().none_value() );
		if ( f->is_opened() ) {
			v = make_pointer<HStream>( _streamClass.raw(), stream );
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
	virtual HHuginn::value_t do_new_instance( HHuginn* );
} filesystemCreator;

HHuginn::value_t HFileSystemCreator::do_new_instance( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::class_t c(
		huginn_->create_class(
			"FileSystem",
			nullptr,
			HHuginn::field_definitions_t{
				{ "open",                      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::open, _1, _2, _3, _4 ) ) },
				{ "reading",                   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::reading, _1, _2, _3, _4 ) ) },
				{ "writting",                  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::writting, _1, _2, _3, _4 ) ) },
				{ "rename",                    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::rename, _1, _2, _3, _4 ) ) },
				{ "remove",                    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::remove, _1, _2, _3, _4 ) ) },
				{ "current_working_directory", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HFileSystem::current_working_directory, _1, _2, _3, _4 ) ) }
			}
		)
	);
	huginn_->register_class( c );
	return ( make_pointer<HFileSystem>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactoryInstance::get_instance();
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

