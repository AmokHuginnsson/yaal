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

char const _defaultTimeFormat_[] = "%a, %d %b %Y %H:%M:%S %z";
char const _iso8601TimeFormat_[] = "%F %T";

HTime::HTime( now_in_t nowIn_ )
	: _value(), _broken(), _format ( _defaultTimeFormat_ ), _cache() {
	M_PROLOG
	set_now( nowIn_ );
	return;
	M_EPILOG
}

HTime::HTime( char const* strTime_, char const* format_ )
	: _value(), _broken(), _format( format_ ), _cache() {
	M_PROLOG
	char const* err( ::strptime( strTime_, _format.raw(), &_broken ) );
	if ( ! err )
		err = ::strptime( strTime_, "%F %T", &_broken );
	M_ENSURE( err );
	_broken.tm_isdst = -1;
	_value = ::mktime( &_broken );
	return;
	M_EPILOG
}

HTime::HTime( HTime const& time_ )
	: _value( time_._value ), _broken( time_._broken ),
	_format( time_._format ), _cache() {
	M_PROLOG
	return;
	M_EPILOG
}

HTime::HTime( i64_t time_, char const* format_ )
	: _value( time_ ), _broken(), _format( format_ ), _cache() {
	M_PROLOG
	time_t t( static_cast<time_t>( _value ) );
	M_ENSURE( localtime_r( &t, &_broken ) );
	return;
	M_EPILOG
}

HTime::HTime( int year_, int month_, int day_,
							 int hour_, int minute_, int second_, char const* format_ )
	: _value(), _broken(), _format( format_ ), _cache() {
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

void HTime::set( i64_t time_ ) {
	M_PROLOG
	time_t t( static_cast<time_t>( _value = time_ ) );
	/* In Visual Studio C++ localtime_r is a macro and cannot be prefixed with ::. */
	M_ENSURE( localtime_r( &t, &_broken ) );
	return;
	M_EPILOG
}

void HTime::set_now( now_in_t nowIn_ ) {
	M_PROLOG
	time_t t( static_cast<time_t>( _value = ::time( NULL ) ) );
	M_ENSURE( ( nowIn_ == UTC ? gmtime_r( &t, &_broken ) : localtime_r( &t, &_broken ) ) != NULL );
	return;
	M_EPILOG
}

void HTime::set_format( char const* format_ ) {
	M_PROLOG
	_format = format_;
	return;
	M_EPILOG
}

void HTime::set_time( int hour_, int minute_, int second_ ) {
	M_PROLOG
	if ( ( hour_ < 0 ) || ( hour_ > 23 ) )
		M_THROW( "bad hour", hour_ );
	_broken.tm_hour = hour_;
	if ( ( minute_ < 0 ) || ( minute_ > 59 ) )
		M_THROW( "bad minute", minute_ );
	_broken.tm_min = minute_;
	if ( ( second_ < 0 ) || ( second_ > 59 ) )
		M_THROW( "bad second", second_ );
	_broken.tm_sec = second_;
	_value = ::mktime( &_broken );
	return;
	M_EPILOG
}

void HTime::set_date( int year_, int month_, int day_ ) {
	M_PROLOG
	_broken.tm_year = year_ - 1900;
	if ( ( month_ < 1 ) || ( month_ > 12 ) )
		M_THROW( "bad month in year", month_ );
	_broken.tm_mon = month_ - 1;
	if ( ( day_ < 1 ) || ( day_ > 31 ) )
		M_THROW( "bad day of month", day_ );
	_broken.tm_mday = day_;
	_value = ::mktime( &_broken );
	return;
	M_EPILOG
}

void HTime::set_datetime( int year_, int month_,
											 int day_, int hour_,
											 int minute_, int second_ ) {
	M_PROLOG
	set_date( year_, month_, day_ );
	set_time( hour_, minute_, second_ );
	return;
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

HTime HTime::operator - ( HTime const& time_ ) const {
	M_PROLOG
	HTime time( *this );
	time -= time_;
	return ( time );
	M_EPILOG
}

HTime & HTime::operator -= ( HTime const& time_ ) {
	M_PROLOG
	_value = static_cast<i64_t>( difftime( static_cast<time_t>( _value ), static_cast<time_t>( time_._value ) ) );
	time_t t( static_cast<time_t>( _value ) );
	M_ENSURE( gmtime_r( &t, &_broken ) );
	return ( *this );
	M_EPILOG
}

bool HTime::operator == ( i64_t time_ ) const {
	M_PROLOG
	return ( _value == time_ );
	M_EPILOG
}

bool HTime::operator != ( i64_t time_ ) const {
	M_PROLOG
	return ( _value != time_ );
	M_EPILOG
}

bool HTime::operator <= ( i64_t time_ ) const {
	M_PROLOG
	return ( _value <= time_ );
	M_EPILOG
}

bool HTime::operator >= ( i64_t time_ ) const {
	M_PROLOG
	return ( _value >= time_ );
	M_EPILOG
}

bool HTime::operator < ( i64_t time_ ) const {
	M_PROLOG
	return ( _value < time_ );
	M_EPILOG
}

bool HTime::operator > ( i64_t time_ ) const {
	M_PROLOG
	return ( _value > time_ );
	M_EPILOG
}

HString HTime::string( void ) const {
	M_PROLOG
#ifdef HAVE_SMART_STRFTIME
	static int const MIN_TIME_STRING_LENGTH( 32 );
	int long size( ::strftime( NULL, 1024, _format.raw(), &_broken ) + 1 );
	if ( size < 2 )
		M_THROW( "bad format", errno );
	_cache.realloc( max<int long>( size, MIN_TIME_STRING_LENGTH ) );
	M_ENSURE( static_cast<int>( ::strftime( _cache.get<char>(),
					size, _format.raw(), &_broken ) ) < size );
#else /* HAVE_SMART_STRFTIME */
	static int const MIN_TIME_STRING_LENGTH( 64 );
	_cache.realloc( MIN_TIME_STRING_LENGTH ); /* FIXME that is pretty dumb hack */
	int long size( static_cast<int long>( ::strftime( _cache.get<char>(), MIN_TIME_STRING_LENGTH - 1, _format.raw(), &_broken ) ) + 1 );
	if ( size < 2 )
		M_THROW( "bad format", errno );
#endif /* not HAVE_SMART_STRFTIME */
	return ( _cache.get<char>() );
	M_EPILOG
}

i64_t HTime::raw( void ) const {
	return ( _value );
}

}

}

