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
#include "hcore/math.hxx"
#include "hcore/pod.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/iterator.hxx"
#include "hcore/hbitflag.hxx"

namespace yaal {

namespace tools {

/*! \brief Meta-function (template) implementation of various math functions.
 */
namespace xmath {

template<typename iterator_t>
typename hcore::iterator_traits<iterator_t>::value_type
select( iterator_t, iterator_t, int long );

typedef yaal::hcore::HBitFlag<struct AGGREGATE_TYPE> aggregate_type_t;
struct AGGREGATE_TYPE {
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const COUNT;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const MINIMUM;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const MAXIMUM;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const SUM;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const AVERAGE;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const VARIANCE;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const POPULATION_VARIANCE;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const STANDARD_DEVIATION;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const POPULATION_STANDARD_DEVIATION;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const BASIC;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const MEDIAN;
};

/*! \brief Provide statistics for set of numbers.
 */
template<typename numeric_t>
class HNumberSetStats {
public:
	typedef HNumberSetStats this_type;
	typedef numeric_t value_type;
private:
	int long _count;
	numeric_t _minimum;
	numeric_t _maximum;
	numeric_t _sum;
	numeric_t _average;
	numeric_t _median;
	numeric_t _variance;
	numeric_t _populationVariance;
	aggregate_type_t _aggregateType;
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
	HNumberSetStats( iterator_t, iterator_t, aggregate_type_t = AGGREGATE_TYPE::BASIC );
	int long count( void ) const {
		M_PROLOG
		M_ENSURE( _aggregateType & AGGREGATE_TYPE::COUNT );
		return ( _count );
		M_EPILOG
	}
	HNumberSetStats( HNumberSetStats const& ) = default;
	HNumberSetStats& operator = ( HNumberSetStats const& ) = default;
	numeric_t minimum( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::MINIMUM ) && ( _count > 0 ) );
		return ( _minimum );
		M_EPILOG
	}
	numeric_t maximum( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::MAXIMUM ) && ( _count > 0 ) );
		return ( _maximum );
		M_EPILOG
	}
	numeric_t sum( void ) const {
		M_PROLOG
		M_ENSURE( _aggregateType & AGGREGATE_TYPE::SUM );
		return ( _sum );
		M_EPILOG
	}
	numeric_t average( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::AVERAGE ) && ( _count > 0 ) );
		return ( _average );
		M_EPILOG
	}
	numeric_t median( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::MEDIAN ) && ( _count > 0 ) );
		return ( _median );
		M_EPILOG
	}
	numeric_t variance( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::VARIANCE ) && ( _count > 1 ) );
		return ( _variance );
		M_EPILOG
	}
	numeric_t population_variance( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::POPULATION_VARIANCE ) && ( _count > 0 ) );
		return ( _populationVariance );
		M_EPILOG
	}
	numeric_t standard_deviation( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::STANDARD_DEVIATION ) && ( _count > 1 ) );
		return ( math::square_root( _variance ) );
		M_EPILOG
	}
	numeric_t population_standard_deviation( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION ) && ( _count > 0 ) );
		return ( math::square_root( _populationVariance ) );
		M_EPILOG
	}
};

template<typename numeric_t>
template<typename iterator_t>
HNumberSetStats<numeric_t>::HNumberSetStats( iterator_t first_, iterator_t last_, aggregate_type_t aggregateType_ )
	: _count( 0 ), _minimum(), _maximum(), _sum(), _average(),
	_median(), _variance(), _populationVariance(),
	_aggregateType( aggregateType_ ) {
	M_PROLOG
	numeric_t acc( 0 );
	for ( iterator_t it( first_ ); it != last_; ++ it, ++ _count ) {
		if ( _count ) {
			if ( *it < _minimum ) {
				_minimum = *it;
			}
			if ( *it > _maximum ) {
				_maximum = *it;
			}
		} else {
			_minimum = *it;
			_maximum = *it;
		}
		_sum += *it;
		acc += ( *it * *it );
	}
	_average = _sum / static_cast<numeric_t>( _count );
	if ( _count > 1 ) {
		_variance = acc / static_cast<numeric_t>( _count - 1 ) - ( ( _average * _average * static_cast<numeric_t>( _count ) ) / static_cast<numeric_t>( _count - 1 ) );
	}
	_populationVariance = acc / static_cast<numeric_t>( _count ) - _average * _average;
	if ( ( _count > 0 ) && ( _aggregateType & AGGREGATE_TYPE::MEDIAN ) ) {
		_median = select( first_, last_, ( _count - 1 ) / 2 );
	}
	return;
	M_EPILOG
}

template<typename iterator_t>
HNumberSetStats<typename trait::ternary<is_floating_point<typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type>::value,
	typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type,
	double long>::type> number_set_stats( iterator_t first_, iterator_t last_, aggregate_type_t aggregateType_ = AGGREGATE_TYPE::BASIC ) {
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
select( iterator_t first_, iterator_t last_, int long kth_ ) {
	M_PROLOG
	typedef typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type value_t;
	typedef hcore::HAuxiliaryBuffer<value_t> aux_t;
	M_ENSURE( first_ != last_ );
	aux_t aux( first_, last_ );
	M_ENSURE( aux.get_size() == aux.get_requested_size() );
	if ( aux.get_requested_size() == 0 ) {
		return ( *first_ );
	}
	M_ENSURE( ( kth_ >= 0 ) && ( kth_ < aux.get_requested_size() ) );
	value_t* left( aux.begin() );
	value_t* right( aux.end() - 1 );
	value_t* kth( left );
	while ( left != right ) {
		kth = partition( left, right, bind2nd( less<value_t>(), choose_pivot( left, right, less<value_t>() ) ) );
		int long idx( kth - left );
		if ( idx == kth_ )
			break;
		if ( kth_ < idx ) {
			right = kth - 1;
		} else {
			kth_ -= ( idx + 1 );
			left = kth + 1;
		}
	}
	return ( *kth );
	M_EPILOG
}

template<typename iterator_t>
void central_moving_average( iterator_t first_, iterator_t last_, iterator_t dst_, int long range_ ) {
	M_ENSURE( ( range_ > 0 ) && ( range_ & 1 ) );
	if ( range_ > 1 ) { /* for range == 1 moving average is a no-op */
		typedef typename hcore::iterator_traits<iterator_t>::value_type value_type;
		value_type sum = value_type();
		iterator_t frontTracker_( first_ );
		iterator_t backTracker_( first_ );
		int long const preRange( ( range_ - 1 ) / 2 );
		int long valuesInSum( 0 );
		for ( ; ( valuesInSum <= preRange ) && ( frontTracker_ != last_ ); ++ valuesInSum, ++ frontTracker_ )
			sum += *frontTracker_;
		bool hadFullRange( false );
		for ( ; first_ != last_; ++ first_, ++ dst_ ) {
			*dst_ = sum / static_cast<value_type>( valuesInSum );
			if ( valuesInSum == range_ )
				hadFullRange = true;
			if ( hadFullRange ) {
				sum -= *backTracker_;
				++ backTracker_;
			}
			if ( frontTracker_ != last_ ) {
				sum += *frontTracker_;
				++ frontTracker_;
				if ( valuesInSum < range_ )
					++ valuesInSum;
			} else
				-- valuesInSum;
		}
	}
	return;
}

template<typename iterator_t>
void moving_average( iterator_t first_, iterator_t last_, iterator_t dst_, int long range_ ) {
	M_ENSURE( range_ > 0 );
	if ( range_ > 1 ) { /* for range == 1 moving average is a no-op */
		typedef typename hcore::iterator_traits<iterator_t>::value_type value_type;
		value_type sum = value_type();
		iterator_t backTracker_( first_ );
		bool hadFullRange( false );
		int long valuesInSum( 1 );
		for ( ; first_ != last_; ++ first_, ++ dst_ ) {
			sum += *first_;
			*dst_ = sum / static_cast<value_type>( valuesInSum );
			if ( valuesInSum == range_ )
				hadFullRange = true;
			if ( hadFullRange ) {
				sum -= *backTracker_;
				++ backTracker_;
			}
			if ( valuesInSum < range_ )
				++ valuesInSum;
		}
	}
	return;
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_XMATH_HXX_INCLUDED */

