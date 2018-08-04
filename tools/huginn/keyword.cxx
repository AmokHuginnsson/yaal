/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "keyword.hxx"
#include "helper.hxx"
#include "runtime.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::identifier_id_t const INVALID_IDENTIFIER( -1 );

namespace KEYWORD {

char const* ASSERT = "assert";
char const* CONSTRUCTOR = "constructor";
char const* DESTRUCTOR = "destructor";
char const* BREAK = "break";
char const* CASE = "case";
char const* CATCH = "catch";
char const* CLASS = "class";
char const* CONTINUE = "continue";
char const* DEFAULT = "default";
char const* ELSE = "else";
char const* ENUM = "enum";
char const* FALSE = "false";
char const* FOR = "for";
char const* IF = "if";
char const* NONE = "none";
char const* RETURN = "return";
char const* SUPER = "super";
char const* SWITCH = "switch";
char const* THIS = "this";
char const* THROW = "throw";
char const* TRUE = "true";
char const* TRY = "try";
char const* WHILE = "while";
HHuginn::identifier_id_t const CONSTRUCTOR_IDENTIFIER( INVALID_IDENTIFIER + 1 );
HHuginn::identifier_id_t const DESTRUCTOR_IDENTIFIER( CONSTRUCTOR_IDENTIFIER + 1 );
HHuginn::identifier_id_t const THIS_IDENTIFIER( DESTRUCTOR_IDENTIFIER + 1 );
HHuginn::identifier_id_t const SUPER_IDENTIFIER( THIS_IDENTIFIER + 1 );
HHuginn::identifier_id_t const ASSERT_IDENTIFIER( SUPER_IDENTIFIER + 1 );

}

namespace BUILTIN {

char const* INTEGER   = "integer";
char const* REAL      = "real";
char const* NUMBER    = "number";
char const* STRING    = "string";
char const* CHARACTER = "character";
char const* BOOLEAN   = "boolean";
char const* SIZE      = "size";
char const* TYPE      = "type";
char const* COPY      = "copy";
char const* OBSERVE   = "observe";
char const* USE       = "use";
char const* TUPLE     = "tuple";
char const* LIST      = "list";
char const* DEQUE     = "deque";
char const* DICT      = "dict";
char const* LOOKUP    = "lookup";
char const* ORDER     = "order";
char const* SET       = "set";
char const* BLOB      = "blob";
char const* TYPE_NONE                = "*none*";
char const* TYPE_OBSERVER            = "*observer*";
char const* TYPE_REFERENCE           = "*reference*";
char const* TYPE_FUNCTION_REFERENCE  = "*function_reference*";
char const* TYPE_OBJECT_REFERENCE    = "*object_reference*";
char const* TYPE_METHOD              = "*method*";
char const* TYPE_UNBOUND_METHOD      = "*unbound_method*";
char const* TYPE_BOUND_METHOD        = "*bound_method*";
char const* TYPE_VARIADIC_PARAMETERS = "*variadic_parameters*";
char const* TYPE_NAMED_PARAMETERS    = "*named_parameters*";
char const* TYPE_UNKNOWN             = "*unknown*";
HHuginn::identifier_id_t const INTEGER_IDENTIFIER( KEYWORD::ASSERT_IDENTIFIER + 1 );
HHuginn::identifier_id_t const REAL_IDENTIFIER( INTEGER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const NUMBER_IDENTIFIER( REAL_IDENTIFIER + 1 );
HHuginn::identifier_id_t const STRING_IDENTIFIER( NUMBER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const CHARACTER_IDENTIFIER( STRING_IDENTIFIER + 1 );
HHuginn::identifier_id_t const BOOLEAN_IDENTIFIER( CHARACTER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const SIZE_IDENTIFIER( BOOLEAN_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_IDENTIFIER( SIZE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const COPY_IDENTIFIER( TYPE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const OBSERVE_IDENTIFIER( COPY_IDENTIFIER + 1 );
HHuginn::identifier_id_t const USE_IDENTIFIER( OBSERVE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TUPLE_IDENTIFIER( USE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const LIST_IDENTIFIER( TUPLE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const DEQUE_IDENTIFIER( LIST_IDENTIFIER + 1 );
HHuginn::identifier_id_t const DICT_IDENTIFIER( DEQUE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const LOOKUP_IDENTIFIER( DICT_IDENTIFIER + 1 );
HHuginn::identifier_id_t const ORDER_IDENTIFIER( LOOKUP_IDENTIFIER + 1 );
HHuginn::identifier_id_t const SET_IDENTIFIER( ORDER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const BLOB_IDENTIFIER( SET_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_NONE_IDENTIFIER( BLOB_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_OBSERVER_IDENTIFIER( TYPE_NONE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_REFERENCE_IDENTIFIER( TYPE_OBSERVER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_FUNCTION_REFERENCE_IDENTIFIER( TYPE_REFERENCE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_OBJECT_REFERENCE_IDENTIFIER( TYPE_FUNCTION_REFERENCE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_METHOD_IDENTIFIER( TYPE_OBJECT_REFERENCE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_UNBOUND_METHOD_IDENTIFIER( TYPE_METHOD_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_BOUND_METHOD_IDENTIFIER( TYPE_UNBOUND_METHOD_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_VARIADIC_PARAMETERS_IDENTIFIER( TYPE_BOUND_METHOD_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TYPE_NAMED_PARAMETERS_IDENTIFIER( TYPE_VARIADIC_PARAMETERS_IDENTIFIER + 1 );

}

namespace INTERFACE {

char const* CLONE            = "clone";
char const* GET_SIZE         = "get_size";
char const* ITERATOR         = "iterator";
char const* IS_VALID         = "is_valid";
char const* NEXT             = "next";
char const* VALUE            = "value";
char const* CALL             = "call";
char const* HASH             = "hash";
char const* EQUALS           = "equals";
char const* LESS             = "less";
char const* GREATER          = "greater";
char const* LESS_OR_EQUAL    = "less_or_equal";
char const* GREATER_OR_EQUAL = "greater_or_equal";
char const* ADD              = "add";
char const* SUBTRACT         = "subtract";
char const* MULTIPLY         = "multiply";
char const* DIVIDE           = "divide";
char const* MODULO           = "modulo";
char const* POWER            = "power";
char const* MODULUS          = "modulus";
char const* NEGATE           = "negate";
char const* TO_STRING        = "to_string";
char const* TO_INTEGER       = "to_integer";
char const* TO_REAL          = "to_real";
char const* TO_NUMBER        = "to_number";
char const* TO_CHARACTER     = "to_character";
char const* TO_BOOLEAN       = "to_boolean";
HHuginn::identifier_id_t const CLONE_IDENTIFIER( BUILTIN::TYPE_NAMED_PARAMETERS_IDENTIFIER + 1 );
HHuginn::identifier_id_t const GET_SIZE_IDENTIFIER( CLONE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const ITERATOR_IDENTIFIER( GET_SIZE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const IS_VALID_IDENTIFIER( ITERATOR_IDENTIFIER + 1 );
HHuginn::identifier_id_t const NEXT_IDENTIFIER( IS_VALID_IDENTIFIER + 1 );
HHuginn::identifier_id_t const VALUE_IDENTIFIER( NEXT_IDENTIFIER + 1 );
HHuginn::identifier_id_t const CALL_IDENTIFIER( VALUE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const HASH_IDENTIFIER( CALL_IDENTIFIER + 1 );
HHuginn::identifier_id_t const EQUALS_IDENTIFIER( HASH_IDENTIFIER + 1 );
HHuginn::identifier_id_t const LESS_IDENTIFIER( EQUALS_IDENTIFIER + 1 );
HHuginn::identifier_id_t const GREATER_IDENTIFIER( LESS_IDENTIFIER + 1 );
HHuginn::identifier_id_t const LESS_OR_EQUAL_IDENTIFIER( GREATER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const GREATER_OR_EQUAL_IDENTIFIER( LESS_OR_EQUAL_IDENTIFIER + 1 );
HHuginn::identifier_id_t const ADD_IDENTIFIER( GREATER_OR_EQUAL_IDENTIFIER + 1 );
HHuginn::identifier_id_t const SUBTRACT_IDENTIFIER( ADD_IDENTIFIER + 1 );
HHuginn::identifier_id_t const MULTIPLY_IDENTIFIER( SUBTRACT_IDENTIFIER + 1 );
HHuginn::identifier_id_t const DIVIDE_IDENTIFIER( MULTIPLY_IDENTIFIER + 1 );
HHuginn::identifier_id_t const MODULO_IDENTIFIER( DIVIDE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const POWER_IDENTIFIER( MODULO_IDENTIFIER + 1 );
HHuginn::identifier_id_t const MODULUS_IDENTIFIER( POWER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const NEGATE_IDENTIFIER( MODULUS_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TO_STRING_IDENTIFIER( NEGATE_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TO_INTEGER_IDENTIFIER( TO_STRING_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TO_REAL_IDENTIFIER( TO_INTEGER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TO_NUMBER_IDENTIFIER( TO_REAL_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TO_CHARACTER_IDENTIFIER( TO_NUMBER_IDENTIFIER + 1 );
HHuginn::identifier_id_t const TO_BOOLEAN_IDENTIFIER( TO_CHARACTER_IDENTIFIER + 1 );

}

namespace STANDARD_FUNCTIONS {

char const* MAIN = "main";
HHuginn::identifier_id_t const MAIN_IDENTIFIER( INTERFACE::TO_BOOLEAN_IDENTIFIER + 1 );

}

namespace BUILTIN {

HHuginn::identifier_id_t const TYPE_UNKNOWN_IDENTIFIER( STANDARD_FUNCTIONS::MAIN_IDENTIFIER + 1 );

}


words_t _directives_ = {{
	"import"
}};

words_t _keywords_ = {{
	KEYWORD::ASSERT,
	KEYWORD::BREAK,
	KEYWORD::CASE,
	KEYWORD::CATCH,
	KEYWORD::CLASS,
	KEYWORD::CONSTRUCTOR,
	KEYWORD::CONTINUE,
	KEYWORD::DEFAULT,
	KEYWORD::DESTRUCTOR,
	KEYWORD::ELSE,
	KEYWORD::ENUM,
	KEYWORD::FALSE,
	KEYWORD::FOR,
	KEYWORD::IF,
	KEYWORD::NONE,
	KEYWORD::RETURN,
	KEYWORD::SUPER,
	KEYWORD::SWITCH,
	KEYWORD::THIS,
	KEYWORD::THROW,
	KEYWORD::TRUE,
	KEYWORD::TRY,
	KEYWORD::WHILE
}};

words_t _builtin_ = {{
	"assert", /* for VM dump */
	type_name( HHuginn::TYPE::BLOB ),
	type_name( HHuginn::TYPE::BOOLEAN ),
	type_name( HHuginn::TYPE::CHARACTER ),
	type_name( HHuginn::TYPE::DEQUE ),
	type_name( HHuginn::TYPE::DICT ),
	type_name( HHuginn::TYPE::INTEGER ),
	type_name( HHuginn::TYPE::TUPLE ),
	type_name( HHuginn::TYPE::LIST ),
	type_name( HHuginn::TYPE::LOOKUP ),
	type_name( HHuginn::TYPE::ORDER ),
	type_name( HHuginn::TYPE::NUMBER ),
	type_name( HHuginn::TYPE::REAL ),
	type_name( HHuginn::TYPE::SET ),
	type_name( HHuginn::TYPE::STRING ),
	BUILTIN::TYPE,
	BUILTIN::SIZE,
	BUILTIN::COPY,
	BUILTIN::OBSERVE,
	BUILTIN::USE
}};

words_t _standardLibrary_ = {{
	"Exception",
	"print",
	"input"
}};

}

}

}

