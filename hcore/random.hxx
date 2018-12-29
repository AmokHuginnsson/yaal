/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/random.hxx
 * \brief Declaration of random namespace and HRandomNumberGenerator class.
 */

#ifndef YAAL_HCORE_RANDOM_HXX_INCLUDED
#define YAAL_HCORE_RANDOM_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/numeric.hxx"

namespace yaal {

namespace random {

/*! \brief (Pseudo)Random number generator.
 *
 * Congruent pseudo random number generator class.
 */
class HRandomNumberGenerator final {
public:
	typedef HRandomNumberGenerator this_type;
	static int const STATE_SIZE = 312;
private:
	int _index;
	u64_t _range;
	u64_t _state[STATE_SIZE];
public:
/*! \brief Construct new randomizer.
 *
 * \param seed - initialize pseudo-random number generator with seed.
 * \param range - upper limit for generated number.
 */
	HRandomNumberGenerator( u64_t seed = 5489ULL, u64_t range = meta::max_unsigned<u64_t>::value );

/*! \brief Construct new randomizer.
 *
 * \param stateFirst_ - beginning of pseudo-random number generator initial state range.
 * \param stateLast_ - one past the end of pseudo-random number generator initial state range.
 * \param range - upper limit for generated number.
 */
	HRandomNumberGenerator( u64_t const* stateFirst_, u64_t const* stateLast_, u64_t range = meta::max_unsigned<u64_t>::value );

/*! \brief Generate next random number.
 *
 * \param range - upper limit for generated number.
 * \return next random number capped to explicit value.
 */
	u64_t operator()( u64_t range );

/*! \brief Generate next random number.
 *
 * \param range - upper limit for generated number.
 * \return next random number capped to randomizer instance default value.
 */
	u64_t operator()( void );

/*! \brief Get upper limit for generated numbers.
 */
	u64_t range( void ) const {
		return ( _range );
	}

	void swap( HRandomNumberGenerator& );
private:
	void init( u64_t );
	void init( u64_t const*, int );
};

/*! \brief Helper namespace for HRandomNumberGenerator related utilities.
 */
namespace rng_helper {

/*! \brief Make randomizer object initialized with "random" seed.
 *
 * \param range - upper limit for generated numbers.
 */
HRandomNumberGenerator make_random_number_generator( u64_t range = meta::max_unsigned<u64_t>::value );

}

}

}

#endif /* #ifndef YAAL_HCORE_RANDOM_HXX_INCLUDED */

