/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/assert.hxx
 * \brief Declarations of assertion testing helpers: HFailedAssertion, failed_assert.
 */

#ifndef YAAL_HCORE_ASSERT_HXX_INCLUDED
#define YAAL_HCORE_ASSERT_HXX_INCLUDED 1

#include "config.hxx"

namespace yaal {

namespace hcore {

/*! \brief Failed assertion exception.
 *
 * In \e DEBUG build failures in assertions does not abort the
 * process, instead the throw instance of HFailedAssertion.
 */
class HFailedAssertion {
	char const* _what;
public:
	HFailedAssertion( char const* what_ ) : _what( what_ ) {}
	HFailedAssertion( HFailedAssertion const& fa ) = default;
	HFailedAssertion& operator = ( HFailedAssertion const& ) = default;
	char const* what( void ) const { return ( _what ); }
};

void failed_assert( char const*, int, char const*, char const* ) __attribute__(( __noreturn__ ));

}

}

#endif /* #ifndef YAAL_HCORE_ASSERT_HXX_INCLUDED */

