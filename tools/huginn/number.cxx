/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  number.cxx - this file is integral part of `yaal' project.

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

namespace number {

inline HHuginn::value_t is_exact( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "number.is_exact";
	verify_arg_count( name, values_, 0, 0, position_ );
	return ( thread_->object_factory().create_boolean( static_cast<HHuginn::HNumber*>( object_->raw() )->value().is_exact() ) );
	M_EPILOG
}

inline HHuginn::value_t get_precision( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "number.get_precision";
	verify_arg_count( name, values_, 0, 0, position_ );
	return ( thread_->object_factory().create_integer( static_cast<HHuginn::HNumber*>( object_->raw() )->value().get_precision() ) );
	M_EPILOG
}

inline HHuginn::value_t set_precision( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "number.set_precision";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, true, position_ );
	int precision = static_cast<int>( get_integer( values_[0] ) );
	static_cast<HHuginn::HNumber*>( object_->raw() )->value().set_precision( precision );
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t get_class( HHuginn* );
HHuginn::class_t get_class( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			type_id( HHuginn::TYPE::NUMBER ),
			huginn_->identifier_id( type_name( HHuginn::TYPE::NUMBER ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "is_exact", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &number::is_exact, _1, _2, _3, _4 ) ) },
				{ "get_precision", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &number::get_precision, _1, _2, _3, _4 ) ) },
				{ "set_precision", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &number::set_precision, _1, _2, _3, _4 ) ) }
			}
		)
	);
	return ( c );
	M_EPILOG
}

}

}


HHuginn::HNumber::HNumber( HHuginn::HClass const* class_, HHuginn::HNumber::value_type const& value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HNumber::do_clone( HHuginn* huginn_ ) const {
	return ( huginn_->object_factory()->create_number( _value ) );
}

}

}

