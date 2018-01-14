/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HASH_HXX_INCLUDED
#define YAAL_TOOLS_HASH_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief Hashing functions.
 */
namespace hash {

enum class FUNCTION {
	MD5,
	SHA1,
	SHA512
};

typedef yaal::hcore::HString ( *hash_string_t )( yaal::hcore::HString const& );
typedef yaal::hcore::HString ( *hash_stream_t )( yaal::hcore::HStreamInterface& );
typedef yaal::hcore::HString ( *hmac_string_t )( yaal::hcore::HString const&, yaal::hcore::HString const& );

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

/*! \brief Calculate keyed-hash message authentication code (HMAC).
 *
 * \param function - a hash function used to generate this HMAC.
 * \param key - authentication key.
 * \param message - data for which HMAC shall be generated.
 * \return HMAC for given message and key using given hash function.
 */
yaal::hcore::HString hmac( FUNCTION function, yaal::hcore::HString const& key, yaal::hcore::HString const& message );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HASH_HXX_INCLUDED */

