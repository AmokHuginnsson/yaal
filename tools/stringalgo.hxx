/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  stringalgo.hxx - this file is integral part of `yaal' project.

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

template<typename collection>
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

/*! \brief The Levenshtein and Damerau-Levenshtein string distance functions.
 */
namespace distance {

/*! \brief Calculate distance between two strings.
 *
 * \param first - first string of a pair of strings to calculate distance.
 * \param second - second string of given pair.
 * \param damerau - use Damerau-Levenshtein definition of string distance if set to true, use Levenshtein definition otherwise.
 * \return calculated (Damerau-)Levenshtein distance between strings.
 */
int levenshtein_damerau( yaal::hcore::HString const& first, yaal::hcore::HString const& second, bool damerau = true );

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
	bool _damerau;
public:
	HAlike( iter_t it, item_t const& item, bool damerau = true )
		: _item( item ), _iter( it ),
		_best( meta::max_signed<int long>::value ),
		_damerau( damerau ) {}
	void operator()( iter_t it ) {
		int long dist = levenshtein_damerau( _item, *it, _damerau );
		if ( dist < _best ) {
			_best = dist;
			_iter = it;
		}
	}
	iter_t operator()( void ) const
		{ return ( _iter ); }
};

template<typename iter_t, typename item_t>
HAlike<iter_t, item_t> alike( iter_t iter, item_t const& item, bool damerau = true ) {
	return ( HAlike<iter_t, item_t>( iter, item, damerau ) );
}

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_STRINGALGO_HXX_INCLUDED */

