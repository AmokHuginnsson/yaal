/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	base.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/base.hxx
 * \brief Basic declarations used across whole library.
 *
 * This file holds main #defines, macros and global variables
 * used acros whole yaal.
 */

#ifndef YAAL_HCORE_BASE_HXX_INCLUDED
#define YAAL_HCORE_BASE_HXX_INCLUDED

#include "hcore/compat.hxx"
#include "hcore/macro.hxx"

/*! \mainpage yaal library documentation.
 *
 * \htmlinclude ./main.xml
 */

/*! \brief All library API belongs here.
 *
 * This namaspace contains all yaal library API and code.
 * YAAL code and API is divided into several sections.
 *
 * yaal comes from YAAL which comes from Y.A.A.L. which comes from:
 *  - Yet
 *  - Another
 *  - Abstraction
 *  - Layer
 */
namespace yaal
{

/*! \brief Convert between distinct datatypes.
 *
 * \tparam to_t - destination type.
 * \param val - value to be converted to destination type.
 * \return val equvalent in to_t representation.
 */
template<typename to_t, typename from_t>
to_t lexical_cast( from_t const& val );

/*! \brief Generate error message (possibly human readable) based on error code.
 *
 * \param code - error code to describe.
 * \return Message string describing error of given error code.
 */
char const* error_message( int const& code );

}

#endif /* not YAAL_HCORE_BASE_HXX_INCLUDED */

