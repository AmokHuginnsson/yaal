/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "complex.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "instruction.hxx"
#include "hcore/safe_int.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HComplex::HComplex( huginn::HThread* thread_, HClass const* class_, HHuginn::values_t& values_, int position_ )
	: HValue( class_ )
	, _data() {
	verify_signature( "Complex.constructor", values_, { HHuginn::TYPE::REAL, HHuginn::TYPE::REAL }, thread_, position_ );
	_data.set( get_real( values_[0] ), get_real( values_[1] ) );
	return;
}

HComplex::HComplex( HClass const* class_, yaal::hcore::HComplex const& data_ )
	: HValue( class_ )
	, _data( data_ ) {
}

HHuginn::value_t HComplex::real( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Complex.real";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	return ( thread_->runtime().object_factory()->create_real( o->_data.re() ) );
	M_EPILOG
}

HHuginn::value_t HComplex::imaginary( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Complex.imaginary";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	return ( thread_->runtime().object_factory()->create_real( o->_data.im() ) );
	M_EPILOG
}

HHuginn::value_t HComplex::get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Complex.get", values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	return (
		thread_->runtime().object_factory()->create_tuple(
			/* values */ {
				thread_->runtime().object_factory()->create_real( o->_data.re() ),
				thread_->runtime().object_factory()->create_real( o->_data.im() )
			}
		)
	);
	M_EPILOG
}

HHuginn::value_t HComplex::set( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Complex.set", values_, { HHuginn::TYPE::REAL, HHuginn::TYPE::REAL }, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	o->_data.set( get_real( values_[0] ), get_real( values_[1] ) );
	return ( *object_ );
	M_EPILOG
}

bool HComplex::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _data == static_cast<HComplex const*>( other_.raw() )->_data );
}

void HComplex::do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_data += static_cast<HComplex const*>( other_.raw() )->_data;
}

void HComplex::do_operator_subtract( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_data -= static_cast<HComplex const*>( other_.raw() )->_data;
}

void HComplex::do_operator_multiply( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	_data *= static_cast<HComplex const*>( other_.raw() )->_data;
}

void HComplex::do_operator_divide( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& other_, int position_ ) {
	M_PROLOG
	hcore::HComplex const& other( static_cast<HComplex const*>( other_.raw() )->_data );
	try {
		_data /= other;
	} catch ( hcore::HException const& e ) {
		thread_->raise( thread_->object_factory().arithmetic_exception_class(), e.what(), position_ );
	}
	M_EPILOG
}

HHuginn::value_t HComplex::do_operator_modulus( HThread* thread_, HHuginn::value_t const&, int ) const {
	return ( thread_->object_factory().create_real( _data.modulus() ) );
}

HHuginn::value_t HComplex::do_operator_negate( HThread* thread_, HHuginn::value_t const&, int ) const {
	return ( thread_->object_factory().create<HComplex>( HValue::get_class(), -_data ) );
}

hash_value_t HComplex::do_operator_hash( HThread*, HHuginn::value_t const&, int ) const {
	return ( hcore::hash<double long>()( _data.re() ) ^ hcore::hash<double long>()( _data.im() ) );
}

yaal::hcore::HString HComplex::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker&, int ) const {
	M_PROLOG
	hcore::HString s( full_class_name( thread_->runtime(), HValue::get_class() ) );
	s.append( "(" ).append( _data.re() ).append( ", " ).append( _data.im() ).append( ")" );
	return ( s );
	M_EPILOG
}

HHuginn::value_t HComplex::argument( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Complex.argument", values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	data_t::value_type arg( 0.L );
	try {
		arg = o->_data.argument();
	} catch ( hcore::HException const& e ) {
		thread_->raise( thread_->object_factory().arithmetic_exception_class(), e.what(), position_ );
	}
	return ( thread_->object_factory().create_real( arg ) );
	M_EPILOG
}

HHuginn::value_t HComplex::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Complex.to_string";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	HRuntime& rt( thread_->runtime() );
	hcore::HString s( full_class_name( rt, *object_ ) );
	s.append( "(" ).append( o->_data.re() ).append( ", " ).append( o->_data.im() ).append( ")" );
	return ( rt.object_factory()->create_string( yaal::move( s ) ) );
	M_EPILOG
}

HHuginn::value_t HComplex::create_instance( HClass const* class_, huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
	return ( thread_->object_factory().create<HComplex>( thread_, class_, values_, position_ ) );
}

HHuginn::class_t HComplex::get_class( HRuntime* runtime_, HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Complex",
			"The `Complex` class provides mathematical concept of complex numbers. It supports operations of addition, multiplication, subtraction, division, modulus and argument.",
			HHuginn::ACCESS::PUBLIC,
			HClass::TYPE::BUILTIN,
			origin_,
			&HComplex::create_instance
		)
	);
	HHuginn::field_definitions_t fd{
		{ "real",      runtime_->create_method( &HComplex::real ),      "get real part of this `Complex` number" },
		{ "imaginary", runtime_->create_method( &HComplex::imaginary ), "get imaginary part of this `Complex` number" },
		{ "get",       runtime_->create_method( &HComplex::get ),       "get both real and imaginary part of this `Complex`" },
		{ "set",       runtime_->create_method( &HComplex::set ),       "( *real*, *imaginary* ) - set both real and imaginary part of this `Complex` number" },
		{ "argument",  runtime_->create_method( &HComplex::argument ),  "get argument value of this `Complex` number" },
		{ "to_string", runtime_->create_method( &HComplex::to_string ), "get string representation of this `Complex` number" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HComplex::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	M_PROLOG
	return ( thread_->object_factory().create<HComplex>( HValue::get_class(), _data ) );
	M_EPILOG
}

}

}

}

