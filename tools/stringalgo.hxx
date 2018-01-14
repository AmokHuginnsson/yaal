/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/stringalgo.hxx
 * \brief Declaration of string algorithms.
 */

#ifndef YAAL_TOOLS_STRINGALGO_HXX_INCLUDED
#define YAAL_TOOLS_STRINGALGO_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/harray.hxx"
#include "hcore/htokenizer.hxx"

namespace yaal {

namespace tools {

namespace string {

typedef yaal::hcore::HArray<yaal::hcore::HString> tokens_t;

template<typename collection = tokens_t>
collection split(
		yaal::hcore::HString const& string_,
		yaal::hcore::HString const& separator_,
		yaal::hcore::HTokenizer::behavior_t behavior_ = yaal::hcore::HTokenizer::DEFAULT ) {
	collection coll;
	yaal::hcore::HTokenizer t( string_, separator_, behavior_ );
	for ( yaal::hcore::HString const& s : t ) {
		coll.emplace_back( s );
	}
	return ( coll );
}

template<typename collection>
yaal::hcore::HString join( collection const& container_, yaal::hcore::HString const& separator_ ) {
	yaal::hcore::HString value;
	for ( typename collection::value_type const& part : container_ ) {
		if ( ! value.is_empty() ) {
			value.append( separator_ );
		}
		value.append( to_string( part ) );
	}
	return ( value );
}

/*! \brief The string distance metrics.
 */
enum class DISTANCE_METRIC {
	LEVENSHTEIN,
	DAMERAU_LEVENSHTEIN,
	QWERTY
};

/*! \brief Calculate distance between two strings.
 *
 * \param first - first string of a pair of strings to calculate distance.
 * \param second - second string of given pair.
 * \param distanceMetric_ - distance metric type to use for calculating string distance.
 * \return calculated (Damerau-)Levenshtein distance between strings.
 */
int distance( yaal::hcore::HString const& first, yaal::hcore::HString const& second, DISTANCE_METRIC distanceMetric_ = DISTANCE_METRIC::DAMERAU_LEVENSHTEIN );

/*! \brief Predicate for find_local algorithm.
 *
 * HAlike looks for string most similar to a given string.
 *
 * \tparam iter_t - type of an iterator that will be used to traverse collection.
 * \tparam item_t - type of item to look for.
 */
template<typename iter_t, typename item_t>
class HAlike {
	item_t const& _item;
	iter_t _iter;
	int long _best;
	DISTANCE_METRIC _distanceMetric;
public:
	HAlike( iter_t it, item_t const& item, DISTANCE_METRIC distanceMetric_ = DISTANCE_METRIC::DAMERAU_LEVENSHTEIN )
		: _item( item )
		, _iter( it )
		, _best( meta::max_signed<int long>::value )
		, _distanceMetric( distanceMetric_ ) {}
	void operator()( iter_t it ) {
		int long dist = distance( _item, *it, _distanceMetric );
		if ( dist < _best ) {
			_best = dist;
			_iter = it;
		}
	}
	iter_t operator()( void ) const {
		return ( _iter );
	}
};

template<typename iter_t, typename item_t>
HAlike<iter_t, item_t> alike( iter_t iter, item_t const& item, DISTANCE_METRIC distanceMetric_ = DISTANCE_METRIC::DAMERAU_LEVENSHTEIN ) {
	return ( HAlike<iter_t, item_t>( iter, item, distanceMetric_ ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_STRINGALGO_HXX_INCLUDED */

