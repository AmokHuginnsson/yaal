/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cwctype>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "instruction.hxx"
#include "keyword.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HOperators : public HPackage {
public:
	typedef void ( HValue::* binary_operator_t )( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
	typedef bool ( HValue::* binary_boolean_operator_t )( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const;
	typedef HHuginn::value_t ( HValue::* unary_operator_t )( HThread*, HHuginn::value_t const&, int ) const;
	HOperators( huginn::HClass* class_ )
		: HPackage( class_ ) {
		return;
	}
	static HHuginn::value_t binary_operator( char const* name_, OPERATOR op_, binary_operator_t binaryOperator_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 2, 2, thread_, position_ );
		HHuginn::value_t& v1( values_[0] );
		HHuginn::value_t& v2( values_[1] );
		huginn::HClass const* c1( v1->get_class() );
		huginn::HClass const* c2( v2->get_class() );
		if ( c1 != c2 ) {
			operands_type_mismatch( op_to_str( op_ ), c1, c2, thread_->file_id(), position_ );
		}
		if ( op_ < OPERATOR::ASSIGN ) {
			v1 = v1->clone( thread_, &v1, position_ );
		}
		( v1.raw()->*binaryOperator_ )( thread_, v1, v2, position_ );
		return ( v1 );
		M_EPILOG
	}
	static HHuginn::value_t binary_boolean_operator( char const* name_, OPERATOR op_, binary_boolean_operator_t binaryBooleanOperator_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 2, 2, thread_, position_ );
		HHuginn::value_t& v1( values_[0] );
		HHuginn::value_t& v2( values_[1] );
		huginn::HClass const* c1( v1->get_class() );
		huginn::HClass const* c2( v2->get_class() );
		if ( c1 != c2 ) {
			operands_type_mismatch( op_to_str( op_ ), c1, c2, thread_->file_id(), position_ );
		}
		bool res( ( v1.raw()->*binaryBooleanOperator_ )( thread_, v1, v2, position_ ) );
		return ( thread_->runtime().boolean_value( res ) );
		M_EPILOG
	}
	static HHuginn::value_t boolean_binary_operator( char const* name_, OPERATOR op_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( name_, values_, { HHuginn::TYPE::BOOLEAN, HHuginn::TYPE::BOOLEAN }, thread_, position_ );
		bool b1( get_boolean( values_[0] ) );
		bool b2( get_boolean( values_[1] ) );
		bool res( false );
		switch ( op_ ) {
			case ( OPERATOR::BOOLEAN_AND ): res = b1 && b2;       break;
			case ( OPERATOR::BOOLEAN_OR ):  res = b1 || b2;       break;
			case ( OPERATOR::BOOLEAN_XOR ): res = exor( b1, b2 ); break;
			default: {
				M_ASSERT( !"Invalid code path."[0] );
			}
		}
		return ( thread_->runtime().boolean_value( res ) );
		M_EPILOG
	}
	static HHuginn::value_t equals( char const* name_, OPERATOR op_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 2, 2, thread_, position_ );
		HHuginn::value_t& v1( values_[0] );
		HHuginn::value_t& v2( values_[1] );
		huginn::HClass const* c1( v1->get_class() );
		huginn::HClass const* c2( v2->get_class() );
		if ( ( c1 != c2 ) && ( c1->type_id() != HHuginn::TYPE::NONE ) && ( c2->type_id() != HHuginn::TYPE::NONE ) ) {
			operands_type_mismatch( op_to_str( OPERATOR::EQUALS ), c1, c2, thread_->file_id(), position_ );
		}
		bool res( v1->operator_equals( thread_, v1, v2, position_ ) );
		res = op_ == OPERATOR::EQUALS ? res : ! res;
		return ( thread_->runtime().boolean_value( res ) );
		M_EPILOG
	}
	static HHuginn::value_t is_element_of( char const* name_, OPERATOR op_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 2, 2, thread_, position_ );
		HHuginn::value_t& v1( values_[0] );
		HHuginn::value_t& v2( values_[1] );
		huginn::HClass const* c2( v2->get_class() );
		if ( ( c2->type_id() <= huginn::type_id( HHuginn::TYPE::UNKNOWN ) ) && ! is_collection_like( c2 ) ) {
			throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_COLL] ).append( a_type_name( c2 ) ), thread_->file_id(), position_ );
		}
		bool res( v2->operator_contains( thread_, v2, v1, position_ ) );
		res = op_ == OPERATOR::IS_ELEMENT_OF ? res : ! res;
		return ( thread_->runtime().boolean_value( res ) );
		M_EPILOG
	}
	static HHuginn::value_t unary_operator( char const* name_, unary_operator_t unaryOperator_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 1, 1, thread_, position_ );
		HHuginn::value_t& v( values_[0] );
		return ( ( v.raw()->*unaryOperator_ )( thread_, v, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t boolean_not( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Operators.not", values_, { HHuginn::TYPE::BOOLEAN }, thread_, position_ );
		return ( instruction::boolean_not( thread_, values_[0], position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t subscript( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Operators.subscript", values_, 2, 2, thread_, position_ );
		HHuginn::value_t& v( values_[0] );
		return ( v->operator_subscript( thread_, v, values_[1], position_ ) );
		M_EPILOG
	}
};

namespace package_factory {

class HOperatorsCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} operatorsCreator;

HPackageCreatorInterface::HInstance HOperatorsCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Operators",
			"The `Operators` package provides callable and referenceable forwarders for Huginn built-in operators.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		create_field( runtime_, IDENTIFIER::INTERFACE::ADD,      "Operators.add",      OPERATOR::PLUS,     &HValue::operator_add,      &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **+** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::SUBTRACT, "Operators.subtract", OPERATOR::MINUS,    &HValue::operator_subtract, &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **-** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::MULTIPLY, "Operators.multiply", OPERATOR::MULTIPLY, &HValue::operator_multiply, &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* __\\*__ *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::DIVIDE,   "Operators.divide",   OPERATOR::DIVIDE,   &HValue::operator_divide,   &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **/** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::MODULO,   "Operators.modulo",   OPERATOR::MODULO,   &HValue::operator_modulo,   &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **%** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::POWER,    "Operators.power",    OPERATOR::POWER,    &HValue::operator_power,    &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **^** *right* expression" ),
		create_field( runtime_, "self_add",      "Operators.self_add",      OPERATOR::PLUS_ASSIGN,     &HValue::operator_add,      &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **+=** *right* expression" ),
		create_field( runtime_, "self_subtract", "Operators.self_subtract", OPERATOR::MINUS_ASSIGN,    &HValue::operator_subtract, &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **-=** *right* expression" ),
		create_field( runtime_, "self_multiply", "Operators.self_multiply", OPERATOR::MULTIPLY_ASSIGN, &HValue::operator_multiply, &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* __\\*=__ *right* expression" ),
		create_field( runtime_, "self_divide",   "Operators.self_divide",   OPERATOR::DIVIDE_ASSIGN,   &HValue::operator_divide,   &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **/=** *right* expression" ),
		create_field( runtime_, "self_modulo",   "Operators.self_modulo",   OPERATOR::MODULO_ASSIGN,   &HValue::operator_modulo,   &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **%=** *right* expression" ),
		create_field( runtime_, "self_power",    "Operators.self_power",    OPERATOR::POWER_ASSIGN,    &HValue::operator_power,    &HOperators::binary_operator, "( *left*, *right* ) - return result of *left* **^=** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::EQUALS, "Operators.equals", OPERATOR::EQUALS, &HOperators::equals, "( *left*, *right* ) - return result of *left* **==** *right* expression" ),
		create_field( runtime_, "not_equals", "Operators.not_equals", OPERATOR::NOT_EQUALS, &HOperators::equals, "( *left*, *right* ) - return result of *left* *≠* *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::LESS,     "Operators.less",     OPERATOR::LESS,     &HValue::operator_less,    &HOperators::binary_boolean_operator, "( *left*, *right* ) - return result of *left* **<** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::GREATER,  "Operators.greater",  OPERATOR::GREATER,  &HValue::operator_greater, &HOperators::binary_boolean_operator, "( *left*, *right* ) - return result of *left* **>** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::LESS_OR_EQUAL,    "Operators.less_or_equal",     OPERATOR::LESS_OR_EQUAL,     &HValue::operator_less_or_equal,    &HOperators::binary_boolean_operator, "( *left*, *right* ) - return result of *left* **≤** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::GREATER_OR_EQUAL, "Operators.greater_or_equal",  OPERATOR::GREATER_OR_EQUAL,  &HValue::operator_greater_or_equal, &HOperators::binary_boolean_operator, "( *left*, *right* ) - return result of *left* **≥** *right* expression" ),
		create_field( runtime_, "is_element_of",     "Operators.is_element_of",     OPERATOR::IS_ELEMENT_OF,     &HOperators::is_element_of, "( *left*, *right* ) - return result of *left* **∈** *right* expression" ),
		create_field( runtime_, "is_not_element_of", "Operators.is_not_element_of", OPERATOR::IS_NOT_ELEMENT_OF, &HOperators::is_element_of, "( *left*, *right* ) - return result of *left* **∉** *right* expression" ),
		create_field( runtime_, "and", "Operators.and", OPERATOR::BOOLEAN_AND, &HOperators::boolean_binary_operator, "( *left*, *right* ) - return result of *left* **⋀** *right* expression" ),
		create_field( runtime_, "or",  "Operators.or",  OPERATOR::BOOLEAN_OR,  &HOperators::boolean_binary_operator, "( *left*, *right* ) - return result of *left* **⋁** *right* expression" ),
		create_field( runtime_, "xor", "Operators.xor", OPERATOR::BOOLEAN_XOR, &HOperators::boolean_binary_operator, "( *left*, *right* ) - return result of *left* **⊕** *right* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::NEGATE,  "Operators.negate",  &HValue::operator_negate, &HOperators::unary_operator, "( *val* ) - return result of __-__*val* expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::MODULUS, "Operators.modulus", &HValue::operator_modulus, &HOperators::unary_operator, "( *val* ) - return result of **|** *val* **|** expression" ),
		create_field( runtime_, "factorial", "Operators.factorial", &HValue::operator_factorial, &HOperators::unary_operator, "( *val* ) - return result of *val*__!__ expression" ),
		create_field( runtime_, IDENTIFIER::INTERFACE::SUBSCRIPT, &HOperators::subscript, "( *coll*, *key* ) - return result of *coll*[*key*] expression" ),
		create_field( runtime_, "not", &HOperators::boolean_not, "( *val* ) - return result of __¬__*val* expression" )
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HOperators>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Operators", &operatorsCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

