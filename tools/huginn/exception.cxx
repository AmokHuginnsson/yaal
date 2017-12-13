/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  exception.cxx - this file is integral part of `yaal' project.

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
			base_
				? HHuginn::field_definitions_t{}
				: HHuginn::field_definitions_t{
						{ "what",    runtime_->object_factory()->create_method( hcore::call( &HExceptionClass::what, _1, _2, _3, _4 ) ),    "get exception message" },
						{ "where",   runtime_->object_factory()->create_method( hcore::call( &HExceptionClass::where, _1, _2, _3, _4 ) ),   "get originating exception position" },
						{ "trace",   runtime_->object_factory()->create_method( hcore::call( &HExceptionClass::trace, _1, _2, _3, _4 ) ),   "get call stack for exception origin" },
						{ "message", runtime_->object_factory()->create_method( hcore::call( &HExceptionClass::message, _1, _2, _3, _4 ) ), "get exception message" }
				},
			doc_
		) {
		return;
	}
private:
	virtual HHuginn::value_t do_create_instance( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) const {
		M_PROLOG
		verify_signature( name() + ".constructor", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		thread_->current_frame()->set_position( position_ );
		return ( thread_->object_factory().create<HHuginn::HException>( thread_, this, get_string( values_[0] ) ) );
		M_EPILOG
	}
	static HHuginn::value_t what( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_->raw() ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->name() + ".what",
			values_, 0, 0, thread_, position_
		);
		return ( thread_->object_factory().create_string( e->what() ) );
		M_EPILOG
	}
	static HHuginn::value_t where( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_->raw() ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->name() + ".where",
			values_, 0, 0, thread_, position_
		);
		return ( thread_->object_factory().create_string( e->where() ) );
		M_EPILOG
	}
	static HHuginn::value_t trace( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_->raw() ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->name() + ".trace",
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
			static_cast<HExceptionClass const*>( e->get_class() )->name() + ".message",
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

HHuginn::class_t create_class( HHuginn::ACCESS access_, HRuntime* runtime_, yaal::hcore::HString const& name_, yaal::hcore::HString const& doc_, HHuginn::HClass const* base_ ) {
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
		runtime_->huginn()->register_class( c, access_ );
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

HHuginn::value_t HStackFrameInfo::do_clone( huginn::HThread* thread_, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on StackFrameInfo.", thread_->current_frame()->file_id(), position_ );
}

HHuginn::class_t HStackFrameInfo::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"StackFrameInfo",
			nullptr,
			HHuginn::field_definitions_t{
				{ "file",      runtime_->object_factory()->create_method( hcore::call( &HStackFrameInfo::file, _1, _2, _3, _4 ) ),      "a name of the file where exception went through" },
				{ "line",      runtime_->object_factory()->create_method( hcore::call( &HStackFrameInfo::line, _1, _2, _3, _4 ) ),      "a line number in given file where an exception went through" },
				{ "column",    runtime_->object_factory()->create_method( hcore::call( &HStackFrameInfo::column, _1, _2, _3, _4 ) ),    "a column number where an exception went through" },
				{ "context",   runtime_->object_factory()->create_method( hcore::call( &HStackFrameInfo::context, _1, _2, _3, _4 ) ),   "a calling context for given execution frame" },
				{ "to_string", runtime_->object_factory()->create_method( hcore::call( &HStackFrameInfo::to_string, _1, _2, _3, _4 ) ), "a string representation of a StackFrameInfo value" }
			},
			"The `StackFrameInfo` is a class representing information about single execution stack frame."
		)
	);
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

HHuginn::value_t HHuginn::HException::do_clone( huginn::HThread* thread_, int ) const {
	HHuginn::value_t e( thread_->object_factory().create<HException>( get_class(), _message, _callStack ) );
	return ( e );
}

}

}

