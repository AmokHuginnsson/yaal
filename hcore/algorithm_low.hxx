/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/algorithm_low.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/hcore/algorithm_low.hxx
 * \brief Implementation of low level alogrithms.
 */

#ifndef YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED
#define YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED 1

namespace yaal
{

/*! \brief Swap contents of two variables.
 *
 * \param left - first variable to be swapped.
 * \param right - second variable to be swapped.
 *
 * \post After the call left holds value of right from before call, and right holds value of left from before call.
 */
template<typename tType>
inline void swap( tType& left, tType& right )
	{
	if ( &left != &right )
		{
		tType tmp( left );
		left = right;
		right = tmp;
		}
	return;
	}

/*! \brief Get smaller of two values.
 *
 * \param left - first value to be considered as smaller.
 * \param right -  second value to be considered as smaller.
 * \return Smaller of two values.
 */
template<typename tType>
inline tType min( tType const& left, tType const& right )
	{
	return ( left < right ? left : right );
	}

/*! \brief Get bigger of two values.
 *
 * \param left - first value to be considered as bigger.
 * \param right -  second value to be considered as bigger.
 * \return Bigger of two values.
 */
template<typename tType>
inline tType max( tType const& left, tType const& right )
	{
	return ( left >= right ? left : right );
	}

}

#endif /* #ifndef YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED */

