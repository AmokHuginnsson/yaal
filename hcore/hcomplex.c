/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomplex.c - this file is integral part of `stdhapi' project.

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

#include <math.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "hcomplex.h"

HComplex::HComplex ( void )
	{
	M_PROLOG
	f_dReal = 0;
	f_dImaginary = 0;
	return;
	M_EPILOG
	}

HComplex::HComplex ( double a_dReal,  double a_dImaginary )
	{
	M_PROLOG
	f_dReal = a_dReal;
	f_dImaginary = a_dImaginary;
	return;
	M_EPILOG
	}

HComplex::HComplex ( const HComplex & a_roComplex, int )
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

double & HComplex::re ( void )
	{
	M_PROLOG
	return ( f_dReal );
	M_EPILOG
	}

double & HComplex::im ( void )
	{
	M_PROLOG
	return ( f_dImaginary );
	M_EPILOG
	}

double HComplex::modulus ( void )
	{
	M_PROLOG
	if ( ! f_dImaginary )
		return ( f_dReal );
	return ( sqrt ( f_dReal * f_dReal + f_dImaginary * f_dImaginary ) );
	M_EPILOG
	}

double HComplex::argument ( void )
	{
	M_PROLOG
	if ( ! f_dReal )
		throw new HException ( __WHERE__,
				"I can not count complex argument, real part equals 0.", g_iErrNo );
	return ( atan ( f_dImaginary  / f_dReal ) );
	M_EPILOG
	}

HComplex & HComplex::operator = ( const HComplex & a_roComplex )
	{
	M_PROLOG
	f_dReal = a_roComplex.f_dReal;
	f_dImaginary = a_roComplex.f_dImaginary;
	return ( * this );
	M_EPILOG
	}

bool HComplex::operator == ( const HComplex & a_roComplex )
	{
	M_PROLOG
	return ( ( f_dReal == a_roComplex.f_dReal )
			&& ( f_dImaginary == a_roComplex.f_dImaginary ) );
	M_EPILOG
	}

bool HComplex::operator != ( const HComplex & a_roComplex )
	{
	M_PROLOG
	return ( ( f_dReal != a_roComplex.f_dReal )
			|| ( f_dImaginary != a_roComplex.f_dImaginary ) );
	M_EPILOG
	}

HComplex & HComplex::operator = ( const double a_dReal )
	{
	M_PROLOG
	f_dReal = a_dReal;
	f_dImaginary = 0;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator += ( const HComplex & a_roComplex )
	{
	M_PROLOG
	( * this ) = ( * this ) + a_roComplex;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator -= ( const HComplex & a_roComplex )
	{
	M_PROLOG
	( * this ) = ( * this ) - a_roComplex;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator *= ( const HComplex & a_roComplex )
	{
	M_PROLOG
	( * this ) = ( * this ) * a_roComplex;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator *= ( const double a_dValue )
	{
	M_PROLOG
	( * this ) = ( * this ) * a_dValue;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator /= ( const HComplex & a_roComplex )
	{
	M_PROLOG
	( * this ) = ( * this ) / a_roComplex;
	return ( * this );
	M_EPILOG
	}

HComplex & HComplex::operator /= ( const double a_dValue )
	{
	M_PROLOG
	( * this ) = ( * this ) / a_dValue;
	return ( * this );
	M_EPILOG
	}

HComplex HComplex::operator + ( const HComplex & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	l_oComplex.f_dReal += a_roComplex.f_dReal;
	l_oComplex.f_dImaginary += a_roComplex.f_dImaginary;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator - ( const HComplex & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	l_oComplex.f_dReal -= a_roComplex.f_dReal;
	l_oComplex.f_dImaginary -= a_roComplex.f_dImaginary;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator - ( )
	{
	M_PROLOG
	HComplex l_oComplex ( 0., 0. );
	l_oComplex -= ( * this );
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator * ( const HComplex & a_roComplex )
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

HComplex HComplex::operator * ( const double a_dValue )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	l_oComplex.f_dReal *= a_dValue;
	l_oComplex.f_dImaginary *= a_dValue;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator / ( const HComplex & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	double l_dDenominator = a_roComplex.f_dReal * a_roComplex.f_dReal
													+ a_roComplex.f_dImaginary * a_roComplex.f_dImaginary;
	if ( ! l_dDenominator )
		throw new HException ( __WHERE__, "denominator equals 0" );
	l_oComplex.f_dReal = ( f_dReal * a_roComplex.f_dReal
			+ f_dImaginary * a_roComplex.f_dImaginary ) / l_dDenominator;
	l_oComplex.f_dImaginary = ( a_roComplex.f_dReal * f_dImaginary
			- f_dReal * a_roComplex.f_dImaginary ) / l_dDenominator;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex HComplex::operator / ( const double a_dValue )
	{
	M_PROLOG
	HComplex l_oComplex ( f_dReal, f_dImaginary );
	if ( ! a_dValue )
		throw new HException ( __WHERE__, "denominator equals 0", g_iErrNo );
	l_oComplex.f_dReal /= a_dValue;
	l_oComplex.f_dImaginary /= a_dValue;
	return ( l_oComplex );
	M_EPILOG
	}

HComplex operator * ( const double a_dValue, const HComplex & a_roComplex )
	{
	M_PROLOG
	HComplex l_oComplex ( a_roComplex.f_dReal, a_roComplex.f_dImaginary );
	return ( l_oComplex * a_dValue );
	M_EPILOG
	}

