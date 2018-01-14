/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_BASE64_HXX_INCLUDED
#define YAAL_TOOLS_BASE64_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief Base64 encoding/decoding functionality.
 */
struct base64 {
	typedef base64 this_type;
	static yaal::hcore::HString encode( yaal::hcore::HUTF8String const&, bool = false );
	static yaal::hcore::HString decode( yaal::hcore::HString const&, bool = false );
	static void encode( yaal::hcore::HStreamInterface&, yaal::hcore::HStreamInterface&, bool = false, int = 0 );
	static void decode( yaal::hcore::HStreamInterface&, yaal::hcore::HStreamInterface&, bool = false );
};

typedef yaal::hcore::HExceptionT<base64> HBase64Exception;

}

}

#endif /* #ifndef YAAL_TOOLS_BASE64_HXX_INCLUDED */

