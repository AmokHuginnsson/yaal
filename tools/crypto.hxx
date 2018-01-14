/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/crypto.hxx
 * \brief Declaration of cryptographic functions.
 */

#ifndef YAAL_TOOLS_CRYPTO_HXX_INCLUDED
#define YAAL_TOOLS_CRYPTO_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief 3DES crypto algorithm functions.
 */
namespace crypto {

void crypt_3des( yaal::hcore::HStreamInterface::ptr_t, yaal::hcore::HStreamInterface::ptr_t, yaal::hcore::HString const& );
void crypt_3des( yaal::hcore::HStreamInterface&, yaal::hcore::HStreamInterface&, yaal::hcore::HString const& );
void decrypt_3des( yaal::hcore::HStreamInterface::ptr_t, yaal::hcore::HStreamInterface::ptr_t, yaal::hcore::HString const& );
void decrypt_3des( yaal::hcore::HStreamInterface&, yaal::hcore::HStreamInterface&, yaal::hcore::HString const& );

}

}

}

#endif /* #ifndef YAAL_TOOLS_CRYPTO_HXX_INCLUDED */

