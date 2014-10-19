/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	reflection.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/reflection.hxx
 * \brief Higher order interfaces for PODs.
 */

#ifndef YAAL_HCORE_REFLECTION_HXX_INCLUDED
#define YAAL_HCORE_REFLECTION_HXX_INCLUDED 1

#include "hcore/hstrongenum.hxx"

namespace yaal {

/*! \brief POD type symbols. (Some non-POD yaal types included too).
 */
struct TYPE {
	typedef enum {
		UNKNOWN       = 0x0000,
		BOOL          = 0x0001,
		CHAR          = 0x0002,
		INT_SHORT     = 0x0004,
		INT           = 0x0008,
		INT_LONG      = 0x0010,
		INT_LONG_LONG = 0x0020,
		FLOAT         = 0x0040,
		DOUBLE        = 0x0080,
		DOUBLE_LONG   = 0x0100,
		POINTER       = 0x0200,
		HSTRING       = 0x0400,
		HNUMBER       = 0x0800,
		HINFO         = 0x1000,
		HTIME         = 0x2000
	} enum_t;
	template<typename tType>
	struct symbolic;
};
typedef yaal::hcore::HStrongEnum<TYPE> type_id_t; /*!< Strong enumeration of PODs. */

/*! \cond */
template<> struct TYPE::symbolic<bool> { static enum_t const value = BOOL; };
template<> struct TYPE::symbolic<char> { static enum_t const value = CHAR; };
template<> struct TYPE::symbolic<char unsigned> { static enum_t const value = CHAR; };
template<> struct TYPE::symbolic<int short> { static enum_t const value = INT_SHORT; };
template<> struct TYPE::symbolic<int short unsigned> { static enum_t const value = INT_SHORT; };
template<> struct TYPE::symbolic<int> { static enum_t const value = INT; };
template<> struct TYPE::symbolic<int unsigned> { static enum_t const value = INT; };
template<> struct TYPE::symbolic<int long> { static enum_t const value = INT_LONG; };
template<> struct TYPE::symbolic<int long unsigned> { static enum_t const value = INT_LONG; };
template<> struct TYPE::symbolic<int long long> { static enum_t const value = INT_LONG_LONG; };
template<> struct TYPE::symbolic<int long long unsigned> { static enum_t const value = INT_LONG_LONG; };
template<> struct TYPE::symbolic<float> { static enum_t const value = FLOAT; };
template<> struct TYPE::symbolic<double> { static enum_t const value = DOUBLE; };
template<> struct TYPE::symbolic<double long> { static enum_t const value = DOUBLE_LONG; };
template<typename T> struct TYPE::symbolic<T*> { static enum_t const value = POINTER; };
template<> struct TYPE::symbolic<yaal::hcore::HString> { static enum_t const value = HSTRING; };
/*! \endcond */

/*! \brief Convert static type information to value.
 *
 * \tparam tType - type to convert to value.
 * \retval value - a value that is symbolical representation of given type.
 */
template<typename tType>
struct TYPE::symbolic
	{ static enum_t const value = UNKNOWN; };

}

#endif /* #ifndef YAAL_HCORE_REFLECTION_HXX_INCLUDED */

