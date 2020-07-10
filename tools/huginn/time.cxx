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

HTime::HTime( huginn::HClass const* class_, yaal::hcore::HTime const& time_ )
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
		huginn::HClass const* ec( !! package ? static_cast<HPackage*>( package.raw() )->exception_class() : r.object_factory()->exception_class() );
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
		huginn::HClass const* ec( !! package ? static_cast<HPackage*>( package.raw() )->exception_class() : r.object_factory()->exception_class() );
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
		huginn::HClass const* ec( !! package ? static_cast<HPackage*>( package.raw() )->exception_class() : r.object_factory()->exception_class() );
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

void HTime::do_operator_subtract( HThread*, HHuginn::value_t&, HHuginn::value_t const& other_, int ) {
	M_PROLOG
	hcore::HTime const& other( static_cast<HTime const*>( other_.raw() )->_time );
	_time -= other;
	return;
	M_EPILOG
}

hash_value_t HTime::do_operator_hash( HThread*, HHuginn::value_t const&, int ) const {
	M_PROLOG
	return ( hash<hcore::HTime::value_type>()( _time.raw() ) );
	M_EPILOG
}

bool HTime::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	M_PROLOG
	hcore::HTime const& other( static_cast<HTime const*>( other_.raw() )->_time );
	return ( _time == other );
	M_EPILOG
}

bool HTime::do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	M_PROLOG
	hcore::HTime const& other( static_cast<HTime const*>( other_.raw() )->_time );
	return ( _time < other );
	M_EPILOG
}

bool HTime::do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	M_PROLOG
	hcore::HTime const& other( static_cast<HTime const*>( other_.raw() )->_time );
	return ( _time <= other );
	M_EPILOG
}

bool HTime::do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	M_PROLOG
	hcore::HTime const& other( static_cast<HTime const*>( other_.raw() )->_time );
	return ( _time > other );
	M_EPILOG
}

bool HTime::do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	M_PROLOG
	hcore::HTime const& other( static_cast<HTime const*>( other_.raw() )->_time );
	return ( _time >= other );
	M_EPILOG
}

yaal::hcore::HString HTime::do_code( huginn::HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const {
	return (
		"Time("_ys
			.append( _time.get_year() )
			.append( ", " )
			.append( static_cast<int>( _time.get_month() ) )
			.append( ", " )
			.append( _time.get_day() )
			.append( ", " )
			.append( _time.get_hour() )
			.append( ", " )
			.append( _time.get_minute() )
			.append( ", " )
			.append( _time.get_second() )
			.append( ")" )
	);
}

yaal::hcore::HString HTime::do_to_string( huginn::HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const {
	M_PROLOG
	return ( _time.to_string() );
	M_EPILOG
}

namespace {
void verify_range( HThread* thread_, char const* item_, HInteger::value_type value_, HInteger::value_type min_, HInteger::value_type max_, int position_ ) {
	if ( ( value_ < min_ ) || ( value_ > max_ ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Invalid value for `"_ys.append( item_ ).append( "` in Time constructor: " ).append( value_ ).append( "." ),
			thread_->file_id(),
			position_
		);
	}
}
}

HHuginn::value_t HTime::create_instance( HClass const* class_, huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
	verify_signature(
		"Time.constructor",
		values_,
		{ HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER },
		thread_,
		position_
	);
	HInteger::value_type year(   get_integer( values_[0] ) );
	HInteger::value_type month(  get_integer( values_[1] ) );
	HInteger::value_type day(    get_integer( values_[2] ) );
	HInteger::value_type hour(   get_integer( values_[3] ) );
	HInteger::value_type minute( get_integer( values_[4] ) );
	HInteger::value_type second( get_integer( values_[5] ) );
	HInteger::value_type const MAX_YEAR( meta::max_signed<int>::value );
	verify_range( thread_, "year",   year,   -MAX_YEAR, MAX_YEAR, position_ );
	verify_range( thread_, "second", second, 0, 59, position_ );
	verify_range( thread_, "minute", minute, 0, 59, position_ );
	verify_range( thread_, "hour",   hour,   0, 23, position_ );
	verify_range( thread_, "month",  month,  1, 12, position_ );
	hcore::HTime trial( static_cast<int>( year ), static_cast<int>( month ), 1, 12, 0, 0 );
	verify_range( thread_, "day",    day,    1, trial.get_days_in_month(), position_ );
	return (
		thread_->object_factory().create<HTime>(
			class_,
			hcore::HTime(
				static_cast<int>( year ),
				static_cast<int>( month ),
				static_cast<int>( day ),
				static_cast<int>( hour ),
				static_cast<int>( minute ),
				static_cast<int>( second )
			)
		)
	);
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
			HHuginn::ACCESS::PUBLIC,
			HClass::TYPE::BUILTIN,
			nullptr,
			&HTime::create_instance
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
			{ "from_string",  runtime_->create_method( &HTime::from_string ),  "( *str* ) - set time from parsed `string` *str*" }
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

