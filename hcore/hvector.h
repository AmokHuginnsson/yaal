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

class HVector
	{
protected:
	/*{*/
	int f_iDimension;
	double * f_pdBody;
	/*}*/
public:
	/*{*/
	HVector( int );
	HVector( const HVector & );
	virtual ~HVector( void );
	int dim( void );
	int change_space( int );
	int set( double * );
	double & operator[]( int );
	int normalize();
	double norm();
	HVector & operator = ( const HVector & );
	HVector & operator = ( double );
	HVector & operator <= ( HVector & );
	HVector operator + ( HVector & );
	HVector operator - ( HVector & );
	HVector operator * ( double );
	HVector operator / ( double );
	HVector & operator += ( HVector & );
	HVector & operator -= ( HVector & );
	HVector & operator *= ( double );
	HVector & operator /= ( double );
	HVector operator ~ ( void );
	double operator | ( HVector & );
	double operator ! ( void );
	int operator == ( HVector & );
	int operator != ( HVector & );
	friend HVector operator * ( double, HVector & );
	/*}*/
	};

#endif /* not __HVECTOR_H */
