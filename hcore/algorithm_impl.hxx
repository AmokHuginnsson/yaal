/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/algorithm_impl.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hcore/algorithm_impl.hxx
 * \brief Implementation of low level alogrithms.
 */

#ifndef YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED
#define YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED 1

#include "hcore/hexception.hxx"

namespace yaal
{

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

/*! \cond */
template<typename type_t>
int long distance( type_t* first_, type_t* last_ )
	{
	M_ASSERT( last_ > first_ );
	return ( last_ - first_ );
	}
/*! \endcond */
/*! \brief Calculate distance between two iterators.
 *
 * \param first - iterator.
 * \param last - iterator.
 * \return last - first.
 */
template<typename iter_t>
int long distance( iter_t first, iter_t last )
	{
	int long dist( 0 );
	while ( first != last )
		++ first, ++ dist;
	return ( dist );
	}

/*! \cond */
template<typename type_t>
void advance( type_t*& it_, int long distance_ )
	{ it_ += distance_; }
/*! \endcond */
/*! \brief Move iterator forward.
 *
 * \param it - iterator to be moved.
 * \param dist - how far iterator shall be moved.
 */
template<typename iter_t>
void advance( iter_t& it, int long dist )
	{
	for ( int long i = 0; i < dist; ++ i, ++ it )
		;
	}

}

#endif /* #ifndef YAAL_HCORE_ALGORITHM_IMPL_HXX_INCLUDED */

