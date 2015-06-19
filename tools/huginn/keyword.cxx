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
	"boolean",
	"character",
	"integer",
	"list",
	"map",
	"number",
	"real",
	"set",
	"string",
	"type",
	"size",
	"copy"
}};

words_t _standardLibrary_ = {{
	"file",
	"print",
	"input"
}};

}

}

}

