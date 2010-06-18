/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	pod.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/pod.hxx
 * \brief Trait helpers for PODs.
 */

#ifndef YAAL_HCORE_POD_HXX_INCLUDED
#define YAAL_HCORE_POD_HXX_INCLUDED 1

#include "hcore/trait.hxx"

namespace yaal
{

/*! \brief Check if type is a POD type.
 *
 * \tparam T - type to test.
 * \retval value - true iff T is a POD type.
 * \retval type - true_type iff T is a POD type.
 */
template<typename T>
struct is_pod
	{
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
struct is_integral
	{
	static bool const value = false;
	typedef trait::false_type type;
	};

/*! \cond */
template<>
struct is_integral<int>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};

template<>
struct is_integral<int unsigned>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};

template<>
struct is_integral<int short>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};

template<>
struct is_integral<int short unsigned>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};

template<>
struct is_integral<int long>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};

template<>
struct is_integral<int long unsigned>
	{
	static bool const value = true;
	typedef trait::true_type type;
	};
/*! \endcond */

typedef char unsigned u8_t; /*!< 8 bit unsigned integer. */
typedef int short unsigned u16_t; /*!< 16 bit unsigned integer. */
typedef int unsigned u32_t; /*!< 32 bit unsigned integer. */
#if 0
typedef int long unsigned u64_t; /*!< 64 bit unsigned integer. */
#endif
typedef char signed i8_t; /*!< 8 bit signed integer. */
typedef int short signed i16_t; /*!< 16 bit signed integer. */
typedef int signed i32_t; /*!< 32 bit signed integer. */
#if 0
typedef int long signed i64_t; /*!< 64 bit signed integer. */
#endif

}

#endif /* not YAAL_HCORE_POD_HXX_INCLUDED */

