/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  value_builtin.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_GRAMMAR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_GRAMMAR_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/frame.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace value_builtin {

HHuginn::value_t subscript( HThread*, HFrame::ACCESS, HHuginn::value_t&, HHuginn::value_t const&, int );
HHuginn::value_t range( HThread*, HHuginn::value_t&, HHuginn::value_t const&, HHuginn::value_t const&, HHuginn::value_t const&, int );
void add( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void sub( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void mul( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void div( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void mod( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void pow( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
HHuginn::value_t abs( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t neg( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t factorial( HThread*, HHuginn::value_t const&, int );
bool equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
int long hash( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t boolean_xor( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
HHuginn::value_t boolean_not( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t string( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t integer( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t real( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t boolean( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t character( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t number( HThread*, HHuginn::value_t const&, int );
yaal::hcore::HString string_representation( HThread*, HHuginn::value_t const&, int );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_GRAMMAR_HXX_INCLUDED */

