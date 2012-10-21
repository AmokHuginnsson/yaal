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

#ifndef YAAL_HCORE_HRANDOMIZER_HXX_INCLUDED
#define YAAL_HCORE_HRANDOMIZER_HXX_INCLUDED 1

#include "hcore/numeric.hxx"

namespace yaal {

namespace hcore {

/*! \brief Pseudo=random number generator.
 *
 * Congurent pseudo random number generator class.
 */
class HRandomizer {
public:
	typedef HRandomizer this_type;
private:
	int long unsigned _seed;
	int _range;
public:
/*! \brief Construct new randomizer.
 *
 * \param seed - initialize pseudo-random numebr generator with seed.
 * \param range - upper limit for generated numer.
 */
	HRandomizer( int long unsigned const seed = 1, int range = meta::max_signed<int>::value );

/*! \brief Generate next random number.
 *
 * \param range - upper limit for generated numer.
 * \return next random number capped to explicit value.
 */
	int operator()( int range );

/*! \brief Generate next random number.
 *
 * \param range - upper limit for generated numer.
 * \return next random number capped to randomizer instance default value.
 */
	int operator()( void );
};

/*! \brief Helper namespace for HRandomizer related utils.
 */
namespace randomizer_helper {

/*! \brief Make randomizer object initialized with "random" seed.
 *
 * \param range - upper limit for generated numbers.
 */
HRandomizer make_randomizer( int range = meta::max_signed<int>::value );

}

}

}
	
#endif /* #ifndef YAAL_HCORE_HRANDOMIZER_HXX_INCLUDED */

