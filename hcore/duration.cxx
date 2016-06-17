/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  duration.cxx - this file is integral part of `yaal' project.

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
#include "duration.hxx"
#include "si.hxx"
#include "htime.hxx"
#include "hhashmap.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hcore {

namespace time {

duration_t duration( yaal::i64_t duration_, UNIT unit_ ) {
	duration_t nanoseconds( 0 );
	switch ( unit_ ) {
		case ( UNIT::WEEK ): {
			nanoseconds = duration_t( duration_ * HTime::DAYS_IN_WEEK * HTime::HOURS_IN_DAY * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE * si::NANO_IN_WHOLE );
		} break;
		case ( UNIT::DAY ): {
			nanoseconds = duration_t( duration_ * HTime::HOURS_IN_DAY * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE * si::NANO_IN_WHOLE );
		} break;
		case ( UNIT::HOUR ): {
			nanoseconds = duration_t( duration_ * HTime::MINUTES_IN_HOUR * HTime::SECONDS_IN_MINUTE * si::NANO_IN_WHOLE );
		} break;
		case ( UNIT::MINUTE ): {
			nanoseconds = duration_t( duration_ * HTime::SECONDS_IN_MINUTE * si::NANO_IN_WHOLE );
		} break;
		case ( UNIT::SECOND ): {
			nanoseconds = duration_t( duration_ * si::NANO_IN_WHOLE );
		} break;
		case ( UNIT::MILLISECOND ): {
			nanoseconds = duration_t( duration_ * si::NANO_IN_MILLI );
		} break;
		case ( UNIT::MICROSECOND ): {
			nanoseconds = duration_t( duration_ * si::NANO_IN_MICRO );
		} break;
		case ( UNIT::NANOSECOND ): {
			nanoseconds = duration_t( duration_ );
		} break;
	}
	return ( nanoseconds );
}

template <>
yaal::i64_t in_units<UNIT::NANOSECOND>( duration_t d_ ) {
	return ( d_.get() );
}

template <>
yaal::i64_t in_units<UNIT::MICROSECOND>( duration_t d_ ) {
	return ( d_.get() / si::NANO_IN_MICRO );
}

template <>
yaal::i64_t in_units<UNIT::MILLISECOND>( duration_t d_ ) {
	return ( d_.get() / si::NANO_IN_MILLI );
}

template <>
yaal::i64_t in_units<UNIT::SECOND>( duration_t d_ ) {
	return ( d_.get() / si::NANO_IN_WHOLE );
}

template <>
yaal::i64_t in_units<UNIT::MINUTE>( duration_t d_ ) {
	return ( ( d_.get() / si::NANO_IN_WHOLE ) / HTime::SECONDS_IN_MINUTE );
}

template <>
yaal::i64_t in_units<UNIT::HOUR>( duration_t d_ ) {
	return ( ( ( d_.get() / si::NANO_IN_WHOLE ) / HTime::SECONDS_IN_MINUTE ) / HTime::MINUTES_IN_HOUR );
}

template <>
yaal::i64_t in_units<UNIT::DAY>( duration_t d_ ) {
	return ( ( ( ( d_.get() / si::NANO_IN_WHOLE ) / HTime::SECONDS_IN_MINUTE ) / HTime::MINUTES_IN_HOUR ) / HTime::HOURS_IN_DAY );
}

template <>
yaal::i64_t in_units<UNIT::WEEK>( duration_t d_ ) {
	return ( ( ( ( ( d_.get() / si::NANO_IN_WHOLE ) / HTime::SECONDS_IN_MINUTE ) / HTime::MINUTES_IN_HOUR ) / HTime::HOURS_IN_DAY ) / HTime::DAYS_IN_WEEK );
}

}

}

namespace {
typedef yaal::hcore::HHashMap<yaal::hcore::HString, time::UNIT> str_to_duration_unit_t;
str_to_duration_unit_t _durationUnits_ = {
	{ "ns", time::UNIT::NANOSECOND },
	{ "nanosecond", time::UNIT::NANOSECOND },
	{ "nanoseconds", time::UNIT::NANOSECOND },
	{ "us", time::UNIT::MICROSECOND },
	{ "microsecond", time::UNIT::MICROSECOND },
	{ "microseconds", time::UNIT::MICROSECOND },
	{ "ms", time::UNIT::MILLISECOND },
	{ "millisecond", time::UNIT::MILLISECOND },
	{ "milliseconds", time::UNIT::MILLISECOND },
	{ "s", time::UNIT::SECOND },
	{ "sec", time::UNIT::SECOND },
	{ "secs", time::UNIT::SECOND },
	{ "second", time::UNIT::SECOND },
	{ "seconds", time::UNIT::SECOND },
	{ "m", time::UNIT::MINUTE },
	{ "min", time::UNIT::MINUTE },
	{ "mins", time::UNIT::MINUTE },
	{ "minute", time::UNIT::MINUTE },
	{ "minutes", time::UNIT::MINUTE },
	{ "h", time::UNIT::HOUR },
	{ "hour", time::UNIT::HOUR },
	{ "hours", time::UNIT::HOUR },
	{ "d", time::UNIT::DAY },
	{ "day", time::UNIT::DAY },
	{ "days", time::UNIT::DAY },
	{ "w", time::UNIT::WEEK },
	{ "week", time::UNIT::WEEK },
	{ "weeks", time::UNIT::WEEK }
};
}

template<>
hcore::time::duration_t lexical_cast( hcore::HString const& val_ ) {
	M_PROLOG
	time::duration_t d( 0 );
	int long pos( 0 );
	HString unitStr;
	while ( pos < val_.get_length() ) {
		pos = val_.find_other_than( _whiteSpace_.data(), pos );
		int long numEnd( val_.find_other_than( _digit_.data(), pos ) );
		if ( numEnd == HString::npos ) {
			throw HLexicalCastException( "Malformed duration specification at: "_ys.append( pos ) );
		}
		i64_t v( lexical_cast<i64_t>( val_.substr( pos, numEnd - pos ) ) );
		pos = numEnd;
		pos = val_.find_other_than( _whiteSpace_.data(), pos );
		int long unitStart( val_.find_one_of( _letter_.data(), pos ) );
		if ( unitStart == HString::npos ) {
			throw HLexicalCastException( "Malformed duration specification at: "_ys.append( pos ) );
		}
		pos = unitStart;
		int long unitEnd( val_.find_other_than( _letter_.data(), pos ) );
		if ( unitEnd == HString::npos ) {
			unitEnd = val_.get_length();
		}
		time::UNIT unit( time::UNIT::SECOND );
		unitStr = val_.substr( pos, unitEnd - pos );
		unitStr.lower();
		try {
			unit = _durationUnits_.at( unitStr );
		} catch ( ... ) {
			throw HLexicalCastException( "Invalid duration unit `"_ys.append( unitStr ).append( "' at: " ).append( pos ) );
		}
		pos = unitEnd;
		d += time::duration( v, unit );
	}
	return ( d );
	M_EPILOG
}

template<>
HString lexical_cast( hcore::time::duration_t const& val_ ) {
	M_PROLOG
	HString s;
	time::duration_t d( val_ );
	i64_t x( time::in_units<time::UNIT::WEEK>( d ) );
	if ( x > 0 ) {
		s.append( x ).append( " week" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::WEEK );
	}
	x = time::in_units<time::UNIT::DAY>( d );
	if ( x > 0 ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " day" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::DAY );
	}
	x = time::in_units<time::UNIT::HOUR>( d );
	if ( x > 0 ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " hour" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::HOUR );
	}
	x = time::in_units<time::UNIT::MINUTE>( d );
	if ( x > 0 ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " minute" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::MINUTE );
	}
	x = time::in_units<time::UNIT::SECOND>( d );
	if ( x > 0 ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " second" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::SECOND );
	}
	x = time::in_units<time::UNIT::MILLISECOND>( d );
	if ( x > 0 ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " millisecond" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::MILLISECOND );
	}
	x = time::in_units<time::UNIT::MICROSECOND>( d );
	if ( x > 0 ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " microsecond" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::MICROSECOND );
	}
	x = time::in_units<time::UNIT::NANOSECOND>( d );
	if ( x > 0 ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " nanosecond" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::NANOSECOND );
	}
	if ( s.is_empty() ) {
		s.assign( "0s" );
	}
	return ( s );
	M_EPILOG
}

}

