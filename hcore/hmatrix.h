/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmatrix.h - this file is integral part of `stdhapi' project.

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

#ifndef __HMATRIX_H
#define __HMATRIX_H

#line 31

#define D_CVSID_HMATRIX_H "$CVSHeader$"

#define E_BADROWS					0
#define E_BADCOLUMNS			1
#define E_NEWRETURNEDNULL	2

extern const char * g_ppcErrMsgHMatrix [ ];

#include "hvector.h"

template < class tType >
class HMatrix : public HArray < HVector < tType > >
	{
protected:
	/* { */
	int f_iRows;
	int f_iColumns;
	/* } */
public:
	/* { */
	HMatrix ( int, int );
	HMatrix ( const HMatrix & );
	virtual ~HMatrix ( void );
	int set ( tType * * );
	int set ( HVector < tType > );
	int row ( void );
	int col ( void );
	tType det ( void );
	tType M ( int, int );
	HMatrix T ( void );
	HMatrix _1 ( void );
	HMatrix & operator = ( const HMatrix & );
	HMatrix & operator = ( tType );
	HMatrix operator + ( HMatrix & );
	HMatrix operator - ( HMatrix & );
	HMatrix operator - ( void );
	HMatrix operator * ( HMatrix & );
	HMatrix operator * ( tType );
	HMatrix operator / ( tType );
	HMatrix & operator += ( HMatrix & );
	HMatrix & operator -= ( HMatrix & );
	HMatrix & operator *= ( HMatrix & );
	HMatrix & operator *= ( tType );
	HMatrix & operator /= ( tType );
	HMatrix operator ~ ( void );
	tType operator ! ( void );
	bool operator == ( HMatrix & );
	bool operator != ( HMatrix & );
template < class ttType >
	friend HVector < ttType > operator * ( HVector < ttType > &, HMatrix & );
template < class ttType >
	friend HVector < ttType > operator *= ( HVector < ttType > &, HMatrix & );
template < class ttType >
	friend HMatrix operator * ( ttType, HMatrix < ttType > & );
	/* } */
	};
	
template < class tType >
HVector < tType > T ( HMatrix < tType > &, HVector < tType > & );

#include "hexception.h"

template < class tType >
HMatrix < tType > ::HMatrix ( int a_iRows, int a_iColumns )
									:	HArray < HVector < tType > > ( a_iRows,
											HVector < tType > ( a_iColumns ) )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( a_iRows < 1 )
		throw new HException ( __WHERE__,
				g_ppcErrMsgHMatrix [ E_BADROWS ], a_iRows );
	else f_iRows = a_iRows;
	if ( a_iColumns < 1 )
		throw new HException ( __WHERE__,
				g_ppcErrMsgHMatrix [ E_BADCOLUMNS ], a_iColumns );
	else f_iColumns = a_iColumns;
	return ;
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > ::HMatrix ( const HMatrix & a_roMatrix )
									:	HArray < HVector < tType > > ( a_roMatrix.f_iRows,
											HVector < tType > ( a_roMatrix.f_iColumns ) )
	{
	M_PROLOG
	f_iRows = a_roMatrix.f_iRows;
	f_iColumns = a_roMatrix.f_iColumns;
	( * this ) = a_roMatrix;
	return;
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > :: ~ HMatrix ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}
	
template < class tType >
int HMatrix < tType > ::set ( tType ** d )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		this->f_ptArray [ l_iCtr ]->set ( d [ l_iCtr ] );
	return ( 0 );
	M_EPILOG
	}
	
template < class tType >
int HMatrix < tType > ::set ( HVector < tType > a_poVector )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		* this->f_ptArray [ l_iCtr ] = ( * a_poVector ) [ l_iCtr ];
	return ( 0 );
	M_EPILOG
	}
	
template < class tType >
int HMatrix < tType > ::row ( void )
	{
	M_PROLOG
	return ( f_iRows );
	M_EPILOG
	}
	
template < class tType >
int HMatrix < tType > ::col ( void )
	{
	M_PROLOG
	return ( f_iColumns );
	M_EPILOG
	}
	
template < class tType >
tType HMatrix < tType > ::det ( void )
	{
	M_PROLOG
	int l_iCtr, k, q = 0;
	tType d;
	if ( f_iRows != f_iColumns )
		throw new HException ( __WHERE__, "matrix is not square",
				f_iRows - f_iColumns );
	HMatrix < tType > l_oMatrix ( f_iRows, f_iColumns );
	HVector < tType > l_oVector ( f_iColumns );
	l_oMatrix = * this;
	for ( k = 0; k < f_iRows; k++ )
		{
		if ( l_oMatrix [ k ] [ k ] != 0 ) continue;
		for ( l_iCtr = 0; l_iCtr < k; l_iCtr++ )
			if ( ( l_oMatrix [ k ] [ l_iCtr ] != 0 ) && ( l_oMatrix [ l_iCtr ] [ k ] != 0 ) )
				{
				l_oVector = l_oMatrix [ l_iCtr ];
				l_oMatrix [ l_iCtr ] = l_oMatrix [ k ];
				l_oMatrix [ k ] = l_oVector;
				q++;
				break;
				}
		if ( l_iCtr == k )
			for ( l_iCtr = k; l_iCtr < f_iRows; l_iCtr++ )
				if ( l_oMatrix [ k ] [ l_iCtr ] != 0 )
					{
					l_oVector = l_oMatrix [ l_iCtr ];
					l_oMatrix [ l_iCtr ] = l_oMatrix [ k ];
					l_oMatrix [ k ] = l_oVector;
					q++;
					break;
					}
		}
	for ( k = 0; k < f_iRows; k++ ) if ( l_oMatrix [ k ] [ k ] == 0 ) return ( 0 );
	for ( k = 0; k < f_iRows; k++ )
		{
		if ( l_oMatrix [ k ] [ k ] == 0 ) return ( 0 );
		for ( l_iCtr = k + 1; l_iCtr < f_iRows; l_iCtr++ )
			{
			d = -l_oMatrix [ l_iCtr ] [ k ] / l_oMatrix [ k ] [ k ];
			HVector < tType > v = ( d * l_oMatrix [ k ] );
			l_oMatrix [ l_iCtr ] += v;
			}
		}
	d = 1;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ ) d *= l_oMatrix [ l_iCtr ] [ l_iCtr ];
	if ( q % 2 ) d = -d;
	return ( d );
	M_EPILOG
	}
	
template < class tType >
tType HMatrix < tType > ::M ( int r, int c )
	{
	M_PROLOG
	int l_iCtr, j, ii = 0, jj = 0;
	if ( ( f_iRows == 1 ) || ( f_iColumns == 1 ) || ( f_iRows != f_iColumns ) ) return ( 0 );
	HMatrix l_oMatrix ( f_iRows - 1, f_iColumns - 1 );
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		{
		if ( l_iCtr == r ) continue;
		for ( j = 0; j < f_iColumns; j++ )
			{
			if ( j == c ) continue;
			l_oMatrix [ ii ] [ jj ] = ( * this->f_ptArray [ l_iCtr ] ) [ j ];
			jj++;
			}
		ii++;
		}
	return ( l_oMatrix.det ( ) );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::T ( void )
	{
	M_PROLOG
	int l_iCtr, j;
	HMatrix l_oMatrix ( f_iColumns, f_iRows );
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ ) for ( j = 0; j < f_iColumns; j++ ) 
		l_oMatrix [ j ] [ l_iCtr ] = ( * this->f_ptArray [ l_iCtr ] ) [ j ];
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::_1 ( void )
	{
	M_PROLOG
	tType d;
	int l_iCtr, j;
	if ( ( d = det ( ) ) != 0 )
		{
		HMatrix l_oMatrix ( f_iRows, f_iColumns );
		for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ ) for ( j = 0; j < f_iColumns; j++ )
			if ( ( l_iCtr + j ) % 2 ) l_oMatrix [ l_iCtr ] [ j ] = -M ( l_iCtr, j );
			else l_oMatrix [ l_iCtr ] [ j ] = M ( l_iCtr, j );
		return ( l_oMatrix.T ( ) / d );
		}
	return ( T ( ) );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > & HMatrix < tType > ::operator = ( const HMatrix & a_roMatrix )
	{
	M_PROLOG
	int l_iCtr = 0;
	if ( & a_roMatrix == this )return ( * this );
	
	if ( ( f_iRows > 0 ) &&  ( f_iRows != a_roMatrix.f_iRows ) )
		throw new HException ( __WHERE__, g_ppcErrMsgHVector [ E_DIMNOTMATCH ] );
	if ( ( f_iColumns > 0 ) &&  ( f_iColumns != a_roMatrix.f_iColumns ) )
		throw new HException ( __WHERE__, g_ppcErrMsgHVector [ E_DIMNOTMATCH ] );
	
	( * this ).HArray < HVector < tType > > ::operator = ( a_roMatrix );
	return ( * this );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > & HMatrix < tType > ::operator = ( tType d )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		( * this->f_ptArray  ) [ l_iCtr ] = d;
	return ( * this );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::operator + ( HMatrix & a_roMatrix )
	{
	M_PROLOG
	HMatrix l_oMatrix ( * this );
	l_oMatrix += a_roMatrix;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::operator - ( HMatrix & a_roMatrix )
	{
	M_PROLOG
	HMatrix l_oMatrix ( * this );
	l_oMatrix -= a_roMatrix;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::operator - ( void )
	{
	M_PROLOG
	HMatrix < tType > l_oMatrix ( f_iRows, f_iColumns );
	l_oMatrix = 0;
	l_oMatrix -= ( * this );
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::operator * ( HMatrix & a_roMatrix )
	{
	M_PROLOG
	tType d;
	int l_iCtr, j, k;
	if ( f_iColumns == a_roMatrix.f_iRows )
		{
		HMatrix l_oMatrix ( f_iRows, a_roMatrix.f_iColumns );
		for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ ) 
			for ( j = 0; j < a_roMatrix.f_iColumns; j++ )
				{
				d = 0;
				for ( k = 0; k < f_iColumns; k++ ) 
					d += ( ( * ( this->f_ptArray [ l_iCtr ] ) ) [ k ] * a_roMatrix [ k ] [ j ] );
				l_oMatrix [ l_iCtr ] [ j ] = d;
				}
		return ( l_oMatrix );
		}
	return ( * this );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::operator * ( tType d )
	{
	M_PROLOG
	HMatrix l_oMatrix ( * this );
	l_oMatrix *= d;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::operator / ( tType d )
	{
	M_PROLOG
	HMatrix l_oMatrix ( * this );
	l_oMatrix /= d;
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > & HMatrix < tType > ::operator += ( HMatrix & a_roMatrix )
	{
	M_PROLOG
	int l_iCtr;
	if ( ( f_iRows == a_roMatrix.f_iRows ) 
			&& ( f_iColumns == a_roMatrix.f_iColumns ) ) 
		for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
			* this->f_ptArray [ l_iCtr ] += a_roMatrix [ l_iCtr ];
	return ( * this );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > & HMatrix < tType > ::operator -= ( HMatrix & a_roMatrix )
	{
	M_PROLOG
	int l_iCtr;
	if ( ( f_iRows == a_roMatrix.f_iRows ) 
			&& ( f_iColumns == a_roMatrix.f_iColumns ) ) 
		for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
			* this->f_ptArray [ l_iCtr ] -= a_roMatrix [ l_iCtr ];
	return ( * this );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > & HMatrix < tType > ::operator *= ( HMatrix & a_roMatrix )
	{
	M_PROLOG
	tType d;
	int l_iCtr, j, k;
	if ( f_iColumns == a_roMatrix.f_iRows == a_roMatrix.f_iColumns )
		{
		HMatrix l_oMatrix ( f_iRows, a_roMatrix.f_iColumns );
		for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
			for ( j = 0; j < a_roMatrix.f_iColumns; j++ )
				{
				d = 0;
				for ( k = 0; k < f_iColumns; k++ ) 
					d += this->f_ptArray [ l_iCtr ] [ k ] * a_roMatrix [ k ] [ j ];
				l_oMatrix [ l_iCtr ] [ j ] = d;
				}
		 * this = l_oMatrix;
		}
	return ( * this );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > & HMatrix < tType > ::operator *= ( tType d )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		this->f_ptArray [ l_iCtr ] *= d;
	return ( * this );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > & HMatrix < tType > ::operator /= ( tType d )
	{
	M_PROLOG
	int l_iCtr;
	if ( d )
		for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
			this->f_ptArray [ l_iCtr ] /= d;
	return ( * this );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > HMatrix < tType > ::operator ~ ( void )
	{
	M_PROLOG
	int l_iCtr;
	HMatrix l_oMatrix ( f_iRows, f_iColumns );
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ )
		l_oMatrix [ l_iCtr ] = ~ this->f_ptArray [ l_iCtr ];
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < class tType >
tType HMatrix < tType > ::operator ! ( void )
	{
	M_PROLOG
	return ( det ( ) );
	M_EPILOG
	}
	
template < class tType >
bool HMatrix < tType > ::operator == ( HMatrix & a_roMatrix )
	{
	M_PROLOG
	int l_iCtr;
	for ( l_iCtr = 0; l_iCtr < f_iRows; l_iCtr++ ) 
		if ( this->f_ptArray [ l_iCtr ] != a_roMatrix [ l_iCtr ] )
			return ( false );
	return ( true );
	M_EPILOG
	}
	
template < class tType >
bool HMatrix < tType > ::operator != ( HMatrix & a_roMatrix )
	{
	M_PROLOG
	return ( ! ( * this == a_roMatrix ) );
	M_EPILOG
	}
	
template < class tType >
HVector < tType > operator * ( HVector < tType > & a_roVector, HMatrix < tType > & a_roMatrix )
	{
	M_PROLOG
	tType d;
	int j, k;
	if ( a_roVector.dim ( ) == a_roMatrix.f_iRows )
		{
		HVector < tType > l_oVector ( a_roMatrix.f_iColumns );
		for ( j = 0; j < a_roMatrix.f_iColumns; j++ )
			{
			d = 0;
			for ( k = 0; k < a_roMatrix.f_iColumns; k++ ) 
				d += a_roVector [ k ] * a_roMatrix [ k ] [ j ];
			l_oVector [ j ] = d;
			}
		return ( l_oVector );
		}
	return ( a_roVector );
	M_EPILOG
	}
	
template < class tType >
HVector < tType > operator *= ( HVector < tType > & a_roVector, HMatrix < tType > & a_roMatrix )
	{
	M_PROLOG
	if ( a_roVector.dim ( ) == a_roMatrix.f_iRows == a_roMatrix.f_iColumns )
		{
		HVector < tType > l_oVector = a_roVector * a_roMatrix;
		a_roVector = l_oVector;
		}
	return ( a_roVector );
	M_EPILOG
	}
	
template < class tType >
HMatrix < tType > operator * ( tType d, HMatrix < tType > & a_roMatrix )
	{
	M_PROLOG
	int l_iCtr;
	HMatrix < tType > l_oMatrix ( a_roMatrix.f_iRows, a_roMatrix.f_iColumns );
	for ( l_iCtr = 0; l_iCtr < a_roMatrix.row ( ); l_iCtr++ )
		{
		l_oMatrix [ l_iCtr ] = ( a_roMatrix [ l_iCtr ] * d );
		}
	return ( l_oMatrix );
	M_EPILOG
	}
	
template < class tType >
HVector < tType > T ( HMatrix < tType > & a_roMatrix, HVector < tType > & a_roVector )
	{
	M_PROLOG
	HMatrix < tType > l_oMatrix = a_roMatrix.T ( );
	return ( a_roVector * l_oMatrix );
	M_EPILOG
	}
	
#endif /* not __HMATRIX_H */

