/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "subprocess.hxx"
#include "runtime.hxx"
#include "stream.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "stream.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HSubprocessClass : public HHuginn::HClass {
	HHuginn::class_t _streamClass;
public:
	HSubprocessClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "Subprocess" ),
			nullptr,
			"The `Subprocess` class represents a system process spawned from this interpreter instance. It allows for a basic process life management."
		)
		, _streamClass( HStream::get_class( runtime_ ) ) {
		HHuginn::field_definitions_t fd{
			{ "is_alive", runtime_->create_method( this, &HSubprocess::is_alive ), "tell if given subprocess is alive and running" },
			{ "kill",     runtime_->create_method( this, &HSubprocess::kill ),     "kill this sub-process" },
			{ "get_pid",  runtime_->create_method( this, &HSubprocess::get_pid ),  "get operating system Process IDentification-number of this subprocess" },
			{ "wait",     runtime_->create_method( this, &HSubprocess::wait ),     "wait for this subprocess to finish is execution and return its exit status" },
			{ "in",       runtime_->create_method( this, &HSubprocess::stream, "Subprocess.in", &HPipedChild::stream_in ),   "standard input stream of a subprocess" },
			{ "out",      runtime_->create_method( this, &HSubprocess::stream, "Subprocess.out", &HPipedChild::stream_out ), "standard output stream of a subprocess" },
			{ "err",      runtime_->create_method( this, &HSubprocess::stream, "Subprocess.err", &HPipedChild::stream_err ), "standard error stream of a subprocess" }
		};
		redefine( nullptr, fd );
		return;
	}
	HHuginn::HClass const* stream_class( void ) const {
		M_PROLOG
		return ( _streamClass.raw() );
		M_EPILOG
	}
};

HSubprocess::HSubprocess( HHuginn::HClass const* class_, HHuginn::values_t& argv_ )
	: HHuginn::HValue( class_ )
	, _pipedChild() {
	HPipedChild::argv_t argv;
	hcore::HString program( get_string( argv_[0] ) );
	for ( int i( 1 ), C( static_cast<int>( argv_.get_size() ) ); i < C; ++ i ) {
		argv.push_back( get_string( argv_[i] ) );
	}
	_pipedChild.spawn( program, argv );
	return;
}

HHuginn::value_t HSubprocess::is_alive(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	M_PROLOG
	char const name[] = "Subprocess.is_alive";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	return ( thread_->runtime().object_factory()->create_boolean( o->_pipedChild.is_running() ) );
	M_EPILOG
}

HHuginn::value_t HSubprocess::kill(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	M_PROLOG
	char const name[] = "Subprocess.kill";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	HPipedChild::STATUS s( o->_pipedChild.finish() );
	return ( thread_->runtime().object_factory()->create_integer( s.value ) );
	M_EPILOG
}

HHuginn::value_t HSubprocess::get_pid(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	M_PROLOG
	char const name[] = "Subprocess.get_pid";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	return ( thread_->runtime().object_factory()->create_integer( o->_pipedChild.get_pid() ) );
	M_EPILOG
}

HHuginn::value_t HSubprocess::wait(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	M_PROLOG
	verify_signature( "Subprocess.wait", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	int waitFor( static_cast<int>( get_integer( values_[0] ) ) );
	if ( waitFor < 0 ) {
		throw HHuginn::HHuginnRuntimeException( "invalid wait time: "_ys.append( waitFor ), thread_->current_frame()->file_id(), position_ );
	}
	HPipedChild::STATUS s( o->_pipedChild.finish( waitFor ) );
	return ( thread_->runtime().object_factory()->create_integer( s.value ) );
	M_EPILOG
}

HHuginn::value_t HSubprocess::stream(
	char const* name_,
	stream_getter_t streamGetter_,
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	M_PROLOG
	verify_arg_count( name_, values_, 0, 0, thread_, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	HSubprocessClass const* c( static_cast<HSubprocessClass const*>( o->HHuginn::HValue::get_class() ) );
	HHuginn::HClass const* sc( c->stream_class() );
	return ( thread_->object_factory().create<HStream>( sc, (o->_pipedChild.*streamGetter_)() ) );
	M_EPILOG
}

HHuginn::class_t HSubprocess::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HSubprocessClass>(
							runtime_,
							typeId_
						)
					);
				}
			)
		)
	);
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HSubprocess::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on Subprocess.", thread_->current_frame()->file_id(), position_ );
	M_EPILOG
}

}

}

}

