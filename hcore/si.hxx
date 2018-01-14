/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/si.hxx
 * \brief Declaration of SI prefix calculation constants.
 */

#ifndef YAAL_HCORE_SI_HXX_INCLUDED
#define YAAL_HCORE_SI_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/numeric.hxx"

namespace yaal {

namespace hcore {

namespace si {

static yaal::i64_t const NANO_IN_WHOLE = static_cast<i64_t>( yaal::meta::power<10, 9>::value );
static yaal::i64_t const MICRO_IN_WHOLE = static_cast<i64_t>( yaal::meta::power<10, 6>::value );
static yaal::i64_t const MILLI_IN_WHOLE = static_cast<i64_t>( yaal::meta::power<10, 3>::value );
static yaal::i64_t const NANO_IN_MILLI = static_cast<i64_t>( yaal::meta::power<10, 6>::value );
static yaal::i64_t const NANO_IN_MICRO = static_cast<i64_t>( yaal::meta::power<10, 3>::value );

}

}

}

#endif /* #ifndef YAAL_HCORE_SI_HXX_INCLUDED */

