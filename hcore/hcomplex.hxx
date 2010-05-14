/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcomplex.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hcomplex.hxx
 * \brief Declaration of HComplex class.
 */

#ifndef YAAL_HCORE_HCOMPLEX_HXX_INCLUDED
#define YAAL_HCORE_HCOMPLEX_HXX_INCLUDED

namespace yaal
{

namespace hcore
{

/*! \brief Complex number representation.
 */
class HComplex
	{
	typedef HComplex self_t;
	double _real;
	double _imaginary;
public:
	HComplex( void );
	HComplex( double const, double const = 0 );
	HComplex( HComplex const& );
	virtual ~HComplex( void );
	double re( void ) const;
	double im( void ) const;
	double set_real( double const );
	double set_imaginary( double const );
	HComplex& set( double const, double const );
	double modulus( void ) const;
	double argument( void ) const;
	HComplex& operator = ( HComplex const& );
	bool operator == ( HComplex const& ) const;
	bool operator != ( HComplex const& ) const;
	HComplex& operator = ( double const );
	HComplex& operator += ( HComplex const& );
	HComplex& operator -= ( HComplex const& );
	HComplex& operator *= ( HComplex const& );
	HComplex& operator *= ( double const );
	HComplex& operator /= ( HComplex const& );
	HComplex& operator /= ( double const );
	HComplex operator + ( HComplex const& );
	HComplex operator - ( HComplex const& );
	HComplex operator - ( void ) const;
	HComplex operator * ( HComplex const& );
	HComplex operator * ( double const );
	HComplex operator / ( HComplex const& );
	HComplex operator / ( double const );
	friend HComplex operator * ( double const, HComplex const& );
	};

}

}

#endif /* not YAAL_HCORE_HCOMPLEX_HXX_INCLUDED */

