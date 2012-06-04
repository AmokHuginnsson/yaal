/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	xmath.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_XMATH_HXX_INCLUDED
#define YAAL_TOOLS_XMATH_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/iterator.hxx"
#include "hcore/pod.hxx"
#include "hcore/algorithm.hxx"

namespace yaal {

namespace tools {

/*! \brief Meta-function (template) implementation of various math functions.
 */
namespace xmath {

template<typename number_t>
number_t square_root( number_t );

template<typename iterator_t>
typename hcore::iterator_traits<iterator_t>::value_type
select( iterator_t, iterator_t, int long );

struct AGGREGATE_TYPE {
	typedef enum {
		COUNT = 1,
		MINIMUM = 1,
		MAXIMUM = 1,
		SUM = 1,
		AVERAGE = 1,
		VARIANCE = 1,
		POPULATION_VARIANCE = 1,
		STANDARD_DEVIATION = 1,
		POPULATION_STANDARD_DEVIATION = 1,
		MEDIAN = 2
	} type_t;
};

/*! \brief Provide statistics for set of numbers.
 */
template<typename numeric_t>
class HNumberSetStats {
private:
	int long _count;
	numeric_t _minimum;
	numeric_t _maximum;
	numeric_t _sum;
	numeric_t _average;
	numeric_t _median;
	numeric_t _variance;
	numeric_t _populationVariance;
	AGGREGATE_TYPE::type_t _aggregateType;
public:
	/*! \brief Construct statictics for given range of numbers.
	 *
	 * Following aggregate types are implicit:
	 * `count', `minimum', `maximum', `sum', `average'
	 * and `variance' (`population variance',
	 * `standard deviation', `population standard deviation').
	 *
	 * One can explicitly request `median' aggregation type.
	 */
	template<typename iterator_t>
	HNumberSetStats( iterator_t, iterator_t, AGGREGATE_TYPE::type_t = AGGREGATE_TYPE::COUNT );
	int long count( void ) const
		{ return ( _count ); }
	numeric_t minimum( void ) const {
		M_PROLOG
		M_ENSURE( _count > 0 );
		return ( _minimum );
		M_EPILOG
	}
	numeric_t maximum( void ) const {
		M_PROLOG
		M_ENSURE( _count > 0 );
		return ( _maximum );
		M_EPILOG
	}
	numeric_t sum( void )
		{ return ( _sum ); }
	numeric_t average( void ) const {
		M_PROLOG
		M_ENSURE( _count > 0 );
		return ( _average );
		M_EPILOG
	}
	numeric_t median( void ) const {
		M_PROLOG
		M_ENSURE( ( _count > 0 ) && ( _aggregateType & AGGREGATE_TYPE::MEDIAN ) );
		return ( _median );
		M_EPILOG
	}
	numeric_t variance( void ) const {
		M_PROLOG
		M_ENSURE( _count > 1 );
		return ( _variance );
		M_EPILOG
	}
	numeric_t population_variance( void ) const {
		M_PROLOG
		M_ENSURE( _count > 0 );
		return ( _populationVariance );
		M_EPILOG
	}
	numeric_t standard_deviation( void ) const {
		M_PROLOG
		M_ENSURE( _count > 1 );
		return ( square_root( _variance ) );
		M_EPILOG
	}
	numeric_t population_standard_deviation( void ) const {
		M_PROLOG
		M_ENSURE( _count > 0 );
		return ( square_root( _populationVariance ) );
		M_EPILOG
	}
};

template<typename numeric_t>
template<typename iterator_t>
HNumberSetStats<numeric_t>::HNumberSetStats( iterator_t first_, iterator_t last_, AGGREGATE_TYPE::type_t aggregateType_ )
	: _count( 0 ), _minimum(), _maximum(), _sum(), _average(),
	_median(), _variance(), _populationVariance(),
	_aggregateType( static_cast<AGGREGATE_TYPE::type_t>( aggregateType_ | AGGREGATE_TYPE::COUNT ) ) {
	M_PROLOG
	numeric_t acc( 0 );
	for ( ; first_ != last_; ++ first_, ++ _count ) {
		if ( _count ) {
			if ( *first_ < _minimum )
				_minimum = *first_;
			if ( *first_ > _maximum )
				_maximum = *first_;
		} else {
			_minimum = *first_;
			_maximum = *first_;
		}
		_sum += *first_;
		acc += ( *first_ * *first_ );
	}
	_average = _sum / static_cast<numeric_t>( _count );
	if ( _count > 1 )
		_variance = acc / static_cast<numeric_t>( _count - 1 ) - ( ( _average * _average * static_cast<numeric_t>( _count ) ) / static_cast<numeric_t>( _count - 1 ) );
	_populationVariance = acc / static_cast<numeric_t>( _count ) - _average * _average;
	if ( ( _count > 0 ) && ( _aggregateType & AGGREGATE_TYPE::MEDIAN ) )
		_median = select( first_, last_, _count / 2 );
	return;
	M_EPILOG
}

template<typename iterator_t>
HNumberSetStats<typename trait::ternary<is_floating_point<typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type>::value,
	typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type,
	double long>::type> number_set_stats( iterator_t first_, iterator_t last_, AGGREGATE_TYPE::type_t aggregateType_ = AGGREGATE_TYPE::COUNT ) {
	M_PROLOG
	return ( HNumberSetStats<typename trait::ternary<is_floating_point<typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type>::value,
		typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type,
		double long>::type>( first_, last_, aggregateType_ ) );
	M_EPILOG
}

template<typename number_t>
inline number_t clip( number_t const& lowerBound_, number_t const& number_, number_t const& upperBound_ ) {
	return ( number_ < lowerBound_ ? lowerBound_ : ( upperBound_ < number_ ? upperBound_ : number_ ) );
}

template<typename number_t>
inline number_t factorial( number_t number_ ) {
	number_t n = 1;
	for ( number_t i = 2; i <= number_; ++ i )
		n *= i;
	return ( n );
}

template<typename number_t>
inline number_t binomial_coefficient( number_t cardinal_, number_t subCardinal_ ) {
	if ( subCardinal_ > ( cardinal_ / 2 ) )
		subCardinal_ = cardinal_ - subCardinal_;
	return ( factorial( cardinal_ ) / ( factorial( subCardinal_ ) * factorial( cardinal_ - subCardinal_ ) ) );
}

template<int long const M, typename number_t>
inline number_t round_up( number_t const& n ) {
	return ( ( ( ( n - 1 ) / M ) + 1 ) * M );
}

template<typename iterator_t>
typename hcore::iterator_traits<iterator_t>::value_type
select( iterator_t first_, iterator_t last_, int long ) {
	typedef typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type value_t;
	typedef hcore::HAuxiliaryBuffer<value_t> aux_t;
	aux_t aux( first_, last_ );
	M_ENSURE( aux.get_size() == aux.get_requested_size() );
	value_t v;
	return ( v );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_XMATH_HXX_INCLUDED */

