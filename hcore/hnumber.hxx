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
 * Integral part of HNumber is never trimmed, fractional part of HNumber
 * is trimmed according to its precision.
 * HNumber's precision can be set to arbitrarily large value.
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
 * and its precision is set large enough to ensure this exactness,
 * default precision is set to 1 + maximum of operands' precisions for `+' and `-' operations
 * and to maximum of maximum of operands precisions and sum of real operands precisions.
 *
 * Example:
 * Let n1 and n2 are both exact numbers, and n1 precision is 20 and n2 precision is 30, then:
 * n1 + n2 precision is 30 (or 31 if necessary) and
 * n1 * n2 precision is anywhere from 30 to 50.
 *
 * For `+', '-' and '*' operations if any of the operands is rounded (is not exact)
 * then result is also rounded and its precision is equal to precision of rounded
 * operand or minimum of both operands precisions if both operands are rounded,
 *
 * For '/' operation if both operands are exact result may be exact
 * if number of decimal (base 10 representation) fractional digits is smaller than
 * default maximum precision set for result.
 *
 * For '/' operation if any of operand is rounded then result is rounded and its precision
 * is set to precision on the rounded operand or minimum of both operands precision
 * if both operands are rounded.
 *
 * Examples:
 *
 * HNumber n = HNumber( 1 ) / HNumber( 3 ) // <-- Rounded.
 *
 * HNumber n1 = 1;
 * n1.set_precision( 16 );
 * n1 /= 3;
 * assert( n1 == ".3333333333333333" );
 */
class HNumber {
public:
	typedef HNumber this_type;
	typedef int size_t;
private:
	size_t _precision;
	size_t _leafCount;
	size_t _integralPartSize;
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
	HNumber( double long value, size_t precision );
	HNumber( double value, size_t precision );

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
	HNumber( int long long value, size_t precision );
	HNumber( int long value, size_t precision );
	HNumber( int value, size_t precision );
	HNumber( int long long unsigned value, size_t precision );
	HNumber( int long unsigned value, size_t precision );
	HNumber( int unsigned value, size_t precision );

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
	HNumber( char const* const, size_t );
	HNumber( HString const& );
	HNumber( HString const&, size_t );
	HNumber( HNumber const& );
#if CXX_STANDARD >= 2011
	HNumber( HNumber&& );
#endif /* #if CXX_STANDARD >= 2011 */
	~HNumber( void );
	HNumber& operator = ( HNumber const& );
#if CXX_STANDARD >= 2011
	HNumber& operator = ( HNumber&& );
#endif /* #if CXX_STANDARD >= 2011 */
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
	HNumber& abs( void );
	HNumber& round( size_t );
	HNumber& floor( void );
	HNumber& ceil( void );
	HNumber& trunc( void );
	/*! \brief Set maximum number of fractional decimal digits that this HNumber can handle.
	 *
	 * \param newPrecision - new maximum precision of this HNumber.
	 */
	void set_precision( size_t precision );
	size_t get_precision( void ) const;
	double long to_floating_point( void ) const;
	int long long to_integer( void ) const;
	HString to_string( void ) const;
	bool is_exact( void ) const;
private:
	bool mutate_addition( i32_t*, size_t, i32_t const* const[], size_t*, size_t*, bool ) const;
	size_t absolute_lower( HNumber const& ) const;
	size_t karatsuba( HChunk&, i32_t const*, size_t, i32_t const*, size_t );
	size_t integral_length( void ) const;
	size_t fractional_length( void ) const;
	size_t fractional_decimal_digits( void ) const;
	void from_string( HString const& );
	void from_floating_point( double long );
	void from_integer( int long long );
	void multiply_by_leaf( i32_t );
	i32_t multiply_by_leaf_low( i32_t*, size_t, i32_t );
	void divide_by_leaf( i32_t, size_t );
	void add_leaf_low( size_t, i32_t );
	void normalize( bool );
public:
	struct ElementaryFunctions;
};

typedef HExceptionT<HNumber> HNumberException;

}

inline void swap( yaal::hcore::HNumber& a, yaal::hcore::HNumber& b )
	{ a.swap( b ); }

yaal::hcore::HNumber square_root( yaal::hcore::HNumber const& );

}

#endif /* #ifndef YAAL_HCORE_HNUMBER_HXX_INCLUDED */

