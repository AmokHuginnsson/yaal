/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	xmath.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_XMATH_HXX_INCLUDED
#define YAAL_TOOLS_XMATH_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/iterator.hxx"
#include "hcore/pod.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Meta-function (template) implementation of various math functions.
 */
namespace xmath
{

/*! \brief Provide statistics for set of numbers.
 */
template<typename numeric_t>
class HNumberSetStats
	{
	int long _count;
	numeric_t _sum;
	numeric_t _average;
public:
	template<typename iterator_t>
	HNumberSetStats( iterator_t, iterator_t );
	int long count( void ) const
		{ return ( _count ); }
	numeric_t sum( void )
		{ return ( _sum ); }
	numeric_t average( void ) const
		{ return ( _average ); }
	};

template<typename numeric_t>
template<typename iterator_t>
HNumberSetStats<numeric_t>::HNumberSetStats( iterator_t first_, iterator_t last_ )
	: _count( 0 ), _sum(), _average()
	{
	M_PROLOG
	for ( ; first_ != last_; ++ first_, ++ _count )
		{
		_sum += *first_;
		}
	_average = _sum / _count;
	return;
	M_EPILOG
	}

template<typename iterator_t>
HNumberSetStats<typename trait::ternary<is_floating_point<typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type>::value,
	typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type,
	double long>::type> number_set_stats( iterator_t first_, iterator_t last_ )
	{
	M_PROLOG
	return ( HNumberSetStats<typename trait::ternary<is_floating_point<typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type>::value,
		typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type,
		double long>::type>( first_, last_ ) );
	M_EPILOG
	}

template<typename number_t>
number_t clip( number_t const& lowerBound_, number_t const& number_, number_t const& upperBound_ )
	{
	return ( number_ < lowerBound_ ? lowerBound_ : ( upperBound_ < number_ ? upperBound_ : number_ ) );
	}

template<typename number_t>
number_t factorial( number_t number_ )
	{
	number_t n = 1;
	for ( number_t i = 2; i <= number_; ++ i )
		n *= i;
	return ( n );
	}

template<typename number_t>
number_t binomial_coefficient( number_t cardinal_, number_t subCardinal_ )
	{
	if ( subCardinal_ > ( cardinal_ / 2 ) )
		subCardinal_ = cardinal_ - subCardinal_;
	return ( factorial( cardinal_ ) / ( factorial( subCardinal_ ) * factorial( cardinal_ - subCardinal_ ) ) );
	}

}

}

}

#endif /* #ifndef YAAL_TOOLS_XMATH_HXX_INCLUDED */

