/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  complex.cxx - this file is integral part of `yaal' project.

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
#include "complex.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "value_builtin.hxx"
#include "hcore/safe_int.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HComplex::HComplex( huginn::HThread* thread_, HHuginn::HClass const* class_, HHuginn::values_t& values_, int position_ )
	: HValue( class_ )
	, _data() {
	verify_signature( "Complex.constructor", values_, { HHuginn::TYPE::REAL, HHuginn::TYPE::REAL }, thread_, position_ );
	_data.set( get_real( values_[0] ), get_real( values_[1] ) );
	return;
}

HComplex::HComplex( HHuginn::HClass const* class_, yaal::hcore::HComplex const& data_ )
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

HHuginn::value_t HComplex::add( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Complex.add";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	verify_arg_type( name, values_, 0, (*object_)->get_class(), ARITY::UNARY, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	HComplex const* arg( static_cast<HComplex const*>( values_[0].raw() ) );
	try {
		o->_data += arg->_data;
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HComplex::subtract( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Complex.subtract";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	verify_arg_type( name, values_, 0, (*object_)->get_class(), ARITY::UNARY, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	HComplex const* arg( static_cast<HComplex const*>( values_[0].raw() ) );
	try {
		o->_data -= arg->_data;
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HComplex::multiply( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Complex.multiply";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	verify_arg_type( name, values_, 0, (*object_)->get_class(), ARITY::UNARY, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	HComplex const* arg( static_cast<HComplex const*>( values_[0].raw() ) );
	try {
		o->_data *= arg->_data;
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HComplex::divide( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Complex.divide";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	verify_arg_type( name, values_, 0, (*object_)->get_class(), ARITY::UNARY, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	HComplex const* arg( static_cast<HComplex const*>( values_[0].raw() ) );
	try {
		o->_data /= arg->_data;
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HComplex::modulus( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Complex.modulus", values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	data_t::value_type mod( 0.L );
	try {
		mod = o->_data.modulus();
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( thread_->object_factory().create_real( mod ) );
	M_EPILOG
}

HHuginn::value_t HComplex::negate( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Complex.negate", values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	return ( thread_->object_factory().create<HComplex>( (*object_)->get_class(), - o->_data ) );
	M_EPILOG
}

HHuginn::value_t HComplex::argument( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Complex.argument", values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	data_t::value_type arg( 0.L );
	try {
		arg = o->_data.argument();
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( thread_->object_factory().create_real( arg ) );
	M_EPILOG
}

HHuginn::value_t HComplex::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Complex.to_string";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HComplex* o( static_cast<HComplex*>( object_->raw() ) );
	HHuginn::HClass const* origin( o->HValue::get_class()->origin() );
	HString s;
	if ( origin ) {
		s.append( thread_->runtime().package_name( origin ) ).append( "." );
	}
	s.append( "Complex(" ).append( o->_data.re() ).append( ", " ).append( o->_data.im() ).append( ")" );
	return ( thread_->runtime().object_factory()->create_string( s ) );
	M_EPILOG
}

HHuginn::value_t HComplex::create_instance( HHuginn::HClass const* class_, huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
	return ( thread_->object_factory().create<HComplex>( thread_, class_, values_, position_ ) );
}

HHuginn::class_t HComplex::get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Complex",
			nullptr,
			HHuginn::field_definitions_t{
				{ "real",      runtime_->object_factory()->create_method( hcore::call( &HComplex::real, _1, _2, _3, _4 ) ),      "get real part of this `Complex` number" },
				{ "imaginary", runtime_->object_factory()->create_method( hcore::call( &HComplex::imaginary, _1, _2, _3, _4 ) ), "get imaginary part of this `Complex` number" },
				{ "get",       runtime_->object_factory()->create_method( hcore::call( &HComplex::get, _1, _2, _3, _4 ) ),       "get both real and imaginary part of this `Complex`" },
				{ "set",       runtime_->object_factory()->create_method( hcore::call( &HComplex::set, _1, _2, _3, _4 ) ),       "( *real*, *imaginary* ) - set both real and imaginary part of this `Complex` number" },
				{ "add",       runtime_->object_factory()->create_method( hcore::call( &HComplex::add, _1, _2, _3, _4 ) ),       "( *other* ) - add *other* `Complex` to this `Complex` number" },
				{ "subtract",  runtime_->object_factory()->create_method( hcore::call( &HComplex::subtract, _1, _2, _3, _4 ) ),  "( *other* ) - subtract *other* `Complex` from this `Complex` number" },
				{ "multiply",  runtime_->object_factory()->create_method( hcore::call( &HComplex::multiply, _1, _2, _3, _4 ) ),  "( *other* ) - multiply this `Complex` by *other* `Complex` number" },
				{ "divide",    runtime_->object_factory()->create_method( hcore::call( &HComplex::divide, _1, _2, _3, _4 ) ),    "( *other* ) - divide this `Complex` by *other* `Complex` number" },
				{ "modulus",   runtime_->object_factory()->create_method( hcore::call( &HComplex::modulus, _1, _2, _3, _4 ) ),   "get modulus value of this `Complex` number" },
				{ "argument",  runtime_->object_factory()->create_method( hcore::call( &HComplex::argument, _1, _2, _3, _4 ) ),  "get argument value of this `Complex` number" },
				{ "negate",    runtime_->object_factory()->create_method( hcore::call( &HComplex::negate, _1, _2, _3, _4 ) ),    "get negation of this `Complex` number" },
				{ "to_string", runtime_->object_factory()->create_method( hcore::call( &HComplex::to_string, _1, _2, _3, _4 ) ), "get string representation of this `Complex` number" }
			},
			"The `Complex` class provides mathematical concept of complex numbers. It supports operations of addition, multiplication, subtraction, division, modulus and argument.",
			origin_,
			&HComplex::create_instance
		)
	);
	runtime_->huginn()->register_class( c, HHuginn::ACCESS::PUBLIC );
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

