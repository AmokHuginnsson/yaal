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
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "ansi.hxx"
#include "hterminal.hxx"

using namespace yaal::hconsole;

namespace yaal {

namespace ansi {

HSequence const bold( "\033[1m" );
HSequence const reset( "\033[0m" );
HSequence const black( "\033[0;30m" );
HSequence const red( "\033[0;31m" );
HSequence const green( "\033[0;32m" );
HSequence const brown( "\033[0;33m" );
HSequence const blue( "\033[0;34m" );
HSequence const magenta( "\033[0;35m" );
HSequence const cyan( "\033[0;36m" );
HSequence const lightgray( "\033[0;37m" );
HSequence const gray( "\033[1;30m" );
HSequence const brightred( "\033[1;31m" );
HSequence const brightgreen( "\033[1;32m" );
HSequence const yellow( "\033[1;33m" );
HSequence const brightblue( "\033[1;34m" );
HSequence const brightmagenta( "\033[1;35m" );
HSequence const brightcyan( "\033[1;36m" );
HSequence const white( "\033[1;37m" );

HSequence const home( "\033[H" );
HSequence const up( "\033[A" );
HSequence const down( "\033[B" );
HSequence const left( "\033[D" );
HSequence const right( "\033[C" );

HSequence const save( "\033[s" );
HSequence const restore( "\033[u" );

HSequence const clear( "\033[2J" );
HSequence const clrtoeol( "\033[K" );

HSequence move( int row_, int col_ ) {
	static int const codeBufferSize( 32 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%d;%dH", row_, col_ );
	return ( code );
}

HSequence up_n( int by_ ) {
	static int const codeBufferSize( 16 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%dA", by_ );
	return ( code );
}

HSequence down_n( int by_ ) {
	static int const codeBufferSize( 16 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%dB", by_ );
	return ( code );
}

HSequence left_n( int by_ ) {
	static int const codeBufferSize( 16 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%dD", by_ );
	return ( code );
}

HSequence right_n( int by_ ) {
	static int const codeBufferSize( 16 );
	static char code[codeBufferSize];
	snprintf( code, codeBufferSize - 1, "\033[%dC", by_ );
	return ( code );
}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& stream_, HSequence const& seq_ ) {
	M_PROLOG
	if ( is_a_tty( stream_ ) )
		stream_ << *seq_;
	return ( stream_ );
	M_EPILOG
}

}

}

