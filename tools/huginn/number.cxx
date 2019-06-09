/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
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

inline HHuginn::value_t is_exact( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "number.is_exact";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	return ( thread_->runtime().boolean_value( static_cast<HNumber*>( object_->raw() )->value().is_exact() ) );
	M_EPILOG
}

inline HHuginn::value_t is_integral( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "number.is_integral";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	return ( thread_->runtime().boolean_value( static_cast<HNumber*>( object_->raw() )->value().is_integral() ) );
	M_EPILOG
}

inline HHuginn::value_t get_precision( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "number.get_precision";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_integer( static_cast<HNumber*>( object_->raw() )->value().get_precision() ) );
	M_EPILOG
}

inline HHuginn::value_t set_precision( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "number.set_precision", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	int precision = static_cast<int>( get_integer( values_[0] ) );
	static_cast<HNumber*>( object_->raw() )->value().set_precision( precision );
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HClass>(
			runtime_,
			objectFactory_,
			type_id( HHuginn::TYPE::NUMBER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::NUMBER ) ),
			"The `number` is a scalar type that is used to represent and operate on fixed point numbers of arbitrary size. "
			"It supports basic operations of addition, subtraction, multiplication, division, modulo, power, factorial and comparisons, "
			"it can also be used as an argument in functions and algorithms from Mathematics package. "
			"The range of possible values it can hold is limited only by size of your hardware resources.",
			&builtin::number
		)
	);
	HHuginn::field_definitions_t fd{
		{ "is_exact",      objectFactory_->create_method( &number::is_exact ),      "tell if this number is an exact representation" },
		{ "is_integral",   objectFactory_->create_method( &number::is_integral ),   "tell if this number does not have any decimal digits" },
		{ "get_precision", objectFactory_->create_method( &number::get_precision ), "get maximum precision of this number" },
		{ "set_precision", objectFactory_->create_method( &number::set_precision ), "( *prec* ) set maximum precision of this number to *prec* (increasing precision works only on numbers that currently have exact representation)" }
	};
	c->redefine( nullptr, fd );
	return ( c );
	M_EPILOG
}

}

HNumber::HNumber( HClass const* class_, HNumber::value_type const& value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HNumber::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_number( _value ) );
}

bool HNumber::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value == static_cast<HNumber const*>( other_.raw() )->_value );
}

bool HNumber::do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value < static_cast<HNumber const*>( other_.raw() )->_value );
}

bool HNumber::do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value > static_cast<HNumber const*>( other_.raw() )->_value );
}

bool HNumber::do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value <= static_cast<HNumber const*>( other_.raw() )->_value );
}

bool HNumber::do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value >= static_cast<HNumber const*>( other_.raw() )->_value );
}

void HNumber::do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value += static_cast<HNumber const*>( other_.raw() )->_value;
}

void HNumber::do_operator_subtract( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value -= static_cast<HNumber const*>( other_.raw() )->_value;
}

void HNumber::do_operator_multipy( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value *= static_cast<HNumber const*>( other_.raw() )->_value;
}

void HNumber::do_operator_divide( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	HNumber::value_type const& denominator( static_cast<HNumber const*>( other_.raw() )->_value );
	if ( denominator != hcore::number::N0 ) {
		_value /= denominator;
	} else {
		HRuntime& rt( thread_->runtime() );
		self_ = thread_->runtime().none_value();
		thread_->raise( rt.object_factory()->arithmetic_exception_class(), "Division by zero.", position_ );
	}
}

}

}

}

