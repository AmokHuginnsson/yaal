/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cmath>
#include <limits>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcomplex.hxx"
#include "hcore.hxx"
#include "math.hxx"
#include "algorithm.hxx"

using namespace yaal::math;

namespace yaal {

namespace hcore {

HComplex::HComplex( void )
	: _real( 0.L ), _imaginary( 0.L ) {
	M_PROLOG
	return;
	M_EPILOG
}

HComplex::HComplex( value_type real_,  value_type imaginary_ )
	: _real( real_ + 0.L ), _imaginary( imaginary_ + 0.L ) {
	M_PROLOG
	return;
	M_EPILOG
}

HComplex::HComplex( HComplex const& complex_ )
	: _real( complex_._real + 0.L ), _imaginary( complex_._imaginary + 0.L ) {
	M_PROLOG
	return;
	M_EPILOG
}

HComplex::~HComplex( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

HComplex::value_type HComplex::re( void ) const {
	M_PROLOG
	return ( _real );
	M_EPILOG
}

HComplex::value_type HComplex::im( void ) const {
	M_PROLOG
	return ( _imaginary );
	M_EPILOG
}

HComplex& HComplex::set_real( value_type real_ ) {
	M_PROLOG
	_real = real_ + 0.L;
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::set_imaginary( value_type imaginary_ ) {
	M_PROLOG
	_imaginary = imaginary_ + 0.L;
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::set( value_type real_, value_type imaginary_ ) {
	M_PROLOG
	_real = real_ + 0.L;
	_imaginary = imaginary_ + 0.L;
	return ( *this );
	M_EPILOG
}

HComplex::value_type HComplex::modulus( void ) const {
	M_PROLOG
	value_type mod( 0.L );
	if ( ( _imaginary + 0.L ) == 0.L ) {
		mod = yaal::math::abs( _real );
	} else if ( ( _real + 0.L ) == 0.L ) {
		mod = yaal::math::abs( _imaginary );
	}	else {
		mod = square_root( _real * _real + _imaginary * _imaginary );
	}
	return ( mod );
	M_EPILOG
}

HComplex::value_type HComplex::argument( void ) const {
	M_PROLOG
	bool reZero( yaal::math::abs( _real ) < ( 2.L * std::numeric_limits<value_type>::epsilon() ) );
	bool imZero( yaal::math::abs( _imaginary ) < ( 2.L * std::numeric_limits<value_type>::epsilon() ) );
	if ( reZero && imZero  ) {
		M_THROW( "Argument for 0 + 0i is undefined.", errno );
	}
	value_type arg( 0.L );
	if ( _real > 0.L ) {
		arg = ::std::atan( _imaginary  / _real );
	} else if ( ( _real < 0.L ) && ( ( _imaginary + 0.L ) >= 0.L ) ) {
		arg = ::std::atan( _imaginary  / _real ) + PI;
	} else if ( ( _real < 0.L ) && ( _imaginary < 0.L ) ) {
		arg = ::std::atan( _imaginary  / _real ) - PI;
	} else if ( reZero && ( _imaginary > 0 ) ) {
		arg = PI / 2.L;
	} else if ( reZero && ( _imaginary < 0 ) ) {
		arg = -PI / 2.L;
	} else {
		M_ASSERT( 0 && "bad code path" );
	}
	return ( arg );
	M_EPILOG
}

HComplex& HComplex::operator = ( HComplex const& complex_ ) {
	M_PROLOG
	if ( &complex_ != this ) {
		set( complex_._real, complex_._imaginary );
	}
	return ( *this );
	M_EPILOG
}

bool HComplex::operator == ( HComplex const& complex_ ) const {
	M_PROLOG
	return (
		eq( _real, complex_._real )
		&& eq( _imaginary, complex_._imaginary )
	);
	M_EPILOG
}

bool HComplex::operator != ( HComplex const& complex_ ) const {
	M_PROLOG
	return ( ! operator == ( complex_ ) );
	M_EPILOG
}

HComplex& HComplex::operator += ( HComplex const& complex_ ) {
	M_PROLOG
	_real += complex_._real;
	_imaginary += complex_._imaginary;
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::operator -= ( HComplex const& complex_ ) {
	M_PROLOG
	_real -= complex_._real;
	_imaginary -= complex_._imaginary;
	return ( * this );
	M_EPILOG
}

HComplex& HComplex::operator *= ( HComplex const& complex_ ) {
	M_PROLOG
	value_type real( _real * complex_._real - _imaginary * complex_._imaginary );
	value_type imaginary( _imaginary * complex_._real + _real * complex_._imaginary );
	set( real, imaginary );
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::operator *= ( value_type value_ ) {
	M_PROLOG
	_real *= value_;
	_imaginary *= value_;
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::operator /= ( HComplex const& complex_ ) {
	M_PROLOG
	value_type denominator( complex_._real * complex_._real + complex_._imaginary * complex_._imaginary );
	if ( denominator == 0.0L ) {
		M_THROW( "denominator equals 0", errno );
	}
	value_type real( ( _real * complex_._real + _imaginary * complex_._imaginary ) / denominator );
	value_type imaginary( ( complex_._real * _imaginary - _real * complex_._imaginary ) / denominator );
	set( real, imaginary );
	return ( *this );
	M_EPILOG
}

HComplex& HComplex::operator /= ( value_type value_ ) {
	M_PROLOG
	if ( value_ == 0.0L ) {
		M_THROW( "denominator equals 0", errno );
	}
	_real /= value_;
	_imaginary /= value_;
	return ( *this );
	M_EPILOG
}

HComplex HComplex::operator + ( HComplex const& complex_ ) const {
	M_PROLOG
	HComplex c( *this );
	c += complex_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator - ( HComplex const& complex_ ) const {
	M_PROLOG
	HComplex c( *this );
	c -= complex_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator - ( void ) const {
	M_PROLOG
	HComplex c( -_real, -_imaginary );
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator * ( HComplex const& complex_ ) const {
	M_PROLOG
	HComplex c( *this );
	c *= complex_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator * ( value_type value_ ) const {
	M_PROLOG
	HComplex c( *this );
	c *= value_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator / ( HComplex const& complex_ ) const {
	M_PROLOG
	HComplex c( *this );
	c /= complex_;
	return ( c );
	M_EPILOG
}

HComplex HComplex::operator / ( value_type value_ ) const {
	M_PROLOG
	HComplex c( *this );
	c /= value_;
	return ( c );
	M_EPILOG
}

HComplex operator + ( HComplex::value_type value_, HComplex const& complex_ ) {
	M_PROLOG
	return ( complex_ + value_ );
	M_EPILOG
}

HComplex operator - ( HComplex::value_type value_, HComplex const& complex_ ) {
	M_PROLOG
	HComplex c( value_ );
	return ( c - complex_ );
	M_EPILOG
}

HComplex operator * ( HComplex::value_type value_, HComplex const& complex_ ) {
	M_PROLOG
	return ( complex_ * value_ );
	M_EPILOG
}

HComplex operator / ( HComplex::value_type value_, HComplex const& complex_ ) {
	M_PROLOG
	HComplex c( value_ );
	return ( c / complex_ );
	M_EPILOG
}

HComplex operator "" _yi ( double long val_ ) {
	return ( HComplex( 0.L, val_ ) );
}

HComplex operator "" _yi ( int long long unsigned val_ ) {
	return ( HComplex( 0.L, static_cast<HComplex::value_type>( val_ ) ) );
}

}

}

