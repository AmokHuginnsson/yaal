/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HASH_HXX_INCLUDED
#define YAAL_TOOLS_HASH_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hstaticarray.hxx"

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

template<int long long unsigned BITS>
struct octets_for_bits {
	static int long long unsigned const value = ( ( BITS >> 3 ) + ( ( BITS & 7 ) ? 1 : 0 ) );
};

typedef yaal::hcore::HStaticArray<yaal::u8_t, octets_for_bits<128>::value> md5_hash_t;
typedef yaal::hcore::HStaticArray<yaal::u8_t, octets_for_bits<160>::value> sha1_hash_t;
typedef yaal::hcore::HStaticArray<yaal::u8_t, octets_for_bits<512>::value> sha512_hash_t;
static_assert( sizeof ( md5_hash_t ) == 16, "bad md5 hash size" );
static_assert( sizeof ( sha1_hash_t ) == 20, "bad sha1 hash size" );
static_assert( sizeof ( sha512_hash_t ) == 64, "bad sha512 hash size" );

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
md5_hash_t md5( yaal::hcore::HStreamInterface& source );

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
md5_hash_t md5( yaal::hcore::HStreamInterface::ptr_t source );

/*! \brief Calculate md5 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
md5_hash_t md5( yaal::hcore::HString const& source );

/*! \brief Calculate sha1 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
sha1_hash_t sha1( yaal::hcore::HStreamInterface& source );

/*! \brief Calculate sha1 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
sha1_hash_t sha1( yaal::hcore::HStreamInterface::ptr_t source );

/*! \brief Calculate sha1 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
sha1_hash_t sha1( yaal::hcore::HString const& source );

/*! \brief Calculate sha512 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
sha512_hash_t sha512( yaal::hcore::HStreamInterface& source );

/*! \brief Calculate sha512 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
sha512_hash_t sha512( yaal::hcore::HStreamInterface::ptr_t source );

/*! \brief Calculate sha512 hash of data.
 *
 * \param source - data source of data which sum is to ba calculated.
 * \return Calculated hash.
 */
sha512_hash_t sha512( yaal::hcore::HString const& source );

/*! \brief Calculate MD5 based keyed-hash message authentication code (HMAC).
 *
 * \param key - authentication key.
 * \param message - data for which HMAC shall be generated.
 * \return HMAC for given message and key using given hash function.
 */
md5_hash_t hmac_md5( yaal::hcore::HString const& key, yaal::hcore::HString const& message );

/*! \brief Calculate SHA1 based keyed-hash message authentication code (HMAC).
 *
 * \param key - authentication key.
 * \param message - data for which HMAC shall be generated.
 * \return HMAC for given message and key using given hash function.
 */
sha1_hash_t hmac_sha1( yaal::hcore::HString const& key, yaal::hcore::HString const& message );

/*! \brief Calculate SHA512 based keyed-hash message authentication code (HMAC).
 *
 * \param key - authentication key.
 * \param message - data for which HMAC shall be generated.
 * \return HMAC for given message and key using given hash function.
 */
sha512_hash_t hmac_sha512( yaal::hcore::HString const& key, yaal::hcore::HString const& message );

/*! \brief Calculate parametrized keyed-hash message authentication code (HMAC).
 *
 * \param function - a hash function used to generate this HMAC.
 * \param key - authentication key.
 * \param message - data for which HMAC shall be generated.
 * \return HMAC for given message and key using given hash function.
 */
yaal::hcore::HString hmac( FUNCTION function, yaal::hcore::HString const& key, yaal::hcore::HString const& message );

yaal::hcore::HString string( FUNCTION function, yaal::hcore::HString const& message );
yaal::hcore::HString string( FUNCTION function, yaal::hcore::HStreamInterface& stream );

yaal::hcore::HString to_string( md5_hash_t const& );
yaal::hcore::HString to_string( sha1_hash_t const& );
yaal::hcore::HString to_string( sha512_hash_t const& );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HASH_HXX_INCLUDED */

