/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hash.hxx - this file is integral part of yaal project.

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

#ifndef YAAL_TOOLS_HASH_HXX_INCLUDED
#define YAAL_TOOLS_HASH_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Hashing functions.
 */
namespace hash
{

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString md5( yaal::hcore::HStreamInterface& source );

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString md5( yaal::hcore::HStreamInterface::ptr_t source );

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString md5( yaal::hcore::HString const& source );

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha1( yaal::hcore::HStreamInterface& source );

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha1( yaal::hcore::HStreamInterface::ptr_t source );

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha1( yaal::hcore::HString const& source );

}

}

}

#endif /* not YAAL_TOOLS_HASH_HXX_INCLUDED */

