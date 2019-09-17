/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/math.hxx
 * \brief Basic mathematical definitions in math namespace.
 */

#ifndef YAAL_HCORE_MATH_HXX_INCLUDED
#define YAAL_HCORE_MATH_HXX_INCLUDED 1

namespace yaal {

/*! \brief Basic mathematical declarations.
 */
namespace math {

M_YAAL_HCORE_PUBLIC_API extern double long const PI;
M_YAAL_HCORE_PUBLIC_API extern double long const E;
#undef INFINITY
M_YAAL_HCORE_PUBLIC_API extern double long const INFINITY;
#undef NAN
M_YAAL_HCORE_PUBLIC_API extern double long const NAN;

/*! \brief Calculate absolute value of a number.
 *
 * \param val - a number which absolute value shall be calculated.
 * \return |val|
 */
template<typename tType>
inline tType abs( tType const& val ) {
	return ( val >= 0 ? val : - val );
}

double long square_root_impl( double long );
double long natural_exponential_impl( double long );
double long natural_logarithm_impl( double long );
double long power_impl( double long, double long );
double long sinus_impl( double long );
double long cosinus_impl( double long );
double long tangens_impl( double long );
double long cotangens_impl( double long );
double long arcus_sinus_impl( double long );
double long arcus_cosinus_impl( double long );
double long arcus_tangens_impl( double long );
double long arcus_cotangens_impl( double long );
double long hyperbolic_sinus_impl( double long );
double long hyperbolic_cosinus_impl( double long );
double long hyperbolic_tangens_impl( double long );
double long hyperbolic_cotangens_impl( double long );
double long sigmoid_impl( double long );
double long error_function_impl( double long );
int long long greatest_common_divisor_impl( int long long, int long long );

template<typename T>
T square_root( T val_ ) {
	return ( static_cast<T>( square_root_impl( val_ ) ) );
}

template<typename T>
T natural_exponential( T val_ ) {
	return ( static_cast<T>( natural_exponential_impl( val_ ) ) );
}

template<typename T>
T natural_logarithm( T val_ ) {
	return ( static_cast<T>( natural_logarithm_impl( val_ ) ) );
}

template<typename T>
T power( T base_, T exponent_ ) {
	return ( static_cast<T>( power_impl( base_, exponent_ ) ) );
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

template<typename T>
T arcus_sinus( T val_ ) {
	return ( static_cast<T>( arcus_sinus_impl( val_ ) ) );
}

template<typename T>
T arcus_cosinus( T val_ ) {
	return ( static_cast<T>( arcus_cosinus_impl( val_ ) ) );
}

template<typename T>
T arcus_tangens( T val_ ) {
	return ( static_cast<T>( arcus_tangens_impl( val_ ) ) );
}

template<typename T>
T arcus_cotangens( T val_ ) {
	return ( static_cast<T>( arcus_cotangens_impl( val_ ) ) );
}

template<typename T>
T hyperbolic_sinus( T val_ ) {
	return ( static_cast<T>( hyperbolic_sinus_impl( val_ ) ) );
}

template<typename T>
T hyperbolic_cosinus( T val_ ) {
	return ( static_cast<T>( hyperbolic_cosinus_impl( val_ ) ) );
}

template<typename T>
T hyperbolic_tangens( T val_ ) {
	return ( static_cast<T>( hyperbolic_tangens_impl( val_ ) ) );
}

template<typename T>
T hyperbolic_cotangens( T val_ ) {
	return ( static_cast<T>( hyperbolic_cotangens_impl( val_ ) ) );
}

template<typename T>
T sigmoid( T val_ ) {
	return ( static_cast<T>( sigmoid_impl( val_ ) ) );
}

template<typename T>
T error_function( T val_ ) {
	return ( static_cast<T>( error_function_impl( val_ ) ) );
}

template<typename T>
T greatest_common_divisor( T a_, T b_ ) {
	return ( static_cast<T>( greatest_common_divisor_impl( a_, b_ ) ) );
}

}

}

#endif /* #ifndef YAAL_HCORE_MATH_HXX_INCLUDED */

