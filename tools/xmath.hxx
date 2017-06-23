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

/* *TODO* Add `mode`. */
/* *TODO* Add `interquartile_range`. */
/* *TODO* Add `mean_absolute_deviation`. */

namespace yaal {

namespace tools {

/*! \brief Meta-function (template) implementation of various math functions.
 */
namespace xmath {

template<typename iterator_t>
yaal::hcore::HPair<
	typename hcore::iterator_traits<iterator_t>::value_type,
	typename hcore::iterator_traits<iterator_t>::value_type
>
select( iterator_t, iterator_t, int long, bool );

typedef yaal::hcore::HBitFlag<struct AGGREGATE_TYPE> aggregate_type_t;
struct AGGREGATE_TYPE {
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const COUNT;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const MINIMUM;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const MAXIMUM;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const SUM;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const ARITHMETIC_MEAN;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const SAMPLE_VARIANCE;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const POPULATION_VARIANCE;
	static M_YAAL_TOOLS_PUBLIC_API aggregate_type_t const SAMPLE_STANDARD_DEVIATION;
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
	numeric_t _arithmeticMean;
	numeric_t _median;
	numeric_t _sampleVariance;
	numeric_t _populationVariance;
	aggregate_type_t _aggregateType;
public:
	/*! \brief Construct statistics for given range of numbers.
	 *
	 * Following aggregate types are implicit:
	 * `count', `minimum', `maximum', `sum', `arithmetic_mean'
	 * and `sample_variance' (`population sample_variance',
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
	numeric_t arithmetic_mean( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::ARITHMETIC_MEAN ) && ( _count > 0 ) );
		return ( _arithmeticMean );
		M_EPILOG
	}
	numeric_t median( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::MEDIAN ) && ( _count > 0 ) );
		return ( _median );
		M_EPILOG
	}
	numeric_t sample_variance( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::SAMPLE_VARIANCE ) && ( _count > 1 ) );
		return ( _sampleVariance );
		M_EPILOG
	}
	numeric_t population_variance( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::POPULATION_VARIANCE ) && ( _count > 0 ) );
		return ( _populationVariance );
		M_EPILOG
	}
	numeric_t sample_standard_deviation( void ) const {
		M_PROLOG
		M_ENSURE( ( _aggregateType & AGGREGATE_TYPE::SAMPLE_STANDARD_DEVIATION ) && ( _count > 1 ) );
		return ( math::square_root( _sampleVariance ) );
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
	: _count( 0 )
	, _minimum()
	, _maximum()
	, _sum()
	, _arithmeticMean()
	, _median()
	, _sampleVariance()
	, _populationVariance()
	, _aggregateType( aggregateType_ ) {
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
	_arithmeticMean = _sum / static_cast<numeric_t>( _count );
	if ( _count > 1 ) {
		_sampleVariance = ( acc - _arithmeticMean * _arithmeticMean * static_cast<numeric_t>( _count ) ) / static_cast<numeric_t>( _count - 1 );
	}
	_populationVariance = acc / static_cast<numeric_t>( _count ) - _arithmeticMean * _arithmeticMean;
	if ( ( _count > 0 ) && ( _aggregateType & AGGREGATE_TYPE::MEDIAN ) ) {
		typedef yaal::hcore::HPair<numeric_t, numeric_t> middle_t;
		if ( _count % 2 ) {
			middle_t m( select( first_, last_, _count / 2, false ) );
			_median = m.first;
		} else {
			middle_t m( select( first_, last_, _count / 2 - 1, true ) );
			_median = ( m.first + m.second ) / 2.;
		}
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

template<typename number_t>
number_t cumulative_distribution_function( number_t const& x_, number_t const& mu_, number_t const& sigma_ ) {
	static number_t const sqrt2( yaal::math::square_root( number_t( 2 ) ) );
	static number_t const halve( number_t( 0.5 ) );
	static number_t const one( number_t( 1 ) );
	number_t arg( ( x_ - mu_ ) / ( sigma_ * sqrt2 ) );
	return ( halve * ( one + yaal::math::error_function( arg ) ) );
}

template<typename iterator_t>
yaal::hcore::HPair<
	typename hcore::iterator_traits<iterator_t>::value_type,
	typename hcore::iterator_traits<iterator_t>::value_type
>
select( iterator_t first_, iterator_t last_, int long kth_, bool two_ ) {
	M_PROLOG
	typedef typename trait::strip_const<typename hcore::iterator_traits<iterator_t>::value_type>::type value_t;
	typedef hcore::HAuxiliaryBuffer<value_t> aux_t;
	M_ENSURE( first_ != last_ );
	aux_t aux( first_, last_ );
	M_ENSURE( aux.get_size() == aux.get_requested_size() );
	if ( aux.get_requested_size() == 0 ) {
		return ( yaal::hcore::make_pair( *first_, 0 ) );
	}
	M_ENSURE( ( kth_ >= 0 ) && ( kth_ < aux.get_requested_size() ) );
	value_t* left( aux.begin() );
	value_t* right( aux.end() );
	value_t* kth( left + kth_ );
	nth_element( left, kth, right );
	value_t v0( *kth );
	value_t v1( 0 );
	if ( two_ ) {
		++ kth;
		nth_element( left, kth, right );
		v1 = *kth;
	}
	return ( yaal::hcore::make_pair( v0, v1 ) );
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

