/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "time.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "packagefactory.hxx"

#include "hcore/hfile.hxx"

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

HHuginn::value_t HTime::mod( char const* name_, time_mod_t timeMod_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( name_, values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	( static_cast<HTime*>( object_->raw() )->_time.*timeMod_ )( static_cast<int>( get_integer( values_[0] ) ) );
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::set( char const* name_, time_set_t timeSet_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( name_, values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER }, thread_, position_ );
	try {
		( static_cast<HTime*>( object_->raw() )->_time.*timeSet_ )(
			static_cast<int>( get_integer( values_[0] ) ),
			static_cast<int>( get_integer( values_[1] ) ),
			static_cast<int>( get_integer( values_[2] ) )
		);
	} catch ( HTimeException const& e ) {
		HRuntime& r( thread_->runtime() );
		HHuginn::value_t package( r.find_package( r.try_identifier_id( "DateTime" ) ) );
		HHuginn::HClass const* ec( !! package ? static_cast<HPackage*>( package.raw() )->exception_class() : r.object_factory()->exception_class() );
		thread_->raise( ec, e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::set_datetime( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Time.set_datetime", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER }, thread_, position_ );
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
		HHuginn::value_t package( r.find_package( r.try_identifier_id( "DateTime" ) ) );
		HHuginn::HClass const* ec( !! package ? static_cast<HPackage*>( package.raw() )->exception_class() : r.object_factory()->exception_class() );
		thread_->raise( ec, e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::from_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Time.from_string", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	try {
		static_cast<HTime*>( object_->raw() )->_time.from_string( get_string( values_[0] ) );
	} catch ( HTimeException const& e ) {
		HRuntime& r( thread_->runtime() );
		HHuginn::value_t package( r.find_package( r.try_identifier_id( "DateTime" ) ) );
		HHuginn::HClass const* ec( !! package ? static_cast<HPackage*>( package.raw() )->exception_class() : r.object_factory()->exception_class() );
		thread_->raise( ec, e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::get( char const* name_, time_get_t timeGet_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_integer( ( static_cast<HTime*>( object_->raw() )->_time.*timeGet_ )() ) );
	M_EPILOG
}

HHuginn::value_t HTime::get_month( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Time.get_month", values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_integer( static_cast<HTime*>( object_->raw() )->_time.get_month() ) );
	M_EPILOG
}

HHuginn::value_t HTime::get_day_of_week( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Time.get_day_of_week", values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_integer( static_cast<int>( static_cast<HTime*>( object_->raw() )->_time.get_day_of_week() ) ) );
	M_EPILOG
}

HHuginn::value_t HTime::get_days_in_month( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Time.get_days_in_month", values_, 0, 0, thread_, position_ );
	return ( thread_->object_factory().create_integer( static_cast<HTime*>( object_->raw() )->_time.get_days_in_month() ) );
	M_EPILOG
}

HHuginn::value_t HTime::subtract( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Time.subtract";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HHuginn::HClass const* c( (*object_)->get_class() );
	verify_arg_type( name, values_, 0, c, ARITY::UNARY, thread_, position_ );
	hcore::HTime& lt( static_cast<HTime*>( object_->raw() )->_time );
	hcore::HTime const& rt( static_cast<HTime const*>( values_[0].raw() )->_time );
	lt -= rt;
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HTime::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Time.to_string";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	huginn::HTime* t( static_cast<huginn::HTime*>( object_->raw() ) );
	return ( thread_->object_factory().create_string( t->_time.to_string() ) );
	M_EPILOG
}

HHuginn::class_t HTime::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	char const name[] = "Time";
	HHuginn::identifier_id_t classIdentifier( runtime_->identifier_id( name ) );
	HHuginn::class_t c( runtime_->get_class( classIdentifier ) );
	if ( ! c ) {
		c = runtime_->create_class(
			name,
			"The `Time` class represent information about point-in-time.",
			HHuginn::ACCESS::PRIVATE
		);
		HHuginn::field_definitions_t fd{
			{ "mod_year",     runtime_->create_method( &HTime::mod, "Time.mod_year",   &hcore::HTime::mod_year ),   "( *num* ) - modify time value by *num* of years" },
			{ "mod_month",    runtime_->create_method( &HTime::mod, "Time.mod_month",  &hcore::HTime::mod_month ),  "( *num* ) - modify time value by *num* of months" },
			{ "mod_day",      runtime_->create_method( &HTime::mod, "Time.mod_day",    &hcore::HTime::mod_day ),    "( *num* ) - modify time value by *num* of days" },
			{ "mod_hour",     runtime_->create_method( &HTime::mod, "Time.mod_hour",   &hcore::HTime::mod_hour ),   "( *num* ) - modify time value by *num* of hours" },
			{ "mod_minute",   runtime_->create_method( &HTime::mod, "Time.mod_minute", &hcore::HTime::mod_minute ), "( *num* ) - modify time value by *num* of minutes" },
			{ "mod_second",   runtime_->create_method( &HTime::mod, "Time.mod_second", &hcore::HTime::mod_second ), "( *num* ) - modify time value by *num* of seconds" },
			{ "set_time",     runtime_->create_method( &HTime::set, "Time.set_time",   &hcore::HTime::set_time ),   "( *hh*, *mm*, *ss* ) - set time value to *hh*:*mm*:*ss*" },
			{ "set_date",     runtime_->create_method( &HTime::set, "Time.set_date",   &hcore::HTime::set_date ),   "( *YYYY*, *MM*, *DD* ) - set date value to *YYYY*-*MM*-*DD*" },
			{ "get_year",     runtime_->create_method( &HTime::get, "Time.get_year",   &hcore::HTime::get_year ),   "get number of years from time" },
			{ "get_day",      runtime_->create_method( &HTime::get, "Time.get_day",    &hcore::HTime::get_day ),    "get number of days from time" },
			{ "get_hour",     runtime_->create_method( &HTime::get, "Time.get_hour",   &hcore::HTime::get_hour ),   "get number of hours from time" },
			{ "get_minute",   runtime_->create_method( &HTime::get, "Time.get_minute", &hcore::HTime::get_minute ), "get number of minutes from time" },
			{ "get_second",   runtime_->create_method( &HTime::get, "Time.get_second", &hcore::HTime::get_second ), "get number of seconds from time" },
			{ "set_datetime", runtime_->create_method( &HTime::set_datetime ), "( *YYYY*, *MM*, *DD*, *hh*, *mm*, *ss* ) - set date value to *YYYY*-*MM*-*DD* *hh*:*mm*:*ss*" },
			{ "get_month",    runtime_->create_method( &HTime::get_month ),    "get number of months from time" },
			{ "get_day_of_week",   runtime_->create_method( &HTime::get_day_of_week ),   "get day of the week for this time" },
			{ "get_days_in_month", runtime_->create_method( &HTime::get_days_in_month ), "get number of days in month for this time" },
			{ "subtract",     runtime_->create_method( &HTime::subtract ),     "( *time* ) - calculate time difference between this and *time* time points" },
			{ "from_string",  runtime_->create_method( &HTime::from_string ),  "( *str* ) - set time from parsed `string` *str*" },
			{ "to_string",    runtime_->create_method( &HTime::to_string ),    "get `string` representation of this point-in-time" }
		};
		c->redefine( nullptr, fd );
		runtime_->huginn()->register_class( c, HHuginn::VISIBILITY::GLOBAL );
	}
	return ( c );
	M_EPILOG
}

HHuginn::value_t HTime::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	M_PROLOG
	return ( thread_->object_factory().create<HTime>( HValue::get_class(), _time ) );
	M_EPILOG
}

}

}

}

