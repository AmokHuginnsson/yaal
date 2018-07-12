/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_OPERATOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_OPERATOR_HXX_INCLUDED 1

#include "hcore/hstack.hxx"

namespace yaal {

namespace tools {

namespace huginn {

/*
 * Order of enumerated operators matters.
 * ASSIGN and *_ASSIGN must be consecutive,
 * starting with ASSIGN and ending with POWER_ASSIGN.
 */
enum class OPERATOR {
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	MODULO,
	POWER,
	POWER_TERM,
	FACTORIAL,
	MODULUS,
	PARENTHESIS,
	ASSIGN,
	ASSIGN_PACK,
	PLUS_ASSIGN,
	MINUS_ASSIGN,
	MULTIPLY_ASSIGN,
	DIVIDE_ASSIGN,
	MODULO_ASSIGN,
	POWER_ASSIGN,
	ASSIGN_TERM,
	SUBSCRIPT,
	RANGE,
	SUBSCRIPT_ARGUMENT,
	NEGATE,
	FUNCTION_CALL,
	FUNCTION_ARGUMENT,
	MEMBER_ACCESS,
	EQUALS,
	NOT_EQUALS,
	LESS,
	GREATER,
	LESS_OR_EQUAL,
	GREATER_OR_EQUAL,
	IS_ELEMENT_OF,
	BOOLEAN_AND,
	BOOLEAN_OR,
	BOOLEAN_XOR,
	BOOLEAN_NOT,
	TERNARY,
	MAKE_DICT,
	MAKE_LOOKUP,
	NONE
};

struct OPositionedOperator {
	OPERATOR _operator;
	int _position;
	OPositionedOperator( OPERATOR operator_, int position_ )
		: _operator( operator_ ), _position( position_ ) {
		return;
	}
};

typedef yaal::hcore::HStack<OPositionedOperator> operations_t;

char const* op_to_str( OPERATOR );
char const* op_to_symbol_str( OPERATOR );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_OPERATOR_HXX_INCLUDED */

