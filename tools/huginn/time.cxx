/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  time.cxx - this file is integral part of `yaal' project.

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
#include "time.hxx"
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

HTime::HTime( HHuginn::HClass const* class_, yaal::hcore::HTime const& time_ )
	: HValue( class_ )
	, _time( time_ ) {
	return;
}

HHuginn::value_t HTime::mod( char const* name_, time_mod_t timeMod_, huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_signature( hcore::to_string( "Time." ).append( name_ ), values_, { HHuginn::TYPE::INTEGER }, position_ );
	( static_cast<HTime*>( object_->raw() )->_time.*timeMod_ )( static_cast<int>( get_integer( values_[0] ) ) );
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::set( char const* name_, time_set_t timeSet_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_signature( hcore::to_string( "Time." ).append( name_ ), values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER }, position_ );
	try {
		( static_cast<HTime*>( object_->raw() )->_time.*timeSet_ )(
			static_cast<int>( get_integer( values_[0] ) ),
			static_cast<int>( get_integer( values_[1] ) ),
			static_cast<int>( get_integer( values_[2] ) )
		);
	} catch ( HTimeException const& e ) {
		HRuntime& r( thread_->runtime() );
		HHuginn::HClass const* ec( r.get_class( r.identifier_id( "DateTimeException" ) ).raw() );
		thread_->raise( ec, e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::set_datetime( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Time.set_datetime", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER }, position_ );
	try {
		static_cast<HTime*>( object_->raw() )->_time.set_datetime(
			static_cast<int>( get_integer( values_[0] ) ),
			static_cast<int>( get_integer( values_[1] ) ),
			static_cast<int>( get_integer( values_[2] ) ),
			static_cast<int>( get_integer( values_[3] ) ),
			static_cast<int>( get_integer( values_[4] ) ),
			static_cast<int>( get_integer( values_[5] ) )
		);
	} catch ( HTimeException const& e ) {
		HRuntime& r( thread_->runtime() );
		HHuginn::HClass const* ec( r.get_class( r.identifier_id( "DateTimeException" ) ).raw() );
		thread_->raise( ec, e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::from_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Time.from_string", values_, { HHuginn::TYPE::STRING }, position_ );
	try {
		static_cast<HTime*>( object_->raw() )->_time.from_string( get_string( values_[0] ) );
	} catch ( HTimeException const& e ) {
		HRuntime& r( thread_->runtime() );
		HHuginn::HClass const* ec( r.get_class( r.identifier_id( "DateTimeException" ) ).raw() );
		thread_->raise( ec, e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::get( char const* name_, time_get_t timeGet_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Time."_ys.append( name_ ), values_, 0, 0, position_ );
	return ( thread_->object_factory().create_integer( ( static_cast<HTime*>( object_->raw() )->_time.*timeGet_ )() ) );
	M_EPILOG
}

HHuginn::value_t HTime::get_month( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Time.get_month", values_, 0, 0, position_ );
	return ( thread_->object_factory().create_integer( static_cast<HTime*>( object_->raw() )->_time.get_month() ) );
	M_EPILOG
}

HHuginn::value_t HTime::subtract( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Time.subtract";
	verify_arg_count( name, values_, 1, 1, position_ );
	HHuginn::HClass const* c( (*object_)->get_class() );
	verify_arg_type( name, values_, 0, c, ARITY::UNARY, position_ );
	hcore::HTime& lt( static_cast<HTime*>( object_->raw() )->_time );
	hcore::HTime const& rt( static_cast<HTime const*>( values_[0].raw() )->_time );
	lt -= rt;
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Time.to_string";
	verify_arg_count( name, values_, 0, 0, position_ );
	huginn::HTime* t( static_cast<huginn::HTime*>( object_->raw() ) );
	return ( thread_->object_factory().create_string( t->_time.to_string() ) );
	M_EPILOG
}

HHuginn::class_t HTime::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Time",
			nullptr,
			HHuginn::field_definitions_t{
				{ "mod_year",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::mod, "mod_year",   &hcore::HTime::mod_year, _1, _2, _3, _4 ) ),   "( *num* ) - modify time value by *num* of years" },
				{ "mod_month",    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::mod, "mod_month",  &hcore::HTime::mod_month, _1, _2, _3, _4 ) ),  "( *num* ) - modify time value by *num* of months" },
				{ "mod_day",      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::mod, "mod_day",    &hcore::HTime::mod_day, _1, _2, _3, _4 ) ),    "( *num* ) - modify time value by *num* of days" },
				{ "mod_hour",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::mod, "mod_hour",   &hcore::HTime::mod_hour, _1, _2, _3, _4 ) ),   "( *num* ) - modify time value by *num* of hours" },
				{ "mod_minute",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::mod, "mod_minute", &hcore::HTime::mod_minute, _1, _2, _3, _4 ) ), "( *num* ) - modify time value by *num* of minutes" },
				{ "mod_second",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::mod, "mod_second", &hcore::HTime::mod_second, _1, _2, _3, _4 ) ), "( *num* ) - modify time value by *num* of seconds" },
				{ "set_time",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::set, "set_time",   &hcore::HTime::set_time, _1, _2, _3, _4 ) ),   "( *hh*, *mm*, *ss* ) - set time value to *hh*:*mm*:*ss*" },
				{ "set_date",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::set, "set_date",   &hcore::HTime::set_date, _1, _2, _3, _4 ) ),   "( *YYYY*, *MM*, *DD* ) - set date value to *YYYY*-*MM*-*DD*" },
				{ "get_year",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::get, "get_year",   &hcore::HTime::get_year, _1, _2, _3, _4 ) ),   "get number of years from time" },
				{ "get_day",      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::get, "get_day",    &hcore::HTime::get_day, _1, _2, _3, _4 ) ),    "get number of days from time" },
				{ "get_hour",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::get, "get_hour",   &hcore::HTime::get_hour, _1, _2, _3, _4 ) ),   "get number of hours from time" },
				{ "get_minute",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::get, "get_minute", &hcore::HTime::get_minute, _1, _2, _3, _4 ) ), "get number of minutes from time" },
				{ "get_second",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::get, "get_second", &hcore::HTime::get_second, _1, _2, _3, _4 ) ), "get number of seconds from time" },
				{ "set_datetime", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::set_datetime, _1, _2, _3, _4 ) ),                                 "( *YYYY*, *MM*, *DD*, *hh*, *mm*, *ss* ) - set date value to *YYYY*-*MM*-*DD* *hh*:*mm*:*ss*" },
				{ "get_month",    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::get_month, _1, _2, _3, _4 ) ),                                    "get number of months from time" },
				{ "subtract",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::subtract, _1, _2, _3, _4 ) ),                                     "( *time* ) - calculate time difference between this and *time* time points" },
				{ "from_string",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::from_string, _1, _2, _3, _4 ) ),                                  "( *str* ) - set time from parsed `string` *str*" },
				{ "to_string",    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HTime::to_string, _1, _2, _3, _4 ) ),                                    "get `string` representation of this point-in-time" }
			},
			"The `Time` class represent information about point-in-time."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HTime::do_clone( huginn::HThread*, int ) const {
	M_PROLOG
	return ( make_pointer<HTime>( HValue::get_class(), _time ) );
	M_EPILOG
}

}

}

}

