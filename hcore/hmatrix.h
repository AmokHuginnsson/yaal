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

#include "hvector.h"

class HMatrix
	{
protected:
	/*{*/
	int f_iRows;
	int f_iColumns;
	HVector * * f_ppoBody;
	/*}*/
public:
	/*{*/
	HMatrix ( int, int );
	HMatrix ( const HMatrix & );
	virtual ~HMatrix( void );
	int set ( double ** );
	int set ( HVector * );
	int row ( void );
	int col ( void );
	int change_space ( int, int );
	HVector & operator [ ] ( int );
	double det ( void );
	double M ( int, int );
	HMatrix T ( void );
	HMatrix _1 ( void );
	HMatrix & operator = ( const HMatrix & );
	HMatrix & operator = ( double );
	HMatrix & operator <= ( HMatrix & );
	HMatrix operator + ( HMatrix & );
	HMatrix operator - ( HMatrix & );
	HMatrix operator * ( HMatrix & );
	HMatrix operator * ( double );
	HMatrix operator / ( double );
	HMatrix & operator += ( HMatrix & );
	HMatrix & operator -= ( HMatrix & );
	HMatrix & operator *= ( HMatrix & );
	HMatrix & operator *= ( double );
	HMatrix & operator /= ( double );
	HMatrix operator ~ ( void );
	double operator ! ( void );
	int operator == ( HMatrix & );
	int operator != ( HMatrix & );
	friend HVector operator * ( HVector &, HMatrix & );
	friend HVector operator *= ( HVector &, HMatrix & );
	friend HMatrix operator * ( double, HMatrix & );
	/*}*/
	};
	
HVector T( HMatrix &, HVector & );

#endif /* __HMATRIX_H */
