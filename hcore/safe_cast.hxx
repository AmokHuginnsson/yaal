/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  safe_cast.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/safe_cast.hxx
 * \brief safe_cast<>() functionality is implemented here.
 */

#ifndef YAAL_HCORE_SAFE_CAST_HXX_INCLUDED
#define YAAL_HCORE_SAFE_CAST_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

/*! \brief Cast from one integer type to another making sure that no information is lost.
 *
 * \tparam to_t - destination integer type.
 * \tparam from_t - source integer type.
 * \param value_ - value to be casted to another type.
 * \return The same value as \e value_.
 * \throw HOutOfRangeException if \e value_ dit not fit in destination type.
 */
template<typename to_t, typename from_t>
to_t safe_cast( from_t value_ ) {
	static_assert( is_integral<from_t>::value, "Source type is not integral pod." );
	static_assert( is_integral<to_t>::value, "Destination type is not integral pod." );
	if ( static_cast<from_t>( static_cast<to_t>( value_ ) ) != value_ ) {
		throw yaal::hcore::HOutOfRangeException( "Cast would lose data." );
	}
	return ( static_cast<to_t>( value_ ) );
}

}

#endif /* #ifndef YAAL_HCORE_SAFE_CAST_HXX_INCLUDED */

