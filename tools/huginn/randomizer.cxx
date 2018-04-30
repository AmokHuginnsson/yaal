/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "randomizer.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "value_builtin.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HRandomizer::HRandomizer( HHuginn::HClass const* class_, yaal::u64_t cap_ )
	: HValue( class_ )
	, _generator( randomizer_helper::make_randomizer( cap_ ) ) {
	return;
}

HRandomizer::HRandomizer( HHuginn::HClass const* class_, yaal::hcore::HRandomizer const& generator_ )
	: HValue( class_ )
	, _generator( generator_ ) {
	return;
}

HHuginn::value_t HRandomizer::create_instance( HHuginn::HClass const* class_, huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Randomizer.constructor";
	verify_arg_count( name, values_, 0, 1, thread_, position_ );
	yaal::u64_t cap( meta::max_unsigned<yaal::u64_t>::value );
	if ( ! values_.is_empty() ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, ARITY::UNARY, thread_, position_ );
		cap = static_cast<yaal::u64_t>( get_integer( values_[0] ) );
	}
	return ( thread_->object_factory().create<huginn::HRandomizer>( class_, cap ) );
	M_EPILOG
}

HHuginn::value_t HRandomizer::seed( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Randomizer.seed";
	verify_arg_count( name, values_, 0, 1, thread_, position_ );
	HRandomizer* o( static_cast<HRandomizer*>( object_->raw() ) );
	if ( ! values_.is_empty() ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, ARITY::UNARY, thread_, position_ );
		yaal::u64_t data( static_cast<yaal::u64_t>( get_integer( values_[0] ) ) );
		o->_generator = hcore::HRandomizer( data, o->_generator.range() );
	} else {
		o->_generator = randomizer_helper::make_randomizer( o->_generator.range() );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HRandomizer::next( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Randomizer.next";
	verify_arg_count( name, values_, 0, 1, thread_, position_ );
	yaal::u64_t cap( 0 );
	if ( ! values_.is_empty() ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, ARITY::UNARY, thread_, position_ );
		cap = static_cast<yaal::u64_t>( get_integer( values_[0] ) );
	}
	HRandomizer* o( static_cast<HRandomizer*>( object_->raw() ) );
	return ( thread_->object_factory().create_integer( static_cast<HHuginn::HInteger::value_type>( cap ? o->_generator( cap ) : o->_generator() ) ) );
	M_EPILOG
}

HHuginn::value_t HRandomizer::next_real( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Randomizer.next_real";
	verify_arg_count( name, values_, 0, 1, thread_, position_ );
	double long range( 1.L );
	if ( ! values_.is_empty() ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::REAL, ARITY::UNARY, thread_, position_ );
		range = get_real( values_[0] );
	}
	if ( range <= 0.L ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid range specified: "_ys.append( range ), thread_->current_frame()->file_id(), position_ );
	}
	HRandomizer* o( static_cast<HRandomizer*>( object_->raw() ) );
	yaal::u64_t cap( o->_generator.range() );
	yaal::u64_t value( o->_generator() );
	return ( thread_->object_factory().create_real( ( static_cast<double long>( value ) / static_cast<double long>( cap ) ) * range ) );
	M_EPILOG
}

HHuginn::value_t HRandomizer::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Randomizer.to_string";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HRandomizer* o( static_cast<HRandomizer*>( object_->raw() ) );
	HHuginn::HClass const* origin( o->HValue::get_class()->origin() );
	HString s;
	if ( origin ) {
		s.append( thread_->runtime().package_name( origin ) ).append( "." );
	}
	s.append( "Randomizer(" );
	yaal::u64_t cap( o->_generator.range() );
	if ( cap != meta::max_unsigned<yaal::u64_t>::value ) {
		s.append( cap );
	}
	s.append( ")" );
	return ( thread_->runtime().object_factory()->create_string( s ) );
	M_EPILOG
}

HHuginn::class_t HRandomizer::get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Randomizer",
			"The `Randomizer` class represents a random number generator concept. `Randomizer` can generate uniform distribution of either `integer` or `real` values from given range.",
			HHuginn::ACCESS::PUBLIC,
			HHuginn::HClass::TYPE::BUILTIN,
			origin_,
			&HRandomizer::create_instance
		)
	);
	HHuginn::field_definitions_t fd{
		{ "next",      runtime_->create_method( &HRandomizer::next ),      "([ *range* ]) - get next `integer` random number, possibly restricted to given range" },
		{ "next_real", runtime_->create_method( &HRandomizer::next_real ), "([ *range* ]) - get next `real` random number, possibly restricted to given range" },
		{ "seed",      runtime_->create_method( &HRandomizer::seed ),      "([ *data* ]) - initialize random generator internal state based on supplied *data*, or using system entropy if no data is given" },
		{ "to_string", runtime_->create_method( &HRandomizer::to_string ), "get string representation of this `Randomizer`" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HRandomizer::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->object_factory().create<HRandomizer>( HValue::get_class(), _generator ) );
}

}

}

}
