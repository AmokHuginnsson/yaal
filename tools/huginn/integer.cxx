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

namespace integer {

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<huginn::HClass>(
			runtime_,
			objectFactory_,
			type_id( HHuginn::TYPE::INTEGER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::INTEGER ) ),
			"The `integer` is a scalar type that is used to represent and operate on integers. "
			"It supports basic operations of addition, subtraction, multiplication, division, modulo and comparisons. "
			"The range of possible values it can hold is [-2^32, 2^32).",
			&builtin::integer
		)
	);
	return ( c );
	M_EPILOG
}

}

HInteger::HInteger( huginn::HClass const* class_, huginn::HInteger::value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HInteger::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_integer( _value ) );
}

bool HInteger::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value == static_cast<HInteger const*>( other_.raw() )->_value );
}

bool HInteger::do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value < static_cast<HInteger const*>( other_.raw() )->_value );
}

bool HInteger::do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value > static_cast<HInteger const*>( other_.raw() )->_value );
}

bool HInteger::do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value <= static_cast<HInteger const*>( other_.raw() )->_value );
}

bool HInteger::do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value >= static_cast<HInteger const*>( other_.raw() )->_value );
}

void HInteger::do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value += static_cast<HInteger const*>( other_.raw() )->_value;
}

void HInteger::do_operator_subtract( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value -= static_cast<HInteger const*>( other_.raw() )->_value;
}

void HInteger::do_operator_multiply( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_value *= static_cast<HInteger const*>( other_.raw() )->_value;
}

void HInteger::do_operator_divide( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	huginn::HInteger::value_type denominator( static_cast<huginn::HInteger const*>( other_.raw() )->_value );
	if ( ( denominator != 0 ) && ( ( _value != meta::min_signed<huginn::HInteger::value_type>::value ) || ( denominator != -1 ) ) ) {
		_value /= denominator;
	} else {
		HRuntime& rt( thread_->runtime() );
		self_ = rt.none_value();
		thread_->raise(
			rt.object_factory()->arithmetic_exception_class(),
			denominator ? "Division overflow." : "Division by zero.",
			position_
		);
	}
}

void HInteger::do_operator_modulo( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
	huginn::HInteger::value_type denominator( static_cast<huginn::HInteger const*>( other_.raw() )->_value );
	if ( ( denominator != 0 ) && ( ( _value != meta::min_signed<huginn::HInteger::value_type>::value ) || ( denominator != -1 ) ) ) {
		_value %= denominator;
	} else {
		HRuntime& rt( thread_->runtime() );
		self_ = rt.none_value();
		thread_->raise(
			rt.object_factory()->arithmetic_exception_class(),
			denominator ? "Division overflow." : "Division by zero.",
			position_
		);
	}
}

HHuginn::value_t HInteger::do_operator_modulus( HThread* thread_, HHuginn::value_t const& self_, int position_ ) const {
	HObjectFactory& of( thread_->object_factory() );
	HHuginn::value_t res( of.none_value() );
	if ( _value >= 0 ) {
		res = self_;
	} else if ( _value != meta::min_signed<huginn::HInteger::value_type>::value ) {
		res = of.create_integer( -_value );
	} else {
		thread_->raise( of.arithmetic_exception_class(), "Integer overflow.", position_ );
	}
	return ( res );
}

HHuginn::value_t HInteger::do_operator_negate( HThread* thread_, HHuginn::value_t const&, int position_ ) const {
	HObjectFactory& of( thread_->object_factory() );
	HHuginn::value_t res( of.none_value() );
	if ( _value != meta::min_signed<huginn::HInteger::value_type>::value ) {
		res = of.create_integer( -_value );
	} else {
		thread_->raise( of.arithmetic_exception_class(), "Integer overflow.", position_ );
	}
	return ( res );
}

int long HInteger::do_operator_hash( HThread*, HHuginn::value_t const&, int ) const {
	return ( hcore::hash<int long long>()( _value ) );
}

yaal::hcore::HString HInteger::do_code( huginn::HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const {
	return ( _value );
}

yaal::hcore::HString HInteger::do_to_string( huginn::HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const {
	return ( _value );
}

}

}

}

