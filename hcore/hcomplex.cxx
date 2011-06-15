/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomplex.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cmath>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcomplex.hxx"
#include "hcore.hxx"

namespace yaal
{

namespace hcore
{

HComplex::HComplex( void ) : _real ( 0 ), _imaginary ( 0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HComplex::HComplex( double long real_,  double long imaginary_ )
	: _real ( real_ ), _imaginary ( imaginary_ )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HComplex::HComplex( HComplex const& complex_ )
	: _real ( complex_._real ), _imaginary ( complex_._imaginary )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HComplex::~HComplex( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

double long HComplex::re( void ) const
	{
	M_PROLOG
	return ( _real );
	M_EPILOG
	}

double long HComplex::im( void ) const
	{
	M_PROLOG
	return ( _imaginary );
	M_EPILOG
	}

double long HComplex::set_real( double long real_ )
	{
	M_PROLOG
	return ( _real = real_ );
	M_EPILOG
	}

double long HComplex::set_imaginary( double long imaginary_ )
	{
	M_PROLOG
	return ( _imaginary = imaginary_ );
	M_EPILOG
	}

HComplex& HComplex::set( double long real_, double long imaginary_ )
	{
	M_PROLOG
	_real = real_;
	_imaginary = imaginary_;
	return ( *this );
	M_EPILOG
	}

double long HComplex::modulus( void ) const
	{
	M_PROLOG
	if ( ! _imaginary )
		return ( _real );
	return ( ::std::sqrt( _real * _real + _imaginary * _imaginary ) );
	M_EPILOG
	}

double long HComplex::argument( void ) const
	{
	M_PROLOG
	if ( ! _real )
		M_THROW( "I cannot count complex argument, real part equals to 0.",
				errno );
	return ( ::std::atan( _imaginary  / _real ) );
	M_EPILOG
	}

HComplex& HComplex::operator = ( HComplex const& complex_ )
	{
	M_PROLOG
	if ( &complex_ != this )
		set( complex_._real, complex_._imaginary );
	return ( *this );
	M_EPILOG
	}

bool HComplex::operator == ( HComplex const& complex_ ) const
	{
	M_PROLOG
	return ( eq( _real, complex_._real )
			&& eq( _imaginary, complex_._imaginary ) );
	M_EPILOG
	}

bool HComplex::operator != ( HComplex const& complex_ ) const
	{
	M_PROLOG
	return ( ! ( eq( _real, complex_._real )
				&& eq( _imaginary, complex_._imaginary ) ) );
	M_EPILOG
	}

HComplex& HComplex::operator = ( double long real_ )
	{
	M_PROLOG
	set( real_, 0. );
	return ( *this );
	M_EPILOG
	}

HComplex& HComplex::operator += ( HComplex const& complex_ )
	{
	M_PROLOG
	_real += complex_._real;
	_imaginary += complex_._imaginary;
	return ( *this );
	M_EPILOG
	}

HComplex& HComplex::operator -= ( HComplex const& complex_ )
	{
	M_PROLOG
	_real -= complex_._real;
	_imaginary -= complex_._imaginary;
	return ( * this );
	M_EPILOG
	}

HComplex& HComplex::operator *= ( HComplex const& complex_ )
	{
	M_PROLOG
	double long real( _real * complex_._real - _imaginary * complex_._imaginary );
	double long imaginary( _imaginary * complex_._real + _real * complex_._imaginary );
	set( real, imaginary );
	return ( *this );
	M_EPILOG
	}

HComplex& HComplex::operator *= ( double long value_ )
	{
	M_PROLOG
	_real *= value_;
	_imaginary *= value_;
	return ( *this );
	M_EPILOG
	}

HComplex& HComplex::operator /= ( HComplex const& complex_ )
	{
	M_PROLOG
	double long denominator( complex_._real * complex_._real + complex_._imaginary * complex_._imaginary );
	if ( ! denominator )
		M_THROW( "denominator equals 0", errno );
	double long real( ( _real * complex_._real + _imaginary * complex_._imaginary ) / denominator );
	double long imaginary( ( complex_._real * _imaginary - _real * complex_._imaginary ) / denominator );
	set( real, imaginary );
	return ( *this );
	M_EPILOG
	}

HComplex& HComplex::operator /= ( double long value_ )
	{
	M_PROLOG
	if ( ! value_ )
		M_THROW( "denominator equals 0", errno );
	_real /= value_;
	_imaginary /= value_;
	return ( *this );
	M_EPILOG
	}

HComplex HComplex::operator + ( HComplex const& complex_ ) const
	{
	M_PROLOG
	HComplex c( *this );
	c += complex_;
	return ( c );
	M_EPILOG
	}

HComplex HComplex::operator - ( HComplex const& complex_ ) const
	{
	M_PROLOG
	HComplex c( *this );
	c -= complex_;
	return ( c );
	M_EPILOG
	}

HComplex HComplex::operator - ( void ) const
	{
	M_PROLOG
	HComplex c( -_real, -_imaginary );
	return ( c );
	M_EPILOG
	}

HComplex HComplex::operator * ( HComplex const& complex_ ) const
	{
	M_PROLOG
	HComplex c( *this );
	c *= complex_;
	return ( c );
	M_EPILOG
	}

HComplex HComplex::operator * ( double long value_ ) const
	{
	M_PROLOG
	HComplex c( *this );
	c *= value_;
	return ( c );
	M_EPILOG
	}

HComplex HComplex::operator / ( HComplex const& complex_ ) const
	{
	M_PROLOG
	HComplex c( *this );
	c /= complex_;
	return ( c );
	M_EPILOG
	}

HComplex HComplex::operator / ( double long value_ ) const
	{
	M_PROLOG
	HComplex c( *this );
	c /= value_;
	return ( c );
	M_EPILOG
	}

HComplex operator * ( double long value_, HComplex const& complex_ )
	{
	M_PROLOG
	return ( complex_ * value_ );
	M_EPILOG
	}

}

}

