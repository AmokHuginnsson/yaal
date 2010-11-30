/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hnumber.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HNUMBER_HXX_INCLUDED
#define YAAL_HCORE_HNUMBER_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"
#include "hcore/hstring.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Finite precision calculation units (numbers).
 */
class HNumber
	{
	typedef HNumber this_type;
private:
	int long _precision;
	bool _negative;
	int long _digitCount;
	int long _integralPartSize;
	HChunk _canonical;
public:
	M_YAAL_HCORE_PUBLIC_API static int DEFAULT_PRECISION;
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
	double long to_double( void ) const;
	HString to_string( void ) const;
	bool is_exact( void ) const;
private:
	bool mutate_addition( char*, int long, char const* const[], int long*, int long*, bool, bool ) const;
	int long absolute_lower( HNumber const& ) const;
	void karatsuba( HChunk&, char const*, int long, char const*, int long ) const;
	int long integral_length( void ) const;
	int long decimal_length( void ) const;
	void from_string( HString const& );
	void from_double( double long );
	void normalize( void );
	};

typedef HExceptionT<HNumber> HNumberException;

class HStreamInterface;

}

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, yaal::hcore::HNumber const& );

inline void swap( yaal::hcore::HNumber& a, yaal::hcore::HNumber& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HNUMBER_HXX_INCLUDED */

