/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	ansi.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_ANSI_HXX_INCLUDED
#define YAAL_TOOLS_ANSI_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace ansi {

class HSequence {
public:
	typedef HSequence this_type;
private:
	char const* const _data;
public:
	HSequence( char const* data_ )
		: _data( data_ )
		{}
	HSequence( HSequence const& seq_ )
		: _data( seq_._data ) { }
	char const* operator* ( void ) const
		{ return ( _data ); }
private:
	HSequence& operator = ( HSequence const& );
};

extern M_YAAL_TOOLS_PUBLIC_API HSequence const bold;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const reset;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const black;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const red;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const green;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brown;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const blue;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const magenta;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const cyan;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const lightgray;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const gray;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightred;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightgreen;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const yellow;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightblue;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightmagenta;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const brightcyan;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const white;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const up;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const down;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const left;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const right;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const up_bol;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const save;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const restore;

extern M_YAAL_TOOLS_PUBLIC_API HSequence const clear;
extern M_YAAL_TOOLS_PUBLIC_API HSequence const clrtoeol;

HSequence move( int, int );
HSequence up_n( int );
HSequence down_n( int );
HSequence left_n( int );
HSequence right_n( int );
HSequence up_bol_n( int );

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, HSequence const& );

}

}

#endif /* #ifndef YAAL_TOOLS_ANSI_HXX_INCLUDED */

