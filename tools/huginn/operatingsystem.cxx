/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hcore.hxx"
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

class HOperatingSystem : public HPackage {
	HHuginn::class_t _subprocessClass;
	enumeration::HEnumerationClass::ptr_t _resourceLimitTypeClass;
public:
	HOperatingSystem( huginn::HClass* class_ )
		: HPackage( class_ )
		, _subprocessClass( HSubprocess::get_class( class_->runtime(), class_ ) )
		, _resourceLimitTypeClass(
			add_enumeration_as_member(
				class_,
				enumeration::create_class(
					class_->runtime(),
					"RESOURCE_LIMIT_TYPE",
					enumeration::descriptions_t{
						{ "MEMORY_SIZE",   "This is the maximum size of the process's data segment specified in bytes", static_cast<int>( system::RESOURCE_LIMIT_TYPE::MEMORY_SIZE ) },
						{ "STACK_SIZE",    "This is the maximum size of the process stack, in bytes.", static_cast<int>( system::RESOURCE_LIMIT_TYPE::STACK_SIZE ) },
						{ "CPU_TIME",      "This is a limit, in seconds, on the amount of CPU time that the process can consume.", static_cast<int>( system::RESOURCE_LIMIT_TYPE::CPU_TIME ) },
						{ "OPEN_FILES",    "This specifies a value one greater than the maximum file descriptor number that can be opened by this process.", static_cast<int>( system::RESOURCE_LIMIT_TYPE::OPEN_FILES ) },
						{ "FILE_SIZE",     "This is the maximum size in bytes of files that the process may create.", static_cast<int>( system::RESOURCE_LIMIT_TYPE::FILE_SIZE ) },
						{ "PROCESS_COUNT", "This is a limit on the number of extant process for the real user ID of the calling process.", static_cast<int>( system::RESOURCE_LIMIT_TYPE::PROCESS_COUNT ) },
						{ "CORE_SIZE",     "This is the maximum size of a core file in bytes that the process may dump.", static_cast<int>( system::RESOURCE_LIMIT_TYPE::CORE_SIZE ) }
					},
					"The `RESOURCE_LIMIT_TYPE` is set of possible resource limit types.",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"set of all possible resource limit types."
			)
		) {
		return;
	}
	static HHuginn::value_t memory_size( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "OperatingSystem.memory_size", values_, 0, 0, thread_, position_ );
		system::HResourceInfo ri;
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			ri = system::get_memory_size_info();
			HHuginn::values_t data;
			HObjectFactory& of( *thread_->runtime().object_factory() );
			data.emplace_back( of.create_integer( ri.total() ) );
			data.emplace_back( of.create_integer( ri.free() ) );
			data.emplace_back( of.create_integer( ri.available() ) );
			v = of.create_tuple( yaal::move( data ) );
		} catch ( hcore::HException const& e ) {
			HOperatingSystem* osc( static_cast<HOperatingSystem*>( object_->raw() ) );
			thread_->raise( osc->exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t core_count( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "OperatingSystem.core_count", values_, 0, 0, thread_, position_ );
		system::HResourceInfo ri;
		int nCPU( 0 );
		try {
			nCPU = system::get_core_count_info();
		} catch ( hcore::HException const& e ) {
			HOperatingSystem* osc( static_cast<HOperatingSystem*>( object_->raw() ) );
			thread_->raise( osc->exception_class(), e.what(), position_ );
		}
		return ( thread_->object_factory().create_integer( nCPU ) );
		M_EPILOG
	}
	static HHuginn::value_t env( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "OperatingSystem.env", values_, 1, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		HUTF8String utf8(  get_string( values_[0] ) );
		char const* val( ::getenv( utf8.c_str() ) );
		HRuntime& rt( thread_->runtime() );
		return ( val ? rt.object_factory()->create_string( val ) : ( values_.get_size() > 1 ?  values_[1] : rt.none_value() ) );
		M_EPILOG
	}
	static HHuginn::value_t set_env( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "OperatingSystem.set_env", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		hcore::set_env( get_string( values_[0] ), get_string( values_[1] ) );
		return ( *object_ );
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
		thread_->raise( static_cast<HOperatingSystem*>( object_->raw() )->exception_class(), strerror( errno ), position_ );
		return ( thread_->runtime().none_value() );
		M_EPILOG
	}
	static HHuginn::value_t exit( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "OperatingSystem.exit", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		thread_->break_execution( HFrame::STATE::EXIT, values_[0]->clone( thread_, &values_[0], position_ ) );
		return ( thread_->runtime().none_value() );
		M_EPILOG
	}
	static HHuginn::value_t set_umask( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "OperatingSystem.set_umask", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		HInteger::value_type v( get_integer( values_[0] ) );
		if ( ( v < 0 ) || ( v > 07777 ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad umask value: "_ys.append( v ), thread_->current_frame()->file_id(), position_ );
		}
		system::set_umask( static_cast<system::mode_t>( v ) );
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t umask( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "OperatingSystem.umask", values_, 0, 0, thread_, position_ );
		return ( thread_->object_factory().create_integer( static_cast<HInteger::value_type>( system::get_umask() ) ) );
		M_EPILOG
	}
	static HHuginn::value_t get_limit( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HOperatingSystem* o( static_cast<HOperatingSystem*>( object_->raw() ) );
		verify_signature_by_class( "OperatingSystem.get_limit", values_, { o->_resourceLimitTypeClass->enumeral_class() }, thread_, position_ );
		HEnumeral::value_type val( get_enumeral( values_[0] ) );
		system::HResourceLimit rl( system::get_limit( static_cast<system::RESOURCE_LIMIT_TYPE>( val ) ) );
		HHuginn::values_t data;
		HObjectFactory& of( thread_->object_factory() );
		data.push_back( of.create_integer( rl.soft() ) );
		data.push_back( of.create_integer( rl.hard() ) );
		return ( of.create_tuple( yaal::move( data ) ) );
		M_EPILOG
	}
	static HHuginn::value_t set_limit( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HOperatingSystem* o( static_cast<HOperatingSystem*>( object_->raw() ) );
		HObjectFactory& of( thread_->object_factory() );
		verify_signature_by_class( "OperatingSystem.set_limit", values_, { o->_resourceLimitTypeClass->enumeral_class(), of.tuple_class() }, thread_, position_ );
		HEnumeral::value_type val( get_enumeral( values_[0] ) );
		HTuple::values_t& data( static_cast<HTuple*>( values_[1].raw() )->value() );
		if ( data.get_size() != 2 ) {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid resource limit shape: "_ys.append( data.get_size() ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		verify_signature(
			"OperatingSystem.set_limit( res, ( soft, hard ) ) - resource limit definition ( soft, hard ) -",
			data,
			{ HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER },
			thread_,
			position_
		);
		HInteger::value_type soft( get_integer( data[0] ) );
		HInteger::value_type hard( get_integer( data[1] ) );
		try {
			system::set_limit( static_cast<system::RESOURCE_LIMIT_TYPE>( val ), system::HResourceLimit( soft, hard ) );
		} catch ( hcore::HException const& e ) {
			thread_->raise( o->exception_class(), e.what(), position_ );
		}
		return ( *object_ );
		M_EPILOG
	}
	static HHuginn::value_t spawn( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "OperatingSystem.spawn";
		verify_arg_count( name, values_, 2, 6, thread_, position_ );
		int argc( static_cast<int>( values_.get_size() ) );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
		verify_arg_type( name, values_, 1, { HHuginn::TYPE::TUPLE, HHuginn::TYPE::LIST, HHuginn::TYPE::DEQUE }, ARITY::MULTIPLE, thread_, position_ );
		verify_arg_collection_value_type( name, values_, 1, ARITY::MULTIPLE, { HHuginn::TYPE::STRING }, UNIFORMITY::REQUIRED, thread_, position_ );
		hcore::HString program( get_string( values_[0] ) );
		HPipedChild::argv_t argv( get_strings( values_[1] ) );
		bool foreground( false );
		if ( argc >= 3 ) {
			verify_arg_type( name, values_, 2, HHuginn::TYPE::BOOLEAN, ARITY::MULTIPLE, thread_, position_ );
			foreground = get_boolean( values_[2] );
		}
		HObjectFactory& of( thread_->object_factory() );
		HHuginn::value_t stream[3];
		char const streamName[][8] = { "input", "output", "error" };
		for ( int i( 3 ); i < argc; ++ i ) {
			verify_arg_type( name, values_, i, classes_t{ of.stream_class(), of.none_class() }, ARITY::MULTIPLE, thread_, position_ );
			if ( values_[i]->get_class() != of.stream_class() ) {
				continue;
			}
			if ( static_cast<HStream*>( values_[i].raw() )->raw()->poll_type() == HStreamInterface::POLL_TYPE::EMULATED ) {
				throw HHuginn::HHuginnRuntimeException(
					"Invalid `"_ys.append( streamName[i - 3] ).append( "` stream type." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			stream[i - 3] = values_[i];
		}
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			HOperatingSystem* o( static_cast<HOperatingSystem*>( object_->raw() ) );
			v = thread_->object_factory().create<HSubprocess>(
				o->_subprocessClass.raw(),
				program,
				yaal::move( argv ),
				foreground,
				stream[0],
				stream[1],
				stream[2]
			);
		} catch ( hcore::HException const& e ) {
			thread_->raise( static_cast<HOperatingSystem*>( object_->raw() )->exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	typedef yaal::hcore::HStreamInterface& ( HHuginn::* stream_getter_t )( void );
	static HHuginn::value_t stream( char const* name_, stream_getter_t streamGetter_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 0, 0, thread_, position_ );
		HObjectFactory& of( thread_->object_factory() );
		HHuginn& h( *thread_->runtime().huginn() );
		return ( of.create<HStream>( of.stream_class(), make_pointer<HSynchronizedStream>( ( h.*streamGetter_ )() ) ) );
		M_EPILOG
	}
};

namespace package_factory {

class HOperatingSystemCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} operatingsystemCreator;

HPackageCreatorInterface::HInstance HOperatingSystemCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"OperatingSystem",
			"The `OperatingSystem` package provides an interface to various operating system functionalities.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "memory_size", runtime_->create_method( &HOperatingSystem::memory_size ), "get information about system virtual memory_size size" },
		{ "core_count",  runtime_->create_method( &HOperatingSystem::core_count ),  "get the number of CPUs in the system" },
		{ "env",       runtime_->create_method( &HOperatingSystem::env ),       "( *name* ) - get value of an environment variable named *name*" },
		{ "set_env",   runtime_->create_method( &HOperatingSystem::set_env ),   "( *name*, *value* ) - set *name* environment variable to *value* value" },
		{ "umask",     runtime_->create_method( &HOperatingSystem::umask ),     "get value of system's umask" },
		{ "set_umask", runtime_->create_method( &HOperatingSystem::set_umask ), "( *newUmask* ) - set value of system's umask to *newUmask*" },
		{ "get_limit", runtime_->create_method( &HOperatingSystem::get_limit ), "( *resourceLimitType* ) - get current value of given resource limit designated by given *resourceLimitType*" },
		{ "set_limit", runtime_->create_method( &HOperatingSystem::set_limit ), "( *resourceLimitType*, ( *soft*, *hard* ) ) - set new *soft* and *hard* limit for a resource designated by given *resourceLimitType*" },
		{ "getpid",    runtime_->create_method( &HOperatingSystem::getpid ),    "get Huginn's interpreter process id" },
		{ "getuid",    runtime_->create_method( &HOperatingSystem::getuid ),    "get Huginn's interpreter process effective user id" },
		{ "getgid",    runtime_->create_method( &HOperatingSystem::getgid ),    "get Huginn's interpreter process effective group id" },
		{ "exec",      runtime_->create_method( &HOperatingSystem::exec ),      "( *prog*, *args*... ) - replace current process space with running image of *prog* providing it with *args* arguments" },
		{ "exit",      runtime_->create_method( &HOperatingSystem::exit ),      "( *status* ) - exit the interpreter with the *status*" },
		{ "spawn",     runtime_->create_method( &HOperatingSystem::spawn ),     "( *prog*, *args*... ) - start a subprocess *prog* providing it with *args* arguments" },
		{ "stdin",     runtime_->create_method( &HOperatingSystem::stream,      "OperatingSystem.stdin",  &HHuginn::input_stream ),  "get access to interpreter's standard input stream" },
		{ "stdout",    runtime_->create_method( &HOperatingSystem::stream,      "OperatingSystem.stdout", &HHuginn::output_stream ), "get access to interpreter's standard output stream" },
		{ "stderr",    runtime_->create_method( &HOperatingSystem::stream,      "OperatingSystem.stderr", &HHuginn::error_stream ),  "get access to interpreter's standard error stream" },
		{ "stdlog",    runtime_->create_method( &HOperatingSystem::stream,      "OperatingSystem.stdlog", &HHuginn::log_stream ),    "get access to interpreter's standard log stream" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HOperatingSystem>( c.raw() ) };
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

