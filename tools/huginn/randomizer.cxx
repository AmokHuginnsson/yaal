/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  randomizer.cxx - this file is integral part of `yaal' project.

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

HHuginn::value_t HRandomizer::next( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Randomizer.next";
	verify_arg_count( name, values_, 0, 1, position_ );
	yaal::u64_t cap( 0 );
	if ( ! values_.is_empty() ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, false, position_ );
		cap = static_cast<yaal::u64_t>( get_integer( values_[0] ) );
	}
	HRandomizer* o( static_cast<HRandomizer*>( object_->raw() ) );
	return ( thread_->object_factory().create_integer( static_cast<HHuginn::HInteger::value_type>( cap ? o->_generator( cap ) : o->_generator() ) ) );
	M_EPILOG
}

HHuginn::value_t HRandomizer::next_real( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Randomizer.next_real";
	verify_arg_count( name, values_, 0, 1, position_ );
	double long range( 1.L );
	if ( ! values_.is_empty() ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::REAL, false, position_ );
		range = static_cast<yaal::u64_t>( get_real( values_[0] ) );
	}
	if ( range <= 0.L ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid range specified: "_ys.append( range ), position_ );
	}
	HRandomizer* o( static_cast<HRandomizer*>( object_->raw() ) );
	yaal::u64_t cap( o->_generator.range() );
	yaal::u64_t value( o->_generator() );
	return ( thread_->object_factory().create_real( ( static_cast<double long>( value ) / static_cast<double long>( cap ) ) * range ) );
	M_EPILOG
}

HHuginn::class_t HRandomizer::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Randomizer",
			nullptr,
			HHuginn::field_definitions_t{
				{ "next",      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HRandomizer::next, _1, _2, _3, _4 ) ) },
				{ "next_real", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HRandomizer::next_real, _1, _2, _3, _4 ) ) }
			}
		)
	);
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HRandomizer::do_clone( huginn::HRuntime* ) const {
	return ( make_pointer<HRandomizer>( HValue::get_class(), _generator ) );
}

}

}

}
