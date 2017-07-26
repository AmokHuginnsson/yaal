/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  color.hxx - this file is integral part of `yaal' project.

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

/*! \file color.hxx
 * \brief Declaration COLOR definition.
 */

#ifndef YAAL_TOOLS_COLOR_HXX_INCLUDED
#define YAAL_TOOLS_COLOR_HXX_INCLUDED 1

#include "tools/ansi.hxx"

namespace yaal {

namespace tools {

/*! \brief Color palette definition.
 */
struct COLOR {
	typedef enum {
		FG_BLACK         = 0,
		FG_RED           = 1,
		FG_GREEN         = 2,
		FG_BROWN         = 3,
		FG_BLUE          = 4,
		FG_MAGENTA       = 5,
		FG_CYAN          = 6,
		FG_LIGHTGRAY     = 7,
		FG_BOLD          = 8,
		FG_GRAY          = FG_BLACK     | FG_BOLD,
		FG_BRIGHTRED     = FG_RED       | FG_BOLD,
		FG_BRIGHTGREEN   = FG_GREEN     | FG_BOLD,
		FG_YELLOW        = FG_BROWN     | FG_BOLD,
		FG_BRIGHTBLUE    = FG_BLUE      | FG_BOLD,
		FG_BRIGHTMAGENTA = FG_MAGENTA   | FG_BOLD,
		FG_BRIGHTCYAN    = FG_CYAN      | FG_BOLD,
		FG_WHITE         = FG_LIGHTGRAY | FG_BOLD,
		FG_MASK          = 0x07,

		BG_BLACK         = 0,
		BG_RED           = 16,
		BG_GREEN         = 32,
		BG_BROWN         = 48,
		BG_BLUE          = 64,
		BG_MAGENTA       = 80,
		BG_CYAN          = 96,
		BG_LIGHTGRAY     = 112,
		BG_BLINK         = 128,
		BG_GRAY          = BG_BLACK     | BG_BLINK,
		BG_BRIGHTRED     = BG_RED       | BG_BLINK,
		BG_BRIGHTGREEN   = BG_GREEN     | BG_BLINK,
		BG_YELLOW        = BG_BROWN     | BG_BLINK,
		BG_BRIGHTBLUE    = BG_BLUE      | BG_BLINK,
		BG_BRIGHTMAGENTA = BG_MAGENTA   | BG_BLINK,
		BG_BRIGHTCYAN    = BG_CYAN      | BG_BLINK,
		BG_WHITE         = BG_LIGHTGRAY | BG_BLINK,
		BG_MASK          = 0x70,
		ATTR_NORMAL      = ( FG_LIGHTGRAY | BG_BLACK ),
		ATTR_DEFAULT     = -1
	} color_t;
	static color_t complementary( color_t );
	static color_t combine( color_t, color_t );
	static color_t fg_to_bg( color_t );
	static color_t from_string( yaal::hcore::HString const& );
	static yaal::ansi::HSequence const& to_ansi( color_t );
};

typedef yaal::hcore::HExceptionT<COLOR> HColorException;

}

}

#endif /* #ifndef YAAL_TOOLS_COLOR_HXX_INCLUDED */

