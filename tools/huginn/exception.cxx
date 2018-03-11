/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "runtime.hxx"
#include "exception.hxx"
#include "tools/hhuginn.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace exception {

class HExceptionClass : public HHuginn::HClass {
public:
	typedef HExceptionClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HUTF8String _constructorName;
	HUTF8String _whatName;
	HUTF8String _whereName;
	HUTF8String _traceName;
	HUTF8String _messageName;
public:
	HExceptionClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::identifier_id_t identifierId_,
		HHuginn::HClass const* base_,
		yaal::hcore::HString const& doc_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			identifierId_,
			base_,
			doc_
		)
		, _constructorName( name() + ".constructor" )
		, _whatName( name() + ".what" )
		, _whereName( name() + ".where" )
		, _traceName( name() + ".trace" )
		, _messageName( name() + ".message" ) {
		redefine(
			base_,
			base_
				? HHuginn::field_definitions_t{}
				: HHuginn::field_definitions_t{
					{ "what",    runtime_->create_method( &HExceptionClass::what ),    "get exception message" },
					{ "where",   runtime_->create_method( &HExceptionClass::where ),   "get originating exception position" },
					{ "trace",   runtime_->create_method( &HExceptionClass::trace ),   "get call stack for exception origin" },
					{ "message", runtime_->create_method( &HExceptionClass::message ), "get exception message" }
				}
		);
		return;
	}
private:
	virtual HHuginn::value_t do_create_instance( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) const {
		M_PROLOG
		verify_signature( _constructorName.c_str(), values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		thread_->current_frame()->set_position( position_ );
		return ( thread_->object_factory().create<HHuginn::HException>( thread_, this, get_string( values_[0] ) ) );
		M_EPILOG
	}
	static HHuginn::value_t what( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_->raw() ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->_whatName.c_str(),
			values_, 0, 0, thread_, position_
		);
		return ( thread_->object_factory().create_string( e->what() ) );
		M_EPILOG
	}
	static HHuginn::value_t where( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_->raw() ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->_whereName.c_str(),
			values_, 0, 0, thread_, position_
		);
		return ( thread_->object_factory().create_string( e->where() ) );
		M_EPILOG
	}
	static HHuginn::value_t trace( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_->raw() ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->_traceName.c_str(),
			values_, 0, 0, thread_, position_
		);
		HIntrospecteeInterface::call_stack_t const& traceData( e->trace() );
		HObjectFactory& of( thread_->object_factory() );
		HHuginn::value_t traceValue( of.create_list() );
		HHuginn::HList::values_t& traceList( static_cast<HHuginn::HList*>( traceValue.raw() )->value() );
		for ( HIntrospecteeInterface::HCallSite const& cs : traceData ) {
			traceList.push_back( of.create<HStackFrameInfo>( of.stack_frame_info_class(), cs ) );
		}
		return ( traceValue );
		M_EPILOG
	}
	static HHuginn::value_t message( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_->raw() ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->_messageName.c_str(),
			values_, 0, 0, thread_, position_
		);
		HString message( e->where() );
		message.append( ": " ).append( e->what() );
		return ( thread_->object_factory().create_string( message ) );
		M_EPILOG
	}
};

HHuginn::class_t get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HExceptionClass>(
							runtime_,
							typeId_,
							runtime_->identifier_id( "Exception" ),
							nullptr,
							"The `Exception` class is a base class for Huginn standard exceptions hierarchy."
						)
					);
				}
			)
		)
	);
	return ( c );
	M_EPILOG
}

HHuginn::class_t create_class( HRuntime* runtime_, yaal::hcore::HString const& name_, yaal::hcore::HString const& doc_, HHuginn::VISIBILITY visibility_, HHuginn::HClass const* base_ ) {
	M_PROLOG
	HHuginn::identifier_id_t classIdentifier( runtime_->identifier_id( name_ ) );
	HHuginn::class_t c( runtime_ ? runtime_->get_class( classIdentifier ) : nullptr );
	if ( ! c ) {
		c =	runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &classIdentifier, &doc_, &base_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HExceptionClass>(
							runtime_,
							typeId_,
							classIdentifier,
							base_ ? base_ : runtime_->object_factory()->exception_class(),
							doc_
						)
					);
				}
			)
		);
	}
	if ( runtime_ ) {
		/* *TODO* *FIXME* Move to the end of if(!c) above. */
		runtime_->huginn()->register_class( c, HHuginn::ACCESS::PUBLIC, visibility_ );
	}
	return ( c );
	M_EPILOG
}

HStackFrameInfo::HStackFrameInfo( HHuginn::HClass const* class_, HIntrospecteeInterface::HCallSite const& callSite_ )
	: HValue( class_ )
	, _callSite( callSite_ ) {
}

HHuginn::value_t HStackFrameInfo::file( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "file", values_, 0, 0, thread_, position_ );
	HStackFrameInfo* sfi( static_cast<HStackFrameInfo*>( object_->raw() ) );
	return ( thread_->object_factory().create_string( sfi->_callSite.file() ) );
	M_EPILOG
}

HHuginn::value_t HStackFrameInfo::line( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "line", values_, 0, 0, thread_, position_ );
	HStackFrameInfo* sfi( static_cast<HStackFrameInfo*>( object_->raw() ) );
	return ( thread_->object_factory().create_integer( sfi->_callSite.line() ) );
	M_EPILOG
}

HHuginn::value_t HStackFrameInfo::column( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "column", values_, 0, 0, thread_, position_ );
	HStackFrameInfo* sfi( static_cast<HStackFrameInfo*>( object_->raw() ) );
	return ( thread_->object_factory().create_integer( sfi->_callSite.column() ) );
	M_EPILOG
}

HHuginn::value_t HStackFrameInfo::context( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "context", values_, 0, 0, thread_, position_ );
	HStackFrameInfo* sfi( static_cast<HStackFrameInfo*>( object_->raw() ) );
	return ( thread_->object_factory().create_string( sfi->_callSite.context() ) );
	M_EPILOG
}

HHuginn::value_t HStackFrameInfo::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "to_string", values_, 0, 0, thread_, position_ );
	HStackFrameInfo* sfi( static_cast<HStackFrameInfo*>( object_->raw() ) );
	hcore::HString s;
	s.assign( sfi->_callSite.file() )
		.append( ":" ).append( sfi->_callSite.line() )
		.append( ":" ).append( sfi->_callSite.column() )
		.append( ":" ).append( sfi->_callSite.context() );
	return ( thread_->object_factory().create_string( s ) );
	M_EPILOG
}

HHuginn::value_t HStackFrameInfo::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on StackFrameInfo.", thread_->current_frame()->file_id(), position_ );
}

HHuginn::class_t HStackFrameInfo::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"StackFrameInfo",
			nullptr,
			"The `StackFrameInfo` is a class representing information about single execution stack frame."
		)
	);
	HHuginn::field_definitions_t fd{
		{ "file",      runtime_->create_method( &HStackFrameInfo::file ),      "a name of the file where exception went through" },
		{ "line",      runtime_->create_method( &HStackFrameInfo::line ),      "a line number in given file where an exception went through" },
		{ "column",    runtime_->create_method( &HStackFrameInfo::column ),    "a column number where an exception went through" },
		{ "context",   runtime_->create_method( &HStackFrameInfo::context ),   "a calling context for given execution frame" },
		{ "to_string", runtime_->create_method( &HStackFrameInfo::to_string ), "a string representation of a StackFrameInfo value" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HException::HException( huginn::HThread* thread_, HHuginn::HClass const* class_, yaal::hcore::HString const& message_ )
	: HValue( class_ )
	, _message( message_ )
	, _callStack( thread_->runtime().get_call_stack( thread_ ) ) {
	return;
}

HHuginn::HException::HException( HHuginn::HClass const* class_, yaal::hcore::HString const& message_, HIntrospecteeInterface::call_stack_t const& callStack_ )
	: HValue( class_ )
	, _message( message_ )
	, _callStack( callStack_ ) {
	return;
}

yaal::hcore::HString const& HHuginn::HException::what( void ) const {
	return ( _message );
}

yaal::hcore::HString HHuginn::HException::where( void ) const {
	HIntrospecteeInterface::HCallSite const& cs( _callStack.front() );
	return ( to_string( cs.file() ).append( ":" ).append( cs.line() ).append( ":" ).append( cs.column() ) );
}

HIntrospecteeInterface::call_stack_t const& HHuginn::HException::trace( void ) const {
	return ( _callStack );
}

HHuginn::value_t HHuginn::HException::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	HHuginn::value_t e( thread_->object_factory().create<HException>( get_class(), _message, _callStack ) );
	return ( e );
}

}

}

