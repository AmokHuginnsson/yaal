/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  operatingsystem.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hsynchronizedstream.hxx"
#include "runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "stream.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "subprocess.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HOperatingSystem : public HHuginn::HValue {
	HHuginn::class_t _exceptionClass;
	HHuginn::class_t _streamClass;
	HHuginn::class_t _subprocessClass;
public:
	HOperatingSystem( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _exceptionClass( exception::create_class( class_->runtime(), "OperatingSystemException", "The `OperatingSystemException` exception type for `OperatingSystem` package." ) )
		, _streamClass( HStream::get_class( class_->runtime() ) )
		, _subprocessClass( HSubprocess::get_class( class_->runtime() ) ) {
		return;
	}
	static HHuginn::value_t env( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "OperatingSystem.env";
		verify_arg_count( name, values_, 1, 1, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
		char const* val( ::getenv( get_string( values_[0] ).raw() ) );
		HRuntime& rt( thread_->runtime() );
		return ( val ? rt.object_factory()->create_string( val ) : rt.none_value() );
		M_EPILOG
	}
	static HHuginn::value_t exec( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		static int const MAX_ARG_COUNT( 65535 );
		char const name[] = "OperatingSystem.exec";
		verify_arg_count( name, values_, 1, MAX_ARG_COUNT, position_ );
		int argc( static_cast<int>( values_.get_size() ) );
		HResource<char*[]> argvHolder( new char*[argc + 1] );
		char** argv( argvHolder.get() );
		argv[argc] = nullptr;
		for ( int i( 0 ); i < argc; ++ i ) {
			verify_arg_type( name, values_, i, HHuginn::TYPE::STRING, false, position_ );
			argv[i] = const_cast<char*>( get_string( values_[i] ).raw() );
		}
		::execvp( argv[0], argv );
		thread_->raise( static_cast<HOperatingSystem*>( object_->raw() )->_exceptionClass.raw(), strerror( errno ), position_ );
		return ( thread_->runtime().none_value() );
		M_EPILOG
	}
	static HHuginn::value_t exit( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "OperatingSystem.exit";
		verify_arg_count( name, values_, 1, 1, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, true, position_ );
		HHuginn::HInteger::value_type val( get_integer( values_[0] ) );
		::exit( static_cast<int>( val ) );
		return ( thread_->runtime().none_value() );
		M_EPILOG
	}
	static HHuginn::value_t spawn( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "OperatingSystem.spawn";
		verify_arg_count( name, values_, 1, meta::max_signed<int short>::value, position_ );
 		int argc( static_cast<int>( values_.get_size() ) );
		for ( int i( 0 ); i < argc; ++ i ) {
			verify_arg_type( name, values_, i, HHuginn::TYPE::STRING, argc == 1, position_ );
		}
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			HOperatingSystem* o( static_cast<HOperatingSystem*>( object_->raw() ) );
			v = make_pointer<HSubprocess>( o->_subprocessClass.raw(), values_ );
		} catch ( HException const& e ) {
			thread_->raise( static_cast<HOperatingSystem*>( object_->raw() )->_exceptionClass.raw(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t stream( char const* name_, yaal::hcore::HStreamInterface* stream_, huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 0, 0, position_ );
		HOperatingSystem* o( static_cast<HOperatingSystem*>( object_->raw() ) );
		return ( make_pointer<HStream>( o->_streamClass.raw(), make_pointer<HSynchronizedStream>( *stream_ ) ) );
		M_EPILOG
	}
};

namespace package_factory {

class HOperatingSystemCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} operatingsystemCreator;

HHuginn::value_t HOperatingSystemCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"OperatingSystem",
			nullptr,
			HHuginn::field_definitions_t{
				{ "env",    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::env, _1, _2, _3, _4 ) ), "( *name* ) - get value of an environment variable named *name*" },
				{ "exec",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::exec, _1, _2, _3, _4 ) ), "( *prog*, *args*... ) - replace current process space with running image of *prog* providing it with *args* arguments" },
				{ "exit",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::exit, _1, _2, _3, _4 ) ), "( *status* ) - exit the interpreter with the *status*" },
				{ "spawn",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::spawn, _1, _2, _3, _4 ) ), "( *prog*, *args*... ) - start a subprocess *prog* providing it with *args* arguments" },
				{ "stdin",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::stream, "OperatingSystem.stdin", &runtime_->huginn()->input_stream(), _1, _2, _3, _4 ) ), "get access to interpreter's standard input stream" },
				{ "stdout", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::stream, "OperatingSystem.stdout", &runtime_->huginn()->output_stream(), _1, _2, _3, _4 ) ), "get access to interpreter's standard output stream" },
				{ "stderr", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::stream, "OperatingSystem.stderr", &runtime_->huginn()->error_stream(), _1, _2, _3, _4 ) ), "get access to interpreter's standard error stream" },
				{ "stdlog", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HOperatingSystem::stream, "OperatingSystem.stdlog", &runtime_->huginn()->log_stream(), _1, _2, _3, _4 ) ), "get access to interpreter's standard log stream" }
			},
			"The `OperatingSystem` package provides an interface to various operating system functionalities."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( make_pointer<HOperatingSystem>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "OperatingSystem", &operatingsystemCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

