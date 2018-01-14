/* Read yaal/LICENSE.md file for copyright and licensing information. */
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

