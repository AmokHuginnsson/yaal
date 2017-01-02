/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hrandomizer.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hrandomizer.hxx
 * \brief Declaration of HRandomizer class.
 */

#ifndef YAAL_HCORE_HRANDOMIZER_HXX_INCLUDED
#define YAAL_HCORE_HRANDOMIZER_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/numeric.hxx"

namespace yaal {

namespace hcore {

/*! \brief Pseudo-random number generator.
 *
 * Congruent pseudo random number generator class.
 */
class HRandomizer final {
public:
	typedef HRandomizer this_type;
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
	HRandomizer( u64_t seed = 5489ULL, u64_t range = meta::max_unsigned<u64_t>::value );

/*! \brief Construct new randomizer.
 *
 * \param stateFirst_ - beginning of pseudo-random number generator initial state range.
 * \param stateLast_ - one past the end of pseudo-random number generator initial state range.
 * \param range - upper limit for generated number.
 */
	HRandomizer( u64_t const* stateFirst_, u64_t const* stateLast_, u64_t range = meta::max_unsigned<u64_t>::value );

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

	void swap( HRandomizer& );
private:
	void init( u64_t );
	void init( u64_t const*, int );
};

/*! \brief Helper namespace for HRandomizer related utilities.
 */
namespace randomizer_helper {

/*! \brief Make randomizer object initialized with "random" seed.
 *
 * \param range - upper limit for generated numbers.
 */
HRandomizer make_randomizer( u64_t range = meta::max_unsigned<u64_t>::value );

}

}

}

#endif /* #ifndef YAAL_HCORE_HRANDOMIZER_HXX_INCLUDED */

