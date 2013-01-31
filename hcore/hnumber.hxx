/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hnumber.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HNUMBER_HXX_INCLUDED
#define YAAL_HCORE_HNUMBER_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"
#include "hcore/hstring.hxx"

namespace yaal {

namespace hcore {

/*! \brief Finite precision calculation units (numbers).
 *
 * HNumber is a representation for finite precision numbers.
 * Although HNumbers have finite precision the precision itself
 * can be arbitrarily large and limited only by the amount
 * of resources in computer system.
 *
 * Each HNumber may be exact or rounded.
 *
 * Every number (HNumber) explicitly specified is by definition exact, i.e.:
 *
 * HNumber n1 = 1; // Exact.
 * HNumber n2 = "3.141592653589793"; // Exact.
 *
 * HNumbers that are results of calculations may be exact depending on two factors:
 * 1) type of an operation (calculation)
 * 2) exactness status of operation operands
 *
 * Detailed rules of exactness of calculation results follows:
 *
 * For `+', `-' and `*' operations if both operands are exact then result is always exact
 * and its precision is set large enough to ensure this exactness.
 *
 * For `+', '-' and '*' operations if any of the operands is rounded (is not exact)
 * then result is also rounded and its precision is equal to precision of rounded
 * operand or minimum of both operands precisions if both operands are rounded.
 *
 * For '/' operation if both operands are exact result may be exact
 * if number of decimal (base 10 representation) fractional digits is smaller than
 * default maximum precision set for result.
 *
 * Fro '/' operation if any of operand is rounded then result is rounded and its precision
 * is set to precision on the rounded operand or minimum of both operands precision
 * if both operands are rounded.
 *
 * Examples:
 *
 * HNumber n = HNumber( 1 ) / HNumber( 3 ) // <-- Rounded.
 */
class HNumber {
	typedef HNumber this_type;
private:
	int long _precision;
	int long _leafCount;
	int long _integralPartSize;
	HChunk _canonical;
	mutable HChunk _cache;
	bool _negative;
public:
	M_YAAL_HCORE_PUBLIC_API static int const DECIMAL_DIGITS_IN_LEAF;
	M_YAAL_HCORE_PUBLIC_API static int DEFAULT_PRECISION;
public:
	/*! \brief Default constructor.
	 * Create new number object and initialize it to value of `0'.
	 */
	HNumber( void );
	/*! \brief Construct new number object from primitive floating point type.
	 *
	 * \param value - value for new number object.
	 */
	HNumber( double long value );
	HNumber( double value );
	/*! \brief Construct new number object from primitive floating point type with given precision.
	 *
	 * \param value - value for new number object.
	 * \param precision - precision for new number object.
	 */
	HNumber( double long value, int precision );
	HNumber( double value, int precision );

	/*! \brief Construct new number object from primitive integral type.
	 *
	 * \param value - value for new number object.
	 */
	HNumber( int long long value );
	HNumber( int long value );
	HNumber( int value );
	HNumber( int long long unsigned value );
	HNumber( int long unsigned value );
	HNumber( int unsigned value );
	/*! \brief Construct new number object from primitive integral type with given precision.
	 *
	 * \param value - value for new number object.
	 * \param precision - precision for new number object.
	 */
	HNumber( int long long value, int precision );
	HNumber( int long value, int precision );
	HNumber( int value, int precision );
	HNumber( int long long unsigned value, int precision );
	HNumber( int long unsigned value, int precision );
	HNumber( int unsigned value, int precision );

	/*! Construct a number from supplied string.
	 *
	 * String may contain other character, first number found will be used,
	 * i.e:
	 * (let '_' be 'any' (non-digit, non-minus and non-dot) character)
	 *
	 * __2__       = 2
	 * __32__111__ = 32
	 * 12__        = 12
	 * __3.14      = 3.14
	 * __-3__      = -3
	 * -.3         = -.3
	 * -0.5        = -0.5
	 * _-.4_       = -0.4
	 * 99-4        = 99
	 * 4.-4        = 4
	 *
	 * beware that "-", "." and "-." starts a number,
	 * first started number must be valid, so following produces an exception:
	 *
	 * -           = ex
	 * .           = ex
	 * -.          = ex
	 * __-__33__   = ex
	 * __._22.2__  = ex
	 * _-._1_      = ex
	 */
	HNumber( char const* const );
	HNumber( char const* const, int );
	HNumber( HString const& );
	HNumber( HString const&, int );
	HNumber( HNumber const& );
	~HNumber( void );
	HNumber& operator = ( HNumber const& );
	bool operator == ( HNumber const& ) const;
	bool operator != ( HNumber const& ) const;
	bool operator < ( HNumber const& ) const;
	bool operator <= ( HNumber const& ) const;
	bool operator > ( HNumber const& ) const;
	bool operator >= ( HNumber const& ) const;
	HNumber& operator += ( HNumber const& );
	HNumber& operator *= ( HNumber const& );
	HNumber& operator -= ( HNumber const& );
	HNumber& operator /= ( HNumber const& );
	HNumber& operator ^= ( int long unsigned );
	HNumber& operator ++ ( void );
	HNumber operator ++ ( int );
	HNumber& operator -- ( void );
	HNumber operator -- ( int );
	HNumber operator + ( HNumber const& ) const;
	HNumber operator * ( HNumber const& ) const;
	HNumber operator - ( HNumber const& ) const;
	HNumber operator - ( void ) const;
	HNumber operator / ( HNumber const& ) const;
	HNumber operator ^ ( int long unsigned ) const;
	void swap( HNumber& );
	void set_precision( int long );
	int long get_precision( void ) const;
	double long to_floating_point( void ) const;
	int long long to_integer( void ) const;
	HString to_string( void ) const;
	bool is_exact( void ) const;
private:
	bool mutate_addition( i32_t*, int long, i32_t const* const[], int long*, int long*, bool, bool ) const;
	int long absolute_lower( HNumber const& ) const;
	int long karatsuba( HChunk&, i32_t const*, int long, i32_t const*, int long );
	int long integral_length( void ) const;
	int long fractional_length( void ) const;
	void from_string( HString const& );
	void from_floating_point( double long );
	void from_integer( int long long );
	void normalize( void );
};

typedef HExceptionT<HNumber> HNumberException;

}

inline void swap( yaal::hcore::HNumber& a, yaal::hcore::HNumber& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HNUMBER_HXX_INCLUDED */

