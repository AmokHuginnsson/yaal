/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cmath>

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

namespace real {

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<huginn::HClass>(
			runtime_,
			objectFactory_,
			type_id( HHuginn::TYPE::REAL ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::REAL ) ),
			"The `real` is a scalar type that is used to represent and operate on floating point numbers. "
			"It supports basic operations of addition, subtraction, multiplication, division, modulo, power and comparisons, "
			"it can also be used as an argument in functions and algorithms from Mathematics package. "
			"The range of possible values it can hold is the same as `double long` from C++ programming language.",
			&builtin::real
		)
	);
	return ( c );
	M_EPILOG
}

}

HReal::HReal( HClass const* class_, HReal::value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HReal::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_real( _value ) );
}

bool HReal::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value == static_cast<HReal const*>( other_.raw() )->_value );
}

bool HReal::do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value < static_cast<HReal const*>( other_.raw() )->_value );
}

bool HReal::do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value > static_cast<HReal const*>( other_.raw() )->_value );
}

bool HReal::do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value <= static_cast<HReal const*>( other_.raw() )->_value );
}

bool HReal::do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value >= static_cast<HReal const*>( other_.raw() )->_value );
}

void HReal::do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value += static_cast<HReal const*>( other_.raw() )->_value;
}

void HReal::do_operator_subtract( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value -= static_cast<HReal const*>( other_.raw() )->_value;
}

void HReal::do_operator_multiply( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value *= static_cast<HReal const*>( other_.raw() )->_value;
}

void HReal::do_operator_divide( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	HReal::value_type denominator( static_cast<HReal const*>( other_.raw() )->_value );
	if ( denominator != 0.0l ) {
		_value /= denominator;
	} else {
		HRuntime& rt( thread_->runtime() );
		self_ = thread_->runtime().none_value();
		thread_->raise( rt.object_factory()->arithmetic_exception_class(), "Division by zero.", position_ );
	}
}

void HReal::do_operator_modulo( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	HReal::value_type denominator( static_cast<HReal const*>( other_.raw() )->_value );
	if ( denominator != 0.0l ) {
		_value = fmodl( _value, denominator );
	} else {
		HRuntime& rt( thread_->runtime() );
		self_ = thread_->runtime().none_value();
		thread_->raise( rt.object_factory()->arithmetic_exception_class(), "Division by zero.", position_ );
	}
}

void HReal::do_operator_power( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	HReal::value_type exp( static_cast<HReal const*>( other_.raw() )->value() );
	if ( ( _value != 0.L ) || ( exp != 0.L ) ) {
		_value = ::powl( _value, exp );
	} else {
		HRuntime& rt( thread_->runtime() );
		self_ = thread_->runtime().none_value();
		thread_->raise( rt.object_factory()->arithmetic_exception_class(), "indeterminate form 0^0", position_ );
	}
}

HHuginn::value_t HReal::do_operator_modulus( HThread* thread_, HHuginn::value_t const& self_, int ) const {
	return ( _value >= 0 ? self_ : thread_->object_factory().create_real( -_value ) );
}

HHuginn::value_t HReal::do_operator_negate( HThread* thread_, HHuginn::value_t const&, int ) const {
	return ( thread_->object_factory().create_real( -_value ) );
}

int long HReal::do_operator_hash( HThread*, HHuginn::value_t const&, int ) const {
	return ( hcore::hash<double long>()( _value ) );
}

yaal::hcore::HString HReal::do_code( huginn::HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const {
	return ( _value );
}

yaal::hcore::HString HReal::do_to_string( huginn::HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const {
	return ( _value );
}

}

}

}

