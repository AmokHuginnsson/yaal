/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/algorithm_low.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hcore/algorithm_low.hxx
 * \brief Implementation of low level alogrithms.
 */

#ifndef YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED
#define YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED 1

#if CXX_STANDARD >= 2011
#include "hcore/trait.hxx"
#endif /* #if CXX_STANDARD >= 2011 */

namespace yaal {

#if CXX_STANDARD >= 2011
/*! \brief If possible force use of move constructor or move assignment operator on lvalues.
 */
template<typename tType>
constexpr typename trait::strip_reference<tType>::type&& move( tType&& val_ ) {
	return ( static_cast<typename trait::strip_reference<tType>::type&&>( val_ ) );
}

/*! \brief Swap contents of two variables.
 *
 * \param left - first variable to be swapped.
 * \param right - second variable to be swapped.
 *
 * \post After the call left holds value of right from before call, and right holds value of left from before call.
 */
template<typename tType>
inline void swap( tType& left, tType& right ) {
	if ( &left != &right ) {
		tType tmp( yaal::move( left ) );
		left = yaal::move( right );
		right = yaal::move( tmp );
	}
	return;
}
#else /* #if CXX_STANDARD >= 2011 */
/*! \brief Swap contents of two variables.
 *
 * \param left - first variable to be swapped.
 * \param right - second variable to be swapped.
 *
 * \post After the call left holds value of right from before call, and right holds value of left from before call.
 */
template<typename tType>
inline void swap( tType& left, tType& right ) {
	if ( &left != &right ) {
		tType tmp( left );
		left = right;
		right = tmp;
	}
	return;
}
#endif /* #else #if CXX_STANDARD >= 2011 */

/*! \brief Get smaller of two values.
 *
 * \param left - first value to be considered as smaller.
 * \param right -  second value to be considered as smaller.
 * \return Smaller of two values.
 */
template<typename tType>
inline tType min( tType const& left, tType const& right ) {
	return ( left < right ? left : right );
}

/*! \brief Get bigger of two values.
 *
 * \param left - first value to be considered as bigger.
 * \param right -  second value to be considered as bigger.
 * \return Bigger of two values.
 */
template<typename tType>
inline tType max( tType const& left, tType const& right ) {
	return ( left >= right ? left : right );
}

}

#endif /* #ifndef YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED */

