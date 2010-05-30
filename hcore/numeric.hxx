/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	numeric.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/numeric.hxx
 * \brief Library of numerical calculus meta (compile time computation) functions.
 */

#ifndef YAAL_HCORE_NUMERIC_HXX_INCLUDED
#define YAAL_HCORE_NUMERIC_HXX_INCLUDED

#include "hcore/static_assert.hxx"

namespace yaal
{

/*! \brief Compile time numerical calculus functions belong here.
 */
namespace meta
{

/*! \brief Perform a logical negation.
 *
 * \tparam value_in - value to be negated.
 * \retval value - conditional value.
 */
template<bool const value_in>
struct boolean_not
	{
	static bool const value = ! value_in;
	};

/*! \brief Test if givent type is a signed type.
 *
 * \tparam T - type to perform the test on.
 * \retval value - true iff T is a signed type.
 *
 * This meta-function should be placed in trait.hxx header, but numeric.hxx
 * is already required there.
 */
template<typename T>
struct is_signed
	{
	static bool const value = static_cast<T>( 0 ) > static_cast<T>( ~0 );
	};

/*! \brief Get maximum positive number for a given type.
 *
 * \tparam T - type to get maximum positive number.
 * \retval value - a maximum positive number that can be fit into variable of type T.
 */
template<typename T>
struct max_signed
	{
	STATIC_ASSERT( is_signed<T>::value );
	static T const value = static_cast<T>( ~( static_cast<T>( 1 ) << ( ( sizeof ( T ) << 3 ) - 1 ) ) );
	};
template<typename T>
T const max_signed<T>::value;

/*! \brief Get maximum number value for a given unsigned type.
 *
 * \tparam T - type to get maximum fit.
 * \retval value - a maximum positive number that can be fit into variable of unsigned type T.
 */
template<typename T>
struct max_unsigned
	{
	STATIC_ASSERT( boolean_not<is_signed<T>::value>::value );
	static T const value = static_cast<T>( ~0 );
	};
template<typename T>
T const max_unsigned<T>::value;

/*! \brief Get minimum negative number for a given type.
 *
 * \tparam T - type to get minimum negative number.
 * \retval value - a minimum negative number that can be fit into variable of type T.
 */
template<typename T>
struct min_signed
	{
	STATIC_ASSERT( is_signed<T>::value );
	static T const value = static_cast<T>( static_cast<T>( 1 ) << ( ( sizeof ( T ) << 3 ) - 1 ) );
	};
template<typename T>
T const min_signed<T>::value;

/*! \brief Convert given boolean value to integer.
 *
 * \tparam boolean - boolean value.
 * \retval value - integer value: 1 for true, 0 for false.
 */
template<bool boolean>
struct to_int
	{
	static int const value = boolean ? 1 : 0;
	};

/*! \brief Convert given integer value to boolean.
 *
 * \tparam integer - integer to convert.
 * \retval value - a converted value, false for 0, true otherwise.
 */
template<int long integer>
struct to_bool
	{
	static bool const value = integer ? true : false;
	};

/*! \brief Staticaly calculate maximul of a set.
 *
 * \tparam a0, a1, ..., a9 - list of integers to find maximum.
 * \retval value - maximum integer from given set.
 */
template<int long a0, int long a1,
	int long a2 = min_signed<int long>::value, int long a3 = min_signed<int long>::value,
	int long a4 = min_signed<int long>::value, int long a5 = min_signed<int long>::value,
	int long a6 = min_signed<int long>::value, int long a7 = min_signed<int long>::value,
	int long a8 = min_signed<int long>::value, int long a9 = min_signed<int long>::value,
	int long a10 = min_signed<int long>::value, int long a11 = min_signed<int long>::value,
	int long a12 = min_signed<int long>::value, int long a13 = min_signed<int long>::value,
	int long a14 = min_signed<int long>::value, int long a15 = min_signed<int long>::value,
	int long a16 = min_signed<int long>::value, int long a17 = min_signed<int long>::value,
	int long a18 = min_signed<int long>::value, int long a19 = min_signed<int long>::value,
	int long a20 = min_signed<int long>::value>
struct max
	{
	static int long const b0 = a0 > a1 ? a0 : a1;
	static int long const b1 = b0 > a2 ? b0 : a2;
	static int long const b2 = b1 > a3 ? b1 : a3;
	static int long const b3 = b2 > a4 ? b2 : a4;
	static int long const b4 = b3 > a5 ? b3 : a5;
	static int long const b5 = b4 > a6 ? b4 : a6;
	static int long const b6 = b5 > a7 ? b5 : a7;
	static int long const b7 = b6 > a8 ? b6 : a8;
	static int long const b8 = b7 > a9 ? b7 : a9;
	static int long const b9 = b8 > a10 ? b8 : a10;
	static int long const b10 = b9 > a11 ? b9 : a11;
	static int long const b11 = b10 > a12 ? b10 : a12;
	static int long const b12 = b11 > a13 ? b11 : a13;
	static int long const b13 = b12 > a14 ? b12 : a14;
	static int long const b14 = b13 > a15 ? b13 : a15;
	static int long const b15 = b14 > a16 ? b14 : a16;
	static int long const b16 = b15 > a17 ? b15 : a17;
	static int long const b17 = b16 > a18 ? b16 : a18;
	static int long const b18 = b17 > a19 ? b17 : a19;
	static int long const value = b18 > a20 ? b18 : a20;
	};

/*! \brief Simulate binary literal.
 *
 * \tparam input - a number in decimal form consisting only 0s and 1s.
 * \retval value - a number of value of binary interpretation of input.
 */
template<int long unsigned const input>
struct binary
	{
	static int long unsigned const value = ( binary<input / 10>::value << 1 ) + ( input % 10 );
	};

/*! \cond */
template<>
struct binary<0>
	{
	static int long unsigned const value = 0;
	};
/*! \endcond */

/*! \brief Simulate binary literal.
 *
 * \tparam input - a number in octal form consisting only 0s and 1s.
 * \retval value - a number of value of binary interpretation of input.
 */
template<int long unsigned const input>
struct obinary
	{
	static int long unsigned const value = ( obinary<(input >> 3)>::value << 1 ) + ( input & 7 );
	};

/*! \cond */
template<>
struct obinary<0>
	{
	static int long unsigned const value = 0;
	};
/*! \endcond */

/*! \brief Compile time power finction calculator.
 *
 * \tparam base - base of power function.
 * \tparam exponent - exponent of power function.
 * \retval value - base**exponent.
 */
template<int long unsigned const base, int long unsigned const exponent, int long unsigned const helper = 1>
struct power
	{
	static int long unsigned const value = power<base, exponent - 1, helper * base>::value;
	};

/*! \cond */
template<int long unsigned const base, int long unsigned const helper>
struct power<base,0,helper>
	{
	static int long unsigned const value = helper;
	};
/*! \endcond */

/*! \brief Perform compile time ternary operator on integers.
 *
 * \tparam condition - staticly checkable condition that tells which type will be used.
 * \tparam value_for_true - value to be used if condition is true.
 * \tparam value_for_false - value to be used if condition is false.
 * \retval value - conditional value.
 */
template<bool const condition, int long value_for_true, int long value_for_false>
struct ternary;

/* \cond */
template<int long value_for_true, int long value_for_false>
struct ternary<true, value_for_true, value_for_false>
	{
	static int long const value = value_for_true;
	};

template<int long value_for_true, int long value_for_false>
struct ternary<false, value_for_true, value_for_false>
	{
	static int long const value = value_for_false;
	};
/* \endcond */

/*! \brief Check if one value is greater than another.
 *
 * \tparam val1 - first value for comparision.
 * \tparam val2 - second value for comparision.
 * \retval value - true iff val1 > val2.
 */
template<int long const val1, int long const val2>
struct greater
	{
	static bool const value = val1 > val2;
	};

/*! \brief Check if one value is less than another.
 *
 * \tparam val1 - first value for comparision.
 * \tparam val2 - second value for comparision.
 * \retval value - true iff val1 < val2.
 */
template<int long const val1, int long const val2>
struct less
	{
	static bool const value = val1 < val2;
	};

}

}

#endif /* not YAAL_HCORE_NUMERIC_HXX_INCLUDED */

