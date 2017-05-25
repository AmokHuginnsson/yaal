/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  character.cxx - this file is integral part of `yaal' project.

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

#include <cctype>

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

namespace character {

inline HHuginn::value_t to_lower( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "character.to_lower";
	verify_arg_count( name, values_, 0, 0, position_ );
	HHuginn::HCharacter* c( static_cast<HHuginn::HCharacter*>( object_->raw() ) );
	c->set( HHuginn::HCharacter::value_type( static_cast<yaal::u32_t>( std::tolower( static_cast<int>( c->value().get() ) ) ) ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t to_upper( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "character.to_upper";
	verify_arg_count( name, values_, 0, 0, position_ );
	HHuginn::HCharacter* c( static_cast<HHuginn::HCharacter*>( object_->raw() ) );
	c->set( HHuginn::HCharacter::value_type( static_cast<yaal::u32_t>( std::toupper( static_cast<int>( c->value().get() ) ) ) ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t is_of_a_kind( char const* name, bool (*isofakind)( code_point_t ), huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name, values_, 0, 0, position_ );
	HHuginn::HCharacter* c( static_cast<HHuginn::HCharacter*>( object_->raw() ) );
	return ( thread_->object_factory().create_boolean( isofakind( c->value() ) != 0 ) );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG

	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::CHARACTER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::CHARACTER ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "to_lower",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::to_lower, _1, _2, _3, _4 ) ), "make this character lower case" },
				{ "to_upper",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::to_upper, _1, _2, _3, _4 ) ), "make this character upper case" },
				{ "is_upper",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_upper",  hcore::is_upper, _1, _2, _3, _4 ) ),      "tell if this character is an upper case character" },
				{ "is_lower",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_lower",  hcore::is_lower, _1, _2, _3, _4 ) ),      "tell if this character is a lower case character" },
				{ "is_digit",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_digit",  hcore::is_digit, _1, _2, _3, _4 ) ),      "tell if this character represents decimal digit" },
				{ "is_xdigit", objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_xdigit", hcore::is_hex_digit, _1, _2, _3, _4 ) ),  "tell if this character represents a hexadecimal digit" },
				{ "is_space",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_space",  hcore::is_whitespace, _1, _2, _3, _4 ) ), "tell if this character represents a white space character" },
				{ "is_alpha",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_alpha",  hcore::is_alpha, _1, _2, _3, _4 ) ),      "tell if this character represents an alphabet character" },
				{ "is_alnum",  objectFactory_->create<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_alnum",  hcore::is_alnum, _1, _2, _3, _4 ) ),      "tell if this character represents any alphanumeric character" }
			},
			"The `character` is a scalar type that is used to represent and operate on single characters. "
			"It supports basic operations of comparisons, case modification and classification."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HCharacter::HCharacter( HHuginn::HClass const* class_, HHuginn::HCharacter::value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

void HHuginn::HCharacter::set( value_type value_ ) {
	_value = value_;
	return;
}

HHuginn::value_t HHuginn::HCharacter::do_clone( huginn::HThread* thread_, int ) const {
	return ( thread_->runtime().object_factory()->create_character( _value ) );
}

}

}

