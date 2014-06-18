/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomplex.cxx - this file is integral part of `yaal' project.

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
#include <limits>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcomplex.hxx"
#include "hcore.hxx"
#include "math.hxx"

using namespace yaal::math;

namespace yaal {

namespace hcore {

HComplex::HComplex( void )
	: _real( 0 ), _imaginary( 0 ) {
	M_PROLOG
	return;
	M_EPILOG
}

HComplex::HComplex( double long real_,  double long imaginary_ )
	: _real( real_ ), _imaginary( imaginary_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HComplex::HComplex( HComplex const& complex_ )
	: _real( complex_._real ), _imaginary( complex_._imaginary ) {
	M_PROLOG
	return;
	M_EPILOG
}

HComplex::~HComplex( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

double long HComplex::re( void ) const {
	M_PROLOG
	return ( _real );
	M_EPILOG
}

double long HComplex::im( void ) const {
	M_PROLOG
	return ( _imaginary );
	M_EPILOG
}

HComplex& HComplex::set_real( double long real_ ) {
	M_PROLOG
	_real = real_;
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::set_imaginary( double long imaginary_ ) {
	M_PROLOG
	_imaginary = imaginary_;
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::set( double long real_, double long imaginary_ ) {
	M_PROLOG
	_real = real_;
	_imaginary = imaginary_;
	return ( *this );
	M_EPILOG
}

double long HComplex::modulus( void ) const {
	M_PROLOG
	double long mod( 0 );
	if ( _imaginary == 0.0l )
		mod = _real;
	else if ( _real == 0.0l )
		mod = _imaginary;
	else mod = square_root( _real * _real + _imaginary * _imaginary );
	return ( mod );
	M_EPILOG
}

double long HComplex::argument( void ) const {
	M_PROLOG
	bool reZero( yaal::abs( _real ) < ( 2.L * std::numeric_limits<double long>::epsilon() ) );
	bool imZero( yaal::abs( _imaginary ) < ( 2.L * std::numeric_limits<double long>::epsilon() ) );
	if ( reZero && imZero  )
		M_THROW( "Argument for 0 + 0i is undefined.", errno );

	double long arg( 0.L );
	if ( _real > 0.L )
		arg = ::std::atan( _imaginary  / _real );
	else if ( ( _real < 0.L ) && ( _imaginary >= 0.L ) )
		arg = ::std::atan( _imaginary  / _real ) + PI;
	else if ( ( _real < 0.L ) && ( _imaginary < 0.L ) )
		arg = ::std::atan( _imaginary  / _real ) - PI;
	else if ( reZero && ( _imaginary > 0 ) )
		arg = PI / 2.L;
	else if ( reZero && ( _imaginary < 0 ) )
		arg = -PI / 2.L;
	else {
		M_ASSERT( 0 && "bad code path" );
	}
	return ( arg );
	M_EPILOG
}

HComplex& HComplex::operator = ( HComplex const& complex_ ) {
	M_PROLOG
	if ( &complex_ != this )
		set( complex_._real, complex_._imaginary );
	return ( *this );
	M_EPILOG
}

bool HComplex::operator == ( HComplex const& complex_ ) const {
	M_PROLOG
	return ( eq( _real, complex_._real )
			&& eq( _imaginary, complex_._imaginary ) );
	M_EPILOG
}

bool HComplex::operator != ( HComplex const& complex_ ) const {
	M_PROLOG
	return ( ! ( eq( _real, complex_._real )
				&& eq( _imaginary, complex_._imaginary ) ) );
	M_EPILOG
}

HComplex& HComplex::operator += ( HComplex const& complex_ ) {
	M_PROLOG
	_real += complex_._real;
	_imaginary += complex_._imaginary;
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::operator -= ( HComplex const& complex_ ) {
	M_PROLOG
	_real -= complex_._real;
	_imaginary -= complex_._imaginary;
	return ( * this );
	M_EPILOG
}

HComplex& HComplex::operator *= ( HComplex const& complex_ ) {
	M_PROLOG
	double long real( _real * complex_._real - _imaginary * complex_._imaginary );
	double long imaginary( _imaginary * complex_._real + _real * complex_._imaginary );
	set( real, imaginary );
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::operator *= ( double long value_ ) {
	M_PROLOG
	_real *= value_;
	_imaginary *= value_;
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::operator /= ( HComplex const& complex_ ) {
	M_PROLOG
	double long denominator( complex_._real * complex_._real + complex_._imaginary * complex_._imaginary );
	if ( denominator == 0.0l )
		M_THROW( "denominator equals 0", errno );
	double long real( ( _real * complex_._real + _imaginary * complex_._imaginary ) / denominator );
	double long imaginary( ( complex_._real * _imaginary - _real * complex_._imaginary ) / denominator );
	set( real, imaginary );
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::operator /= ( double long value_ ) {
	M_PROLOG
	if ( value_ == 0.0l )
		M_THROW( "denominator equals 0", errno );
	_real /= value_;
	_imaginary /= value_;
	return ( *this );
	M_EPILOG
}

HComplex HComplex::operator + ( HComplex const& complex_ ) const {
	M_PROLOG
	HComplex c( *this );
	c += complex_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator - ( HComplex const& complex_ ) const {
	M_PROLOG
	HComplex c( *this );
	c -= complex_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator - ( void ) const {
	M_PROLOG
	HComplex c( -_real, -_imaginary );
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator * ( HComplex const& complex_ ) const {
	M_PROLOG
	HComplex c( *this );
	c *= complex_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator * ( double long value_ ) const {
	M_PROLOG
	HComplex c( *this );
	c *= value_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator / ( HComplex const& complex_ ) const {
	M_PROLOG
	HComplex c( *this );
	c /= complex_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator / ( double long value_ ) const {
	M_PROLOG
	HComplex c( *this );
	c /= value_;
	return ( c );
	M_EPILOG
}

HComplex operator + ( double long value_, HComplex const& complex_ ) {
	M_PROLOG
	return ( complex_ + value_ );
	M_EPILOG
}

HComplex operator - ( double long value_, HComplex const& complex_ ) {
	M_PROLOG
	HComplex c( value_ );
	return ( c - complex_ );
	M_EPILOG
}

HComplex operator * ( double long value_, HComplex const& complex_ ) {
	M_PROLOG
	return ( complex_ * value_ );
	M_EPILOG
}

HComplex operator / ( double long value_, HComplex const& complex_ ) {
	M_PROLOG
	HComplex c( value_ );
	return ( c / complex_ );
	M_EPILOG
}

}

}

