/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/stringalgo.hxx
 * \brief Declaration of string algorithms.
 */

#ifndef YAAL_TOOLS_STRINGALGO_HXX_INCLUDED
#define YAAL_TOOLS_STRINGALGO_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/harray.hxx"
#include "hcore/htokenizer.hxx"
#include "hcore/unicode.hxx"

namespace yaal {

namespace tools {

namespace string {

typedef yaal::hcore::HArray<yaal::hcore::HString> tokens_t;

template<typename collection = tokens_t>
collection split(
		yaal::hcore::HString const& string_,
		yaal::hcore::HString const& separator_,
		yaal::hcore::HTokenizer::behavior_t behavior_ = yaal::hcore::HTokenizer::DEFAULT,
		code_point_t escape_ = code_point_t( 0 )
) {
	collection coll;
	yaal::hcore::HTokenizer t( string_, separator_, behavior_, escape_ );
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

template<typename iterator_t>
yaal::hcore::HString longest_common_prefix_impl( iterator_t first_, iterator_t last_, hcore::iterator_category::random_access ) {
	yaal::hcore::HString prefix;
	if ( first_ == last_ ) {
		return ( yaal::hcore::HString() );
	}
	yaal::hcore::HString const& base( *first_ );
	iterator_t first( first_ );
	++ first;
	yaal::hcore::HString::size_type len( 0 );
	while ( true ) {
		if ( base.get_length() <= len ) {
			break;
		}
		yaal::code_point_t character( base[len] );
		bool common( true );
		for ( iterator_t it( first_ ); it != last_; ++ it ) {
			if ( it->get_length() <= len ) {
				common = false;
				break;
			}
			if ( (*it)[len] != character ) {
				common = false;
				break;
			}
		}
		if ( ! common ) {
			break;
		}
		prefix.push_back( character );
		++ len;
	}
	return ( prefix );
}

yaal::hcore::HString::size_type common_prefix_length(
	yaal::hcore::HString const&,
	yaal::hcore::HString const&,
	yaal::hcore::HString::size_type = yaal::meta::min_signed<yaal::hcore::HString::size_type>::value
);

template<typename iterator_t>
yaal::hcore::HString longest_common_prefix_impl( iterator_t first_, iterator_t last_, hcore::iterator_category::forward ) {
	if ( first_ == last_ ) {
		return ( yaal::hcore::HString() );
	}
	yaal::hcore::HString const& base( *first_ );
	yaal::hcore::HString::size_type commonPrefixLength( base.get_length() );
	++ first_;
	while ( first_ != last_ ) {
		commonPrefixLength = common_prefix_length( base, *first_, commonPrefixLength );
		++ first_;
	}
	return ( base.left( commonPrefixLength ) );
}

template<typename iterator_t>
yaal::hcore::HString longest_common_prefix( iterator_t first_, iterator_t last_ ) {
	return ( longest_common_prefix_impl( first_, last_, typename hcore::iterator_traits<iterator_t>::category_type() ) );
}

template<typename collection>
yaal::hcore::HString longest_common_prefix( collection const& container_ ) {
	return ( longest_common_prefix( yaal::begin( container_ ), yaal::end( container_ ) ) );
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

