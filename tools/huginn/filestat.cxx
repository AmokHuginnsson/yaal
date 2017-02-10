/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  filestat.cxx - this file is integral part of `yaal' project.

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
#include "filestat.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "time.hxx"
#include "objectfactory.hxx"
#include "value_builtin.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::filesystem;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HFileStat::HFileStat( HHuginn::HClass const* class_, HHuginn::HClass const* exceptionClass_, HHuginn::HClass const* timeClass_, filesystem::path_t const& path_ )
	: HValue( class_ )
	, _exceptionClass( exceptionClass_ )
	, _timeClass( timeClass_ )
	, _path( path_ ) {
	return;
}

HHuginn::value_t HFileStat::id( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.id", values_, 0, 0, position_ );
	HFileStat* o( static_cast<HFileStat*>( object_->raw() ) );
	HFSItem fi( o->_path );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = thread_->object_factory().create_integer( static_cast<HHuginn::HInteger::value_type>( fi.id() ) );
	} catch ( HFSItemException const& e ) {
		thread_->raise( o->_exceptionClass, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFileStat::name( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.name", values_, 0, 0, position_ );
	HFileStat* o( static_cast<HFileStat*>( object_->raw() ) );
	HFSItem fi( o->_path );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		fi.id();
		v = thread_->object_factory().create_string( fi.get_path() );
	} catch ( HFSItemException const& e ) {
		thread_->raise( o->_exceptionClass, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFileStat::size( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.size", values_, 0, 0, position_ );
	HFileStat* o( static_cast<HFileStat*>( object_->raw() ) );
	HFSItem fi( o->_path );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = thread_->object_factory().create_integer( fi.size() );
	} catch ( HFSItemException const& e ) {
		thread_->raise( o->_exceptionClass, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFileStat::mode( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.mode", values_, 0, 0, position_ );
	HFileStat* o( static_cast<HFileStat*>( object_->raw() ) );
	HFSItem fi( o->_path );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = thread_->object_factory().create_integer( fi.get_permissions() );
	} catch ( HFSItemException const& e ) {
		thread_->raise( o->_exceptionClass, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFileStat::type( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.type", values_, 0, 0, position_ );
	HFileStat* o( static_cast<HFileStat*>( object_->raw() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = thread_->object_factory().create_string( filesystem::file_type_name( filesystem::file_type( o->_path ) ) );
	} catch ( HFileSystemException const& e ) {
		thread_->raise( o->_exceptionClass, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFileStat::user( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.user", values_, 0, 0, position_ );
	HFileStat* o( static_cast<HFileStat*>( object_->raw() ) );
	HFSItem fi( o->_path );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = thread_->object_factory().create_string( fi.get_user() );
	} catch ( HFSItemException const& e ) {
		thread_->raise( o->_exceptionClass, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFileStat::group( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.group", values_, 0, 0, position_ );
	HFileStat* o( static_cast<HFileStat*>( object_->raw() ) );
	HFSItem fi( o->_path );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = thread_->object_factory().create_string( fi.get_group() );
	} catch ( HFSItemException const& e ) {
		thread_->raise( o->_exceptionClass, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HFileStat::get_times( char const* name_, time_getter_t timeGetter_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat."_ys.append( name_ ), values_, 0, 0, position_ );
	HFileStat* o( static_cast<HFileStat*>( object_->raw() ) );
	HFSItem fi( o->_path );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = thread_->object_factory().create<huginn::HTime>( o->_timeClass, (fi.*timeGetter_)() );
	} catch ( HFSItemException const& e ) {
		thread_->raise( o->_exceptionClass, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::class_t HFileStat::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"FileStat",
			nullptr,
			HHuginn::field_definitions_t{
				{ "id",       runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::id, _1, _2, _3, _4 ) ),    "get file's identification number" },
				{ "name",     runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::name, _1, _2, _3, _4 ) ),  "get the name of the file" },
				{ "size",     runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::size, _1, _2, _3, _4 ) ),  "get file size in bytes" },
				{ "mode",     runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::mode, _1, _2, _3, _4 ) ),  "get file's access permissions" },
				{ "type",     runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::type, _1, _2, _3, _4 ) ),  "get the type of the file" },
				{ "user",     runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::user, _1, _2, _3, _4 ) ),  "get an user name of the file owner" },
				{ "group",    runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::group, _1, _2, _3, _4 ) ), "get a group name of the file owner" },
				{ "created",  runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::get_times, "created",  &HFSItem::created, _1, _2, _3, _4 ) ),  "get file's metadata modification `time`" },
				{ "changed",  runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::get_times, "changed",  &HFSItem::created, _1, _2, _3, _4 ) ),  "get file's metadata modification `time`" },
				{ "modified", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::get_times, "modified", &HFSItem::modified, _1, _2, _3, _4 ) ), "get file's data modification `time`" },
				{ "accessed", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HFileStat::get_times, "accessed", &HFSItem::accessed, _1, _2, _3, _4 ) ), "get file's last access `time`" }
			},
			"The `FileStat` class allows obtaining metadata information about various file system objects."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HFileStat::do_clone( huginn::HThread*, int ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on FileStat.", 0 );
}

}

}

}
