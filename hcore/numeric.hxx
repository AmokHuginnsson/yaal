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

namespace yaal
{

template<bool boolean>
struct to_int
	{
	static int const value = boolean ? 1 : 0;
	};

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
	int long a2 = LONG_MIN, int long a3 = LONG_MIN,
	int long a4 = LONG_MIN, int long a5 = LONG_MIN,
	int long a6 = LONG_MIN, int long a7 = LONG_MIN,
	int long a8 = LONG_MIN, int long a9 = LONG_MIN>
struct static_max
	{
	static int long const b0 = a0 > a1 ? a0 : a1;
	static int long const b1 = b0 > a2 ? b0 : a2;
	static int long const b2 = b1 > a3 ? b1 : a3;
	static int long const b3 = b2 > a4 ? b2 : a4;
	static int long const b4 = b3 > a5 ? b3 : a5;
	static int long const b5 = b4 > a6 ? b4 : a6;
	static int long const b6 = b5 > a7 ? b5 : a7;
	static int long const b7 = b6 > a8 ? b6 : a8;
	static int long const value = b7 > a9 ? b7 : a9;
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

}

#endif /* not YAAL_HCORE_NUMERIC_HXX_INCLUDED */

