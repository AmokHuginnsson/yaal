/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_KEYWORD_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_KEYWORD_HXX_INCLUDED 1

#include "hcore/hhashset.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace IDENTIFIER {

extern HHuginn::identifier_id_t const INVALID;

namespace KEYWORD {

extern HHuginn::identifier_id_t const CONSTRUCTOR;
extern HHuginn::identifier_id_t const DESTRUCTOR;
extern HHuginn::identifier_id_t const THIS;
extern HHuginn::identifier_id_t const SUPER;
extern HHuginn::identifier_id_t const ASSERT;

}

namespace BUILTIN {

extern HHuginn::identifier_id_t const INTEGER;
extern HHuginn::identifier_id_t const REAL;
extern HHuginn::identifier_id_t const NUMBER;
extern HHuginn::identifier_id_t const STRING;
extern HHuginn::identifier_id_t const CHARACTER;
extern HHuginn::identifier_id_t const BOOLEAN;
extern HHuginn::identifier_id_t const TUPLE;
extern HHuginn::identifier_id_t const LIST;
extern HHuginn::identifier_id_t const DEQUE;
extern HHuginn::identifier_id_t const DICT;
extern HHuginn::identifier_id_t const LOOKUP;
extern HHuginn::identifier_id_t const ORDER;
extern HHuginn::identifier_id_t const SET;
extern HHuginn::identifier_id_t const BLOB;
extern HHuginn::identifier_id_t const TYPE_NONE;
extern HHuginn::identifier_id_t const TYPE_OBSERVER;
extern HHuginn::identifier_id_t const TYPE_REFERENCE;
extern HHuginn::identifier_id_t const TYPE_FUNCTION_REFERENCE;
extern HHuginn::identifier_id_t const TYPE_OBJECT_REFERENCE;
extern HHuginn::identifier_id_t const TYPE_METHOD;
extern HHuginn::identifier_id_t const TYPE_UNBOUND_METHOD;
extern HHuginn::identifier_id_t const TYPE_BOUND_METHOD;
extern HHuginn::identifier_id_t const TYPE_VARIADIC_PARAMETERS;
extern HHuginn::identifier_id_t const TYPE_NAMED_PARAMETERS;
extern HHuginn::identifier_id_t const TYPE_UNKNOWN;
extern HHuginn::identifier_id_t const SIZE;
extern HHuginn::identifier_id_t const TYPE;
extern HHuginn::identifier_id_t const COPY;
extern HHuginn::identifier_id_t const OBSERVE;
extern HHuginn::identifier_id_t const USE;
extern HHuginn::identifier_id_t const SQUARE_ROOT;

}

namespace INTERFACE {

extern HHuginn::identifier_id_t const CLONE;
extern HHuginn::identifier_id_t const GET_SIZE;
extern HHuginn::identifier_id_t const SUBSCRIPT;
extern HHuginn::identifier_id_t const ITERATOR;
extern HHuginn::identifier_id_t const VALUE;
extern HHuginn::identifier_id_t const NEXT;
extern HHuginn::identifier_id_t const IS_VALID;
extern HHuginn::identifier_id_t const CALL;
extern HHuginn::identifier_id_t const HASH;
extern HHuginn::identifier_id_t const EQUALS;
extern HHuginn::identifier_id_t const LESS;
extern HHuginn::identifier_id_t const GREATER;
extern HHuginn::identifier_id_t const LESS_OR_EQUAL;
extern HHuginn::identifier_id_t const GREATER_OR_EQUAL;
extern HHuginn::identifier_id_t const ADD;
extern HHuginn::identifier_id_t const SUBTRACT;
extern HHuginn::identifier_id_t const MULTIPLY;
extern HHuginn::identifier_id_t const DIVIDE;
extern HHuginn::identifier_id_t const MODULO;
extern HHuginn::identifier_id_t const POWER;
extern HHuginn::identifier_id_t const MODULUS;
extern HHuginn::identifier_id_t const NEGATE;
extern HHuginn::identifier_id_t const TO_STRING;
extern HHuginn::identifier_id_t const TO_INTEGER;
extern HHuginn::identifier_id_t const TO_REAL;
extern HHuginn::identifier_id_t const TO_NUMBER;
extern HHuginn::identifier_id_t const TO_CHARACTER;
extern HHuginn::identifier_id_t const TO_BOOLEAN;

}

namespace STANDARD_FUNCTIONS {

extern HHuginn::identifier_id_t const MAIN;

}

}

namespace KEYWORD {

extern char const* ASSERT;
extern char const* CONSTRUCTOR;
extern char const* DESTRUCTOR;
extern char const* BREAK;
extern char const* CASE;
extern char const* CATCH;
extern char const* CLASS;
extern char const* CONTINUE;
extern char const* DEFAULT;
extern char const* ELSE;
extern char const* ENUM;
extern char const* FALSE;
extern char const* FOR;
extern char const* IF;
extern char const* NONE;
extern char const* RETURN;
extern char const* SUPER;
extern char const* SWITCH;
extern char const* THIS;
extern char const* THROW;
extern char const* TRUE;
extern char const* TRY;
extern char const* WHILE;

}

namespace BUILTIN {

extern char const* INTEGER;
extern char const* REAL;
extern char const* NUMBER;
extern char const* STRING;
extern char const* CHARACTER;
extern char const* BOOLEAN;
extern char const* TUPLE;
extern char const* LIST;
extern char const* DEQUE;
extern char const* DICT;
extern char const* LOOKUP;
extern char const* ORDER;
extern char const* SET;
extern char const* BLOB;
extern char const* TYPE_NONE;
extern char const* TYPE_OBSERVER;
extern char const* TYPE_REFERENCE;
extern char const* TYPE_FUNCTION_REFERENCE;
extern char const* TYPE_OBJECT_REFERENCE;
extern char const* TYPE_METHOD;
extern char const* TYPE_UNBOUND_METHOD;
extern char const* TYPE_BOUND_METHOD;
extern char const* TYPE_VARIADIC_PARAMETERS;
extern char const* TYPE_NAMED_PARAMETERS;
extern char const* TYPE_UNKNOWN;
extern char const* SIZE;
extern char const* TYPE;
extern char const* COPY;
extern char const* OBSERVE;
extern char const* USE;
extern char const* SQUARE_ROOT;

}

namespace INTERFACE {

extern char const* CLONE;
extern char const* GET_SIZE;
extern char const* SUBSCRIPT;
extern char const* ITERATOR;
extern char const* VALUE;
extern char const* NEXT;
extern char const* IS_VALID;
extern char const* CALL;
extern char const* HASH;
extern char const* EQUALS;
extern char const* LESS;
extern char const* GREATER;
extern char const* LESS_OR_EQUAL;
extern char const* GREATER_OR_EQUAL;
extern char const* ADD;
extern char const* SUBTRACT;
extern char const* MULTIPLY;
extern char const* DIVIDE;
extern char const* MODULO;
extern char const* POWER;
extern char const* MODULUS;
extern char const* NEGATE;
extern char const* TO_STRING;
extern char const* TO_INTEGER;
extern char const* TO_REAL;
extern char const* TO_NUMBER;
extern char const* TO_CHARACTER;
extern char const* TO_BOOLEAN;

}

namespace STANDARD_FUNCTIONS {

extern char const* MAIN;

}

typedef yaal::hcore::HHashSet<yaal::hcore::HString>  words_t;

extern words_t _directives_;
extern words_t _keywords_;
extern words_t _builtin_;
extern words_t _standardLibrary_;

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_KEYWORD_HXX_INCLUDED */

