/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

yaal::hcore::HString duration_to_string( duration_t val_, time::UNIT unit_ ) {
	M_PROLOG
	HString s;
	time::duration_t d( val_ );
	i64_t x( time::in_units<time::UNIT::WEEK>( d ) );
	if ( x > 0 ) {
		s.append( x ).append( " week" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, time::UNIT::WEEK );
	}
	x = time::in_units<UNIT::DAY>( d );
	if ( ( unit_ <= UNIT::DAY ) && ( x > 0 ) ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " day" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, UNIT::DAY );
	}
	x = time::in_units<UNIT::HOUR>( d );
	if ( ( unit_ <= UNIT::HOUR ) && ( x > 0 ) ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " hour" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, UNIT::HOUR );
	}
	x = time::in_units<UNIT::MINUTE>( d );
	if ( ( unit_ <= UNIT::MINUTE ) && ( x > 0 ) ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " minute" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, UNIT::MINUTE );
	}
	x = time::in_units<UNIT::SECOND>( d );
	if ( ( unit_ <= UNIT::SECOND ) && ( x > 0 ) ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " second" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, UNIT::SECOND );
	}
	x = time::in_units<UNIT::MILLISECOND>( d );
	if ( ( unit_ <= UNIT::MILLISECOND ) && ( x > 0 ) ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " millisecond" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, UNIT::MILLISECOND );
	}
	x = time::in_units<UNIT::MICROSECOND>( d );
	if ( ( unit_ <= UNIT::MICROSECOND ) && ( x > 0 ) ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " microsecond" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, UNIT::MICROSECOND );
	}
	x = time::in_units<UNIT::NANOSECOND>( d );
	if ( ( unit_ <= UNIT::NANOSECOND ) && ( x > 0 ) ) {
		s.append( ! s.is_empty() ? " " : "" ).append( x ).append( " nanosecond" ).append( x > 1 ? "s" : "" );
		d -= time::duration( x, UNIT::NANOSECOND );
	}
	if ( s.is_empty() ) {
		s.assign( "0s" );
	}
	return ( s );
	M_EPILOG
}

UNIT scale( duration_t duration_, UNIT minScale_ ) {
	UNIT unit( UNIT::NANOSECOND );
	i64_t x( 0 );
	x = time::in_units<UNIT::MICROSECOND>( duration_ );
	if ( x > 0 ) {
		unit = UNIT::NANOSECOND;
	}
	x = time::in_units<UNIT::MILLISECOND>( duration_ );
	if ( x > 0 ) {
		unit = UNIT::MICROSECOND;
	}
	x = time::in_units<UNIT::SECOND>( duration_ );
	if ( x > 0 ) {
		unit = UNIT::MILLISECOND;
	}
	x = time::in_units<UNIT::MINUTE>( duration_ );
	if ( x > 0 ) {
		unit = UNIT::SECOND;
	}
	x = time::in_units<UNIT::HOUR>( duration_ );
	if ( x > 0 ) {
		unit = UNIT::MINUTE;
	}
	x = time::in_units<UNIT::DAY>( duration_ );
	if ( x > 0 ) {
		unit = UNIT::HOUR;
	}
	x = time::in_units<UNIT::WEEK>( duration_ );
	if ( x > 0 ) {
		unit = UNIT::DAY;
	}
	if ( minScale_ < unit ) {
		unit = minScale_;
	}
	return ( unit );
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
		pos = val_.find_other_than( character_class<CHARACTER_CLASS::WHITESPACE>().data(), pos );
		int long numEnd( val_.find_other_than( character_class<CHARACTER_CLASS::DIGIT>().data(), pos ) );
		if ( numEnd == HString::npos ) {
			throw HLexicalCastException( "Malformed duration specification at: "_ys.append( pos ) );
		}
		i64_t v( lexical_cast<i64_t>( val_.substr( pos, numEnd - pos ) ) );
		pos = numEnd;
		pos = val_.find_other_than( character_class<CHARACTER_CLASS::WHITESPACE>().data(), pos );
		int long unitStart( val_.find_one_of( character_class<CHARACTER_CLASS::LETTER>().data(), pos ) );
		if ( unitStart == HString::npos ) {
			throw HLexicalCastException( "Malformed duration specification at: "_ys.append( pos ) );
		}
		pos = unitStart;
		int long unitEnd( val_.find_other_than( character_class<CHARACTER_CLASS::LETTER>().data(), pos ) );
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
	return ( duration_to_string( val_, scale( val_ ) ) );
}

}

