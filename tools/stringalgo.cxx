/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "stringalgo.hxx"
#include "hcore/hresource.hxx"
#include "tools/util.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace string {

int distance( yaal::hcore::HString const& one_, yaal::hcore::HString const& two_, DISTANCE_METRIC distanceMetric_ ) {
	M_PROLOG
	int lengthOne = static_cast<int>( one_.length() );
	int lengthTwo = static_cast<int>( two_.length() );
	if ( ! lengthTwo ) {
		return lengthOne;
	}
	if ( ! lengthOne ) {
		return lengthTwo;
	}
	++ lengthOne;
	++ lengthTwo;
	int** distanceMatrix = nullptr;
	HResource<int*[]> distanceMatrixHolder( new ( memory::yaal ) int*[ static_cast<int long long unsigned>( lengthOne ) ] );
	HResource<int[]> distanceMatrixBuffer( new ( memory::yaal ) int[ static_cast<int long long unsigned>( lengthOne * lengthTwo ) ] );
	distanceMatrix = distanceMatrixHolder.raw();
	for ( int indexOne( 0 ); indexOne < lengthOne; ++ indexOne ) {
		distanceMatrix[ indexOne ] = distanceMatrixBuffer.raw() + indexOne * lengthTwo;
	}
	for ( int indexOne( 0 ); indexOne < lengthOne; ++ indexOne ) {
		distanceMatrix[ indexOne ][ 0 ] = indexOne;
	}
	for ( int indexTwo( 0 ); indexTwo < lengthTwo; ++ indexTwo ) {
		distanceMatrix[ 0 ][ indexTwo ] = indexTwo;
	}
	-- lengthTwo;
	-- lengthOne;
	/* real magic starts here */
	for ( int indexOne( 0 ); indexOne < lengthOne; ++ indexOne ) {
		code_point_t one( one_[ indexOne ] );
		for ( int indexTwo( 0 ); indexTwo < lengthTwo; ++ indexTwo ) {
			int cost( 0 );
			int swapCost( 0 );
			code_point_t two( two_[ indexTwo ] );
			if ( one != two ) {
				cost = (
					( distanceMetric_ == DISTANCE_METRIC::QWERTY )
					&& is_ascii( one )
					&& is_ascii( two )
					&& ( strchr( util::near_keys( static_cast<char>( one.get() ) ), static_cast<char>( two.get() ) ) == nullptr )
				) ? 2 : 1;
				swapCost = 1;
			}
			distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ] = min(
				distanceMatrix[ indexOne ][ indexTwo + 1 ] + 1,
				distanceMatrix[ indexOne + 1 ][ indexTwo ] + 1,
				distanceMatrix[ indexOne ][ indexTwo ] + cost
			);
			if (
				( distanceMetric_ != DISTANCE_METRIC::LEVENSHTEIN )
				&& ( indexOne > 0 ) && ( indexTwo > 0 )
				&& ( one_[ indexOne - 1 ] == two )
				&& ( one == two_[ indexTwo - 1 ] )
			) {
				distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ] = min(
					distanceMatrix[ indexOne + 1 ][ indexTwo + 1 ],
					distanceMatrix[ indexOne - 1 ][ indexTwo - 1 ] + swapCost
				);
			}
		}
	}
	return ( distanceMatrix[ lengthOne ][ lengthTwo ] );
	M_EPILOG
}

yaal::hcore::HString::size_type common_prefix_length(
	yaal::hcore::HString const& str1_,
	yaal::hcore::HString const& str2_,
	yaal::hcore::HString::size_type maxLen_
) {
	HString::size_type len( 0 );
	maxLen_ = min( str1_.get_length(), str2_.get_length(), maxLen_ );
	while ( ( len < maxLen_ ) && ( str1_[len] == str2_[len] ) ) {
		++ len;
	}
	return len;
}

}

}

}

