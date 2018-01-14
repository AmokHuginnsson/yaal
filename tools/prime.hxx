/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/prime.hxx
 * \brief Declaration of prime number generator function.
 */

#ifndef YAAL_TOOLS_PRIME_HXX_INCLUDED
#define YAAL_TOOLS_PRIME_HXX_INCLUDED 1

#include "hcore/harray.hxx"

namespace yaal {

namespace tools {

typedef yaal::hcore::HArray<int long unsigned> primes_t;

void find_primes( primes_t&, int long unsigned, int long unsigned );

}

}

#endif /* #ifndef YAAL_TOOLS_PRIME_HXX_INCLUDED */

