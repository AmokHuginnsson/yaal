/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

HHuginn::value_t HFileStat::id( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.id", values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HFileStat::name( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.name", values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HFileStat::size( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.size", values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HFileStat::mode( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.mode", values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HFileStat::type( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.type", values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HFileStat::user( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.user", values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HFileStat::group( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "FileStat.group", values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HFileStat::get_times( char const* name_, time_getter_t timeGetter_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 0, 0, thread_, position_ );
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
			"The `FileStat` class allows obtaining metadata information about various file system objects."
		)
	);
	HHuginn::field_definitions_t fd{
		{ "id",       runtime_->create_method( c.raw(), &HFileStat::id ),    "get file's identification number" },
		{ "name",     runtime_->create_method( c.raw(), &HFileStat::name ),  "get the name of the file" },
		{ "size",     runtime_->create_method( c.raw(), &HFileStat::size ),  "get file size in bytes" },
		{ "mode",     runtime_->create_method( c.raw(), &HFileStat::mode ),  "get file's access permissions" },
		{ "type",     runtime_->create_method( c.raw(), &HFileStat::type ),  "get the type of the file" },
		{ "user",     runtime_->create_method( c.raw(), &HFileStat::user ),  "get an user name of the file owner" },
		{ "group",    runtime_->create_method( c.raw(), &HFileStat::group ), "get a group name of the file owner" },
		{ "created",  runtime_->create_method( c.raw(), &HFileStat::get_times, "FileStat.created",  &HFSItem::created ),  "get file's metadata modification `time`" },
		{ "changed",  runtime_->create_method( c.raw(), &HFileStat::get_times, "FileStat.changed",  &HFSItem::created ),  "get file's metadata modification `time`" },
		{ "modified", runtime_->create_method( c.raw(), &HFileStat::get_times, "FileStat.modified", &HFSItem::modified ), "get file's data modification `time`" },
		{ "accessed", runtime_->create_method( c.raw(), &HFileStat::get_times, "FileStat.accessed", &HFSItem::accessed ), "get file's last access `time`" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HFileStat::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on FileStat.", thread_->current_frame()->file_id(), position_ );
}

}

}

}
