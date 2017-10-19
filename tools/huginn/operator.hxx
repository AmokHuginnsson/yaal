/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  operator.hxx - this file is integral part of `yaal' project.

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
	ABSOLUTE,
	PARENTHESIS,
	ASSIGN,
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

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_OPERATOR_HXX_INCLUDED */

