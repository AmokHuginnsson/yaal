/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	ansi.cxx - this file is integral part of `yaal' project.

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

#include <cstdio>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "ansi.hxx"
#include "hterminal.hxx"

using namespace yaal::hconsole;

namespace yaal {

namespace ansi {

namespace {
char const EMPTY_STRING[] = "";
}
char const* const bold = _terminal_.exists() ? "\033[1m" : EMPTY_STRING;
char const* const reset = _terminal_.exists() ? "\033[0m" : EMPTY_STRING;
char const* const black = _terminal_.exists() ? "\033[0;30m" : EMPTY_STRING;
char const* const red = _terminal_.exists() ? "\033[0;31m" : EMPTY_STRING;
char const* const green = _terminal_.exists() ? "\033[0;32m" : EMPTY_STRING;
char const* const brown = _terminal_.exists() ? "\033[0;33m" : EMPTY_STRING;
char const* const blue = _terminal_.exists() ? "\033[0;34m" : EMPTY_STRING;
char const* const magenta = _terminal_.exists() ? "\033[0;35m" : EMPTY_STRING;
char const* const cyan = _terminal_.exists() ? "\033[0;36m" : EMPTY_STRING;
char const* const lightgray = _terminal_.exists() ? "\033[0;37m" : EMPTY_STRING;
char const* const gray = _terminal_.exists() ? "\033[1;30m" : EMPTY_STRING;
char const* const brightred = _terminal_.exists() ? "\033[1;31m" : EMPTY_STRING;
char const* const brightgreen = _terminal_.exists() ? "\033[1;32m" : EMPTY_STRING;
char const* const yellow = _terminal_.exists() ? "\033[1;33m" : EMPTY_STRING;
char const* const brightblue = _terminal_.exists() ? "\033[1;34m" : EMPTY_STRING;
char const* const brightmagenta = _terminal_.exists() ? "\033[1;35m" : EMPTY_STRING;
char const* const brightcyan = _terminal_.exists() ? "\033[1;36m" : EMPTY_STRING;
char const* const white = _terminal_.exists() ? "\033[1;37m" : EMPTY_STRING;

char const* const home = _terminal_.exists() ? "\033[H" : EMPTY_STRING;
char const* const up = _terminal_.exists() ? "\033[A" : EMPTY_STRING;
char const* const down = _terminal_.exists() ? "\033[B" : EMPTY_STRING;
char const* const left = _terminal_.exists() ? "\033[D" : EMPTY_STRING;
char const* const right = _terminal_.exists() ? "\033[C" : EMPTY_STRING;

char const* const save = _terminal_.exists() ? "\033[s" : EMPTY_STRING;
char const* const restore = _terminal_.exists() ? "\033[u" : EMPTY_STRING;

char const* const clear = _terminal_.exists() ? "\033[2J" : EMPTY_STRING;
char const* const clrtoeol = _terminal_.exists() ? "\033[K" : EMPTY_STRING;

char const* move( int row_, int col_ ) {
	static int const codeBufferSize( 32 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%d;%dH", row_, col_ );
	return ( code );
}

char const* up_n( int by_ ) {
	static int const codeBufferSize( 16 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%dA", by_ );
	return ( code );
}

char const* down_n( int by_ ) {
	static int const codeBufferSize( 16 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%dB", by_ );
	return ( code );
}

char const* left_n( int by_ ) {
	static int const codeBufferSize( 16 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%dD", by_ );
	return ( code );
}

char const* right_n( int by_ ) {
	static int const codeBufferSize( 16 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%dC", by_ );
	return ( code );
}

}

}

