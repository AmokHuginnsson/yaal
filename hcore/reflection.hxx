/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	reflection.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/reflection.hxx
 * \brief Higher order interfaces for PODs. 
 */

#ifndef YAAL_HCORE_REFLECTION_HXX_INCLUDED
#define YAAL_HCORE_REFLECTION_HXX_INCLUDED

#include "hcore/hstrongenum.hxx"

namespace yaal
{

/*! \brief POD type symbols. (Some non-POD yaal types included too).
 */
struct TYPE
	{
	typedef enum
		{
		UNKNOWN     = 0x0000,
		BOOL        = 0x0001,
		CHAR        = 0x0002,
		INT_SHORT   = 0x0004,
		INT         = 0x0008,
		INT_LONG    = 0x0010,
		FLOAT       = 0x0020,
		DOUBLE      = 0x0040,
		DOUBLE_LONG = 0x0080,
		VOID_PTR    = 0x0100,
		CHAR_PTR    = 0x0280,
		HSTRING     = 0x0400,
		HNUMBER     = 0x0800,
		HINFO       = 0x1000,
		HTIME       = 0x2000,
		CONTAINER   = 0x4000,
		VOID        = 0x8000
		} enum_t;
	template<typename tType>
	struct symbolic;
	};
typedef yaal::hcore::HStrongEnum<TYPE> type_t; /*!< Strong enumeration of PODs. */
template<> struct TYPE::symbolic<void> { static enum_t const type = VOID; };
template<> struct TYPE::symbolic<bool> { static enum_t const type = BOOL; };
template<> struct TYPE::symbolic<char> { static enum_t const type = CHAR; };
template<> struct TYPE::symbolic<int short> { static enum_t const type = INT_SHORT; };
template<> struct TYPE::symbolic<int> { static enum_t const type = INT; };
template<> struct TYPE::symbolic<int long> { static enum_t const type = INT_LONG; };
template<> struct TYPE::symbolic<float> { static enum_t const type = FLOAT; };
template<> struct TYPE::symbolic<double> { static enum_t const type = DOUBLE; };
template<> struct TYPE::symbolic<double long> { static enum_t const type = DOUBLE_LONG; };
template<> struct TYPE::symbolic<void*> { static enum_t const type = VOID_PTR; };
template<> struct TYPE::symbolic<char*> { static enum_t const type = CHAR_PTR; };
template<> struct TYPE::symbolic<yaal::hcore::HString> { static enum_t const type = HSTRING; };

template<typename tType>
struct TYPE::symbolic
	{ static enum_t const type = UNKNOWN; };

}

#endif /* not YAAL_HCORE_REFLECTION_HXX_INCLUDED */

