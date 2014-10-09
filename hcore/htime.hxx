/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htime.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HTIME_HXX_INCLUDED
#define YAAL_HCORE_HTIME_HXX_INCLUDED 1

#include <ctime>

#include "hcore/pod.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hchunk.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const _defaultTimeFormat_[];
extern M_YAAL_HCORE_PUBLIC_API char const _iso8601DateFormat_[];
extern M_YAAL_HCORE_PUBLIC_API char const _iso8601TimeFormat_[];
extern M_YAAL_HCORE_PUBLIC_API char const _iso8601DateTimeFormat_[];

/*! \brief Date and time handling class.
 *
 * HTime stores number of seconds since epoch (1970-01-01).
 * If we want to have broken-down time diff we must compensate for that fact.
 * In short time-diff makes no sense for dates in broken-down time form.
 */
class HTime {
public:
	typedef HTime this_type;
	typedef enum {
		UTC,
		LOCAL
	} now_in_t;
	static int const MONTHS_IN_YEAR = 12;
	static int const DAYS_IN_WEEK = 7;
	static int const HOURS_IN_DAY = 24;
	static int const MINUTES_IN_HOUR = 60;
	static int const SECONDS_IN_MINUTE = 60;
private:
	i64_t _value;
	tm _broken;
	HString	_format;
	mutable HChunk _cache;
public:
	/*! \brief Construct HTime object based on current moment in time.
	 *
	 * \param nowIn_ - get time as UTC or local time (time considering current time zone).
	 * \param format_ - textual time representation format.
	 */
	HTime( now_in_t nowIn_, char const* format_ = _defaultTimeFormat_ );
	HTime( HTime const& );
	/*! \brief Construct time object from string.
	 *
	 * \param time_ - textual representation of time.
	 * \param format_ - textual time representation format.
	 */
	HTime( yaal::hcore::HString const& time_, char const* format_ = _defaultTimeFormat_ );
	HTime( i64_t, char const* = _defaultTimeFormat_ );
	HTime( int, int, int, int = 0, int = 0, int = 0, char const* = _defaultTimeFormat_ );
	virtual ~HTime ( void );
	void swap( HTime& );
	void set_now( now_in_t );
	void set( i64_t );
	void set_format( char const* = _defaultTimeFormat_ );
	void set_time( int = 0, int = 0, int = 0 );
	void set_date( int, int = 1, int = 1 );
	void set_datetime( int, int = 1, int = 1, int = 0, int = 0, int = 0 );
	void from_string( yaal::hcore::HString const& );
	int get_year( void ) const;
	int get_month( void ) const;
	int get_day( void ) const;
	int get_hour( void ) const;
	int get_minute( void ) const;
	int get_second( void ) const;
	int get_day_of_week( void ) const;
	int get_days_in_month( void ) const;
	void mod_year( int );
	void mod_month( int );
	void mod_day( int );
	void mod_hour( int );
	void mod_minute( int );
	void mod_second( int );
	HTime& operator = ( HTime const& );
	HTime operator - ( HTime const& ) const;
	HTime& operator -= ( HTime const& );
	bool operator == ( HTime const& ) const;
	bool operator != ( HTime const& ) const;
	bool operator <= ( HTime const& ) const;
	bool operator >= ( HTime const& ) const;
	bool operator < ( HTime const& ) const;
	bool operator > ( HTime const& ) const;
	HString string( void ) const;
	HString to_string( HString const& = HString() ) const;
	i64_t raw( void ) const;
};

typedef HExceptionT<HTime> HTimeException;

inline HTime now_utc( void ) {
	return ( HTime( HTime::UTC ) );
}

inline HTime now_local( void ) {
	return ( HTime( HTime::LOCAL ) );
}

inline void swap( yaal::hcore::HTime& a, yaal::hcore::HTime& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HTIME_HXX_INCLUDED */

