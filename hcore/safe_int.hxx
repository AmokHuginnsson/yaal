/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/safe_int.hxx
 * \brief safe_int<>() functionality is implemented here.
 */

#ifndef YAAL_HCORE_SAFE_CAST_HXX_INCLUDED
#define YAAL_HCORE_SAFE_CAST_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/numeric.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace safe_int {

/*! \brief Cast from one integer type to another making sure that no information is lost.
 *
 * \tparam to_t - destination integer type.
 * \tparam from_t - source integer type.
 * \param value_ - value to be casted to another type.
 * \return The same value as \e value_.
 * \throw yaal::hcore::HOutOfRangeException if \e value_ dit not fit in destination type.
 */
template<typename to_t, typename from_t>
to_t cast( from_t value_ ) {
	static_assert( is_integral<from_t>::value, "Source type is not integral pod." );
	static_assert( is_integral<to_t>::value, "Destination type is not integral pod." );
	if ( static_cast<from_t>( static_cast<to_t>( value_ ) ) != value_ ) {
		throw yaal::hcore::HOutOfRangeException( "Cast would lose data." );
	}
	return ( static_cast<to_t>( value_ ) );
}

template<bool const>
struct integer;

template<>
struct integer<true> {
	template<typename T>
	static T add( T a, T b ) {
		if ( ( a > 0 ) && ( b > ( meta::max_signed<T>::value - a ) ) ) {
			throw yaal::hcore::HOutOfRangeException( "Signed integer addition overflow." );
		}
		if ( ( a < 0 ) && ( b < ( meta::min_signed<T>::value - a ) ) ) {
			throw yaal::hcore::HOutOfRangeException( "Signed integer addition underflow." );
		}
		return ( a + b );
	}
	template<typename T>
	static T sub( T a, T b ) {
		if ( ( a < 0 ) && ( b > ( meta::max_signed<T>::value + a ) ) ) {
			throw yaal::hcore::HOutOfRangeException( "Signed integer subtraction overflow" );
		}
		if ( ( a > 0 ) && ( b < ( meta::min_signed<T>::value + a ) ) ) {
			throw yaal::hcore::HOutOfRangeException( "Signed integer subtraction underflow." );
		}
		return ( a - b );
	}
	template<typename T>
	static T mul( T a, T b ) {
		if ( ( a > 0 ) && ( b > ( meta::max_signed<T>::value / a ) ) ) {
			throw yaal::hcore::HOutOfRangeException( "Signed integer multiplication overflow." );
		} else if ( ( a < 0 ) && ( b < ( meta::max_signed<T>::value / a ) ) ) {
			throw yaal::hcore::HOutOfRangeException( "Signed integer multiplication overflow." );
		}
		if ( ( a < 0 ) && ( b > ( meta::min_signed<T>::value / a ) ) ) {
			throw yaal::hcore::HOutOfRangeException( "Signed integer multiplication underflow." );
		} else if ( ( a > 0 ) && ( b < ( meta::min_signed<T>::value / a ) ) ) {
			throw yaal::hcore::HOutOfRangeException( "Signed integer multiplication underflow." );
		}
		return ( a * b );
	}
};

template<>
struct integer<false> {
	template<typename T>
	static T add( T a, T b ) {
		if ( b > ( meta::max_unsigned<T>::value - a ) ) {
			throw yaal::hcore::HOutOfRangeException( "Unsigned integer addition overflow." );
		}
		return ( a + b );
	}
	template<typename T>
	static T sub( T a, T b ) {
		if ( b > a ) {
			throw yaal::hcore::HOutOfRangeException( "Unsigned integer subtraction underflow." );
		}
		return ( a - b );
	}
	template<typename T>
	static T mul( T a, T b ) {
		if ( b > ( meta::max_unsigned<T>::value / a ) ) {
			throw yaal::hcore::HOutOfRangeException( "Unsigned integer multiplication overflow." );
		}
		return ( a * b );
	}
};

template<typename T>
inline T add( T a, T b ) {
	return ( integer<yaal::meta::is_signed<T>::value>::add( a, b ) );
}

template<typename T>
inline T sub( T a, T b ) {
	return ( integer<yaal::meta::is_signed<T>::value>::sub( a, b ) );
}

template<typename T>
inline T mul( T a, T b ) {
	return ( integer<yaal::meta::is_signed<T>::value>::mul( a, b ) );
}

}

}

#endif /* #ifndef YAAL_HCORE_SAFE_CAST_HXX_INCLUDED */

