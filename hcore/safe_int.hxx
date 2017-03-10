/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  safe_int.hxx - this file is integral part of `yaal' project.

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

