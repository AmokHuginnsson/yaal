/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	HHuginn::class_t _streamClass;
	HHuginn::class_t _subprocessClass;
	HHuginn::class_t _exceptionClass;
public:
	HOperatingSystem( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _streamClass( HStream::get_class( class_->runtime() ) )
		, _subprocessClass( HSubprocess::get_class( class_->runtime() ) )
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t env( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "OperatingSystem.env", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HUTF8String utf8(  get_string( values_[0] ) );
		char const* val( ::getenv( utf8.c_str() ) );
		HRuntime& rt( thread_->runtime() );
		return ( val ? rt.object_factory()->create_string( val ) : rt.none_value() );
		M_EPILOG
	}
	static HHuginn::value_t getpid( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "OperatingSystem.getpid", values_, 0, 0, thread_, position_ );
		return ( thread_->object_factory().create_integer( system::getpid() ) );
		M_EPILOG
	}
	static HHuginn::value_t getuid( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "OperatingSystem.getuid", values_, 0, 0, thread_, position_ );
		return ( thread_->object_factory().create_integer( system::get_user_id() ) );
		M_EPILOG
	}
	static HHuginn::value_t getgid( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "OperatingSystem.getgid", values_, 0, 0, thread_, position_ );
		return ( thread_->object_factory().create_integer( system::get_group_id() ) );
		M_EPILOG
	}
	static HHuginn::value_t exec( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		static int const MAX_ARG_COUNT( 65535 );
		char const name[] = "OperatingSystem.exec";
		verify_arg_count( name, values_, 1, MAX_ARG_COUNT, thread_, position_ );
		int argc( static_cast<int>( values_.get_size() ) );
		HResource<char*[]> argvHolder( new char*[argc + 1] );
		char** argv( argvHolder.get() );
		argv[argc] = nullptr;
		typedef yaal::hcore::HArray<HUTF8String> utf8_strings_t;
		utf8_strings_t argvDataHolder;
		argvDataHolder.reserve( argc );
		for ( int i( 0 ); i < argc; ++ i ) {
			verify_arg_type( name, values_, i, HHuginn::TYPE::STRING, argc == 1 ? ARITY::UNARY : ARITY::MULTIPLE, thread_, position_ );
			argvDataHolder.emplace_back( get_string( values_[i] ) );
			argv[i] = const_cast<char*>( argvDataHolder.back().c_str() );
		}
		::execvp( argv[0], argv );
		thread_->raise( static_cast<HOperatingSystem*>( object_->raw() )->_exceptionClass.raw(), strerror( errno ), position_ );
		return ( thread_->runtime().none_value() );
		M_EPILOG
	}
	static HHuginn::value_t exit( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "OperatingSystem.exit", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		HHuginn::HInteger::value_type val( get_integer( values_[0] ) );
		::exit( static_cast<int>( val ) );
		return ( thread_->runtime().none_value() );
		M_EPILOG
	}
	static HHuginn::value_t spawn( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "OperatingSystem.spawn";
		verify_arg_count( name, values_, 1, meta::max_signed<int short>::value, thread_, position_ );
 		int argc( static_cast<int>( values_.get_size() ) );
		for ( int i( 0 ); i < argc; ++ i ) {
			verify_arg_type( name, values_, i, HHuginn::TYPE::STRING, argc == 1 ? ARITY::UNARY : ARITY::MULTIPLE, thread_, position_ );
		}
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			HOperatingSystem* o( static_cast<HOperatingSystem*>( object_->raw() ) );
			v = thread_->object_factory().create<HSubprocess>( o->_subprocessClass.raw(), values_ );
		} catch ( HException const& e ) {
			thread_->raise( static_cast<HOperatingSystem*>( object_->raw() )->_exceptionClass.raw(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t stream( char const* name_, yaal::hcore::HStreamInterface* stream_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 0, 0, thread_, position_ );
		HOperatingSystem* o( static_cast<HOperatingSystem*>( object_->raw() ) );
		return ( thread_->object_factory().create<HStream>( o->_streamClass.raw(), make_pointer<HSynchronizedStream>( *stream_ ) ) );
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
			"The `OperatingSystem` package provides an interface to various operating system functionalities.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "env",    runtime_->create_method( &HOperatingSystem::env ),    "( *name* ) - get value of an environment variable named *name*" },
		{ "getpid", runtime_->create_method( &HOperatingSystem::getpid ), "get Huginn's interpreter process id" },
		{ "getuid", runtime_->create_method( &HOperatingSystem::getuid ), "get Huginn's interpreter process effective user id" },
		{ "getgid", runtime_->create_method( &HOperatingSystem::getgid ), "get Huginn's interpreter process effective group id" },
		{ "exec",   runtime_->create_method( &HOperatingSystem::exec ),   "( *prog*, *args*... ) - replace current process space with running image of *prog* providing it with *args* arguments" },
		{ "exit",   runtime_->create_method( &HOperatingSystem::exit ),   "( *status* ) - exit the interpreter with the *status*" },
		{ "spawn",  runtime_->create_method( &HOperatingSystem::spawn ),  "( *prog*, *args*... ) - start a subprocess *prog* providing it with *args* arguments" },
		{ "stdin",  runtime_->create_method( &HOperatingSystem::stream, "OperatingSystem.stdin", &runtime_->huginn()->input_stream() ),   "get access to interpreter's standard input stream" },
		{ "stdout", runtime_->create_method( &HOperatingSystem::stream, "OperatingSystem.stdout", &runtime_->huginn()->output_stream() ), "get access to interpreter's standard output stream" },
		{ "stderr", runtime_->create_method( &HOperatingSystem::stream, "OperatingSystem.stderr", &runtime_->huginn()->error_stream() ),  "get access to interpreter's standard error stream" },
		{ "stdlog", runtime_->create_method( &HOperatingSystem::stream, "OperatingSystem.stdlog", &runtime_->huginn()->log_stream() ),    "get access to interpreter's standard log stream" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HOperatingSystem>( c.raw() ) );
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

