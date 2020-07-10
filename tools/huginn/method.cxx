/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "method.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HClass::HMethod::HMethod(
	huginn::HClass const* class_,
	HHuginn::function_t const& function_
) : HValue( class_ )
	, _function( function_ ) {
	return;
}

HHuginn::value_t HClass::HMethod::fast_call( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& arguments_, int position_ ) {
	return ( _function( thread_, object_, arguments_, position_ ) );
}

HHuginn::value_t HClass::HMethod::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->object_factory().create_method_raw( _function ) );
}

HClass::HUnboundMethod::HUnboundMethod(
	huginn::HClass const* class_,
	huginn::HClass const* juncture_,
	HHuginn::function_t const& function_
) : HValue( class_ )
	, _juncture( juncture_ )
	, _function( function_ ) {
	return;
}

HHuginn::value_t HClass::HUnboundMethod::do_operator_call( huginn::HThread* thread_, HHuginn::value_t&, HHuginn::values_t& arguments_, int position_ ) {
	if ( arguments_.is_empty() ) {
		throw HHuginn::HHuginnRuntimeException(
			"Calling method without an object.",
			thread_->file_id(),
			position_
		);
	}
	HHuginn::value_t o( yaal::move( arguments_.front() ) );
	if ( o->get_class() != _juncture ) {
		throw HHuginn::HHuginnRuntimeException(
			"Method of class "_ys
				.append( a_type_name( _juncture ) )
				.append( " called on an object of type " )
				.append( a_type_name( o->get_class() ) )
				.append( "." ),
			thread_->file_id(),
			position_
		);
	}
	arguments_.erase( arguments_.begin() );
	return ( _function( thread_, &o, arguments_, position_ ) );
}

HHuginn::value_t HClass::HUnboundMethod::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->object_factory().create_unbound_method( _juncture, _function ) );
}

yaal::hcore::HString HClass::HUnboundMethod::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker&, int ) const {
	HRuntime& rt( thread_->runtime() );
	return ( full_class_name( rt, _juncture ).append( "." ).append( rt.identifier_name( _juncture->method_id( _function.id() ) ) ) );
}

HClass::HBoundMethod::HBoundMethod( huginn::HClass const* class_, HHuginn::function_t const& method_, HHuginn::value_t const& object_ )
	: HValue( class_ )
	, _function( method_ )
	, _objectHolder( object_ ) {
	return;
}

HHuginn::value_t HClass::HBoundMethod::do_operator_call( huginn::HThread* thread_, HHuginn::value_t&, HHuginn::values_t& arguments_, int position_ ) {
	return ( _function( thread_, &_objectHolder, arguments_, position_ ) );
}

HHuginn::value_t HClass::HBoundMethod::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	return (
		thread_->runtime().object_factory()->create_bound_method(
			_function,
			_objectHolder->clone( thread_, const_cast<HHuginn::value_t*>( &_objectHolder ), position_ )
		)
	);
}

}

}

}

