/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  keyword.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "keyword.hxx"
#include "helper.hxx"
#include "runtime.hxx"

namespace yaal {

namespace tools {

namespace huginn {

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
HHuginn::identifier_id_t const CONSTRUCTOR_IDENTIFIER( 0 );
HHuginn::identifier_id_t const DESTRUCTOR_IDENTIFIER( 1 );
HHuginn::identifier_id_t const THIS_IDENTIFIER( 2 );
HHuginn::identifier_id_t const SUPER_IDENTIFIER( 3 );
HHuginn::identifier_id_t const ASSERT_IDENTIFIER( 4 );

}

namespace BUILTIN {

char const* SIZE = "size";
char const* TYPE = "type";
char const* COPY = "copy";
char const* OBSERVE = "observe";
char const* USE = "use";
HHuginn::identifier_id_t const INTEGER_IDENTIFIER( 5 );
HHuginn::identifier_id_t const REAL_IDENTIFIER( 6 );
HHuginn::identifier_id_t const NUMBER_IDENTIFIER( 7 );
HHuginn::identifier_id_t const STRING_IDENTIFIER( 8 );
HHuginn::identifier_id_t const CHARACTER_IDENTIFIER( 9 );
HHuginn::identifier_id_t const BOOLEAN_IDENTIFIER( 10 );
HHuginn::identifier_id_t const SIZE_IDENTIFIER( 11 );
HHuginn::identifier_id_t const TYPE_IDENTIFIER( 12 );
HHuginn::identifier_id_t const COPY_IDENTIFIER( 13 );
HHuginn::identifier_id_t const OBSERVE_IDENTIFIER( 14 );
HHuginn::identifier_id_t const USE_IDENTIFIER( 15 );
HHuginn::identifier_id_t const LIST_IDENTIFIER( 16 );
HHuginn::identifier_id_t const DEQUE_IDENTIFIER( 17 );
HHuginn::identifier_id_t const DICT_IDENTIFIER( 18 );
HHuginn::identifier_id_t const LOOKUP_IDENTIFIER( 19 );
HHuginn::identifier_id_t const ORDER_IDENTIFIER( 20 );
HHuginn::identifier_id_t const SET_IDENTIFIER( 21 );

}

namespace INTERFACE {

char const* GET_SIZE = "get_size";
char const* ITERATOR = "iterator";
char const* IS_VALID = "is_valid";
char const* NEXT = "next";
char const* VALUE = "value";
char const* HASH = "hash";
char const* EQUALS = "equals";
char const* LESS = "less";
char const* GREATER = "greater";
char const* LESS_OR_EQUAL = "less_or_equal";
char const* GREATER_OR_EQUAL = "greater_or_equal";
char const* ADD = "add";
char const* SUBTRACT = "subtract";
char const* MULTIPLY = "multiply";
char const* DIVIDE = "divide";
char const* MODULO = "modulo";
char const* POWER = "power";

}

namespace STANDARD_FUNCTIONS {

char const* MAIN = "main";
HHuginn::identifier_id_t const MAIN_IDENTIFIER( 22 );

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
	type_name( HHuginn::TYPE::BOOLEAN ),
	type_name( HHuginn::TYPE::CHARACTER ),
	type_name( HHuginn::TYPE::DEQUE ),
	type_name( HHuginn::TYPE::DICT ),
	type_name( HHuginn::TYPE::INTEGER ),
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

