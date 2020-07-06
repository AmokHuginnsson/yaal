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

namespace PREFIX {

static yaal::i64_t const KILO = static_cast<i64_t>( yaal::meta::power<10,  3>::value );
static yaal::i64_t const KIBI = static_cast<i64_t>( yaal::meta::power< 2, 10>::value );
static yaal::i64_t const MEGA = static_cast<i64_t>( yaal::meta::power<10,  6>::value );
static yaal::i64_t const MEBI = static_cast<i64_t>( yaal::meta::power< 2, 20>::value );
static yaal::i64_t const GIGA = static_cast<i64_t>( yaal::meta::power<10,  9>::value );
static yaal::i64_t const GIBI = static_cast<i64_t>( yaal::meta::power< 2, 30>::value );
static yaal::i64_t const TERA = static_cast<i64_t>( yaal::meta::power<10, 12>::value );
static yaal::i64_t const TEBI = static_cast<i64_t>( yaal::meta::power< 2, 40>::value );
static yaal::i64_t const PETA = static_cast<i64_t>( yaal::meta::power<10, 15>::value );
static yaal::i64_t const PEBI = static_cast<i64_t>( yaal::meta::power< 2, 50>::value );
static yaal::i64_t const EXA  = static_cast<i64_t>( yaal::meta::power<10, 18>::value );
static yaal::i64_t const EXBI = static_cast<i64_t>( yaal::meta::power< 2, 60>::value );

}

}

}

}

#endif /* #ifndef YAAL_HCORE_SI_HXX_INCLUDED */

