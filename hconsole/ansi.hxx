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

#ifndef YAAL_HCONSOLE_ANSI_HXX_INCLUDED
#define YAAL_HCONSOLE_ANSI_HXX_INCLUDED 1

namespace yaal {

namespace ansi {

extern M_YAAL_HCONSOLE_PUBLIC_API char const* const bold;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const reset;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const black;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const red;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const green;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brown;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const blue;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const magenta;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const cyan;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const lightgray;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const gray;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightred;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightgreen;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const yellow;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightblue;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightmagenta;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const brightcyan;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const white;

extern M_YAAL_HCONSOLE_PUBLIC_API char const* const up;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const down;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const left;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const right;

extern M_YAAL_HCONSOLE_PUBLIC_API char const* const save;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const restore;

extern M_YAAL_HCONSOLE_PUBLIC_API char const* const clear;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const clrtoeol;

char const* move( int, int );
char const* up_n( int );
char const* down_n( int );
char const* left_n( int );
char const* right_n( int );

}

}

#endif /* #ifndef YAAL_HCONSOLE_ANSI_HXX_INCLUDED */

