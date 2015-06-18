/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  keyword.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_KEYWORD_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_KEYWORD_HXX_INCLUDED 1

#include "hcore/hhashset.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace KEYWORD {

extern char const* CONSTRUCTOR;
extern char const* DESTRUCTOR;
extern char const* BREAK;
extern char const* CASE;
extern char const* CATCH;
extern char const* CLASS;
extern char const* CONTINUE;
extern char const* DEFAULT;
extern char const* ELSE;
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

typedef yaal::hcore::HHashSet<yaal::hcore::HString>  words_t;

extern words_t _directives_;
extern words_t _keywords_;
extern words_t _builtin_;
extern words_t _standardLibrary_;

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_KEYWORD_HXX_INCLUDED */
