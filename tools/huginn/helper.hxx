/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  helper.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;

bool is_keyword( yaal::hcore::HString const& );
bool is_builtin( yaal::hcore::HString const& );
bool is_restricted( yaal::hcore::HString const& );

void operands_type_mismatch( char const*, HHuginn::type_id_t, HHuginn::type_id_t, int ) __attribute__(( noreturn ));
void verify_arg_count( yaal::hcore::HString const&, HHuginn::values_t const&, int, int, int );
void verify_arg_type( yaal::hcore::HString const&, HHuginn::values_t const&, int, HHuginn::TYPE, bool, int );
HHuginn::type_id_t verify_arg_numeric( yaal::hcore::HString const&, HHuginn::values_t const&, int, bool, int );
HHuginn::type_id_t verify_arg_collection( yaal::hcore::HString const&, HHuginn::values_t const&, int, bool, int );

HHuginn::HString::value_type const& get_string( HHuginn::value_t const& );
HHuginn::HInteger::value_type get_integer( HHuginn::value_t const& );
HHuginn::HReal::value_type get_real( HHuginn::value_t const& );
HHuginn::HNumber::value_type const& get_number( HHuginn::value_t const& );
HHuginn::HBoolean::value_type get_boolean( HHuginn::value_t const& );
HHuginn::HCharacter::value_type  get_character( HHuginn::value_t const& );

HHuginn::HString::value_type const& get_string( HHuginn::HValue const* );
HHuginn::HInteger::value_type get_integer( HHuginn::HValue const* );
HHuginn::HReal::value_type get_real( HHuginn::HValue const* );
HHuginn::HNumber::value_type const& get_number( HHuginn::HValue const* );
HHuginn::HBoolean::value_type get_boolean( HHuginn::HValue const* );
HHuginn::HCharacter::value_type get_character( HHuginn::HValue const* );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED */

