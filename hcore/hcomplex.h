/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomplex.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HCOMPLEX_H
#define __HCORE_HCOMPLEX_H

namespace stdhapi
{

namespace hcore
{

class HComplex
	{
protected:
	/*{*/
	double f_dReal;
	double f_dImaginary;
	/*}*/
public:
	/*{*/
	HComplex ( void );
	HComplex ( const double, const double = 0 );
	HComplex ( const HComplex & );
	virtual ~HComplex ( void );
	double & re ( void );
	double & im ( void );
	double modulus ( void );
	double argument ( void );
	HComplex & operator = ( const HComplex & );
	bool operator == ( const HComplex & );
	bool operator != ( const HComplex & );
	HComplex & operator = ( const double );
	HComplex & operator += ( const HComplex & );
	HComplex & operator -= ( const HComplex & );
	HComplex & operator *= ( const HComplex & );
	HComplex & operator *= ( const double );
	HComplex & operator /= ( const HComplex & );
	HComplex & operator /= ( const double );
	HComplex operator + ( const HComplex & );
	HComplex operator - ( const HComplex & );
	HComplex operator - ( void );
	HComplex operator * ( const HComplex & );
	HComplex operator * ( const double );
	HComplex operator / ( const HComplex & );
	HComplex operator / ( const double );
	friend HComplex operator * ( const double, const HComplex & );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

}

}

#endif /* not __HCORE_HCOMPLEX_H */

