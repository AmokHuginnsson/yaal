/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

