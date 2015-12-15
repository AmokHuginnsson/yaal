/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  boolean.cxx - this file is integral part of `yaal' project.

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
#include "tools/hhuginn.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace boolean {

HHuginn::class_t get_class( HHuginn* );
HHuginn::class_t get_class( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			type_id( HHuginn::TYPE::BOOLEAN ),
			type_name( HHuginn::TYPE::BOOLEAN ),
			nullptr,
			HHuginn::field_names_t{
			},
			HHuginn::values_t{
			}
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HBoolean::HBoolean( HHuginn::HClass const* class_, HHuginn::HBoolean::value_type value_ )
	: HObject( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::HBoolean::value_type HHuginn::HBoolean::value( void ) const {
	return ( _value );
}

HHuginn::value_t HHuginn::HBoolean::do_clone( HHuginn* huginn_ ) const {
	return ( huginn_->object_factory()->create_boolean( _value ) );
}

}

}

