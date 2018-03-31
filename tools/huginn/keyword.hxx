/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_KEYWORD_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_KEYWORD_HXX_INCLUDED 1

#include "hcore/hhashset.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

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
extern HHuginn::identifier_id_t const CONSTRUCTOR_IDENTIFIER;
extern HHuginn::identifier_id_t const DESTRUCTOR_IDENTIFIER;
extern HHuginn::identifier_id_t const THIS_IDENTIFIER;
extern HHuginn::identifier_id_t const SUPER_IDENTIFIER;
extern HHuginn::identifier_id_t const ASSERT_IDENTIFIER;

}

namespace BUILTIN {

extern char const* INTEGER;
extern char const* REAL;
extern char const* NUMBER;
extern char const* STRING;
extern char const* CHARACTER;
extern char const* BOOLEAN;
extern char const* SIZE;
extern char const* TYPE;
extern char const* COPY;
extern char const* OBSERVE;
extern char const* USE;
extern char const* TUPLE;
extern char const* LIST;
extern char const* DEQUE;
extern char const* DICT;
extern char const* LOOKUP;
extern char const* ORDER;
extern char const* SET;
extern char const* BLOB;
extern HHuginn::identifier_id_t const INTEGER_IDENTIFIER;
extern HHuginn::identifier_id_t const REAL_IDENTIFIER;
extern HHuginn::identifier_id_t const NUMBER_IDENTIFIER;
extern HHuginn::identifier_id_t const STRING_IDENTIFIER;
extern HHuginn::identifier_id_t const CHARACTER_IDENTIFIER;
extern HHuginn::identifier_id_t const BOOLEAN_IDENTIFIER;
extern HHuginn::identifier_id_t const SIZE_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_IDENTIFIER;
extern HHuginn::identifier_id_t const COPY_IDENTIFIER;
extern HHuginn::identifier_id_t const OBSERVE_IDENTIFIER;
extern HHuginn::identifier_id_t const USE_IDENTIFIER;
extern HHuginn::identifier_id_t const TUPLE_IDENTIFIER;
extern HHuginn::identifier_id_t const LIST_IDENTIFIER;
extern HHuginn::identifier_id_t const DEQUE_IDENTIFIER;
extern HHuginn::identifier_id_t const DICT_IDENTIFIER;
extern HHuginn::identifier_id_t const LOOKUP_IDENTIFIER;
extern HHuginn::identifier_id_t const ORDER_IDENTIFIER;
extern HHuginn::identifier_id_t const SET_IDENTIFIER;
extern HHuginn::identifier_id_t const BLOB_IDENTIFIER;

}

namespace INTERFACE {

extern char const* CLONE;
extern char const* GET_SIZE;
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
extern HHuginn::identifier_id_t const CLONE_IDENTIFIER;
extern HHuginn::identifier_id_t const GET_SIZE_IDENTIFIER;
extern HHuginn::identifier_id_t const ITERATOR_IDENTIFIER;
extern HHuginn::identifier_id_t const VALUE_IDENTIFIER;
extern HHuginn::identifier_id_t const NEXT_IDENTIFIER;
extern HHuginn::identifier_id_t const IS_VALID_IDENTIFIER;
extern HHuginn::identifier_id_t const CALL_IDENTIFIER;
extern HHuginn::identifier_id_t const HASH_IDENTIFIER;
extern HHuginn::identifier_id_t const EQUALS_IDENTIFIER;
extern HHuginn::identifier_id_t const LESS_IDENTIFIER;
extern HHuginn::identifier_id_t const GREATER_IDENTIFIER;
extern HHuginn::identifier_id_t const LESS_OR_EQUAL_IDENTIFIER;
extern HHuginn::identifier_id_t const GREATER_OR_EQUAL_IDENTIFIER;
extern HHuginn::identifier_id_t const ADD_IDENTIFIER;
extern HHuginn::identifier_id_t const SUBTRACT_IDENTIFIER;
extern HHuginn::identifier_id_t const MULTIPLY_IDENTIFIER;
extern HHuginn::identifier_id_t const DIVIDE_IDENTIFIER;
extern HHuginn::identifier_id_t const MODULO_IDENTIFIER;
extern HHuginn::identifier_id_t const POWER_IDENTIFIER;
extern HHuginn::identifier_id_t const MODULUS_IDENTIFIER;
extern HHuginn::identifier_id_t const NEGATE_IDENTIFIER;
extern HHuginn::identifier_id_t const TO_STRING_IDENTIFIER;
extern HHuginn::identifier_id_t const TO_INTEGER_IDENTIFIER;
extern HHuginn::identifier_id_t const TO_REAL_IDENTIFIER;
extern HHuginn::identifier_id_t const TO_NUMBER_IDENTIFIER;
extern HHuginn::identifier_id_t const TO_CHARACTER_IDENTIFIER;
extern HHuginn::identifier_id_t const TO_BOOLEAN_IDENTIFIER;

}

namespace STANDARD_FUNCTIONS {
extern char const* MAIN;
extern HHuginn::identifier_id_t const MAIN_IDENTIFIER;
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

