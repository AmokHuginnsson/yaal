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

namespace IDENTIFIER {

HHuginn::identifier_id_t const INVALID( -1 );

namespace KEYWORD {

HHuginn::identifier_id_t const CONSTRUCTOR( INVALID + 1 );
HHuginn::identifier_id_t const DESTRUCTOR( CONSTRUCTOR + 1 );
HHuginn::identifier_id_t const THIS( DESTRUCTOR + 1 );
HHuginn::identifier_id_t const SUPER( THIS + 1 );
HHuginn::identifier_id_t const ASSERT( SUPER + 1 );

}

namespace TYPE {
}

namespace BUILTIN {

HHuginn::identifier_id_t const INTEGER( KEYWORD::ASSERT + 1 );
HHuginn::identifier_id_t const REAL( INTEGER + 1 );
HHuginn::identifier_id_t const NUMBER( REAL + 1 );
HHuginn::identifier_id_t const STRING( NUMBER + 1 );
HHuginn::identifier_id_t const CHARACTER( STRING + 1 );
HHuginn::identifier_id_t const BOOLEAN( CHARACTER + 1 );
HHuginn::identifier_id_t const SIZE( BOOLEAN + 1 );
HHuginn::identifier_id_t const TYPE( SIZE + 1 );
HHuginn::identifier_id_t const COPY( TYPE + 1 );
HHuginn::identifier_id_t const OBSERVE( COPY + 1 );
HHuginn::identifier_id_t const USE( OBSERVE + 1 );
HHuginn::identifier_id_t const TUPLE( USE + 1 );
HHuginn::identifier_id_t const LIST( TUPLE + 1 );
HHuginn::identifier_id_t const DEQUE( LIST + 1 );
HHuginn::identifier_id_t const DICT( DEQUE + 1 );
HHuginn::identifier_id_t const LOOKUP( DICT + 1 );
HHuginn::identifier_id_t const ORDER( LOOKUP + 1 );
HHuginn::identifier_id_t const SET( ORDER + 1 );
HHuginn::identifier_id_t const BLOB( SET + 1 );
HHuginn::identifier_id_t const TYPE_NONE( BLOB + 1 );
HHuginn::identifier_id_t const TYPE_OBSERVER( TYPE_NONE + 1 );
HHuginn::identifier_id_t const TYPE_REFERENCE( TYPE_OBSERVER + 1 );
HHuginn::identifier_id_t const TYPE_FUNCTION_REFERENCE( TYPE_REFERENCE + 1 );
HHuginn::identifier_id_t const TYPE_OBJECT_REFERENCE( TYPE_FUNCTION_REFERENCE + 1 );
HHuginn::identifier_id_t const TYPE_METHOD( TYPE_OBJECT_REFERENCE + 1 );
HHuginn::identifier_id_t const TYPE_UNBOUND_METHOD( TYPE_METHOD + 1 );
HHuginn::identifier_id_t const TYPE_BOUND_METHOD( TYPE_UNBOUND_METHOD + 1 );
HHuginn::identifier_id_t const TYPE_VARIADIC_PARAMETERS( TYPE_BOUND_METHOD + 1 );
HHuginn::identifier_id_t const TYPE_NAMED_PARAMETERS( TYPE_VARIADIC_PARAMETERS + 1 );

}

namespace INTERFACE {

HHuginn::identifier_id_t const CLONE( BUILTIN::TYPE_NAMED_PARAMETERS + 1 );
HHuginn::identifier_id_t const GET_SIZE( CLONE + 1 );
HHuginn::identifier_id_t const SUBSCRIPT( GET_SIZE + 1 );
HHuginn::identifier_id_t const ITERATOR( SUBSCRIPT + 1 );
HHuginn::identifier_id_t const IS_VALID( ITERATOR + 1 );
HHuginn::identifier_id_t const NEXT( IS_VALID + 1 );
HHuginn::identifier_id_t const VALUE( NEXT + 1 );
HHuginn::identifier_id_t const CALL( VALUE + 1 );
HHuginn::identifier_id_t const HASH( CALL + 1 );
HHuginn::identifier_id_t const EQUALS( HASH + 1 );
HHuginn::identifier_id_t const LESS( EQUALS + 1 );
HHuginn::identifier_id_t const GREATER( LESS + 1 );
HHuginn::identifier_id_t const LESS_OR_EQUAL( GREATER + 1 );
HHuginn::identifier_id_t const GREATER_OR_EQUAL( LESS_OR_EQUAL + 1 );
HHuginn::identifier_id_t const ADD( GREATER_OR_EQUAL + 1 );
HHuginn::identifier_id_t const SUBTRACT( ADD + 1 );
HHuginn::identifier_id_t const MULTIPLY( SUBTRACT + 1 );
HHuginn::identifier_id_t const DIVIDE( MULTIPLY + 1 );
HHuginn::identifier_id_t const MODULO( DIVIDE + 1 );
HHuginn::identifier_id_t const POWER( MODULO + 1 );
HHuginn::identifier_id_t const MODULUS( POWER + 1 );
HHuginn::identifier_id_t const NEGATE( MODULUS + 1 );
HHuginn::identifier_id_t const TO_STRING( NEGATE + 1 );
HHuginn::identifier_id_t const TO_INTEGER( TO_STRING + 1 );
HHuginn::identifier_id_t const TO_REAL( TO_INTEGER + 1 );
HHuginn::identifier_id_t const TO_NUMBER( TO_REAL + 1 );
HHuginn::identifier_id_t const TO_CHARACTER( TO_NUMBER + 1 );
HHuginn::identifier_id_t const TO_BOOLEAN( TO_CHARACTER + 1 );

}

namespace STANDARD_FUNCTIONS {

HHuginn::identifier_id_t const MAIN( INTERFACE::TO_BOOLEAN + 1 );

}

namespace BUILTIN {

HHuginn::identifier_id_t const TYPE_UNKNOWN( STANDARD_FUNCTIONS::MAIN + 1 );

}

}

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

}

namespace INTERFACE {

char const* CLONE            = "clone";
char const* GET_SIZE         = "get_size";
char const* SUBSCRIPT        = "subscript";
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

}

namespace STANDARD_FUNCTIONS {

char const* MAIN = "main";

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

