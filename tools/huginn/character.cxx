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
	c->set( static_cast<HHuginn::HCharacter::value_type>( std::tolower( c->value() ) ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t to_upper( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "character.to_upper";
	verify_arg_count( name, values_, 0, 0, position_ );
	HHuginn::HCharacter* c( static_cast<HHuginn::HCharacter*>( object_->raw() ) );
	c->set( static_cast<HHuginn::HCharacter::value_type>( std::toupper( c->value() ) ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t is_of_a_kind( char const* name, int (*isofakind)(int), huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name, values_, 0, 0, position_ );
	HHuginn::HCharacter* c( static_cast<HHuginn::HCharacter*>( object_->raw() ) );
	return ( thread_->object_factory().create_boolean( isofakind( c->value() ) ) );
	M_EPILOG
}

HHuginn::class_t get_class( HHuginn* );
HHuginn::class_t get_class( HHuginn* huginn_ ) {
	M_PROLOG

	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			type_id( HHuginn::TYPE::CHARACTER ),
			huginn_->identifier_id( type_name( HHuginn::TYPE::CHARACTER ) ),
			nullptr,
			HHuginn::field_names_t{
				"to_lower",
				"to_upper",
				"is_upper",
				"is_lower",
				"is_digit",
				"is_xdigit",
				"is_space",
				"is_alpha",
				"is_alnum"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::to_lower, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::to_upper, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_upper", static_cast<int(*)(int)>( ::std::isupper ), _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_lower", static_cast<int(*)(int)>( ::std::islower ), _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_digit", static_cast<int(*)(int)>( ::std::isdigit ), _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_xdigit", static_cast<int(*)(int)>( ::std::isxdigit ), _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_space", static_cast<int(*)(int)>( ::std::isspace ), _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_alpha", static_cast<int(*)(int)>( ::std::isalpha ), _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &character::is_of_a_kind, "character.is_alnum", static_cast<int(*)(int)>( ::std::isalnum ), _1, _2, _3, _4 ) )
			}
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

HHuginn::HCharacter::value_type HHuginn::HCharacter::value( void ) const {
	return ( _value );
}

void HHuginn::HCharacter::set( value_type value_ ) {
	_value = value_;
	return;
}

HHuginn::value_t HHuginn::HCharacter::do_clone( HHuginn* huginn_ ) const {
	return ( huginn_->object_factory()->create_character( _value ) );
}

}

}

