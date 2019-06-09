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
#include "keyword.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HValue::HValue( HClass const* class_ )
	: _class( class_ ) {
	return;
}

int HValue::field_index( HHuginn::identifier_id_t const& identifierId_ ) const {
	M_PROLOG
	return ( _class->field_index( identifierId_ ) );
	M_EPILOG
}

bool HValue::is_kind_of( HHuginn::identifier_id_t const& typeName_ ) const {
	M_PROLOG
	return ( _class->is_kind_of( typeName_ ) );
	M_EPILOG
}

HHuginn::value_t HValue::do_field( HHuginn::value_t const& object_, int index_ ) const {
	M_PROLOG
	value_t const& f( _class->field( index_ ) );
	return (
		( f->type_id() == HHuginn::TYPE::METHOD )
			?_class->runtime()->object_factory()->create_bound_method(
				static_cast<HClass::HMethod const*>( f.raw() )->function(),
				object_
			)
			: f
	);
	M_EPILOG
}

HHuginn::value_t const& HValue::do_field( int index_ ) const {
	M_PROLOG
	return ( _class->field( index_ ) );
	M_EPILOG
}

HHuginn::value_t HValue::clone( huginn::HThread* thread_, HHuginn::value_t* object_, int position_ ) const {
	return ( do_clone( thread_, object_, position_ ) );
}

HHuginn::value_t HValue::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	M_ASSERT( _class->type_id() == HHuginn::TYPE::NONE );
	return ( thread_->runtime().none_value() );
}

namespace {
bool fallback_compare( HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, char const* oper_, HHuginn::value_t const& v1_, HHuginn::value_t const& v2_, int position_ ) {
	HObject const* o( dynamic_cast<HObject const*>( v1_.raw() ) );
	HHuginn::value_t v;
	if ( o ) {
		v = o->call_method( thread_, v1_, methodIdentifier_, HArguments( thread_, v2_ ), position_ );
		if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException(
				"Comparison method `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "` returned non-boolean result of " )
					.append( a_type_name( v->get_class() ) )
					.append( " type." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	} else {
		HClass const* c( v1_->get_class() );
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( c->field( idx ).raw() ) );
			v = m.function()( thread_, const_cast<HHuginn::value_t*>( &v1_ ), HArguments( thread_, v2_ ), position_ );
			M_ASSERT( ( v->type_id() == HHuginn::TYPE::BOOLEAN ) || ! thread_->can_continue() );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"There is no `"_ys.append( oper_ ).append( "` operator for " ).append( a_type_name( v1_->get_class() ) ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	HBoolean* b( static_cast<HBoolean*>( v.raw() ) );
	return ( b->value() );
}
}

void fallback_arithmetic( HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, char const* oper_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	HHuginn::value_t v;
	HHuginn::type_id_t t( v1_->type_id() );
	if ( HObject const* o = dynamic_cast<HObject const*>( v1_.raw() ) ) {
		v = o->call_method( thread_, v1_, methodIdentifier_, HArguments( thread_, v2_ ), position_ );
		if ( v->type_id() != t ) {
			throw HHuginn::HHuginnRuntimeException(
				"Arithmetic method `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "` on " )
					.append( a_type_name( v1_->get_class() ) )
					.append( " returned result of incompatible type " )
					.append( a_type_name( v->get_class() ) )
					.append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	} else {
		HClass const* c( v1_->get_class() );
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( c->field( idx ).raw() ) );
			v = m.function()( thread_, &v1_, HArguments( thread_, v2_ ), position_ );
			M_ASSERT( ( v->type_id() == t ) || ! thread_->can_continue() );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"There is no `"_ys.append( oper_ ).append( "` operator for " ).append( a_type_name( t ) ).append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	return;
}

bool HValue::do_operator_equals( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
	return ( fallback_compare( thread_, IDENTIFIER::INTERFACE::EQUALS, op_to_str( OPERATOR::EQUALS ), self_, other_, position_ ) );
}

bool HValue::do_operator_less( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
	return ( fallback_compare( thread_, IDENTIFIER::INTERFACE::LESS, op_to_str( OPERATOR::LESS ), self_, other_, position_ ) );
}

bool HValue::do_operator_greater( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
	return ( fallback_compare( thread_, IDENTIFIER::INTERFACE::GREATER, op_to_str( OPERATOR::GREATER ), self_, other_, position_ ) );
}

bool HValue::do_operator_less_or_equal( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
	return ( fallback_compare( thread_, IDENTIFIER::INTERFACE::LESS_OR_EQUAL, op_to_str( OPERATOR::LESS_OR_EQUAL ), self_, other_, position_ ) );
}

bool HValue::do_operator_greater_or_equal( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
	return ( fallback_compare( thread_, IDENTIFIER::INTERFACE::GREATER_OR_EQUAL, op_to_str( OPERATOR::GREATER_OR_EQUAL ), self_, other_, position_ ) );
}

bool HValue::do_operator_contains( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
	M_ASSERT( is_collection_like( _class ) || ( _class->type_id() > huginn::type_id( HHuginn::TYPE::UNKNOWN ) ) );
	return ( fallback_compare( thread_, IDENTIFIER::INTERFACE::CONTAINS, op_to_str( OPERATOR::IS_ELEMENT_OF ), self_, other_, position_ ) );
}

void HValue::do_operator_add( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::ADD, op_to_str( OPERATOR::PLUS ), self_, other_, position_ );
}

void HValue::do_operator_subtract( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::SUBTRACT, op_to_str( OPERATOR::MINUS ), self_, other_, position_ );
}

void HValue::do_operator_multipy( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::MULTIPLY, op_to_str( OPERATOR::MULTIPLY ), self_, other_, position_ );
}

void HValue::do_operator_divide( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::DIVIDE, op_to_str( OPERATOR::DIVIDE ), self_, other_, position_ );
}

void HValue::do_operator_modulo( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::MODULO, op_to_str( OPERATOR::MODULO ), self_, other_, position_ );
}

}

}

}

