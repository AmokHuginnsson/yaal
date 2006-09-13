/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomplex.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cmath>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hcomplex.h"
#include "hcore.h"

namespace yaal
{

namespace hcore
{

HComplex::HComplex ( void ) : f_dReal ( 0 ), f_dImaginary ( 0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HComplex::HComplex ( double const a_dReal,  double const a_dImaginary )
	: f_dReal ( a_dReal ), f_dImaginary ( a_dImaginary )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HComplex::HComplex ( HComplex const & a_roComplex ) : f_dReal ( 0 ), f_dImaginary ( 0 )
	{
	M_PROLOG
	( * this ) = a_roComplex;
	M_EPILOG
	}

HComplex::~HComplex ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

double HComplex::re ( void ) const
	{
	M_PROLOG
	return ( f_dReal );
	M_EPILOG
	}

double HComplex::im ( void ) const
	{
	M_PROLOG
	return ( f_dImaginary );
	M_EPILOG
	}

double HComplex::set_real ( double const a_dReal )
	{
	M_PROLOG
	return ( f_dReal = a_dReal );
	M_EPILOG
	}

double HComplex::set_imaginary ( double const a_dImaginary )
	{
	M_PROLOG
	return ( f_dImaginary = a_dImaginary );
	M_EPILOG
	}

HComplex & HComplex::set ( double const a_dReal, double const a_dImaginary )
	{
	M_PROLOG
	f_dReal = a_dReal;
	f_dImaginary = a_dImaginary;
	return ( * this );
	M_EPILOG
	}

double HComplex::modulus ( void ) const
	{
	M_PROLOG
	if ( ! f_dImaginary )
		return ( f_dReal );
	return ( sqrt ( f_dReal * f_dReal + f_dImaginary * f_dImaginary ) );
	M_EPILOG
	}

double HComplex::argument ( void ) const
	{
	M_PROLOG
	if ( ! f_dReal )
		M_THROW ( "I cannot count complex argument, real part equals to 0.",
				errno );
	return ( atan ( f_dImaginary  / f_dReal ) );
	M_EPILOG
	}

HComplex & HComplex::operator = ( HComplex const & a_roComplex )
	{
	M_PROLOG
	if ( this != & a_roComplex )
		set ( a_roComplex.f_dReal, a_roComplex.f_dImaginary );
	return ( * this );
	M_EPILOG
	}

bool HComplex::operator == ( HComplex const & a_roComplex ) const
	{
	M_PROLOG
	return ( eq ( f_dReal, a_roComplex.f_dReal )
			&& eq ( f_dImaginary, a_roComplex.f_dImaginary ) );
	M_EPILOG
	}

bool HComplex::operator != ( HComplex const & a_roComplex ) const
	{
	M_PROLOG
	return ( ! ( eq ( f_dReal, a_roComplex.f_dReal )
				&& eq ( f_dImaginary, a_roComplex.f_dImaginary ) ) );
	M_EPILOG
	}

HComplex & HComplex::operator = ( double const a_dReal )
	{
	M_PROLOG
	set ( a_dReal, 0. );
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator += ( HComplex const & a_roComplex )
	{
	M_PROLOG
	( * this ) = ( * this ) + a_roComplex;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator -= ( HComplex const & a_roComplex )
	{
	M_PROLOG
	( * this ) = ( * this ) - a_roComplex;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator *= ( HComplex const & a_roComplex )
	{
	M_PROLOG
	( * this ) = ( * this ) * a_roComplex;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator *= ( double const a_dValue )
	{
	M_PROLOG
	( * this ) = ( * this ) * a_dValue;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator /= ( HComplex const & a_roComplex )
	{
	M_PROLOG
	( * this ) = ( * this ) / a_roComplex;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator /= ( double const a_dValue )
	{
	M_PROLOG
	( * this ) = ( * this ) / a_dValue;
	return ( * this );
	M_EPILOG
	}

HComplex HComplex::operator + ( HComplex const & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	l_oComplex.f_dReal += a_roComplex.f_dReal;
	l_oComplex.f_dImaginary += a_roComplex.f_dImaginary;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator - ( HComplex const & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	l_oComplex.f_dReal -= a_roComplex.f_dReal;
	l_oComplex.f_dImaginary -= a_roComplex.f_dImaginary;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator - ( void ) const
	{
	M_PROLOG
	HComplex l_oComplex ( 0., 0. );
	l_oComplex -= ( * this );
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator * ( HComplex const & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	l_oComplex.f_dReal = f_dReal * a_roComplex.f_dReal
		- f_dImaginary * a_roComplex.f_dImaginary;
	l_oComplex.f_dImaginary = f_dImaginary * a_roComplex.f_dReal
		+ f_dReal * a_roComplex.f_dImaginary;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator * ( double const a_dValue )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	l_oComplex.f_dReal *= a_dValue;
	l_oComplex.f_dImaginary *= a_dValue;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator / ( HComplex const & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	double l_dDenominator = a_roComplex.f_dReal * a_roComplex.f_dReal
													+ a_roComplex.f_dImaginary * a_roComplex.f_dImaginary;
	if ( ! l_dDenominator )
		M_THROW ( "denominator equals 0", errno );
	l_oComplex.f_dReal = ( f_dReal * a_roComplex.f_dReal
			+ f_dImaginary * a_roComplex.f_dImaginary ) / l_dDenominator;
	l_oComplex.f_dImaginary = ( a_roComplex.f_dReal * f_dImaginary
			- f_dReal * a_roComplex.f_dImaginary ) / l_dDenominator;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator / ( double const a_dValue )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	if ( ! a_dValue )
		M_THROW ( "denominator equals 0", errno );
	l_oComplex.f_dReal /= a_dValue;
	l_oComplex.f_dImaginary /= a_dValue;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex operator * ( double const a_dValue, HComplex const & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( a_roComplex.f_dReal, a_roComplex.f_dImaginary );
	return ( l_oComplex * a_dValue );
	M_EPILOG
	}

}

}

