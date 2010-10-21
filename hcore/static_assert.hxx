/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	static_assert.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/static_assert.hxx
 * \brief STATIS_ASSERT functionality implemented here.
 */

#ifndef YAAL_HCORE_STATIC_ASSERT_HXX_INCLUDED
#define YAAL_HCORE_STATIC_ASSERT_HXX_INCLUDED 1

#define M_CONCAT_REAL( a, b ) a ## b
#define M_CONCAT( a, b ) M_CONCAT_REAL( a, b )

/*! \brief Perform static (compile time) code check.
 *
 * \param condition - condition to be check during compilation.
 *
 * \post Unmet condition will make compilation fail on line where STATIC_ASSERT is used.
 */
#define STATIC_ASSERT( condition ) typedef char M_CONCAT( SAF, __LINE__ )[ static_assert_failure<( condition )>::value ]

namespace yaal
{

/*! \cond */
template<bool> struct static_assert_failure;
/*! \endcond */
/*! \brief Static assert.
 *
 * If static condition that is specified as a parameter is not met
 * then compilation of an unit fails.
 */
template<> struct static_assert_failure<true> { enum { value = 1 }; };

}

#endif /* #ifndef YAAL_HCORE_STATIC_ASSERT_HXX_INCLUDED */

