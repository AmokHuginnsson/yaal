/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hvector.h - this file is integral part of `stdhapi' project.

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

#ifndef __HVECTOR_H
#define __HVECTOR_H

#line 31

#define D_CVSID_HVECTOR_H "$CVSHeader$"

extern const char * g_pcErrorMessageDimensionsDoNotMatch;

#define M_CHECK_DIMENSIONS( ) \
	if ( f_iSize != a_roVector.f_iSize ) \
		throw new HException ( __WHERE__, g_pcErrorMessageDimensionsDoNotMatch, \
				f_iSize - a_roVector.f_iSize )

#include "harray.h"

template < class tType >
class HVector : public HArray < tType >
	{
protected:
	/*{*/
	/*}*/
public:
	/*{*/
	HVector ( int );
	HVector ( const HVector &, int = 0 );
	virtual ~HVector ( void );
	int dim ( void );
	int set ( tType * );
	int normalize ( );
	tType norm ( );
	HVector & operator = ( const HVector & );
	HVector & operator = ( tType );
	HVector & operator <= ( HVector & );
	HVector operator + ( HVector & );
	HVector operator - ( HVector & );
	HVector operator * ( tType );
	HVector operator / ( tType );
	HVector & operator += ( HVector & );
	HVector & operator -= ( HVector & );
	HVector & operator *= ( tType );
	HVector & operator /= ( tType );
	HVector operator ~ ( void );
	tType operator | ( HVector & );
	tType operator ! ( void );
	int operator == ( HVector & );
	int operator != ( HVector & );
template < class ttType >
	friend HVector operator * ( ttType, HVector & );
	/*}*/
	};

#include <math.h>

#include "hexception.h"

template < class tType >
HVector < tType > ::HVector ( int a_iDimension )
				: HArray < tType > ( a_iDimension )
	{
	M_PROLOG
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		f_ptArray [ l_iCtr ] = 0;
	return ;
	M_EPILOG
	}

template < class tType >
HVector < tType > ::~HVector ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template < class tType >
HVector < tType > ::HVector ( const HVector & a_roVector, int )
				: HArray < tType > ( a_roVector )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template < class tType >
int HVector < tType > ::set ( tType * a_pdBody )
	{
	M_PROLOG
	for ( int l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		f_ptArray [ l_iCtr ] = a_pdBody [ l_iCtr ];
	return ( 0 );
	M_EPILOG
	}

template < class tType >
tType HVector < tType > ::norm ( )
	{
	M_PROLOG
	int l_iCtr;
	tType d = 0;
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		d += ( f_ptArray [ l_iCtr ] * f_ptArray [ l_iCtr ] );
	return ( sqrt ( d ) );
	M_EPILOG
	}

template < class tType >
int HVector < tType > ::normalize ( )
	{
	M_PROLOG
	int l_iCtr;
	tType l_tLenght;
	l_tLenght = norm ( );
	if ( ! l_tLenght ) return ( 1 );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		f_ptArray [ l_iCtr ] /= l_tLenght;
	return ( 0 );
	M_EPILOG
	}

template < class tType >
HVector < tType > & 
HVector < tType > ::operator = ( const HVector & a_roVector )
	{
	M_PROLOG
	( * this ).HArray < tType > ::operator = ( a_roVector );
	return ( * this );
	M_EPILOG
	}

template < class tType >
HVector < tType > & HVector < tType > ::operator = ( tType d )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		f_ptArray [ l_iCtr ] = d;
	return ( *this );
	M_EPILOG
	}

template < class tType >
HVector < tType > & HVector < tType > ::operator <= ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr, k;
	if ( this != & a_roVector )
		{
		k = ( f_iSize < a_roVector.f_iSize ? f_iSize : a_roVector.f_iSize );
		for ( l_iCtr = 0; l_iCtr < k; l_iCtr ++ )
			f_ptArray [ l_iCtr ] = a_roVector.f_ptArray [ l_iCtr ];
		}
	return ( *this );
	M_EPILOG
	}

template < class tType >
HVector < tType > HVector < tType > ::operator + ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_CHECK_DIMENSIONS ( );
	HVector l_oVector ( f_iSize );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ ) 
		l_oVector.f_ptArray [ l_iCtr ] = f_ptArray [ l_iCtr ] + a_roVector.f_ptArray [ l_iCtr ];
	return ( l_oVector );
	M_EPILOG
	}

template < class tType >
HVector < tType > HVector < tType > ::operator - ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_CHECK_DIMENSIONS ( );
	HVector l_oVector ( f_iSize );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ ) 
		l_oVector.f_ptArray [ l_iCtr ] = f_ptArray [ l_iCtr ] - a_roVector.f_ptArray [ l_iCtr ];
	return ( l_oVector );
	M_EPILOG
	}

template < class tType >
HVector < tType > HVector < tType > ::operator * ( tType d )
	{
	M_PROLOG
	int l_iCtr = 0;
	HVector l_oVector ( f_iSize );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ ) 
		l_oVector.f_ptArray [ l_iCtr ] = f_ptArray [ l_iCtr ] * d;
	return ( l_oVector );
	M_EPILOG
	}

template < class tType >
HVector < tType > HVector < tType > ::operator / ( tType d )
	{
	M_PROLOG
	int l_iCtr = 0;
	HVector l_oVector ( f_iSize );
	if ( d ) for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		l_oVector.f_ptArray [ l_iCtr ] = f_ptArray [ l_iCtr ] / d;
	return ( l_oVector );
	M_EPILOG
	}

template < class tType >
HVector < tType > & HVector < tType > ::operator += ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_CHECK_DIMENSIONS ( );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ ) 
		f_ptArray [ l_iCtr ] += a_roVector.f_ptArray [ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

template < class tType >
HVector < tType > & HVector < tType > ::operator -= ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_CHECK_DIMENSIONS ( );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		f_ptArray [ l_iCtr ] -= a_roVector.f_ptArray [ l_iCtr ];
	return ( *this );
	M_EPILOG
	}

template < class tType >
HVector < tType > & HVector < tType > ::operator *= ( tType d )
	{
	M_PROLOG
	int l_iCtr = 0;
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		f_ptArray [ l_iCtr ] *= d;
	return ( *this );
	M_EPILOG
	}

template < class tType >
HVector < tType > & HVector < tType > ::operator /= ( tType d )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( d ) for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		f_ptArray [ l_iCtr ] /= d;
	return ( *this );
	M_EPILOG
	}

template < class tType >
HVector < tType > HVector < tType > ::operator ~ ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	HVector l_oVector ( f_iSize );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ )
		l_oVector.f_ptArray [ l_iCtr ] = - f_ptArray [ l_iCtr ];
	return ( l_oVector );
	M_EPILOG
	}

template < class tType >
tType HVector < tType > ::operator | ( HVector & a_roVector )
	{
	M_PROLOG
	tType d = 0;
	int l_iCtr = 0;
	M_CHECK_DIMENSIONS ( );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ ) 
		d += ( f_ptArray [ l_iCtr ] * a_roVector.f_ptArray [ l_iCtr ] );
	return ( d );
	M_EPILOG
	}

template < class tType >
tType HVector < tType > ::operator ! ( void )
	{
	M_PROLOG
	return ( norm ( ) );
	M_EPILOG
	}

template < class tType >
int HVector < tType > ::operator == ( HVector & a_roVector )
	{
	M_PROLOG
	int l_iCtr = 0;
	M_CHECK_DIMENSIONS ( );
	for ( l_iCtr = 0; l_iCtr < f_iSize; l_iCtr ++ ) 
		if ( f_ptArray [ l_iCtr ] != a_roVector.f_ptArray [ l_iCtr ] ) return ( 0 );
	return ( 1 );
	M_EPILOG
	}

template < class tType >
int HVector < tType > ::operator != ( HVector & a_roVector )
	{
	M_PROLOG
	return ( ! ( * this == a_roVector ) );
	M_EPILOG
	}

template < class tType >
HVector < tType > operator * ( tType d, HVector < tType > & a_roVector )
	{
	M_PROLOG
	int l_iCtr = 0;
	HVector < tType > l_oVector ( a_roVector.f_iSize );
	for ( l_iCtr = 0; l_iCtr < l_oVector.f_iSize; l_iCtr ++ )
		l_oVector.f_ptArray [ l_iCtr ] = a_roVector.f_ptArray [ l_iCtr ] * d;
	return ( l_oVector );
	M_EPILOG
	}

#endif /* not __HVECTOR_H */
