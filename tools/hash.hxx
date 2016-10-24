/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hash.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HASH_HXX_INCLUDED
#define YAAL_TOOLS_HASH_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief Hashing functions.
 */
namespace hash {

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

/*! \brief Calculate sha1 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha1( yaal::hcore::HStreamInterface& source );

/*! \brief Calculate sha1 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha1( yaal::hcore::HStreamInterface::ptr_t source );

/*! \brief Calculate sha1 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha1( yaal::hcore::HString const& source );

/*! \brief Calculate sha512 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha512( yaal::hcore::HStreamInterface& source );

/*! \brief Calculate sha512 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha512( yaal::hcore::HStreamInterface::ptr_t source );

/*! \brief Calculate sha512 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
yaal::hcore::HString sha512( yaal::hcore::HString const& source );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HASH_HXX_INCLUDED */

