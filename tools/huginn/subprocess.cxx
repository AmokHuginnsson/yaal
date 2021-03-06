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
#include "hcore/si.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HSubprocessClass : public huginn::HClass {
public:
	HSubprocessClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_
	) : huginn::HClass(
		runtime_,
		typeId_,
		runtime_->identifier_id( "Subprocess" ),
		"The `Subprocess` class represents a system process spawned from this interpreter instance. It allows for a basic process life management.",
		HHuginn::ACCESS::PRIVATE
	) {
		HHuginn::field_definitions_t fd{
			{ "is_alive", runtime_->create_method( &HSubprocess::is_alive ), "tell if given subprocess is alive and running" },
			{ "kill",     runtime_->create_method( &HSubprocess::kill ),     "kill this sub-process" },
			{ "get_pid",  runtime_->create_method( &HSubprocess::get_pid ),  "get operating system Process IDentification-number of this subprocess" },
			{ "wait",     runtime_->create_method( &HSubprocess::wait ),     "( [milliseconds] ) wait for *milliseconds* for this subprocess to finish is execution and return its exit status" },
			{ "in",       runtime_->create_method( &HSubprocess::stream, "Subprocess.in", &HPipedChild::stream_in ),   "standard input stream of a subprocess" },
			{ "out",      runtime_->create_method( &HSubprocess::stream, "Subprocess.out", &HPipedChild::stream_out ), "standard output stream of a subprocess" },
			{ "err",      runtime_->create_method( &HSubprocess::stream, "Subprocess.err", &HPipedChild::stream_err ), "standard error stream of a subprocess" }
		};
		redefine( nullptr, fd );
		return;
	}
};

HSubprocess::HSubprocess(
	huginn::HClass const* class_,
	yaal::hcore::HString const& program_,
	yaal::tools::HPipedChild::argv_t&& argv_,
	bool foreground_,
	HHuginn::value_t const& in_,
	HHuginn::value_t const& out_,
	HHuginn::value_t const& err_
) : huginn::HValue( class_ )
	, _in( in_ )
	, _out( out_ )
	, _err( err_ )
	, _pipedChild() {
	_pipedChild.spawn(
		program_,
		argv_,
		!! _in  ? static_cast<HStream*>( _in.raw()  )->raw().raw() : nullptr,
		!! _out ? static_cast<HStream*>( _out.raw() )->raw().raw() : nullptr,
		!! _err ? static_cast<HStream*>( _err.raw() )->raw().raw() : nullptr,
		foreground_ ? HPipedChild::PROCESS_GROUP_LEADER : -1,
		foreground_
	);
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
	return ( thread_->runtime().boolean_value( o->_pipedChild.is_running() ) );
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
	char const name[] = "Subprocess.wait";
	verify_arg_count( name, values_, 0, 1, thread_, position_ );
	HInteger::value_type const MAX_WAIT_FOR( meta::max_signed<i32_t>::value * si::MILLI_IN_WHOLE );
	HInteger::value_type waitFor( MAX_WAIT_FOR );
	if ( ! values_.is_empty() ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, ARITY::UNARY, thread_, position_ );
		waitFor = get_integer( values_[0] );
	}
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	if ( ( waitFor < 0 ) || ( waitFor > MAX_WAIT_FOR ) ) {
		throw HHuginn::HHuginnRuntimeException( "invalid wait time: "_ys.append( waitFor ), thread_->file_id(), position_ );
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
	HObjectFactory& of( thread_->object_factory() );
	HStreamInterface::ptr_t s( (o->_pipedChild.*streamGetter_)() );
	return ( !! s ? of.create<HStream>( of.stream_class(), s ) : of.none_value() );
	M_EPILOG
}

HHuginn::class_t HSubprocess::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
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
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return c;
	M_EPILOG
}

HHuginn::value_t HSubprocess::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on Subprocess.", thread_->file_id(), position_ );
	M_EPILOG
}

}

}

}

