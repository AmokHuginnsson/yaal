/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htime.cxx - this file is integral part of `yaal' project.

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

#include <cstring>

#include "config.hxx"

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "htime.hxx"

namespace yaal {

namespace hcore {

char const _rfc2822DateTimeFormat_[] = "%a, %d %b %Y %H:%M:%S %z";
char const _iso8601TimeFormat_[] = "%T";
char const _iso8601DateFormat_[] = "%Y-%m-%d";
char const _iso8601DateTimeFormat_[] = "%Y-%m-%d %T";

static i64_t const DAYS_IN_MILION_MONTHS = 30436875;
static int const DAYS_IN_MONTH[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
static int const DAYS_SINCE_JANUARY_FOR_MONTH[] = {
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

inline bool is_leap_year( int year_ ) {
	return ( ( ( ( year_ % 4 ) == 0 ) && ( year_ % 100 ) != 0 ) || ( ( year_ % 400 ) == 0 ) );
}

inline i64_t yaal_epoch_to_unix_epoch( i64_t time_ ) {
	return ( time_ - HTime::SECONDS_TO_UNIX_EPOCH );
}

inline i64_t unix_epoch_to_yaal_epoch( i64_t time_ ) {
	return ( time_ + HTime::SECONDS_TO_UNIX_EPOCH );
}

HTime::HTime( TZ tz_, char const* format_ )
	: _value(), _broken(), _tz( tz_ ), _format( format_ ), _cache() {
	M_PROLOG
	set_now();
	return;
	M_EPILOG
}

HTime::HTime( yaal::hcore::HString const& strTime_, char const* format_ )
	: HTime( TZ::LOCAL, strTime_, format_ ) {
	M_PROLOG
	from_string( strTime_ );
	return;
	M_EPILOG
}

HTime::HTime( TZ tz_, yaal::hcore::HString const& strTime_, char const* format_ )
	: _value(), _broken(), _tz( tz_ ), _format( format_ ), _cache() {
	M_PROLOG
	from_string( strTime_ );
	return;
	M_EPILOG
}

HTime::HTime( HTime const& time_ )
	: _value( time_._value ), _broken( time_._broken ), _tz( time_._tz ),
	_format( time_._format ), _cache() {
	M_PROLOG
	return;
	M_EPILOG
}

HTime::HTime( i64_t time_, char const* format_ )
	: HTime( TZ::LOCAL, time_, format_ ) {
	return;
}

HTime::HTime( TZ tz_, i64_t time_, char const* format_ )
	: _value( time_ ), _broken(), _tz( tz_ ), _format( format_ ), _cache() {
	M_PROLOG
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value ) ) );
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return;
	M_EPILOG
}

HTime::HTime( int year_, int month_, int day_,
		int hour_, int minute_, int second_, char const* format_ )
	: HTime( TZ::LOCAL, year_, month_, day_, hour_, minute_, second_, format_ ) {
	return;
}

HTime::HTime( TZ tz_, int year_, int month_, int day_,
		int hour_, int minute_, int second_, char const* format_ )
	: _value(), _broken(), _tz( tz_ ), _format( format_ ), _cache() {
	M_PROLOG
	set_datetime( year_, month_, day_, hour_, minute_, second_ );
	return;
	M_EPILOG
}

HTime::~HTime( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

HTime& HTime::set( i64_t time_ ) {
	M_PROLOG
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value = time_ ) ) );
	/* In Visual Studio C++ localtime_r is a macro and cannot be prefixed with ::. */
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
	M_EPILOG
}

HTime& HTime::set_tz( TZ tz_ ) {
	M_PROLOG
	_tz = tz_;
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value ) ) );
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
	M_EPILOG
}

HTime& HTime::set_now( void ) {
	M_PROLOG
	time_t t( ::time( NULL ) );
	_value = unix_epoch_to_yaal_epoch( t );
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
	M_EPILOG
}

HTime& HTime::set_format( char const* format_ ) {
	M_PROLOG
	_format = format_;
	return ( *this );
	M_EPILOG
}

HTime& HTime::set_time( int hour_, int minute_, int second_ ) {
	M_PROLOG
	if ( ( hour_ < 0 ) || ( hour_ > 23 ) )
		M_THROW( "bad hour", hour_ );
	if ( ( minute_ < 0 ) || ( minute_ > 59 ) )
		M_THROW( "bad minute", minute_ );
	if ( ( second_ < 0 ) || ( second_ > 59 ) )
		M_THROW( "bad second", second_ );
	_broken.tm_hour = hour_;
	_broken.tm_min = minute_;
	_broken.tm_sec = second_;
	_broken.tm_isdst = -1;
	_value = unix_epoch_to_yaal_epoch( _tz == TZ::UTC ? mkgmtime( &_broken ) : mktime( &_broken ) );
	return ( *this );
	M_EPILOG
}

HTime& HTime::set_date( int year_, int month_, int day_ ) {
	M_PROLOG
	if ( ( month_ < 1 ) || ( month_ > 12 ) )
		M_THROW( "bad month in year", month_ );
	if ( ( day_ < 1 ) || ( day_ > 31 ) )
		M_THROW( "bad day of month", day_ );
	_broken.tm_year = year_ - 1900;
	_broken.tm_mon = month_ - 1;
	_broken.tm_mday = day_;
	_broken.tm_isdst = -1;
	_value = unix_epoch_to_yaal_epoch( _tz == TZ::UTC ? mkgmtime( &_broken ) : mktime( &_broken ) );
	return ( *this );
	M_EPILOG
}

HTime& HTime::set_datetime(
		int year_, int month_, int day_,
		int hour_, int minute_, int second_ ) {
	M_PROLOG
	set_date( year_, month_, day_ );
	set_time( hour_, minute_, second_ );
	return ( *this );
	M_EPILOG
}

HTime& HTime::from_string( yaal::hcore::HString const& str_ ) {
	M_PROLOG
	char const* err( ::strptime( str_.c_str(), _format.raw(), &_broken ) );
	if ( ! err ) {
		err = ::strptime( str_.c_str(), _iso8601DateTimeFormat_, &_broken );
	}
	if ( ! err ) {
		err = ::strptime( str_.c_str(), _iso8601DateFormat_, &_broken );
	}
	if ( ! err ) {
		err = ::strptime( str_.c_str(), _iso8601TimeFormat_, &_broken );
	}
	M_ENSURE( err );
	_broken.tm_isdst = -1;
	_value = unix_epoch_to_yaal_epoch( _tz == TZ::UTC ? mkgmtime( &_broken ) : mktime( &_broken ) );
	return ( *this );
	M_EPILOG
}

int HTime::get_year( void ) const {
	M_PROLOG
	return ( _broken.tm_year + 1900 );
	M_EPILOG
}

int HTime::get_month( void ) const {
	M_PROLOG
	return ( _broken.tm_mon + 1 );
	M_EPILOG
}

int HTime::get_day( void ) const {
	M_PROLOG
	return ( _broken.tm_mday );
	M_EPILOG
}

int HTime::get_hour( void ) const {
	M_PROLOG
	return ( _broken.tm_hour );
	M_EPILOG
}

int HTime::get_minute( void ) const {
	M_PROLOG
	return ( _broken.tm_min );
	M_EPILOG
}

int HTime::get_second( void ) const {
	M_PROLOG
	return ( _broken.tm_sec );
	M_EPILOG
}

int HTime::get_day_of_week( void ) const {
	M_PROLOG
	return ( ( _broken.tm_wday + DAYS_IN_WEEK - 1 ) % DAYS_IN_WEEK );
	M_EPILOG
}

int HTime::get_days_in_month( void ) const {
	M_PROLOG
	return ( DAYS_IN_MONTH[_broken.tm_mon] + ( ( _broken.tm_mon == 1 ) && is_leap_year( get_year() ) ? 1 : 0 ) );
	M_EPILOG
}

HTime& HTime::mod_year( int mod_ ) {
	_broken.tm_year += mod_;
	_broken.tm_isdst = -1;
	_value = unix_epoch_to_yaal_epoch( _tz == TZ::UTC ? mkgmtime( &_broken ) : mktime( &_broken ) );
	return ( *this );
}

HTime& HTime::mod_month( int mod_ ) {
	_broken.tm_year += mod_ / MONTHS_IN_YEAR;
	_broken.tm_mon += mod_ % MONTHS_IN_YEAR;
	_broken.tm_isdst = -1;
	if ( _broken.tm_mon < 0 ) {
		_broken.tm_mon += MONTHS_IN_YEAR;
		-- _broken.tm_year;
	} else if ( _broken.tm_mon >= MONTHS_IN_YEAR ) {
		_broken.tm_mon -= MONTHS_IN_YEAR;
		++ _broken.tm_year;
	}
	_value = unix_epoch_to_yaal_epoch( _tz == TZ::UTC ? mkgmtime( &_broken ) : mktime( &_broken ) );
	return ( *this );
}

HTime& HTime::mod_day( int mod_ ) {
	_value += ( mod_ * HOURS_IN_DAY * MINUTES_IN_HOUR * SECONDS_IN_MINUTE );
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value ) ) );
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
}

HTime& HTime::mod_hour( int mod_ ) {
	_value += ( mod_ * MINUTES_IN_HOUR * SECONDS_IN_MINUTE );
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value ) ) );
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
}

HTime& HTime::mod_minute( int mod_ ) {
	_value += ( mod_ * SECONDS_IN_MINUTE );
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value ) ) );
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
}

HTime& HTime::mod_second( int mod_ ) {
	_value += mod_;
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value ) ) );
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
}

void HTime::swap( HTime& time_ ) {
	M_PROLOG
	if ( &time_ != this ) {
		using yaal::swap;
		swap( _value, time_._value );
		swap( _format, time_._format );
		swap( _cache, time_._cache );
		swap( _broken, time_._broken );
	}
	return;
	M_EPILOG
}

HTime& HTime::operator = ( HTime const& time_ ) {
	M_PROLOG
	if ( this != &time_ ) {
		HTime tmp( time_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

HTime& HTime::operator -= ( HTime const& time_ ) {
	M_PROLOG
	_value = static_cast<i64_t>( difftime( static_cast<time_t>( _value ), static_cast<time_t>( time_._value ) ) );
#if SIZEOF_TIME_T == 8
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value ) ) );
#else
	time_t t( static_cast<time_t>( _value ) );
#endif
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
	M_EPILOG
}

HTime HTime::operator - ( HTime const& time_ ) const {
	M_PROLOG
	HTime time( *this );
	time -= time_;
	return ( time );
	M_EPILOG
}

HTime& HTime::operator += ( HTime const& time_ ) {
	M_PROLOG
	_value += time_._value;
	time_t t( static_cast<time_t>( yaal_epoch_to_unix_epoch( _value ) ) );
	M_ENSURE( ( _tz == TZ::UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return ( *this );
	M_EPILOG
}

HTime HTime::operator + ( HTime const& time_ ) const {
	M_PROLOG
	HTime time( *this );
	time += time_;
	return ( time );
	M_EPILOG
}

bool HTime::operator == ( HTime const& time_ ) const {
	M_PROLOG
	return ( _value == time_._value );
	M_EPILOG
}

bool HTime::operator != ( HTime const& time_ ) const {
	M_PROLOG
	return ( _value != time_._value );
	M_EPILOG
}

bool HTime::operator <= ( HTime const& time_ ) const {
	M_PROLOG
	return ( _value <= time_._value );
	M_EPILOG
}

bool HTime::operator >= ( HTime const& time_ ) const {
	M_PROLOG
	return ( _value >= time_._value );
	M_EPILOG
}

bool HTime::operator < ( HTime const& time_ ) const {
	M_PROLOG
	return ( _value < time_._value );
	M_EPILOG
}

bool HTime::operator > ( HTime const& time_ ) const {
	M_PROLOG
	return ( _value > time_._value );
	M_EPILOG
}

HString HTime::to_string( HString const& format_ ) const {
	M_PROLOG
	HString const& format( ! format_.is_empty() ? format_ : _format );
#ifdef HAVE_SMART_STRFTIME
	static int const MIN_TIME_STRING_LENGTH( 32 );
	int long size( static_cast<int long>( ::strftime( NULL, 1024, format.raw(), &_broken ) + 1 ) );
	if ( size < 2 )
		M_THROW( "bad format", errno );
	_cache.realloc( max<int long>( size, MIN_TIME_STRING_LENGTH ) );
	M_ENSURE( static_cast<int>( ::strftime( _cache.get<char>(),
					static_cast<size_t>( size ), format.raw(), &_broken ) ) < size );
#else /* HAVE_SMART_STRFTIME */
	static int const MIN_TIME_STRING_LENGTH( 64 );
	_cache.realloc( MIN_TIME_STRING_LENGTH ); /* FIXME that is pretty dumb hack */
	int long size( static_cast<int long>( ::strftime( _cache.get<char>(), MIN_TIME_STRING_LENGTH - 1, format.raw(), &_broken ) ) + 1 );
	if ( size < 2 )
		M_THROW( "bad format", errno );
#endif /* not HAVE_SMART_STRFTIME */
	return ( _cache.get<char>() );
	M_EPILOG
}

HString HTime::string( void ) const {
	return ( to_string() );
}

i64_t HTime::raw( void ) const {
	return ( _value );
}

HTime operator "" _yt ( char const* str_, size_t len_ ) {
	return ( HString( str_, static_cast<int>( len_ ) ) );
}

HTime operator "" _ys ( int long long unsigned seconds_ ) {
	return ( HTime( HTime::TZ::UTC, static_cast<i64_t>( seconds_ ) ) );
}

HTime operator "" _ym ( int long long unsigned minutes_ ) {
	return ( HTime( HTime::TZ::UTC, static_cast<i64_t>( minutes_ * HTime::SECONDS_IN_MINUTE ) ) );
}

HTime operator "" _yh ( int long long unsigned hours_ ) {
	return ( HTime( HTime::TZ::UTC, static_cast<i64_t>( hours_ * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE ) ) );
}

HTime operator "" _yD ( int long long unsigned days_ ) {
	return ( HTime( HTime::TZ::UTC, static_cast<i64_t>( days_ * HTime::HOURS_IN_DAY * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE ) ) );
}

HTime operator "" _yW ( int long long unsigned weeks_ ) {
	return ( HTime( HTime::TZ::UTC, static_cast<i64_t>( weeks_ * HTime::DAYS_IN_WEEK * HTime::HOURS_IN_DAY * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE ) ) );
}

HTime operator "" _yM ( int long long unsigned months_ ) {
	/*
	 * Month has 30.436875 days on average.
	 * We try our best not to loose any precision so we do all out calculations on 64 bit integers.
	 * First we multiply average number of days in a month by 10^6 so it becomes a integer number.
	 * Next we do calculations trying not to overflow and not to loose precision until last step
	 * which means we do division in parts:
	 * a) 10^6 == 1600 * 625
	 * b) 24 * 60 * 60 == 86400 is divisible by 1600 without remainder ( 86400 / 1600 == 54.(0) )
	 * we use that facts in our calculations.
	 */
	return ( HTime( HTime::TZ::UTC, static_cast<i64_t>( months_ * DAYS_IN_MILION_MONTHS * ( ( HTime::HOURS_IN_DAY * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE ) / 1600 ) ) / 625 ) );
}

HTime operator "" _yY ( int long long unsigned years_ ) {
	return ( HTime( HTime::TZ::UTC, static_cast<int>( years_ ), 1, 1 ) );
}

i64_t mkgmtime( struct tm* tm_ ) {
	int year( tm_->tm_year + 1900 );
	bool leapYear( is_leap_year( year ) );
	i64_t days( year * 365 + ( year / 4 ) - ( year / 100 ) + ( year / 400 ) + ( ! leapYear ? 1 : 0 ) );
	days += DAYS_SINCE_JANUARY_FOR_MONTH[tm_->tm_mon];
	if ( ( tm_->tm_mon > 1 ) && leapYear ) {
		++ days;
	}
	days += ( tm_->tm_mday - 1 );
	i64_t seconds( days * HTime::HOURS_IN_DAY * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE );
	seconds += ( tm_->tm_hour * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE );
	seconds += ( tm_->tm_min * HTime::SECONDS_IN_MINUTE );
	seconds += tm_->tm_sec;
	return ( yaal_epoch_to_unix_epoch( seconds ) );
}

}

}

