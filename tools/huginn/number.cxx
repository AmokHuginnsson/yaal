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
#include "runtime.hxx"
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

inline HHuginn::value_t is_integral( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "number.is_integral";
	verify_arg_count( name, values_, 0, 0, position_ );
	return ( thread_->object_factory().create_boolean( static_cast<HHuginn::HNumber*>( object_->raw() )->value().is_integral() ) );
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
	verify_signature( "number.set_precision", values_, { HHuginn::TYPE::INTEGER }, position_ );
	int precision = static_cast<int>( get_integer( values_[0] ) );
	static_cast<HHuginn::HNumber*>( object_->raw() )->value().set_precision( precision );
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::NUMBER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::NUMBER ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "is_exact",      objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &number::is_exact, _1, _2, _3, _4 ) ),      "tell if this number is an exact representation" },
				{ "is_integral",   objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &number::is_integral, _1, _2, _3, _4 ) ),   "tell if this number does not have any decimal digits" },
				{ "get_precision", objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &number::get_precision, _1, _2, _3, _4 ) ), "get maximum precision of this number" },
				{ "set_precision", objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &number::set_precision, _1, _2, _3, _4 ) ), "( *prec* ) set maximum precision of this number to *prec* (increasing precision works only on numbers that currently have exact representation)" }
			},
			"The `number` is a scalar type that is used to represent and operate on fixed point numbers of arbitrary size. "
			"It supports basic operations of addition, subtraction, multiplication, division, modulo, power, factorial and comparisons, "
			"it can also be used as an argument in functions and algorithms from Mathematics package. "
			"The range of possible values it can hold is limited only by size of your hardware resources."
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

HHuginn::value_t HHuginn::HNumber::do_clone( huginn::HThread* thread_, int ) const {
	return ( thread_->runtime().object_factory()->create_number( _value ) );
}

}

}

