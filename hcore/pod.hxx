/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	pod.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/pod.hxx
 * \brief Trait helpers for PODs.
 */

#ifndef YAAL_HCORE_POD_HXX_INCLUDED
#define YAAL_HCORE_POD_HXX_INCLUDED 1

#include "hcore/trait.hxx"

namespace yaal {

/*! \brief Check if type is a POD type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is a POD type.
 * \retval type - true_type iff T is a POD type.
 */
template<typename T>
struct is_pod {
	static bool const value = trait::is_pointer<T>::value;
	typedef typename trait::ternary<trait::is_pointer<T>::value, trait::true_type, trait::false_type>::type type;
};

/*! \cond */
template<> struct is_pod<bool> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<char> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<char unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int short> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int short unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int long> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<int long unsigned> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<double> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<double long> { static bool const value = true; typedef trait::true_type type; };
template<> struct is_pod<float> { static bool const value = true; typedef trait::true_type type; };
/*! \endcond */

/*! \brief Check if type is an integral type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is an integral type.
 * \retval type - true_type iff T is an integral type.
 */
template<typename T>
struct is_integral {
	static bool const value = false;
	typedef trait::false_type type;
};

/*! \brief Check if type is a floating point type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is a floating point type.
 * \retval type - true_type iff T is a floating point type.
 */
template<typename T>
struct is_floating_point {
	static bool const value = false;
	typedef trait::false_type type;
};

/*! \brief Check if type is a numeric type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is a numeric type.
 * \retval type - true_type iff T is a numeric type.
 */
template<typename T>
struct is_numeric {
	static bool const value = ( is_integral<T>::value || is_floating_point<T>::value );
	typedef typename trait::ternary<value, trait::true_type, trait::false_type>::type type;
};

/*! \cond */
template<>
struct is_integral<int> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int unsigned> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int short> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int short unsigned> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int long> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_integral<int long unsigned> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_floating_point<float> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_floating_point<double> {
	static bool const value = true;
	typedef trait::true_type type;
};

template<>
struct is_floating_point<double long> {
	static bool const value = true;
	typedef trait::true_type type;
};

namespace hcore { class HNumber; }

template<>
struct is_floating_point<hcore::HNumber> {
	static bool const value = true;
	typedef trait::true_type type;
};

/*! \endcond */

typedef char unsigned u8_t; /*!< 8 bit unsigned integer. */
typedef int short unsigned u16_t; /*!< 16 bit unsigned integer. */
typedef int unsigned u32_t; /*!< 32 bit unsigned integer. */
#if ( SIZEOF_INT_LONG == 8 )
typedef int long unsigned u64_t; /*!< 64 bit unsigned integer. */
#elif ( SIZEOF_INT_LONG_LONG == 8 ) /* #if ( SIZEOF_INT_LONG == 8 ) */
typedef int long long unsigned u64_t; /*!< 64 bit unsigned integer. */
#else /* #elif ( SIZEOF_INT_LONG_LONG == 8 ) #if ( SIZEOF_INT_LONG == 8 ) */
#error Required type: a 64bit integer, is not available.
#endif /* #else #elif ( SIZEOF_INT_LONG_LONG == 8 ) #if ( SIZEOF_INT_LONG == 8 ) */
typedef char signed i8_t; /*!< 8 bit signed integer. */
typedef int short signed i16_t; /*!< 16 bit signed integer. */
typedef int signed i32_t; /*!< 32 bit signed integer. */
#if ( SIZEOF_INT_LONG == 8 )
typedef int long signed i64_t; /*!< 64 bit signed integer. */
#elif ( SIZEOF_INT_LONG_LONG == 8 ) /* #if ( SIZEOF_INT_LONG == 8 ) */
typedef int long long signed i64_t; /*!< 64 bit signed integer. */
#else /* #elif ( SIZEOF_INT_LONG_LONG == 8 ) #if ( SIZEOF_INT_LONG == 8 ) */
#error Required type: a 64bit integer, is not available.
#endif /* #else #elif ( SIZEOF_INT_LONG_LONG == 8 ) #if ( SIZEOF_INT_LONG == 8 ) */

}

#endif /* #ifndef YAAL_HCORE_POD_HXX_INCLUDED */

