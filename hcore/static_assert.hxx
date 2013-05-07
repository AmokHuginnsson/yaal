/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	static_assert.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/static_assert.hxx
 * \brief STATIC_ASSERT functionality implemented here.
 */

#ifndef YAAL_HCORE_STATIC_ASSERT_HXX_INCLUDED
#define YAAL_HCORE_STATIC_ASSERT_HXX_INCLUDED 1

#include "hcore/macro.hxx"

#ifdef STATIC_ASSERT
#	error Yaal redefines STATIC_ASSERT macro.
#endif /* #ifdef STATIC_ASSERT */

/*! \brief Perform static (compile time) code check.
 *
 * \param condition - condition to be check during compilation.
 *
 * \post Unmet condition will make compilation fail on line where STATIC_ASSERT is used.
 */
#define STATIC_ASSERT( condition ) typedef char M_CONCAT( SAF, __LINE__ )[ static_assert_failure<( condition )>::value ] __attribute__((unused))

namespace yaal {

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

