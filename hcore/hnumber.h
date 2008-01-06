/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hnumber.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HNUMBER_H
#define __YAAL_HCORE_HNUMBER_H

#include "hcore/hpool.h"
#include "hcore/hstring.h"

namespace yaal
{

namespace hcore
{

class HNumber
	{
	typedef HPool<char> canonical_t;
	int f_iPrecision;
	bool f_bNegative;
	int f_iDigitCount;
	int f_iIntegralPartSize;
	canonical_t f_oCanonical;
public:
	static int D_DEFAULT_PRECISION;
public:
	HNumber( void );
	HNumber( double long );
	HNumber( double long, int );

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
	 * __-2__      = -3
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
	HNumber operator + ( HNumber const& );
	HNumber operator * ( HNumber const& );
	HNumber operator - ( HNumber const& );
	HNumber operator / ( HNumber const& );
	void swap( HNumber& );
	void set_precision( int );
	int get_precision( void ) const;
	double long to_double( void ) const;
	HString to_string( void ) const;
	bool is_exact( void ) const;
private:
	int integral_length( void ) const;
	int decimal_length( void ) const;
	void from_string( HString const& );
	void from_double( double long );
	};

}

}

#endif /* not __YAAL_HCORE_HNUMBER_H */

