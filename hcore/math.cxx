/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

double long natural_exponential_impl( double long val_ ) {
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

double long arcus_sinus_impl( double long val_ ) {
	return ( asinl( val_ ) );
}

double long arcus_cosinus_impl( double long val_ ) {
	return ( acosl( val_ ) );
}

double long arcus_tangens_impl( double long val_ ) {
	return ( atanl( val_ ) );
}

double long arcus_cotangens_impl( double long val_ ) {
	return ( math::PI / 2.0L - atanl( val_ ) );
}

double long hyperbolic_sinus_impl( double long val_ ) {
	return ( sinhl( val_ ) );
}

double long hyperbolic_cosinus_impl( double long val_ ) {
	return ( coshl( val_ ) );
}

double long hyperbolic_tangens_impl( double long val_ ) {
	return ( tanhl( val_ ) );
}

double long hyperbolic_cotangens_impl( double long val_ ) {
	return ( 1.L / tanhl( val_ ) );
}

double long sigmoid_impl( double long val_ ) {
	double long v( ( tanhl( val_ / 2.L ) + 1L ) / 2.L );
	if ( v > 1.L ) {
		v = 1.L;
	} else if ( v < -1.L ) {
		v = -1.L;
	}
	return ( v );
}

double long error_function_impl( double long val_ ) {
	return ( erfl( val_ ) );
}

int long long greatest_common_divisor_impl( int long long a_, int long long b_ ) {
	a_ = yaal::math::abs( a_ );
	b_ = yaal::math::abs( b_ );
	while ( a_ && b_ ) {
		if ( a_ > b_ ) {
			a_ %= b_;
		} else {
			b_ %= a_;
		}
	}
	return ( a_ ? a_ : b_ );
}

}

}

