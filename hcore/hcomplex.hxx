/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hcomplex.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/hcomplex.hxx
 * \brief Declaration of HComplex class.
 */

#ifndef YAAL_HCORE_HCOMPLEX_HXX_INCLUDED
#define YAAL_HCORE_HCOMPLEX_HXX_INCLUDED 1

#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

/*! \brief Complex number representation.
 */
class HComplex final {
public:
	typedef HComplex this_type;
	typedef double long value_type;
private:
	value_type _real;
	value_type _imaginary;
public:
	HComplex( void );
	HComplex( value_type, value_type = 0 );
	HComplex( HComplex const& );
	~HComplex( void );
	value_type re( void ) const;
	value_type im( void ) const;
	HComplex& set_real( value_type );
	HComplex& set_imaginary( value_type );
	HComplex& set( value_type, value_type );
	value_type modulus( void ) const;
	value_type argument( void ) const;
	HComplex& operator = ( HComplex const& );
	bool operator == ( HComplex const& ) const;
	bool operator != ( HComplex const& ) const;
	HComplex& operator += ( HComplex const& );
	HComplex& operator -= ( HComplex const& );
	HComplex& operator *= ( HComplex const& );
	HComplex& operator *= ( value_type );
	HComplex& operator /= ( HComplex const& );
	HComplex& operator /= ( value_type );
	HComplex operator + ( HComplex const& ) const;
	HComplex operator - ( HComplex const& ) const;
	HComplex operator - ( void ) const;
	HComplex operator * ( HComplex const& ) const;
	HComplex operator * ( value_type ) const;
	HComplex operator / ( HComplex const& ) const;
	HComplex operator / ( value_type ) const;
	friend HComplex operator + ( HComplex::value_type, HComplex const& );
	friend HComplex operator - ( HComplex::value_type, HComplex const& );
	friend HComplex operator * ( HComplex::value_type, HComplex const& );
	friend HComplex operator / ( HComplex::value_type, HComplex const& );
};

HComplex operator "" _yi ( double long );
HComplex operator "" _yi ( int long long unsigned );

typedef yaal::hcore::HExceptionT<HComplex> HComplexException;

}

}

#endif /* #ifndef YAAL_HCORE_HCOMPLEX_HXX_INCLUDED */

