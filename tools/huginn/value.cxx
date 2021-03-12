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
#include "instruction.hxx"

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
				thread_->file_id(),
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
				"There is no `"_ys.append( oper_ ).append( "` operator for " ).append( a_type_name( c ) ).append( "." ),
				thread_->file_id(),
				position_
			);
		}
	}
	HBoolean* b( static_cast<HBoolean*>( v.raw() ) );
	return ( b->value() );
}

void fallback_arithmetic( HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, char const* oper_, HHuginn::value_t& v1_, HHuginn::value_t const& v2_, int position_ ) {
	HHuginn::value_t v;
	HClass const* c( v1_->get_class() );
	if ( HObject const* o = dynamic_cast<HObject const*>( v1_.raw() ) ) {
		v = o->call_method( thread_, v1_, methodIdentifier_, HArguments( thread_, v2_ ), position_ );
		HClass const* rc( v->get_class() );
		if ( rc != c ) {
			throw HHuginn::HHuginnRuntimeException(
				"Arithmetic method `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "` on " )
					.append( a_type_name( c ) )
					.append( " returned result of incompatible type " )
					.append( a_type_name( rc ) )
					.append( "." ),
				thread_->file_id(),
				position_
			);
		}
	} else {
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( c->field( idx ).raw() ) );
			v = m.function()( thread_, &v1_, HArguments( thread_, v2_ ), position_ );
			M_ASSERT( ( v->get_class() == c ) || ! thread_->can_continue() );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"There is no `"_ys.append( oper_ ).append( "` operator for " ).append( a_type_name( c ) ).append( "." ),
				thread_->file_id(),
				position_
			);
		}
	}
	return;
}

enum class OPERATION {
	OPEN,
	CLOSED
};

HHuginn::value_t fallback_unary_arithmetic( HThread* thread_, HHuginn::identifier_id_t methodIdentifier_, char const* oper_, HHuginn::value_t const& v_, OPERATION operation_, int position_ ) {
	HHuginn::value_t v;
	HClass const* c( v_->get_class() );
	if ( HObject const* o = dynamic_cast<HObject const*>( v_.raw() ) ) {
		v = o->call_method( thread_, v_, methodIdentifier_, HArguments( thread_ ), position_ );
		HClass const* rc( v->get_class() );
		if ( ( operation_ == OPERATION::CLOSED ) && ( rc != c ) ) {
			throw HHuginn::HHuginnRuntimeException(
				"Arithmetic method `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "` on " )
					.append( a_type_name( c ) )
					.append( " returned result of incompatible type " )
					.append( a_type_name( rc ) )
					.append( "." ),
				thread_->file_id(),
				position_
			);
		}
	} else {
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( c->field( idx ).raw() ) );
			v = m.function()( thread_, const_cast<HHuginn::value_t*>( &v_ ), HArguments( thread_ ), position_ );
			M_ASSERT( ( operation_ == OPERATION::OPEN ) || ( v->get_class() == c ) || ! thread_->can_continue() );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"There is no `"_ys.append( oper_ ).append( "` operator for " ).append( a_type_name( c ) ).append( "." ),
				thread_->file_id(),
				position_
			);
		}
	}
	return v;
}

HHuginn::value_t nullary_fallback( HThread* thread_, HHuginn::value_t const& v_, HHuginn::identifier_id_t methodIdentifier_, HClass const* resType_, int position_ ) {
	HHuginn::value_t res;
	if ( HObject const* o = dynamic_cast<HObject const*>( v_.raw() ) ) {
		res = o->call_method( thread_, v_, methodIdentifier_, HArguments( thread_ ), position_ );
		if ( res->get_class() != resType_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"User supplied `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "` function returned an invalid type " )
					.append( a_type_name( res->get_class() ) )
					.append( " instead of " )
					.append( a_type_name( resType_ ) )
					.append( "." ),
				thread_->file_id(),
				position_
			);
		}
	} else {
		HClass const* c( v_->get_class() );
		int idx( c->field_index( methodIdentifier_ ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( c->field( idx ).raw() ) );
			res = m.function()( thread_, const_cast<HHuginn::value_t*>( &v_ ), HArguments( thread_ ), position_ );
			M_ASSERT( res->get_class() == resType_ );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"There is no `"_ys
					.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
					.append( "` method in " )
					.append( a_type_name( c ) )
					.append( "." ),
				thread_->file_id(),
				position_
			);
		}
	}
	return res;
}

char const* type_to_cycle_str( HHuginn::TYPE type_ ) {
	char const* sym( "/*cycle*/" );
	switch ( type_ ) {
		case ( HHuginn::TYPE::TUPLE ):  { sym = "/*(cycle)*/"; } break;
		case ( HHuginn::TYPE::LIST ):   { sym = "/*[cycle]*/"; } break;
		case ( HHuginn::TYPE::LOOKUP ): { sym = "/*{cycle}*/"; } break;
		default:;
	}
	return sym;
}

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

void HValue::do_operator_multiply( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::MULTIPLY, op_to_str( OPERATOR::MULTIPLY ), self_, other_, position_ );
}

void HValue::do_operator_divide( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::DIVIDE, op_to_str( OPERATOR::DIVIDE ), self_, other_, position_ );
}

void HValue::do_operator_modulo( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::MODULO, op_to_str( OPERATOR::MODULO ), self_, other_, position_ );
}

void HValue::do_operator_power( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	fallback_arithmetic( thread_, IDENTIFIER::INTERFACE::POWER, op_to_str( OPERATOR::POWER ), self_, other_, position_ );
}

HHuginn::value_t HValue::do_operator_modulus( HThread* thread_, HHuginn::value_t const& self_, int position_ ) const {
	return ( fallback_unary_arithmetic( thread_, IDENTIFIER::INTERFACE::MODULUS, op_to_str( OPERATOR::MODULUS ), self_, OPERATION::OPEN, position_ ) );
}

HHuginn::value_t HValue::do_operator_negate( HThread* thread_, HHuginn::value_t const& self_, int position_ ) const {
	return ( fallback_unary_arithmetic( thread_, IDENTIFIER::INTERFACE::NEGATE, op_to_str( OPERATOR::NEGATE ), self_, OPERATION::CLOSED, position_ ) );
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
HHuginn::value_t HValue::do_operator_factorial( HThread* thread_, HHuginn::value_t const& self_, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException(
		"There is no `!` operator for "_ys.append( a_type_name( self_->get_class() ) ).append( "." ),
		thread_->file_id(), position_
	);
}
#pragma GCC diagnostic pop

HHuginn::value_t HValue::do_operator_subscript( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	HHuginn::value_t res;
	if ( HObject const* o = dynamic_cast<HObject const*>( self_.raw() ) ) {
		res = o->call_method( thread_, self_, IDENTIFIER::INTERFACE::SUBSCRIPT, HArguments( thread_, key_ ), position_ );
	} else {
		HClass const* c( self_->get_class() );
		int idx( c->field_index( IDENTIFIER::INTERFACE::SUBSCRIPT ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( c->field( idx ).raw() ) );
			res = m.function()( thread_, const_cast<HHuginn::value_t*>( &self_ ), HArguments( thread_, key_ ), position_ );
		}
	}
	if ( ! res ) {
		throw HHuginn::HHuginnRuntimeException(
			"Subscript is not supported on "_ys.append( a_type_name( self_->get_class() ) ).append( "." ),
			thread_->file_id(),
			position_
		);
	}
	if ( ! thread_->can_continue() ) {
		throw instruction::Interrupt();
	}
	return res;
	M_EPILOG
}

void HValue::do_operator_subscript_assign( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& key_, HHuginn::value_t&& value_, int position_ ) {
	M_PROLOG
	if ( HObject const* o = dynamic_cast<HObject const*>( self_.raw() ) ) {
		o->call_method( thread_, self_, IDENTIFIER::INTERFACE::SET_SUBSCRIPT, HArguments( thread_, value_, key_ ), position_ );
	} else {
		HClass const* c( self_->get_class() );
		int idx( c->field_index( IDENTIFIER::INTERFACE::SET_SUBSCRIPT ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( c->field( idx ).raw() ) );
			m.function()( thread_, const_cast<HHuginn::value_t*>( &self_ ), HArguments( thread_, value_, key_ ), position_ );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"Subscript assignment is not supported on "_ys.append( a_type_name( self_->get_class() ) ).append( "." ),
				thread_->file_id(),
				position_
			);
		}
	}
	if ( ! thread_->can_continue() ) {
		throw instruction::Interrupt();
	}
	return;
	M_EPILOG
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
HHuginn::value_t HValue::do_operator_range( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const&, HHuginn::value_t const&, HHuginn::value_t const&, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException(
		"Range operator is not supported on "_ys.append( a_type_name( self_->get_class() ) ).append( "." ),
		thread_->file_id(),
		position_
	);
}
#pragma GCC diagnostic pop

hash_value_t HValue::do_operator_hash( HThread* thread_, HHuginn::value_t const& self_, int position_ ) const {
	if ( _class->type_id() == HHuginn::TYPE::NONE ) {
		return ( 0 );
	}
	HHuginn::value_t res( nullary_fallback( thread_, self_, IDENTIFIER::INTERFACE::HASH, thread_->runtime().object_factory()->integer_class(), position_ ) );
	return ( hcore::hash<int long long>()( get_integer( res ) ) );
}

HHuginn::value_t HValue::do_operator_call( huginn::HThread* thread_, HHuginn::value_t& self_, HHuginn::values_t& arguments_, int position_ ) {
	HHuginn::value_t v;
	if ( HObject* o = dynamic_cast<HObject*>( this ) ) {
		v = o->call_method( thread_, self_, IDENTIFIER::INTERFACE::CALL, arguments_, position_ );
	} else {
		int idx( _class->field_index( IDENTIFIER::INTERFACE::CALL ) );
		if ( idx >= 0 ) {
			HClass::HMethod const& m( *static_cast<HClass::HMethod const*>( _class->field( idx ).raw() ) );
			v = m.function()( thread_, &self_, arguments_, position_ );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"Reference `"_ys.append( _class->name() ).append( "` is not a callable." ),
				thread_->file_id(),
				position_
			);
		}
	}
	return v;
}

yaal::hcore::HString HValue::do_code( huginn::HThread* thread_, HHuginn::value_t const& self_, HCycleTracker&, int position_ ) const {
	if ( _class->type_id() == HHuginn::TYPE::NONE ) {
		return ( KEYWORD::NONE );
	}
	if ( ! thread_ ) {
		return ( _class->name() );
	}
	return ( get_string( nullary_fallback( thread_, self_, IDENTIFIER::INTERFACE::CODE, thread_->runtime().object_factory()->string_class(), position_ ) ) );
}

yaal::hcore::HString HValue::code( HThread* thread_, HHuginn::value_t const& self_, HCycleTracker& cycleTracker_, int position_ ) const {
	M_ASSERT( self_.raw() == this );
	if ( ! cycleTracker_.track( this ) ) {
		return ( hcore::to_string( KEYWORD::NONE ).append( type_to_cycle_str( static_cast<HHuginn::TYPE>( type_id().get() ) ) ) );
	}
	yaal::hcore::HString str;
	do {
		try {
			str = do_code( thread_, self_, cycleTracker_, position_ );
		} catch ( ... ) {
			try {
				str = do_to_string( thread_, self_, cycleTracker_, position_ );
				break;
			} catch ( ... ) {
			}
			throw;
		}
	} while ( false );
	cycleTracker_.done( this );
	return str;
}

yaal::hcore::HString HValue::do_to_string( huginn::HThread* thread_, HHuginn::value_t const& self_, HCycleTracker&, int position_ ) const {
	if ( _class->type_id() == HHuginn::TYPE::NONE ) {
		return ( KEYWORD::NONE );
	}
	if ( ! thread_ ) {
		return ( _class->name() );
	}
	return ( get_string( nullary_fallback( thread_, self_, IDENTIFIER::INTERFACE::TO_STRING, thread_->runtime().object_factory()->string_class(), position_ ) ) );
}

yaal::hcore::HString HValue::to_string( HThread* thread_, HHuginn::value_t const& self_, HCycleTracker& cycleTracker_, int position_ ) const {
	M_ASSERT( self_.raw() == this );
	if ( ! cycleTracker_.track( this ) ) {
		return ( hcore::to_string( KEYWORD::NONE ).append( type_to_cycle_str( static_cast<HHuginn::TYPE>( type_id().get() ) ) ) );
	}
	yaal::hcore::HString str;
	do {
		try {
			str = do_to_string( thread_, self_, cycleTracker_, position_ );
		} catch ( ... ) {
			try {
				str = do_code( thread_, self_, cycleTracker_, position_ );
				break;
			} catch ( ... ) {
			}
			throw;
		}
	} while ( false );
	cycleTracker_.done( this );
	return str;
}

HCycleTracker::HCycleTracker( void )
	: _valueNoter() {
}

bool HCycleTracker::track( HValue const* value_ ) {
	M_PROLOG
	HHuginn::type_id_t t( value_->type_id() );
	return (
		( t == HHuginn::TYPE::NONE )
		|| ( t == HHuginn::TYPE::BOOLEAN )
		|| ( t == HHuginn::TYPE::FUNCTION_REFERENCE )
		|| ( t == HHuginn::TYPE::UNBOUND_METHOD )
		|| _valueNoter.insert( value_ ).second
	);
	M_EPILOG
}

void HCycleTracker::check( HValue const* value_, int fileId_, int position_ ) {
	M_PROLOG
	if ( ! track( value_ ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Cycle detected on type: "_ys.append( value_->get_class()->name() ),
			fileId_,
			position_
		);
	}
	return;
	M_EPILOG
}

void HCycleTracker::done( HValue const* value_ ) {
	M_PROLOG
	_valueNoter.erase( value_ );
	return;
	M_EPILOG
}

}

}

}

