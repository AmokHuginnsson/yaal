/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cwctype>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "builtin.hxx"
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

inline HHuginn::value_t to_lower( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "character.to_lower";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HCharacter* c( static_cast<HCharacter*>( object_->raw() ) );
	c->set( HCharacter::value_type( static_cast<yaal::u32_t>( std::towlower( static_cast<wint_t>( c->value().get() ) ) ) ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t to_upper( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "character.to_upper";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HCharacter* c( static_cast<HCharacter*>( object_->raw() ) );
	c->set( HCharacter::value_type( static_cast<yaal::u32_t>( std::towupper( static_cast<wint_t>( c->value().get() ) ) ) ) );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t is_of_a_kind( char const* name, bool (*isofakind)( code_point_t ), huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HCharacter* c( static_cast<HCharacter*>( object_->raw() ) );
	return ( thread_->runtime().boolean_value( isofakind( c->value() ) != 0 ) );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG

	HHuginn::class_t c(
		make_pointer<huginn::HClass>(
			runtime_,
			objectFactory_,
			type_id( HHuginn::TYPE::CHARACTER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::CHARACTER ) ),
			"The `character` is a scalar type that is used to represent and operate on single characters. "
			"It supports basic operations of comparisons, case modification and classification.",
			&builtin::character
		)
	);
	HHuginn::field_definitions_t fd{
		{ "to_lower",  objectFactory_->create_method( &character::to_lower ), "make this character lower case" },
		{ "to_upper",  objectFactory_->create_method( &character::to_upper ), "make this character upper case" },
		{ "is_upper",  objectFactory_->create_method( &character::is_of_a_kind, "character.is_upper",  hcore::is_upper ),      "tell if this character is an upper case character" },
		{ "is_lower",  objectFactory_->create_method( &character::is_of_a_kind, "character.is_lower",  hcore::is_lower ),      "tell if this character is a lower case character" },
		{ "is_digit",  objectFactory_->create_method( &character::is_of_a_kind, "character.is_digit",  hcore::is_digit ),      "tell if this character represents decimal digit" },
		{ "is_xdigit", objectFactory_->create_method( &character::is_of_a_kind, "character.is_xdigit", hcore::is_hex_digit ),  "tell if this character represents a hexadecimal digit" },
		{ "is_space",  objectFactory_->create_method( &character::is_of_a_kind, "character.is_space",  hcore::is_whitespace ), "tell if this character represents a white space character" },
		{ "is_alpha",  objectFactory_->create_method( &character::is_of_a_kind, "character.is_alpha",  hcore::is_alpha ),      "tell if this character represents an alphabet character" },
		{ "is_alnum",  objectFactory_->create_method( &character::is_of_a_kind, "character.is_alnum",  hcore::is_alnum ),      "tell if this character represents any alphanumeric character" }
	};
	c->redefine( nullptr, fd );
	return ( c );
	M_EPILOG
}

}

HCharacter::HCharacter( HClass const* class_, HCharacter::value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

void HCharacter::set( value_type value_ ) {
	_value = value_;
	return;
}

HHuginn::value_t HCharacter::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_character( _value ) );
}

bool HCharacter::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value == static_cast<HCharacter const*>( other_.raw() )->_value );
}

bool HCharacter::do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value < static_cast<HCharacter const*>( other_.raw() )->_value );
}

}

}

}

