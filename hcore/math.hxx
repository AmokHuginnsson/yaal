/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcore/math.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/math.hxx
 * \brief Basic mathematical definitions in math namespace.
 */

#ifndef YAAL_HCORE_MATH_HXX_INCLUDED
#define YAAL_HCORE_MATH_HXX_INCLUDED 1

namespace yaal {

/*! \brief Basic mathematical declarations.
 */
namespace math {

M_YAAL_HCORE_PUBLIC_API extern double long PI;
M_YAAL_HCORE_PUBLIC_API extern double long E;

double long square_root_impl( double long );
double long natural_expotential_impl( double long );
double long natural_logarithm_impl( double long );
double long sinus_impl( double long );
double long cosinus_impl( double long );
double long tangens_impl( double long );
double long cotangens_impl( double long );

template<typename T>
T square_root( T val_ ) {
	return ( static_cast<T>( square_root_impl( val_ ) ) );
}

template<typename T>
T natural_expotential( T val_ ) {
	return ( static_cast<T>( natural_expotential_impl( val_ ) ) );
}

template<typename T>
T natural_logarithm( T val_ ) {
	return ( static_cast<T>( natural_logarithm_impl( val_ ) ) );
}

template<typename T>
T sinus( T val_ ) {
	return ( static_cast<T>( sinus_impl( val_ ) ) );
}

template<typename T>
T cosinus( T val_ ) {
	return ( static_cast<T>( cosinus_impl( val_ ) ) );
}

template<typename T>
T tangens( T val_ ) {
	return ( static_cast<T>( tangens_impl( val_ ) ) );
}

template<typename T>
T cotangens( T val_ ) {
	return ( static_cast<T>( cotangens_impl( val_ ) ) );
}

}

}

#endif /* #ifndef YAAL_HCORE_MATH_HXX_INCLUDED */

