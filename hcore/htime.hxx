/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/htime.hxx
 * \brief Declaration of HTime class.
 */

#ifndef YAAL_HCORE_HTIME_HXX_INCLUDED
#define YAAL_HCORE_HTIME_HXX_INCLUDED 1

#include <ctime>

#include "hcore/pod.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hchunk.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const _rfc2822DateTimeFormat_[];
extern M_YAAL_HCORE_PUBLIC_API char const _rfc7231DateTimeFormat_[];
extern M_YAAL_HCORE_PUBLIC_API char const _iso8601DateFormat_[];
extern M_YAAL_HCORE_PUBLIC_API char const _iso8601TimeFormat_[];
extern M_YAAL_HCORE_PUBLIC_API char const _iso8601DateTimeFormat_[];

/*! \brief Date and time handling class.
 *
 * HTime stores number of seconds since epoch:
 * 0-01-01 00:00:00 - on systems with 64 bit time_t,
 * 1970-01-01 00:00:00 - on system with 32 bit time_t.
 * If we want to have broken-down time diff we must compensate for that fact.
 * In short time-diff makes no sense for dates in broken-down time form
 * on 32 bit time_t systems.
 */
class HTime final {
public:
	typedef HTime this_type;
	typedef i64_t value_type;
	enum class TZ {
		UTC,
		LOCAL
	};
	struct MONTH {
		enum month_t {
			JANUARY = 1,
			FEBRUARY,
			MARCH,
			APRIL,
			MAY,
			JUNE,
			JULY,
			AUGUST,
			SEPTEMBER,
			OCTOBER,
			NOVEMBER,
			DECEMBER
		};
	};
	enum class DAY_OF_WEEK {
		MONDAY = 0,
		TUESDAY,
		WEDNESDAY,
		THURSDAY,
		FRIDAY,
		SATURDAY,
		SUNDAY
	};
	static int const MONTHS_IN_YEAR = 12;
	static int const DAYS_IN_WEEK = 7;
	static int const HOURS_IN_DAY = 24;
	static int const MINUTES_IN_HOUR = 60;
	static int const SECONDS_IN_MINUTE = 60;
private:
	value_type _value;
	tm _broken;
	TZ _tz;
	HUTF8String _format;
	mutable HChunk _cache;
public:
	/*! \brief Construct HTime object based on current moment in time.
	 *
	 * \param tz_ - get time as UTC or local time (time considering current time zone).
	 * \param format_ - textual time representation format.
	 */
	HTime( TZ tz_, char const* format_ = _iso8601DateTimeFormat_ );
	HTime( HTime const& );
	/*! \brief Construct time object from string.
	 *
	 * \param time_ - textual representation of time.
	 * \param format_ - textual time representation format.
	 */
	HTime( yaal::hcore::HString const& time_, char const* format_ = _iso8601DateTimeFormat_ );
	HTime( TZ, yaal::hcore::HString const& time_, char const* format_ = _iso8601DateTimeFormat_ );
	HTime( i64_t, char const* = _iso8601DateTimeFormat_ );
	HTime( TZ, i64_t, char const* = _iso8601DateTimeFormat_ );
	HTime( int, int, int, int = 0, int = 0, int = 0, char const* = _iso8601DateTimeFormat_ );
	HTime( TZ, int, int, int, int = 0, int = 0, int = 0, char const* = _iso8601DateTimeFormat_ );
	~HTime( void );
	void swap( HTime& );
	HTime& set_tz( TZ );
	HTime& set_now( void );
	HTime& set( i64_t );
	HTime& set_format( HUTF8String const& = _iso8601DateTimeFormat_ );
	HTime& set_time( int = 0, int = 0, int = 0 );
	HTime& set_date( int, int = 1, int = 1 );
	HTime& set_datetime( int, int = 1, int = 1, int = 0, int = 0, int = 0 );
	HTime& from_string( yaal::hcore::HString const& );
	int get_year( void ) const;
	MONTH::month_t get_month( void ) const;
	int get_day( void ) const;
	int get_hour( void ) const;
	int get_minute( void ) const;
	int get_second( void ) const;
	DAY_OF_WEEK get_day_of_week( void ) const;
	int get_days_in_month( void ) const;
	HTime& mod_year( int );
	HTime& mod_month( int );
	HTime& mod_day( int );
	HTime& mod_hour( int );
	HTime& mod_minute( int );
	HTime& mod_second( int );
	HTime& operator = ( HTime const& );
	HTime& operator -= ( HTime const& );
	HTime operator - ( HTime const& ) const;
	HTime& operator += ( HTime const& );
	HTime operator + ( HTime const& ) const;
	bool operator == ( HTime const& ) const;
	bool operator != ( HTime const& ) const;
	bool operator <= ( HTime const& ) const;
	bool operator >= ( HTime const& ) const;
	bool operator < ( HTime const& ) const;
	bool operator > ( HTime const& ) const;
	HString string( void ) const;
	HString to_string( HUTF8String const& = HUTF8String() ) const;
	i64_t raw( void ) const;
};

typedef HExceptionT<HTime> HTimeException;

HTime operator "" _yt ( char const*, size_t );
HTime operator "" _ys ( int long long unsigned );
HTime operator "" _ym ( int long long unsigned );
HTime operator "" _yh ( int long long unsigned );
HTime operator "" _yD ( int long long unsigned );
HTime operator "" _yW ( int long long unsigned );
HTime operator "" _yM ( int long long unsigned );
HTime operator "" _yY ( int long long unsigned );

inline HTime now_utc( void ) {
	return ( HTime( HTime::TZ::UTC ) );
}

inline HTime now_local( void ) {
	return ( HTime( HTime::TZ::LOCAL ) );
}

/*! \brief Get number of seconds since Unix Epoch.
 *
 * Also fix day of week.
 *
 * \param tm_ - broken down time.
 * \return Number of seconds since Unix Epoch.
 */
i64_t mkgmtime( struct tm* tm_ );

/*! \brief Convert UNIX timestamp to absolute time.
 *
 * \param time_ - number of seconds since beginning of UNIX epoch.
 * \return Number of seconds since beginning of AD era.
 */
i64_t unix_epoch_to_yaal_epoch( i64_t time_ );

/*! \brief Convert absolute time to UNIX timestamp.
 *
 * \param time_ - number of seconds since beginning of AD era.
 * \return Number of seconds since beginning of UNIX epoch.
 */
i64_t yaal_epoch_to_unix_epoch( i64_t time_ );

inline void swap( yaal::hcore::HTime& a, yaal::hcore::HTime& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HTIME_HXX_INCLUDED */

