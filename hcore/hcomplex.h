/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomplex.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HCOMPLEX_H
#define __YAAL_HCORE_HCOMPLEX_H

namespace yaal
{

namespace hcore
{

class HComplex
	{
	typedef HComplex self_t;
	double f_dReal;
	double f_dImaginary;
public:
	HComplex ( void );
	HComplex ( double const, double const = 0 );
	HComplex ( HComplex const & );
	virtual ~HComplex ( void );
	double re ( void ) const;
	double im ( void ) const;
	double set_real ( double const );
	double set_imaginary ( double const );
	HComplex & set ( double const, double const );
	double modulus ( void ) const;
	double argument ( void ) const;
	HComplex & operator = ( HComplex const & );
	bool operator == ( HComplex const & ) const;
	bool operator != ( HComplex const & ) const;
	HComplex & operator = ( double const );
	HComplex & operator += ( HComplex const & );
	HComplex & operator -= ( HComplex const & );
	HComplex & operator *= ( HComplex const & );
	HComplex & operator *= ( double const );
	HComplex & operator /= ( HComplex const & );
	HComplex & operator /= ( double const );
	HComplex operator + ( HComplex const & );
	HComplex operator - ( HComplex const & );
	HComplex operator - ( void ) const;
	HComplex operator * ( HComplex const & );
	HComplex operator * ( double const );
	HComplex operator / ( HComplex const & );
	HComplex operator / ( double const );
	friend HComplex operator * ( double const, HComplex const & );
	};

}

}

#endif /* not __YAAL_HCORE_HCOMPLEX_H */

