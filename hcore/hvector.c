/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hvector.c - this file is integral part of `stdhapi' project.

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

#include "hvector.h"
#include "hexception.h"

HVector::HVector ( int a_iDimension )
	{
	M_PROLOG
	int l_iCtr;
	if ( a_iDimension < 1 )
		throw new HException ( __WHERE__, "Dimension < 1", a_iDimension );
	else f_iDimension = a_iDimension;
	if ( !( f_pdBody = new double [ f_iDimension ] ) )
		throw new HException ( __WHERE__, "new returned", ( int ) f_pdBody );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) f_pdBody [ l_iCtr ] = 0;
	return ;
	M_EPILOG
	}

HVector::HVector( const HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr;
	f_iDimension = a_roVector.f_iDimension;
	if ( !( f_pdBody = new double [ f_iDimension ] ) )
		throw new HException ( __WHERE__, "new returned", ( int ) f_pdBody );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ )
		f_pdBody [ l_iCtr ] = a_roVector.f_pdBody [ l_iCtr ];
	return ;
	M_EPILOG
	}

HVector::~HVector( void )
	{
	M_PROLOG
	delete [] f_pdBody;
	return ;
	M_EPILOG
	}

int HVector::dim( void )
	{
	M_PROLOG
	return ( f_iDimension );
	M_EPILOG
	}

int HVector::change_space( int a_iDimension )
	{
	M_PROLOG
	int l_iOldDimension, l_iCtr, k;
	double * l_pdTmp;
	l_iOldDimension = f_iDimension;
	l_pdTmp = f_pdBody;
	if ( a_iDimension < 1 )
		throw new HException ( __WHERE__, "Dimension < 1", a_iDimension );
	else f_iDimension = a_iDimension;
	if ( !( f_pdBody = new double [ f_iDimension ] ) )
		throw new HException ( __WHERE__, "new returned", ( int ) f_pdBody );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) f_pdBody[ l_iCtr ] = 0;
	k = ( l_iOldDimension < f_iDimension ? l_iOldDimension : f_iDimension );
	for ( l_iCtr = 0; l_iCtr < k; l_iCtr++ )
		f_pdBody [ l_iCtr ] = l_pdTmp [ l_iCtr ];
	delete [] l_pdTmp;
	return ( 0 );
	M_EPILOG
	}

int HVector::set( double * a_pdBody )
	{
	M_PROLOG
	for ( int l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) f_pdBody [ l_iCtr ] = a_pdBody [ l_iCtr ];
	return ( 0 );
	M_EPILOG
	}

double & HVector::operator [ ] ( int l_iCtr )
	{
	M_PROLOG
	while ( l_iCtr < 0 ) if ( l_iCtr < 0 ) l_iCtr = f_iDimension - l_iCtr;
	l_iCtr %= f_iDimension;
	return ( f_pdBody[ l_iCtr ] );
	M_EPILOG
	}

double HVector::norm()
	{
	M_PROLOG
	int l_iCtr;
	double d = 0;
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) d += ( f_pdBody [ l_iCtr ] * f_pdBody [ l_iCtr ] );
	return ( sqrt( d ) );
	M_EPILOG
	}

int HVector::normalize()
	{
	M_PROLOG
	int l_iCtr;
	double lenght;
	lenght = norm();
	if ( !lenght ) return ( 1 );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) f_pdBody [ l_iCtr ] /= lenght;
	return ( 0 );
	M_EPILOG
	}

HVector & HVector::operator = ( const HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr;
	if ( f_iDimension != a_roVector.f_iDimension )
		throw new HException ( __WHERE__, "dimensions do not match", f_iDimension - a_roVector.f_iDimension );
	if ( this != & a_roVector ) 
		for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) 
			f_pdBody [ l_iCtr ] = a_roVector.f_pdBody [ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

HVector & HVector::operator = ( double d )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) f_pdBody [ l_iCtr ] = d;
	return ( *this );
	M_EPILOG
	}

HVector & HVector::operator <= ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr, k;
	if ( this != & a_roVector )
		{
		k = ( f_iDimension < a_roVector.f_iDimension ? f_iDimension : a_roVector.f_iDimension );
		for ( l_iCtr = 0; l_iCtr < k; l_iCtr++ ) f_pdBody [ l_iCtr ] = a_roVector.f_pdBody [ l_iCtr ];
		}
	return ( *this );
	M_EPILOG
	}

HVector HVector::operator + ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr;
	HVector l_oVector( f_iDimension );
	if ( f_iDimension != a_roVector.f_iDimension )
		throw new HException ( __WHERE__, "dimensions do not match", f_iDimension - a_roVector.f_iDimension );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) 
		l_oVector.f_pdBody [ l_iCtr ] = f_pdBody [ l_iCtr ] + a_roVector.f_pdBody [ l_iCtr ];
	return ( l_oVector );
	M_EPILOG
	}

HVector HVector::operator - ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr;
	if ( f_iDimension != a_roVector.f_iDimension )
		throw new HException ( __WHERE__, "dimensions do not match", f_iDimension - a_roVector.f_iDimension );
	HVector l_oVector ( f_iDimension );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) 
		l_oVector.f_pdBody [ l_iCtr ] = f_pdBody [ l_iCtr ] - a_roVector.f_pdBody [ l_iCtr ];
	return ( l_oVector );
	M_EPILOG
	}

HVector HVector::operator * ( double d )
	{
	M_PROLOG
	HVector l_oVector ( f_iDimension );
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) 
		l_oVector.f_pdBody [ l_iCtr ] = f_pdBody [ l_iCtr ] * d;
	return ( l_oVector );
	M_EPILOG
	}

HVector HVector::operator / ( double d )
	{
	M_PROLOG
	HVector l_oVector ( f_iDimension );
	int l_iCtr;
	if ( d ) for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ )
		l_oVector.f_pdBody [ l_iCtr ] = f_pdBody [ l_iCtr ] / d;
	return ( l_oVector );
	M_EPILOG
	}

HVector & HVector::operator += ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr;
	if ( f_iDimension != a_roVector.f_iDimension )
		throw new HException ( __WHERE__, "dimensions do not match", f_iDimension - a_roVector.f_iDimension );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) 
		f_pdBody [ l_iCtr ] += a_roVector.f_pdBody [ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

HVector & HVector::operator -= ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr;
	if ( f_iDimension != a_roVector.f_iDimension )
		throw new HException ( __WHERE__, "dimensions do not match", f_iDimension - a_roVector.f_iDimension );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ )
		f_pdBody [ l_iCtr ] -= a_roVector.f_pdBody [ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

HVector & HVector::operator *= ( double d )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) f_pdBody [ l_iCtr ] *= d;
	return ( *this );
	M_EPILOG
	}

HVector & HVector::operator /= ( double d )
	{
	M_PROLOG
	int l_iCtr;
	if ( d ) for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) f_pdBody [ l_iCtr ] /= d;
	return ( *this );
	M_EPILOG
	}

HVector HVector::operator ~ ( void )
	{
	M_PROLOG
	int l_iCtr;
	HVector l_oVector ( f_iDimension );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ )
		l_oVector.f_pdBody [ l_iCtr ] = -f_pdBody [ l_iCtr ];
	return ( l_oVector );
	M_EPILOG
	}

double HVector::operator | ( HVector & a_roVector )
	{
	M_PROLOG
	double d = 0;
	int l_iCtr;
	if ( f_iDimension != a_roVector.f_iDimension )
		throw new HException ( __WHERE__, "dimensions do not match", f_iDimension - a_roVector.f_iDimension );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) 
		d += ( f_pdBody [ l_iCtr ] * a_roVector.f_pdBody [ l_iCtr ] );
	return ( d );
	M_EPILOG
	}

double HVector::operator ! ( void )
	{
	M_PROLOG
	return ( norm() );
	M_EPILOG
	}

int HVector::operator == ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr;
	if ( f_iDimension != a_roVector.f_iDimension )
		throw new HException ( __WHERE__, "dimensions do not match", f_iDimension - a_roVector.f_iDimension );
	for ( l_iCtr = 0; l_iCtr < f_iDimension; l_iCtr++ ) 
		if ( f_pdBody [ l_iCtr ] != a_roVector.f_pdBody [ l_iCtr ] ) return ( 0 );
	return ( 1 );
	M_EPILOG
	}

int HVector::operator != ( HVector & a_roVector )
	{
	M_PROLOG
	return ( 1 - ( *this == a_roVector ) );
	M_EPILOG
	}

HVector operator * ( double d, HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr;
	HVector l_oVector ( a_roVector.f_iDimension );
	for ( l_iCtr = 0; l_iCtr < l_oVector.f_iDimension; l_iCtr++ )
		l_oVector.f_pdBody [ l_iCtr ] = a_roVector.f_pdBody [ l_iCtr ] * d;
	return ( l_oVector );
	M_EPILOG
	}

