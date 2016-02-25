/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  math.cxx - this file is integral part of `yaal' project.

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

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "math.hxx"

namespace yaal {

namespace math {

/* Number of digits here is limited by Visual Studio C++ compiler internal identifier buffer size. */
double long PI( 3.14159265358979323846264338327950288419706939937510582097494459230781640628620899862803482534211706798214808651328230664709384L );
double long E( 2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746639193200305992181741L );

double long square_root_impl( double long val_ ) {
	return ( sqrtl( val_ ) );
}

double long natural_expotential_impl( double long val_ ) {
	return ( expl( val_ ) );
}

double long natural_logarithm_impl( double long val_ ) {
	return ( logl( val_ ) );
}

double long sinus_impl( double long val_ ) {
	return ( sinl( val_ ) );
}

double long cosinus_impl( double long val_ ) {
	return ( cosl( val_ ) );
}

double long tangens_impl( double long val_ ) {
	return ( tanl( val_ ) );
}

double long cotangens_impl( double long val_ ) {
	double long sinusVal( 0 );
	double long cosinusVal( 0 );
	sincosl( val_, &sinusVal, &cosinusVal );
	return ( cosinusVal / sinusVal );
}

}

}

